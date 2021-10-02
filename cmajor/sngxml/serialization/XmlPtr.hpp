// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_PTR_INCLUDED
#define SNGXML_XML_PTR_INCLUDED
#include <sngxml/serialization/XmlSerializable.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>

namespace sngxml { namespace xmlser {

class XmlContainer;
class XmlSerializable;

class SNGXML_SERIALIZATION_API XmlPtrBase
{
public:
    XmlPtrBase();
    virtual ~XmlPtrBase();
    const boost::uuids::uuid& TargetObjectId() const { return targetObjectId; }
    void SetTargetObjectId(const boost::uuids::uuid& targetObjectId_);
    virtual bool IncludeInBundle() const { return includeInBundle; }
    void SetIncludeInBundle(bool includeInBundle_);
    void Resolve(XmlContainer* container);
    virtual XmlSerializable* GetPtr() const = 0;
    virtual void SetPtr(XmlSerializable* p) = 0;
private:
    boost::uuids::uuid targetObjectId;
    bool includeInBundle;
};

template<class T>
class XmlPtr : public XmlPtrBase
{
public:
    XmlPtr() : XmlPtrBase(), ptr(nullptr) {}
    explicit XmlPtr(T* ptr_) : XmlPtrBase(), ptr(ptr_) { SetId(); }
    void operator=(T* ptr_) { ptr = ptr_; SetId(); }
    void Reset(T* ptr_) { ptr = ptr_; SetId(); }
    void Reset() { ptr = nullptr; SetId(); }
    T* Get() const { return ptr; }
    T* operator->() { return ptr; }
    const T* operator->() const { return ptr; }
    T& operator*() { return *ptr; }
    const T& operator*() const { return *ptr; }
    XmlSerializable* GetPtr() const override;
    void SetPtr(XmlSerializable* p) override;
private:
    void SetId()
    {
        if (ptr)
        {
            if (XmlSerializable* intf = dynamic_cast<XmlSerializable*>(ptr))
            { 
                SetTargetObjectId(intf->ObjectId());
            }
            else
            {
                SetTargetObjectId(boost::uuids::nil_uuid());
            }
        }
        else
        {
            SetTargetObjectId(boost::uuids::nil_uuid());
        }
    }
    T* ptr;
};

template<class T>
XmlSerializable* XmlPtr<T>::GetPtr() const 
{ 
    return dynamic_cast<XmlSerializable*>(ptr); 
}

template<class T>
void XmlPtr<T>::SetPtr(XmlSerializable* p) 
{ 
    ptr = dynamic_cast<T*>(p); 
}

template<class T>
inline bool operator==(const XmlPtr<T>& left, const XmlPtr<T>& right)
{
    return left.Get() == right.Get();
}

template<class T>
inline bool operator!=(const XmlPtr<T>& left, const XmlPtr<T>& right)
{
    return left.Get() != right.Get();
}

template <class T>
class UniqueXmlPtr : public XmlPtrBase
{
public:
    UniqueXmlPtr() : ptr() {}
    explicit UniqueXmlPtr(T* ptr_) : ptr(ptr_) { SetId(); }
    UniqueXmlPtr(const UniqueXmlPtr&) = delete;
    UniqueXmlPtr(UniqueXmlPtr&& that) : XmlPtrBase(std::move(that)), ptr(std::move(that.ptr)) { }
    void operator=(T* ptr_) { ptr.reset(ptr_); SetId(); }
    UniqueXmlPtr& operator=(const UniqueXmlPtr& that) = delete;
    void Reset() { ptr.reset(); SetId(); }
    void Reset(T* ptr_) { ptr.reset(ptr_); SetId(); }
    T* Get() const { return ptr.get(); }
    T* Release() { T* p = ptr.release(); SetId(); return p; }
    T* operator->() { return ptr.get(); }
    const T* operator->() const { return ptr.get(); }
    T& operator*() { return *ptr; }
    const T& operator*() const { return *ptr; }
    XmlSerializable* GetPtr() const override;
    void SetPtr(XmlSerializable* p) override;
private:
    void SetId()
    {
        if (ptr)
        {
            if (XmlSerializable* intf = dynamic_cast<XmlSerializable*>(ptr.get()))
            {
                SetTargetObjectId(intf->ObjectId());
            }
            else
            {
                SetTargetObjectId(boost::uuids::nil_uuid());
            }
        }
        else
        {
            SetTargetObjectId(boost::uuids::nil_uuid());
        }
    }
    std::unique_ptr<T> ptr;
};

template<class T>
XmlSerializable* UniqueXmlPtr<T>::GetPtr() const
{ 
    return dynamic_cast<XmlSerializable*>(ptr.get()); 
}

template<class T>
void UniqueXmlPtr<T>::SetPtr(XmlSerializable* p)
{ 
    ptr.reset(dynamic_cast<T*>(p)); 
}

template<class T>
inline bool operator==(const UniqueXmlPtr<T>& left, const UniqueXmlPtr<T>& right)
{
    return left.Get() == right.Get();
}

template<class T>
inline bool operator!=(const UniqueXmlPtr<T>& left, const UniqueXmlPtr<T>& right)
{
    return left.Get() != right.Get();
}

} } // namespace sngxml::xmlser

#endif // SNGXML_XML_PTR_INCLUDED
