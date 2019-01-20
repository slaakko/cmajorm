// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/ConceptRepository.hpp>

namespace cmajor { namespace binder {

BoundConcept* ConceptRepository::GetBoundConcept(const BoundConceptKey& key) const
{
    auto it = boundConceptMap.find(key);
    if (it != boundConceptMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void ConceptRepository::AddBoundConcept(const BoundConceptKey& key, std::unique_ptr<BoundConcept>&& boundConcept)
{
    boundConceptMap[key] = boundConcept.get();
    boundConcepts.push_back(std::move(boundConcept));
}

} } // namespace cmajor::binder
