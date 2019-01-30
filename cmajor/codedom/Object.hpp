// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEDOM_OBJECT_INCLUDED
#define CMAJOR_CODEDOM_OBJECT_INCLUDED
#include <cmajor/codedom/CodeDomApi.hpp>
#include <cmajor/util/CodeFormatter.hpp>
#include <string>
#include <vector>
#include <memory>

namespace cmajor { namespace codedom {

using cmajor::util::CodeFormatter;
class Visitor;

class CODEDOM_API CppObject
{
public:
    CppObject(const std::u32string& name_);
    CppObject(const CppObject&) = delete;
    CppObject& operator=(const CppObject&) = delete;
    virtual ~CppObject();
    const std::u32string& Name() const { return name; }
    void SetName(const std::u32string& newName) { name = newName; }
    void Own(CppObject* object);
    bool IsOwned() const { return isOwned; }
    void SetOwned() { isOwned = true; }
    void ResetOwned() { isOwned = false; }
    virtual std::u32string ToString() const;
    virtual void Print(CodeFormatter& formatter) ;
    virtual void Accept(Visitor& visitor) = 0;
    virtual int Rank() const { return 100; }
private:
    std::u32string name;
    bool isOwned;
    std::vector<std::unique_ptr<CppObject>> ownedObjects;
};

} } // namespace cmajor::codedom

#endif // CMAJOR_CODEDOM_OBJECT_INCLUDED
