// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_REGISTER_ALLOCATOR_INCLUDED
#define CMSX_INTERMEDIATE_REGISTER_ALLOCATOR_INCLUDED
#include <system-x/intermediate/Api.hpp>
#include <memory>
#include <map>
#include <set>
#include <vector>

namespace cmsx::assembler {

class DecimalConstant;

}  // namespace cmsx::assembler

namespace cmsx::intermediate {

class Instruction;
class Context;

enum class Locations : int
{
    none = 0, reg = 1 << 0, frame = 1 << 1
};

CMSX_INTERMEDIATE_API inline Locations operator|(Locations left, Locations right)
{
    return Locations(int(left) | int(right));
}

CMSX_INTERMEDIATE_API inline Locations operator&(Locations left, Locations right)
{
    return Locations(int(left) & int(right));
}

CMSX_INTERMEDIATE_API inline Locations operator~(Locations locs)
{
    return Locations(~int(locs));
}

struct CMSX_INTERMEDIATE_API FrameLocation
{
    FrameLocation() : index(-1) {}
    FrameLocation(int index_) : index(index_) {}
    bool Valid() const { return index != -1; }
    bool IsWithinImmediateRange() const { return index >= 0 && index < 256 / 8; }
    int index;
};

class CMSX_INTERMEDIATE_API ArgLocation
{
public:
    ArgLocation(int index_);
private:
    friend class CallFrame;
    void SetConstant(cmsx::assembler::DecimalConstant* constant_);
    void SetValue(uint64_t start);
    int index;
    cmsx::assembler::DecimalConstant* constant;
};

class CMSX_INTERMEDIATE_API CallFrame
{
public:
    CallFrame();
    CallFrame(const CallFrame&) = delete;
    CallFrame& operator=(const CallFrame&) = delete;
    void NextArgLocation(cmsx::assembler::DecimalConstant* constant);
    int SaveNumLocals() const { return saveNumLocals; }
    void SetSaveNumLocals(int saveNumLocals_) { saveNumLocals = saveNumLocals_; }
    void Resolve(int frameSize);
private:
    int saveNumLocals;
    std::vector<std::unique_ptr<ArgLocation>> argLocations;
};

class CMSX_INTERMEDIATE_API Frame
{
public:
    Frame();
    FrameLocation NextFrameLocation() { return FrameLocation(nextFrameLocationIndex++); }
    int Size() const { return nextFrameLocationIndex * 8; }
    bool IsWithinWydeRange() const { return nextFrameLocationIndex * 8 < 65536; }
    CallFrame* CurrentCallFrame() { return currentCallFrame.get(); }
    void ResetCallFrame();
    void AddCallFrame();
    void ResolveCallFrames();
private:
    int nextFrameLocationIndex;
    std::unique_ptr<CallFrame> currentCallFrame;
    std::vector<std::unique_ptr<CallFrame>> callFrames;
};

enum class RegisterKind : int
{
    none, local, global
};

struct CMSX_INTERMEDIATE_API Register
{
    Register() : kind(RegisterKind::none), number(0) {}
    Register(RegisterKind kind_, uint8_t number_) : kind(kind_), number(number_) {}
    bool Valid() const { return kind != RegisterKind::none; }
    bool IsLocal() const { return kind == RegisterKind::local; }
    bool IsGlobal() const { return kind == RegisterKind::global; }
    RegisterKind kind;
    uint8_t number;
};

CMSX_INTERMEDIATE_API inline bool operator<(const Register& left, const Register& right)
{
    if (left.kind == RegisterKind::local && right.kind == RegisterKind::global) return true;
    if (left.kind == RegisterKind::global && right.kind == RegisterKind::local) return false;
    return left.number < right.number;
}

class CMSX_INTERMEDIATE_API RegisterPool
{
public:
    static void Init();
    static void Done();
    static RegisterPool& Instance() { return *instance; }
    void AddLocalRegister(const Register& reg);
    Register GetLocalRegister();
    Register GetGlobalRegister(uint8_t number);
    int LocalRegisterCount() const { return localRegisterCount; }
    int NumFreeLocalRegisters() const { return localRegisterPool.size(); }
private:
    RegisterPool();
    static std::unique_ptr<RegisterPool> instance;
    int localRegisterCount;
    std::set<Register> localRegisterPool;
    std::map<uint8_t, Register> globalRegisterMap;
};

struct CMSX_INTERMEDIATE_API SpillData
{
    Register registerToSpill;;
    FrameLocation spillToFrameLocation;
};

enum class RegisterAllocationAction
{
    none, spill, registerAllocated
};

class CMSX_INTERMEDIATE_API RegisterAllocator
{
public:
    virtual ~RegisterAllocator();
    virtual RegisterAllocationAction Run(Instruction* inst) = 0;
    virtual const std::vector<SpillData>& GetSpillData() const = 0;
    virtual Locations GetLocations(Instruction* inst) const = 0;
    virtual Register GetRegister(Instruction* inst) const = 0;
    virtual Frame& GetFrame() = 0;
    virtual FrameLocation GetFrameLocation(Instruction* inst) const = 0;
    virtual int LastActiveLocalReg() const = 0;
};

Register GetGlobalRegister(uint8_t registerNumber);
Register GetLocalRegister();

CMSX_INTERMEDIATE_API void InitRegisterAllocator();
CMSX_INTERMEDIATE_API void DoneRegisterAllocator();

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_ALLOCATION_INCLUDED
