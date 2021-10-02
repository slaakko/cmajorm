#include <sngxml/xmlserializationtest/test.hpp>
#include <sngxml/serialization/XmlClassRegistry.hpp>
#include <sngxml/serialization/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Parser.hpp>
#include <boost/uuid/random_generator.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/InitDone.hpp>
#include <iostream>
#include <sstream>

using namespace soulng::util;

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        sngxml::xmlser::Init();
    }
    ~InitDone()
    {
        sngxml::xmlser::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

const int simpleClassClassId = 0;
const int baseClassClassId = 1;
const int derivedClassClassId = 2;
const int xmlTestClassClassId = 3;

bool CheckCmajorRootEnv()
{
    try
    {
        soulng::unicode::CmajorRoot();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return false;
    }
    return true;
}

int main()
{
    if (!CheckCmajorRootEnv()) return 1;
    InitDone initDone;
    try
    {
        SimpleClass::Register(simpleClassClassId);
        BaseClass::Register(baseClassClassId);
        DerivedClass::Register(derivedClassClassId);
        XmlTestClass::Register(xmlTestClassClassId);

        XmlTestClass testClass;
        testClass.SetRandomObjectId();
        testClass.f0 = true;
        testClass.f1 = 1;
        testClass.f2 = 2;
        testClass.f3 = 3;
        testClass.f4 = 4;
        testClass.f5 = 5;
        testClass.f6 = 6;
        testClass.f7 = 7;
        testClass.f8 = 8;
        testClass.f9 = 9.1f;
        testClass.f10 = 10.2;
        testClass.f11 = 'x';
        testClass.f12 = u'y';
        testClass.f13 = U'z';
        testClass.f14 = soulng::util::GetCurrentDate();
        testClass.f15 = soulng::util::GetCurrentDateTime();
        testClass.ts = soulng::util::GetCurrentTimestamp();
        testClass.time = soulng::util::GetCurrentTime();
        testClass.f16 = "f16.string";
        testClass.f17 = u"f17.wstring";
        testClass.f18 = U"f18.ustring";
        testClass.f19 = std::chrono::steady_clock::now();
        testClass.f20 = std::chrono::nanoseconds{ 100000 };
        testClass.f21 = boost::uuids::random_generator()();
        testClass.f22.SetRandomObjectId();
        testClass.f22.member = "simple_class.member";
        std::vector<SimpleClass> v;
        SimpleClass v0;
        v0.SetRandomObjectId();
        v0.member = "v0";
        testClass.f23.push_back(v0);
        SimpleClass v1;
        v1.SetRandomObjectId();
        v1.member = "v1";
        testClass.f23.push_back(v1);
        testClass.f24.reset();
        std::unique_ptr<BaseClass> v25(new BaseClass());
        v25->SetRandomObjectId();
        v25->member = "base_class.v25";
        testClass.f25.reset(v25.release());
        DerivedClass* d = new DerivedClass();
        d->SetRandomObjectId();
        d->member = "base.member";
        d->derivedMember = "derivedClass.member";
        std::unique_ptr<BaseClass> v26(d);
        testClass.f26.reset(v26.release());
        std::unique_ptr<BaseClass> v271;
        testClass.f27.push_back(std::move(v271));
        std::unique_ptr<BaseClass> v272(new BaseClass());
        v272->SetRandomObjectId();
        v272->member = "base_class.v272.member";
        testClass.f27.push_back(std::move(v272));
        DerivedClass* d273 = new DerivedClass();
        d273->SetRandomObjectId();
        d273->member = "base.member";
        d273->derivedMember = "derivedClass.member";
        std::unique_ptr<BaseClass> v273(d273);
        testClass.f27.push_back(std::move(v273));
        testClass.f28 = nullptr;
        std::unique_ptr<BaseClass> v29(new BaseClass());
        v29->SetRandomObjectId();
        testClass.f29 = v29.get();
        std::unique_ptr<BaseClass> v30(new DerivedClass());
        v30->SetRandomObjectId();
        testClass.f30 = v30.get();
        testClass.f31 = nullptr;
        std::unique_ptr<BaseClass> v32(new BaseClass());
        v32->SetRandomObjectId();
        testClass.f32.Reset(v32.release());
        std::unique_ptr<BaseClass> v33(new DerivedClass());
        v33->SetRandomObjectId();
        testClass.f33.Reset(v33.release());
        testClass.f34.push_back(xml_ptr<BaseClass>());
        testClass.f34.push_back(xml_ptr<BaseClass>(v29.get()));
        testClass.f34.push_back(xml_ptr<BaseClass>(v30.get()));
        testClass.f35.push_back(unique_xml_ptr<BaseClass>());
        std::unique_ptr<BaseClass> v351(new BaseClass());
        v351->SetRandomObjectId();
        testClass.f35.push_back(unique_xml_ptr<BaseClass>(v351.release()));
        std::unique_ptr<BaseClass> v352(new DerivedClass());
        v352->SetRandomObjectId();
        testClass.f35.push_back(unique_xml_ptr<BaseClass>(v352.release()));
        std::unique_ptr<sngxml::dom::Element> element = testClass.ToXml("object");
        sngxml::dom::Document doc;
        doc.AppendChild(std::unique_ptr<sngxml::dom::Node>(element.release()));
        std::stringstream s;
        CodeFormatter formatter(s);
        formatter.SetIndentSize(1);
        doc.Write(formatter);

        std::string str = s.str();
        std::cout << str << std::endl;

        std::u32string content = soulng::unicode::ToUtf32(str);
        std::unique_ptr<sngxml::dom::Document> readDoc = sngxml::dom::ParseDocument(content, "string");

        XmlTestClass readClass;
        readClass.FromXml(readDoc->DocumentElement());
        std::unique_ptr<sngxml::dom::Element> readElement = readClass.ToXml("object");
        sngxml::dom::Document dc;
        dc.AppendChild(std::move(readElement));
        CodeFormatter form(std::cout);
        form.SetIndentSize(1);
        dc.Write(form);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
