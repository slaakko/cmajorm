// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_CLASSES_INCLUDED
#define CMAJOR_RTS_CLASSES_INCLUDED
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/uuid/uuid.hpp>

namespace cmajor { namespace rt {

void InitClasses(int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray, int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray);
void DoneClasses();
bool DynamicInitVmtsAndCompare(void* vmt1, void* vmt2);
boost::multiprecision::uint128_t GetClassId(const boost::uuids::uuid& typeId);

} } // namespace cmajor::rt

#endif // CMAJOR_RTS_CLASSES_INCLUDED
