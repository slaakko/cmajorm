// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_PASS_MANAGER_INCLUDED
#define CMSX_INTERMEDIATE_PASS_MANAGER_INCLUDED
#include <system-x/intermediate/Pass.hpp>
#include <sngxml/dom/Element.hpp>
#include <map>
#include <memory>
#include <vector>

namespace cmsx::intermediate {

class CMSX_INTERMEDIATE_API PassManager
{
public:
    static void Init();
    static void Done();
    static PassManager& Instance() { return *instance; }
    PassManager(const PassManager&) = delete;
    PassManager& operator=(const PassManager&) = delete;
    void Initialize();
    void AddPass(Pass* pass);
    Pass* GetPass(const std::string& passName) const;
    void Run(int logStreamId, Context* context, const std::string& passName, bool verbose);
private:
    PassManager();
    void ReadPassesXml();
    void ReadPassContent(sngxml::dom::Element* passElement, Pass* pass);
    std::vector<Pass*> CollectPasses(const std::string& passName) const;
    std::vector<FunctionPass*> GetFunctionPasses(const std::vector<Pass*>& passes);
    std::vector<ModulePass*> GetModuleAndCompositePasses(const std::vector<Pass*>& passes);
    void RunFunctionPasses(Context* context, const std::vector<FunctionPass*>& functionPasses);
    void RunModuleAndCompositePasses(Context* context, const std::vector<ModulePass*>& moduleAndCompositePasses);
    static std::unique_ptr<PassManager> instance;
    std::vector<std::unique_ptr<Pass>> passes;
    std::map<std::string, Pass*> passMap;
};

CMSX_INTERMEDIATE_API void InitPassManager();
CMSX_INTERMEDIATE_API void DonePassManager();

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_PASS_MANAGER_INCLUDED
