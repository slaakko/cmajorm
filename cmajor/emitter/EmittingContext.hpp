// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_EMITTER_EMITTING_CONTEXT_INCLUDED
#define CMAJOR_EMITTER_EMITTING_CONTEXT_INCLUDED

namespace cmajor { namespace emitter {

class EmittingContextImpl;

class EmittingContext
{
public:
    EmittingContext();
    ~EmittingContext();
    EmittingContextImpl* GetEmittingContextImpl() { return emittingContextImpl; }
private:
    EmittingContextImpl * emittingContextImpl;
};

} } // namespace cmajor::emitter

#endif // CMAJOR_EMITTER_EMITTING_CONTEXT_INCLUDED
