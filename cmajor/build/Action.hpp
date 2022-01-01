// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_ACTION_INCLUDED
#define CMAJOR_BUILD_ACTION_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <sngcm/ast/Project.hpp>
#include <map>

namespace cmajor { namespace build {

class BUILD_API Variable
{
public:
    Variable(const std::string& name_, const std::string& value_);
    const std::string& Name() const { return name; }
    const std::string& Value() const { return value; }
private:
    std::string name;
    std::string value;
};

class BUILD_API Variables
{
public:
    Variables();
    Variables(const Variables&) = delete;
    Variables& operator=(const Variables&) = delete;
    void AddVariable(Variable* variable);
    Variable* GetVariable(const std::string& name) const;
private:
    std::map<std::string, Variable*> variableMap;
    std::vector<std::unique_ptr<Variable>> variables;
};

BUILD_API void RunBuildActions(const sngcm::ast::Project& project, const Variables& variables);

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_ACTION_INCLUDED
