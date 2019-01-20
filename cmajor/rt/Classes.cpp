// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Classes.hpp>
#include <cmajor/rt/Statics.hpp>
#include <cmajor/rt/Error.hpp>
#include <cmajor/rt/Io.hpp>
#include <cmajor/util/System.hpp>
#include <cmajor/util/BinaryReader.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/Prime.hpp>
#include <cmajor/util/Uuid.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <mutex>
#include <sstream>

namespace cmajor { namespace rt {

using namespace cmajor::util;

class ClassIdMap
{
public:
    static void Init();
    static void Done();
    static ClassIdMap& Instance() { return *instance; }
    void SetClassId(const boost::uuids::uuid& typeId, uint64_t classId);
    uint64_t GetClassId(const boost::uuids::uuid& typeId) const;
private:
    static std::unique_ptr<ClassIdMap> instance;
    std::unordered_map<boost::uuids::uuid, uint64_t, boost::hash<boost::uuids::uuid>> classIdMap;
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

void ClassIdMap::SetClassId(const boost::uuids::uuid& typeId, uint64_t classId)
{
    classIdMap[typeId] = classId;
}

uint64_t ClassIdMap::GetClassId(const boost::uuids::uuid& typeId) const
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
        RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        exit(exitCodeInternalError);
    }
}

uint64_t GetClassId(const boost::uuids::uuid& typeId)
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
            uint64_t typeId1 = polymorphicClassIdArray[3 * i];
            uint64_t typeId2 = polymorphicClassIdArray[3 * i + 1];
            uint64_t classId = polymorphicClassIdArray[3 * i + 2];
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
        RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        exit(exitCodeInternalError);
    }
}

std::mutex dynamicInitVmtMutex;

uint64_t DynamicInitVmt(void* vmt)
{
    std::lock_guard<std::mutex> lock(dynamicInitVmtMutex);
    uint64_t* vmtHeader = reinterpret_cast<uint64_t*>(vmt);
    if (vmtHeader[0] == 0) // zero class id expected at the start of the VMT
    {
        uint64_t typeId1 = vmtHeader[1];
        uint64_t typeId2 = vmtHeader[2];
        boost::uuids::uuid typeId;
        IntsToUuid(typeId1, typeId2, typeId); 
        uint64_t classId = ClassIdMap::Instance().GetClassId(typeId);
        vmtHeader[0] = classId;
    }
    return vmtHeader[0];
}

void DoneClasses()
{
    ClassIdMap::Done();
}

} } // namespace cmajor::rt
