// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_CONCEPT_REPOSITORY_INCLUDED
#define CMAJOR_BINDER_CONCEPT_REPOSITORY_INCLUDED
#include <cmajor/binder/BoundConstraint.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>

namespace cmajor { namespace binder {
   
using namespace cmajor::symbols;

class ConceptRepository
{
public:
    BoundConcept* GetBoundConcept(const BoundConceptKey& key) const;
    void AddBoundConcept(const BoundConceptKey& key, std::unique_ptr<BoundConcept>&& boundConcept);
private:
    std::unordered_map<BoundConceptKey, BoundConcept*, BoundConceptKeyHash> boundConceptMap;
    std::vector<std::unique_ptr<BoundConcept>> boundConcepts;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_CONCEPT_REPOSITORY_INCLUDED
