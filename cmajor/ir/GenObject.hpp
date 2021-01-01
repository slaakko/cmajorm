// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_IR_GEN_OBJECT_INCLUDED
#define CMAJOR_IR_GEN_OBJECT_INCLUDED
#include <cmajor/ir/IrApi.hpp>
#include <stdint.h>

namespace cmajor { namespace ir {

class Emitter;

enum class OperationFlags : uint16_t
{
    none = 0,
    addr = 1 << 0,
    deref = 1 << 1,
    virtualCall = 1 << 2,
    leaveFirstArg = 1 << 3,
    copyFirst = 1 << 4,
    functionCallFlags = leaveFirstArg,
    derefCount = 0xFF << 8
};

inline OperationFlags operator|(OperationFlags left, OperationFlags right)
{
    return OperationFlags(uint16_t(left) | uint16_t(right));
}

inline OperationFlags operator&(OperationFlags left, OperationFlags right)
{
    return OperationFlags(uint16_t(left) & uint16_t(right));
}

inline uint8_t GetDerefCount(OperationFlags flags)
{
    return uint8_t(uint16_t(flags & OperationFlags::derefCount) >> 8);
}

inline OperationFlags SetDerefCount(OperationFlags flags, uint8_t n)
{
    return OperationFlags(flags | OperationFlags(n << 8));
}

class IR_API GenObject
{
public:
    GenObject();
    virtual ~GenObject();
    virtual void Load(Emitter& emitter, OperationFlags flags) = 0;
    virtual void Store(Emitter& emitter, OperationFlags flags) = 0;
    void SetType(void* type_) { type = type_; }
    void* GetType() { return type; }
private:
    void* type;
};

class IR_API NativeValue : public GenObject
{
public:
    NativeValue(void* value_) : value(value_) {}
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
private:
    void* value;
};

} } // namespace cmajor::ir

#endif // CMAJOR_IR_GEN_OBJECT_INCLUDED
