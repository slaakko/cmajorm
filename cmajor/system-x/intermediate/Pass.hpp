// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_PASS_INCLUDED
#define CMSX_INTERMEDIATE_PASS_INCLUDED
#include <system-x/intermediate/Api.hpp>
#include <string>
#include <set>

namespace cmsx::intermediate {

class Function;
class Context;

enum class PassKind
{
    function, module, composite
};

class CMSX_INTERMEDIATE_API Pass
{
public:
    Pass(const std::string& name_, PassKind kind_);
    virtual ~Pass();
    PassKind Kind() const { return kind; }
    const std::string& Name() const { return name; }
    const std::set<Pass*>& RequiredPasses() const { return requiredPasses; }
    const std::set<Pass*>& InvalidatedPasses() const { return invalidatedPasses; }
    void AddRequiredPass(Pass* pass);
    void AddInvalidatedPass(Pass* pass);
    bool Valid() const { return valid; }
    void SetValid() { valid = true; }
    void ResetValid() { valid = false; }
private:
    std::string name;
    PassKind kind;
    bool valid;
    std::set<Pass*> requiredPasses;
    std::set<Pass*> invalidatedPasses;
};

class CMSX_INTERMEDIATE_API FunctionPass : public Pass
{
public:
    FunctionPass(const std::string& name_);
    virtual void Run(Function& function);
    virtual void Clear(Function& function);
};

class CMSX_INTERMEDIATE_API ModulePass : public Pass
{
public:
    ModulePass(const std::string& name_);
    ModulePass(const std::string& name_, PassKind kind_);
    virtual void Run(Context* context);
    virtual void Clear(Context* context);
};

class CMSX_INTERMEDIATE_API CompositePass : public ModulePass
{
public:
    CompositePass(const std::string& name_);
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_PASS_INCLUDED
