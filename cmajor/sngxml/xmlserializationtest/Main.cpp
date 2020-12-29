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
        sngxml::xmlser::Init();
        sngxml::xpath::Init();
        soulng::util::Init();
    }
    ~InitDone()
    {
        soulng::util::Done();
        sngxml::xpath::Done();
        sngxml::xmlser::Done();
    }
};

int main()
{
    InitDone initDone;
    try
    {
        sngxml::xmlser::RegisterXmlClass<Polymorphic>();
        sngxml::xmlser::RegisterXmlClass<DerivedClass>();
        XmlTestClass testClass;
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
        testClass.time = soulng::util::GetCurrentTime();
        testClass.f16 = "f16.string";
        testClass.f17 = u"f17.wstring";
        testClass.f18 = U"f18.ustring";
        testClass.f19 = std::chrono::steady_clock::now();
        testClass.f20 = std::chrono::nanoseconds{ 100000 };
        testClass.f21 = boost::uuids::random_generator()();
        testClass.f22.member = "nonpolymorphic.member";
        std::vector<Nonpolymorphic> v;
        Nonpolymorphic v0;
        v0.member = "v0";
        testClass.f23.push_back(v0);
        Nonpolymorphic v1;
        v1.member = "v1";
        testClass.f23.push_back(v1);
        testClass.f24.reset();
        std::unique_ptr<Polymorphic> v25(new Polymorphic());
        v25->member = "polymorphic.v25";
        testClass.f25.reset(v25.release());
        DerivedClass* d = new DerivedClass();
        d->member = "base.member";
        d->derivedMember = "derivedClass.member";
        std::unique_ptr<Polymorphic> v26(d);
        testClass.f26.reset(v26.release());
        std::unique_ptr<Polymorphic> v271;
        testClass.f27.push_back(std::move(v271));
        std::unique_ptr<Polymorphic> v272(new Polymorphic());
        v272->member = "polymorphic.member";
        testClass.f27.push_back(std::move(v272));
        DerivedClass* d273 = new DerivedClass();
        d273->member = "base.member";
        d273->derivedMember = "derivedClass.member";
        std::unique_ptr<Polymorphic> v273(d273);
        testClass.f27.push_back(std::move(v273));

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

        XmlTestClass readClass(readDoc->DocumentElement());
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
