// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ClassTemplateSpecializationSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Sha1.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

std::u32string MakeClassTemplateSpecializationName(ClassTypeSymbol* classTemplate, const std::vector<TypeSymbol*>& templateArgumentTypes)
{
    std::u32string name = classTemplate->GroupName();
    name.append(1, '<');
    int n = templateArgumentTypes.size();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            name.append(U", ");
        }
        name.append(templateArgumentTypes[i]->FullName());
    }
    name.append(1, '>');
    return name;
}

ClassTemplateSpecializationSymbol::ClassTemplateSpecializationSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    ClassTypeSymbol(SymbolType::classTemplateSpecializationSymbol, span_, sourceModuleId_, name_), classTemplate(nullptr), templateArgumentTypes(), flags(ClassTemplateSpecializationFlags::none)
{
}

ClassTemplateSpecializationSymbol::ClassTemplateSpecializationSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, std::u32string& name_, ClassTypeSymbol* classTemplate_, const std::vector<TypeSymbol*>& templateArgumentTypes_) :
    ClassTypeSymbol(SymbolType::classTemplateSpecializationSymbol, span_, sourceModuleId_, name_), classTemplate(classTemplate_), templateArgumentTypes(templateArgumentTypes_), flags(ClassTemplateSpecializationFlags::none)
{
}

ClassTemplateSpecializationSymbol::~ClassTemplateSpecializationSymbol()
{
}

std::u32string ClassTemplateSpecializationSymbol::SimpleName() const
{
    std::u32string simpleName = classTemplate->GroupName();
    int n = templateArgumentTypes.size();
    for (int i = 0; i < n; ++i)
    {
        simpleName.append(U"_").append(templateArgumentTypes[i]->SimpleName());
    }
    return simpleName;
}

void ClassTemplateSpecializationSymbol::Write(SymbolWriter& writer)
{
    ClassTypeSymbol::Write(writer);
    const boost::uuids::uuid& classTemplateId = classTemplate->TypeId();
    writer.GetBinaryWriter().Write(classTemplateId);
    uint32_t n = templateArgumentTypes.size();
    writer.GetBinaryWriter().WriteULEB128UInt(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        TypeSymbol* templateArgumentType = templateArgumentTypes[i];
        const boost::uuids::uuid& templateArgumentTypeId = templateArgumentType->TypeId();
        writer.GetBinaryWriter().Write(templateArgumentTypeId);
    }
    writer.GetBinaryWriter().Write(uint8_t(flags));
}

void ClassTemplateSpecializationSymbol::Read(SymbolReader& reader)
{
    ClassTypeSymbol::Read(reader);
    boost::uuids::uuid classTemplateId;
    reader.GetBinaryReader().ReadUuid(classTemplateId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, classTemplateId, -1);
    uint32_t n = reader.GetBinaryReader().ReadULEB128UInt();
    templateArgumentTypes.resize(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        boost::uuids::uuid typeArgumentId;
        reader.GetBinaryReader().ReadUuid(typeArgumentId);
        reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeArgumentId, -2 - i);
    }
    flags = ClassTemplateSpecializationFlags(reader.GetBinaryReader().ReadByte());
}

void ClassTemplateSpecializationSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index < 0)
    {
        if (index == -1)
        {
            Assert(typeSymbol->GetSymbolType() == SymbolType::classTypeSymbol, "class type symbol expected");
            classTemplate = static_cast<ClassTypeSymbol*>(typeSymbol);
        }
        else
        {
            int typeArgumentIndex = -(index + 2);
            if (typeArgumentIndex < 0 || typeArgumentIndex >= templateArgumentTypes.size())
            {
                Assert(false, "invalid emplace type index in class template specialization");
            }
            templateArgumentTypes[typeArgumentIndex] = typeSymbol;
        }
    }
    else
    {
        ClassTypeSymbol::EmplaceType(typeSymbol, index);
    }
}

bool ClassTemplateSpecializationSymbol::IsPrototypeTemplateSpecialization() const
{
    return IsPrototype();
}

void* ClassTemplateSpecializationSymbol::IrType(Emitter& emitter)
{
    if (IsRecursive())
    {
        void* localIrType = emitter.GetIrTypeByTypeId(TypeId());
        if (!localIrType)
        {
            localIrType = ClassTypeSymbol::IrType(emitter);
            emitter.SetIrTypeByTypeId(TypeId(), localIrType);
        }
        return localIrType;
    }
    else
    {
        return ClassTypeSymbol::IrType(emitter);
    }
}

void ClassTemplateSpecializationSymbol::SetGlobalNs(std::unique_ptr<Node>&& globalNs_)
{
    globalNs = std::move(globalNs_); 
}

void ClassTemplateSpecializationSymbol::SetFileScope(FileScope* fileScope_)
{
    fileScope.reset(fileScope_);
}

FileScope* ClassTemplateSpecializationSymbol::ReleaseFileScope()
{
    return fileScope.release();
}

TypeSymbol* ClassTemplateSpecializationSymbol::UnifyTemplateArgumentType(SymbolTable& symbolTable, const std::unordered_map<TemplateParameterSymbol*, TypeSymbol*>& templateParameterMap, 
    const Span& span, const boost::uuids::uuid& moduleId)
{
    std::vector<TypeSymbol*> targetTemplateArgumentTypes;
    for (int i = 0; i < templateArgumentTypes.size(); ++i)
    {
        TypeSymbol* templateArgumentType = templateArgumentTypes[i]->UnifyTemplateArgumentType(symbolTable, templateParameterMap, span, moduleId);
        if (templateArgumentType)
        {
            targetTemplateArgumentTypes.push_back(templateArgumentType);
        }
        else
        {
            return nullptr;
        }
    }
    return symbolTable.MakeClassTemplateSpecialization(classTemplate, targetTemplateArgumentTypes, span, GetRootModuleForCurrentThread()->Id());
}

std::u32string ClassTemplateSpecializationSymbol::Id() const
{
    if (IsPrototype())
    {
        return MangledName();
    }
    else
    {
        return TypeSymbol::Id();
    }
}

void ClassTemplateSpecializationSymbol::Check()
{
    ClassTypeSymbol::Check();
    if (!classTemplate)
    {
        throw SymbolCheckException("class template specialization has no class template", GetSpan(), SourceModuleId());
    }
    for (TypeSymbol* templateArguementType : templateArgumentTypes)
    {
        if (!templateArguementType)
        {
            throw SymbolCheckException("class template specialization has no template argument type", GetSpan(), SourceModuleId());
        }
    }
}

} } // namespace cmajor::symbols
