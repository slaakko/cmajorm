#include <sngxml/serialization/InitDone.hpp>
#include <sngxml/serialization/XmlContainer.hpp>
#include <sngxml/serialization/XmlBundle.hpp>
#include <sngxml/xml_bundle_test/bicycle.hpp>
#include <sngxml/xml_bundle_test/car.hpp>
#include <sngxml/xml_bundle_test/person.hpp>
#include <sngxml/xml_bundle_test/vehicle.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/random_generator.hpp>
#include <iostream>

void InitApplication()
{
    soulng::util::Init();
    sngxml::xpath::Init();
    sngxml::xmlser::Init();
}

// these should match in the C++ and Cmajor code:

const int carClassId = 0;
const int bicycleClassId = 1;
const int personClassId = 2;

int main()
{
    try
    {
        InitApplication();
        Car::Register(carClassId);
        Bicycle::Register(bicycleClassId);
        Person::Register(personClassId);

        sngxml::xmlser::XmlContainer container;

        unique_xml_ptr<Car> c(new Car());
        c->SetRandomObjectId();
        c->registrationNumber = "ABC-123";
        c->color = VehicleColor::red;
        container.Add(c.Get());

        unique_xml_ptr<Bicycle> b(new Bicycle());
        b->SetRandomObjectId();
        b->color = VehicleColor::green;
        b->price = 1000.0;
        container.Add(b.Get());

        unique_xml_ptr<Person> p(new Person());
        p->SetRandomObjectId();
        p->name = "Joe Coder";
        p->age = 25;
        p->AddVehicle(c.Release());
        p->AddVehicle(b.Release());

        std::unique_ptr<sngxml::xmlser::XmlBundle> bundle = container.CreateBundle(p.Get(), sngxml::xmlser::XmlBundleKind::deep);
        std::string xmlStr = bundle->ToXmlString();
        std::cout << xmlStr << std::endl;
        
        std::unique_ptr<sngxml::xmlser::XmlBundle> readBundle = sngxml::xmlser::ToXmlBundle(xmlStr);
        xml_ptr<Person> root(sngxml::xmlser::XmlCast<Person>(readBundle->Root()));
        if (root.Get())
        {
            // use root
        }
        else
        {
            throw std::runtime_error("person expected");
        }
        std::string readXmlStr = readBundle->ToXmlString();
        std::cout << readXmlStr << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
