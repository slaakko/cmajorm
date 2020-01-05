// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_UUID_REPOSITORY_INCLUDED
#define CMAJOR_BINDER_UUID_REPOSITORY_INCLUDED
#include <soulng/util/Error.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <vector>

namespace cmajor { namespace binder {

class UuidRepository
{
public:
    int Install(const boost::uuids::uuid& uuid);
    const boost::uuids::uuid& GetUuid(int id) const;
private:
    std::unordered_map<boost::uuids::uuid, int, boost::hash<boost::uuids::uuid>> uuidMap;
    std::vector<boost::uuids::uuid> uuids;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_UUID_REPOSITORY_INCLUDED
