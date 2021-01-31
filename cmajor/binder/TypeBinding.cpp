// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/TypeBinding.hpp>
#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/TypeBinder.hpp>

namespace cmajor { namespace binder {

std::vector<std::string> BindTypes(Module* module, CompileUnitNode* compileUnit)
{
    std::vector<std::string> errors;
    try
    {
        AttributeBinder attributeBinder(module);
        std::unique_ptr<BoundCompileUnit> boundCompileUnit(new BoundCompileUnit(*module, compileUnit, &attributeBinder));
        boundCompileUnit->PushBindingTypes();
        TypeBinder typeBinder(*boundCompileUnit);
        typeBinder.SetEditMode();
        compileUnit->Accept(typeBinder);
        boundCompileUnit->PopBindingTypes();
        std::vector<std::string> e = typeBinder.Errors();
        errors.insert(errors.end(), e.cbegin(), e.cend());
    }
    catch (const cmajor::symbols::Exception& ex)
    {
        errors.push_back(ex.Message());
    }
    catch (const std::exception& ex)
    {
        errors.push_back(ex.what());
    }
    catch (...)
    {
        errors.push_back("unknown error occurred");
    }
    return errors;
}

} } // namespace cmajor::binder
