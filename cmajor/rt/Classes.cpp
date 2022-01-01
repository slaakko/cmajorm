// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Classes.hpp>
#include <cmajor/rt/Statics.hpp>
#include <cmajor/rt/Error.hpp>
#include <cmajor/rt/Io.hpp>
#include <soulng/util/System.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Prime.hpp>
#include <soulng/util/Uuid.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <mutex>
#include <sstream>

namespace cmajor { namespace rt {

using namespace soulng::util;

class ClassIdMap
{
public:
    static void Init();
    static void Done();
    static ClassIdMap& Instance() { return *instance; }
    void SetClassId(const boost::uuids::uuid& typeId, const boost::multiprecision::uint128_t& classId);
    boost::multiprecision::uint128_t GetClassId(const boost::uuids::uuid& typeId) const;
private:
    static std::unique_ptr<ClassIdMap> instance;
    std::unordered_map<boost::uuids::uuid, boost::multiprecision::uint128_t, boost::hash<boost::uuids::uuid>> classIdMap;
};

std::unique_ptr<ClassIdMap> ClassIdMap::instance;

void ClassIdMap::Init()
{
    instance.reset(new ClassIdMap());
}

void ClassIdMap::Done()
{
    instance.reset();
}

void ClassIdMap::SetClassId(const boost::uuids::uuid& typeId, const boost::multiprecision::uint128_t& classId)
{
    classIdMap[typeId] = classId;
}

boost::multiprecision::uint128_t ClassIdMap::GetClassId(const boost::uuids::uuid& typeId) const
{
    auto it = classIdMap.find(typeId);
    if (it != classIdMap.cend())
    {
        return it->second;
    }
    else
    {
        std::stringstream s;
        s << "internal error : class id for type id " << typeId << " not found.\n";
        std::string str = s.str();
        int32_t errorStringHandle = -1;
        void* stdError = RtOpenStdFile(2, errorStringHandle);
        RtWrite(stdError, reinterpret_cast<const uint8_t*>(str.c_str()), str.length(), errorStringHandle);
        RtFlush(stdError, errorStringHandle);
        exit(exitCodeInternalError);
    }
}

boost::multiprecision::uint128_t GetClassId(const boost::uuids::uuid& typeId)
{
    return ClassIdMap::Instance().GetClassId(typeId);
}

void InitClasses(int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray, int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray)
{
    try
    {
        ClassIdMap::Init();
        boost::uuids::uuid dynamicTypeId;
        for (int64_t i = 0; i < numberOfPolymorphicClassIds; ++i)
        {
            uint64_t typeId1 = polymorphicClassIdArray[4 * i];
            uint64_t typeId2 = polymorphicClassIdArray[4 * i + 1];
            boost::multiprecision::uint128_t classId = boost::multiprecision::uint128_t(polymorphicClassIdArray[4 * i + 2]) << 64 | polymorphicClassIdArray[4 * i + 3];
            IntsToUuid(typeId1, typeId2, dynamicTypeId);
            ClassIdMap::Instance().SetClassId(dynamicTypeId, classId);
        }
        boost::uuids::uuid staticTypeId;
        std::vector<boost::uuids::uuid> staticClassIds;
        for (int64_t i = 0; i < numberOfStaticClassIds; ++i)
        {
            uint64_t typeId1 = staticClassIdArray[2 * i];
            uint64_t typeId2 = staticClassIdArray[2 * i + 1];
            IntsToUuid(typeId1, typeId2, staticTypeId);
            staticClassIds.push_back(staticTypeId);
        }
        AllocateMutexes(staticClassIds);
    }
    catch (const std::exception& ex)
    {
        std::stringstream s;
        s << "internal error in program initialization: " << ex.what() << "\n";
        std::string str = s.str();
        int32_t errorStringHandle = -1;
        void* stdError = RtOpenStdFile(2, errorStringHandle);
        RtWrite(stdError, reinterpret_cast<const uint8_t*>(str.c_str()), str.length(), errorStringHandle);
        RtFlush(stdError, errorStringHandle);
        exit(exitCodeInternalError);
    }
}

std::mutex dynamicInitVmtMutex;

bool DynamicInitVmtsAndCompare(void* vmt1, void* vmt2)
{
    std::lock_guard<std::mutex> lock(dynamicInitVmtMutex);
    uint64_t* vmt1Header = reinterpret_cast<uint64_t*>(vmt1);
    boost::multiprecision::uint128_t classId1(boost::multiprecision::uint128_t(vmt1Header[0]) << 64 | vmt1Header[1]);
    if (classId1 == 0) // class id is zero at the start of the VMT if not yet initialized
    {
        uint64_t typeId1 = vmt1Header[2];
        uint64_t typeId2 = vmt1Header[3];
        boost::uuids::uuid typeId;
        IntsToUuid(typeId1, typeId2, typeId);
        classId1 = ClassIdMap::Instance().GetClassId(typeId);
        vmt1Header[0] = static_cast<uint64_t>(classId1 >> 64);
        vmt1Header[1] = static_cast<uint64_t>(classId1);
    }
    uint64_t* vmt2Header = reinterpret_cast<uint64_t*>(vmt2);
    boost::multiprecision::uint128_t classId2(boost::multiprecision::uint128_t(vmt2Header[0]) << 64 | vmt2Header[1]);
    if (classId2 == 0) // class id is zero at the start of the VMT if not yet initialized
    {
        uint64_t typeId1 = vmt2Header[2];
        uint64_t typeId2 = vmt2Header[3];
        boost::uuids::uuid typeId;
        IntsToUuid(typeId1, typeId2, typeId);
        classId2 = ClassIdMap::Instance().GetClassId(typeId);
        vmt2Header[0] = static_cast<uint64_t>(classId2 >> 64);
        vmt2Header[1] = static_cast<uint64_t>(classId2);
    }
    return classId1 % classId2 == 0;
}

void DoneClasses()
{
    ClassIdMap::Done();
}

} } // namespace cmajor::rt
