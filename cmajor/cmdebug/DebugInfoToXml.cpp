// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DebugInfoToXml.hpp>
#include <cmajor/cmdebug/DebugInfoIo.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <cmajor/cmdebug/DIVariable.hpp>
#include <cmajor/cmdebug/DIEnumConstant.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <fstream>

namespace cmajor { namespace debug {

using namespace soulng::unicode;

sngxml::dom::Element* VariableToXmlElement(DIVariable* variable)
{
    sngxml::dom::Element* variableElement = new sngxml::dom::Element(U"variable");
    variableElement->SetAttribute(U"name", ToUtf32(variable->Name()));
    variableElement->SetAttribute(U"irName", ToUtf32(variable->IrName()));
    variableElement->SetAttribute(U"typeId", ToUtf32(boost::uuids::to_string(variable->TypeId())));
    return variableElement;
}

sngxml::dom::Element* TypeToXmlElement(DIType* type)
{
    sngxml::dom::Element* typeElement = typeElement = new sngxml::dom::Element(U"type");
    switch (type->GetKind())
    {
        case DIType::Kind::primitiveType:
        {
            DIPrimitiveType* primitiveType = static_cast<DIPrimitiveType*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(primitiveType->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(primitiveType->Name()));
            typeElement->SetAttribute(U"irName", ToUtf32(primitiveType->IrName()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(primitiveType->Id())));
            break;
        }
        case DIType::Kind::enumType:
        {
            DIEnumType* enumType = static_cast<DIEnumType*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(enumType->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(enumType->Name()));
            typeElement->SetAttribute(U"irName", ToUtf32(enumType->IrName()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(enumType->Id())));
            typeElement->SetAttribute(U"underlyingTypeId", ToUtf32(boost::uuids::to_string(enumType->UnderlyingTypeId())));
            sngxml::dom::Element* enumConstantsElement = new sngxml::dom::Element(U"enumConstants");
            int n = enumType->EnumConstants().size();
            for (int i = 0; i < n; ++i)
            {
                const DIEnumConstant& enumConstant = enumType->EnumConstants()[i];
                sngxml::dom::Element* enumConstantElement = new sngxml::dom::Element(U"enumConstant");
                enumConstantElement->SetAttribute(U"name", ToUtf32(enumConstant.Name()));
                enumConstantElement->SetAttribute(U"value", ToUtf32(enumConstant.Value()));
                enumConstantElement->SetAttribute(U"strValue", ToUtf32(enumConstant.StrValue()));
                enumConstantsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(enumConstantElement));
            }
            typeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(enumConstantsElement));
            break;
        }
        case DIType::Kind::templateParameter:
        {
            DITemplateParameter* templateParameter = static_cast<DITemplateParameter*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(templateParameter->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(templateParameter->Name()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(templateParameter->Id())));
            break;
        }
        case DIType::Kind::classType:
        {
            DIClassType* classType = static_cast<DIClassType*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(classType->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(classType->Name()));
            typeElement->SetAttribute(U"irName", ToUtf32(classType->IrName()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(classType->Id())));
            if (!classType->BaseClassId().is_nil())
            {
                typeElement->SetAttribute(U"baseClassId", ToUtf32(boost::uuids::to_string(classType->BaseClassId())));
            }
            if (!classType->TemplateParameters().empty())
            {
                sngxml::dom::Element* templateParametersElement = new sngxml::dom::Element(U"templateParameters");
                int n = classType->TemplateParameters().size();
                for (int i = 0; i < n; ++i)
                {
                    DITemplateParameter* templateParameter = classType->TemplateParameters()[i].get();
                    sngxml::dom::Element* templateParameterElement = TypeToXmlElement(templateParameter);
                    templateParametersElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(templateParameterElement));
                }
                typeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(templateParametersElement));
            }
            if (!classType->MemberVariables().empty())
            {
                sngxml::dom::Element* memberVariablesElement = new sngxml::dom::Element(U"memberVariables");
                int n = classType->MemberVariables().size();
                for (int i = 0; i < n; ++i)
                {
                    DIVariable* memberVariable = classType->MemberVariables()[i].get();
                    sngxml::dom::Element* memberVariableElement = VariableToXmlElement(memberVariable);
                    memberVariablesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(memberVariableElement));
                }
                typeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(memberVariablesElement));
            }
            if (classType->IsPolymorphic())
            {
                typeElement->SetAttribute(U"polymorphic", U"true");
                int32_t vmtPtrIndex = classType->VmtPtrIndex();
                typeElement->SetAttribute(U"vmtPtrIndex", ToUtf32(std::to_string(vmtPtrIndex)));
                typeElement->SetAttribute(U"vmtVariableName", ToUtf32(classType->VmtVariableName()));
            }
            break;
        }
        case DIType::Kind::specializationType:
        {
            DIClassTemplateSpecializationType* specializationType = static_cast<DIClassTemplateSpecializationType*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(specializationType->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(specializationType->Name()));
            typeElement->SetAttribute(U"irName", ToUtf32(specializationType->IrName()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(specializationType->Id())));
            boost::uuids::uuid primaryTypeId = specializationType->PrimaryTypeId();
            typeElement->SetAttribute(U"primaryTypeId", ToUtf32(boost::uuids::to_string(primaryTypeId)));
            if (!specializationType->BaseClassId().is_nil())
            {
                typeElement->SetAttribute(U"baseClassId", ToUtf32(boost::uuids::to_string(specializationType->BaseClassId())));
            }
            if (!specializationType->MemberVariables().empty())
            {
                sngxml::dom::Element* memberVariablesElement = new sngxml::dom::Element(U"memberVariables");
                int n = specializationType->MemberVariables().size();
                for (int i = 0; i < n; ++i)
                {
                    DIVariable* memberVariable = specializationType->MemberVariables()[i].get();
                    sngxml::dom::Element* memberVariableElement = VariableToXmlElement(memberVariable);
                    memberVariablesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(memberVariableElement));
                }
                typeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(memberVariablesElement));
            }
            if (specializationType->GetContainerClassTemplateKind() != ContainerClassTemplateKind::notContainerClassTemplate)
            {
                typeElement->SetAttribute(U"container", ToUtf32(ContainerName(specializationType->GetContainerClassTemplateKind())));
                typeElement->SetAttribute(U"valueTypeId", ToUtf32(boost::uuids::to_string(specializationType->ValueTypeId())));
            }
            sngxml::dom::Element* templateArgumentTypesElement = new sngxml::dom::Element(U"templateArgumentTypes");
            int32_t n = specializationType->TemplateArgumentTypeIds().size();
            for (int32_t i = 0; i < n; ++i)
            {
                sngxml::dom::Element* templateArgumentTypeElement = new sngxml::dom::Element(U"templateArgumentType");
                templateArgumentTypeElement->SetAttribute(U"templateArgumentTypeId", ToUtf32(boost::uuids::to_string(specializationType->TemplateArgumentTypeIds()[i])));
                templateArgumentTypesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(templateArgumentTypeElement));
            }
            typeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(templateArgumentTypesElement));
            if (specializationType->IsPolymorphic())
            {
                typeElement->SetAttribute(U"polymorphic", U"true");
                int32_t vmtPtrIndex = specializationType->VmtPtrIndex();
                typeElement->SetAttribute(U"vmtPtrIndex", ToUtf32(std::to_string(vmtPtrIndex)));
                typeElement->SetAttribute(U"vmtVariableName", ToUtf32(specializationType->VmtVariableName()));
            }
            break;
        }
        case DIType::Kind::delegateType:
        {
            DIDelegateType* delegateType = static_cast<DIDelegateType*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(delegateType->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(delegateType->Name()));
            typeElement->SetAttribute(U"irName", ToUtf32(delegateType->IrName()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(delegateType->Id())));
            break;
        }
        case DIType::Kind::classDelegateType:
        {
            DIClassDelegateType* classDelegateType = static_cast<DIClassDelegateType*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(classDelegateType->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(classDelegateType->Name()));
            typeElement->SetAttribute(U"irName", ToUtf32(classDelegateType->IrName()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(classDelegateType->Id())));
            break;
        }
        case DIType::Kind::interfaceType:
        {
            DIInterfaceType* interfaceType = static_cast<DIInterfaceType*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(interfaceType->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(interfaceType->Name()));
            typeElement->SetAttribute(U"irName", ToUtf32(interfaceType->IrName()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(interfaceType->Id())));
            break;
        }
        case DIType::Kind::constType:
        {
            DIConstType* constType = static_cast<DIConstType*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(constType->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(constType->Name()));
            typeElement->SetAttribute(U"irName", ToUtf32(constType->IrName()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(constType->Id())));
            typeElement->SetAttribute(U"baseTypeId", ToUtf32(boost::uuids::to_string(constType->BaseTypeId())));
            break;
        }
        case DIType::Kind::referenceType:
        {
            DIReferenceType* referenceType = static_cast<DIReferenceType*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(referenceType->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(referenceType->Name()));
            typeElement->SetAttribute(U"irName", ToUtf32(referenceType->IrName()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(referenceType->Id())));
            typeElement->SetAttribute(U"baseTypeId", ToUtf32(boost::uuids::to_string(referenceType->BaseTypeId())));
            break;
        }
        case DIType::Kind::pointerType:
        {
            DIPointerType* pointerType = static_cast<DIPointerType*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(pointerType->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(pointerType->Name()));
            typeElement->SetAttribute(U"irName", ToUtf32(pointerType->IrName()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(pointerType->Id())));
            typeElement->SetAttribute(U"pointedTypeId", ToUtf32(boost::uuids::to_string(pointerType->PointedTypeId())));
            break;
        }
        case DIType::Kind::arrayType:
        {
            DIArrayType* arrayType = static_cast<DIArrayType*>(type);
            typeElement->SetAttribute(U"kind", ToUtf32(DIType::KindStr(arrayType->GetKind())));
            typeElement->SetAttribute(U"name", ToUtf32(arrayType->Name()));
            typeElement->SetAttribute(U"irName", ToUtf32(arrayType->IrName()));
            typeElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(arrayType->Id())));
            typeElement->SetAttribute(U"elementTypeId", ToUtf32(boost::uuids::to_string(arrayType->ElementTypeId())));
            typeElement->SetAttribute(U"size", ToUtf32(std::to_string(arrayType->Size())));
            break;
        }
    }
    return typeElement;
}

std::unique_ptr<sngxml::dom::Document> GetDebugInfoAsXml(const std::string& cmdbFilePath)
{
    std::unique_ptr<sngxml::dom::Document> doc(new sngxml::dom::Document());
    sngxml::dom::Element* cmdbElement = new sngxml::dom::Element(U"cmdb");
    doc->AppendChild(std::unique_ptr<sngxml::dom::Node>(cmdbElement));
    BinaryReader reader(cmdbFilePath);
    ReadCmdbFileTag(reader);
    std::string mainProjectName;
    ReadMainProjectName(reader, mainProjectName);
    cmdbElement->SetAttribute(U"mainProjectName", ToUtf32(mainProjectName));
    int32_t numProjects;
    ReadNumberOfProjects(reader, numProjects);
    for (int32_t i = 0; i < numProjects; ++i)
    {
        sngxml::dom::Element* projectElement = new sngxml::dom::Element(U"project");
        std::string projectName;
        std::string projectDirectoryPath;
        int32_t numCompileUnits;
        boost::uuids::uuid mainFunctionId;
        ReadProjectTableHeader(reader, projectName, projectDirectoryPath, numCompileUnits, mainFunctionId);
        projectElement->SetAttribute(U"name", ToUtf32(projectName));
        projectElement->SetAttribute(U"directoryPath", ToUtf32(projectDirectoryPath));
        if (!mainFunctionId.is_nil())
        {
            projectElement->SetAttribute(U"mainFunctionId", ToUtf32(boost::uuids::to_string(mainFunctionId)));
        }
        for (int32_t i = 0; i < numCompileUnits; ++i)
        {
            sngxml::dom::Element* compileUnitElement = new sngxml::dom::Element(U"compileUnit");
            std::string compileUnitBaseName;
            ReadProjectTableRecord(reader, compileUnitBaseName);
            compileUnitElement->SetAttribute(U"baseName", ToUtf32(compileUnitBaseName));
            int32_t numFunctions;
            ReadNumberOfCompileUnitFunctionRecords(reader, numFunctions);
            for (int32_t i = 0; i < numFunctions; ++i)
            {
                int32_t fileIndex;
                boost::uuids::uuid functionId;
                ReadCompileUnitFunctionRecord(reader, fileIndex, functionId);
                sngxml::dom::Element* functionElement = new sngxml::dom::Element(U"function");
                functionElement->SetAttribute(U"fileIndex", ToUtf32(std::to_string(fileIndex)));
                functionElement->SetAttribute(U"functionId", ToUtf32(boost::uuids::to_string(functionId)));
                int32_t numInstructionRecords;
                ReadNumberOfInstructionRecords(reader, numInstructionRecords);
                for (int32_t i = 0; i < numInstructionRecords; ++i)
                {
                    int32_t cppLineNumber;
                    SourceSpan span;
                    int32_t cppLineIndex;
                    int16_t scopeId;
                    int16_t flags;
                    ReadInstructionRecord(reader, cppLineNumber, span, cppLineIndex, scopeId, flags);
                    sngxml::dom::Element* instructionElement = new sngxml::dom::Element(U"instruction");
                    instructionElement->SetAttribute(U"cppLineNumber", ToUtf32(std::to_string(cppLineNumber)));
                    instructionElement->SetAttribute(U"span", ToUtf32(span.ToString()));
                    instructionElement->SetAttribute(U"cppLineIndex", ToUtf32(std::to_string(cppLineIndex)));
                    instructionElement->SetAttribute(U"scopeId", ToUtf32(std::to_string(scopeId)));
                    if (flags != 0)
                    {
                        instructionElement->SetAttribute(U"flags", ToUtf32(InstructionFlagsStr(static_cast<InstructionFlags>(flags))));
                    }
                    functionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(instructionElement));
                }
                sngxml::dom::Element* scopesElement = nullptr;
                int32_t numScopes;
                ReadNumberOfScopes(reader, numScopes);
                if (numScopes > 0)
                {
                    scopesElement = new sngxml::dom::Element(U"scopes");
                }
                for (int32_t i = 0; i < numScopes; ++i)
                {
                    int16_t scopeId;
                    int16_t parentScopeId;
                    int32_t numLocalVariables;
                    ReadScopeRecord(reader, scopeId, parentScopeId, numLocalVariables);
                    sngxml::dom::Element* scopeElement = new sngxml::dom::Element(U"scope");
                    scopeElement->SetAttribute(U"id", ToUtf32(std::to_string(scopeId)));
                    scopeElement->SetAttribute(U"parentScopeId", ToUtf32(std::to_string(parentScopeId)));
                    for (int32_t i = 0; i < numLocalVariables; ++i)
                    {
                        DIVariable localVariable(DIVariable::Kind::localVariable);
                        localVariable.Read(reader);
                        sngxml::dom::Element* localVariableElement = new sngxml::dom::Element(U"localVar");
                        localVariableElement->SetAttribute(U"name", ToUtf32(localVariable.Name()));
                        localVariableElement->SetAttribute(U"irName", ToUtf32(localVariable.IrName()));
                        localVariableElement->SetAttribute(U"typeId", ToUtf32(boost::uuids::to_string(localVariable.TypeId())));
                        scopeElement->AppendChild(std::unique_ptr<sngxml::dom::Element>(localVariableElement));
                    }
                    scopesElement->AppendChild(std::unique_ptr<sngxml::dom::Element>(scopeElement));
                }
                if (scopesElement)
                {
                    functionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(scopesElement));
                }
                sngxml::dom::Element* controlFlowGraphElement = new sngxml::dom::Element(U"controlFlowGraph");
                int32_t controlFlowGraphNodeCount;
                ReadControlFlowGraphNodeCount(reader, controlFlowGraphNodeCount);
                for (int32_t i = 0; i < controlFlowGraphNodeCount; ++i)
                {
                    int32_t nodeId;
                    SourceSpan span;
                    int32_t cppLineIndex;
                    int32_t cppLineNumber;
                    ReadControlFlowGraphNode(reader, nodeId, span, cppLineIndex, cppLineNumber);
                    sngxml::dom::Element* controlFlowGraphNodeElement = new sngxml::dom::Element(U"controlFlowGraphNode");
                    controlFlowGraphNodeElement->SetAttribute(U"nodeId", ToUtf32(std::to_string(nodeId)));
                    controlFlowGraphNodeElement->SetAttribute(U"span", ToUtf32(span.ToString()));
                    controlFlowGraphNodeElement->SetAttribute(U"cppLineIndex", ToUtf32(std::to_string(cppLineIndex)));
                    controlFlowGraphNodeElement->SetAttribute(U"cppLineNumber", ToUtf32(std::to_string(cppLineNumber)));
                    int32_t edgeCount;
                    ReadControlFlowGraphNodeEdgeCount(reader, edgeCount);
                    for (int32_t i = 0; i < edgeCount; ++i)
                    {
                        int32_t endNodeId;
                        ReadControlFlowGraphNodeEdge(reader, endNodeId);
                        sngxml::dom::Element* nextElement = new sngxml::dom::Element(U"next");
                        nextElement->SetAttribute(U"id", ToUtf32(std::to_string(endNodeId)));
                        controlFlowGraphNodeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(nextElement));
                    }
                    controlFlowGraphElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(controlFlowGraphNodeElement));
                }
                functionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(controlFlowGraphElement));
                compileUnitElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionElement));
            }
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(compileUnitElement));
        }
        sngxml::dom::Element* filesElement = new sngxml::dom::Element(U"files");
        int32_t numFileIndexRecords;
        ReadNumberOfFileIndexRecords(reader, numFileIndexRecords);
        for (int32_t i = 0; i < numFileIndexRecords; ++i)
        {
            int32_t fileIndex;
            std::string sourceFilePath;
            ReadFileIndexRecord(reader, fileIndex, sourceFilePath);
            sngxml::dom::Element* fileElement = new sngxml::dom::Element(U"file");
            fileElement->SetAttribute(U"index", ToUtf32(std::to_string(fileIndex)));
            fileElement->SetAttribute(U"sourceFilePath", ToUtf32(sourceFilePath));
            filesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(fileElement));
        }
        projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(filesElement));
        sngxml::dom::Element* functionsElement = new sngxml::dom::Element(U"functions");
        int32_t numFunctionIndexFunctionRecords;
        ReadNumberOfFunctionIndexFunctionRecords(reader, numFunctionIndexFunctionRecords);
        for (int32_t i = 0; i < numFunctionIndexFunctionRecords; ++i)
        {
            boost::uuids::uuid functionId;
            std::string fullFunctionName;
            std::string mangledFunctionName;
            ReadFunctionIndexFunctionRecord(reader, functionId, fullFunctionName, mangledFunctionName);
            sngxml::dom::Element* functionElement = new sngxml::dom::Element(U"function");
            functionElement->SetAttribute(U"functionId", ToUtf32(boost::uuids::to_string(functionId)));
            functionElement->SetAttribute(U"fullName", ToUtf32(fullFunctionName));
            functionElement->SetAttribute(U"mangledName", ToUtf32(mangledFunctionName));
            functionsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionElement));
        }
        projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionsElement));
        sngxml::dom::Element* typesElement = new sngxml::dom::Element(U"types");
        int32_t numTypeIndexRecords = reader.ReadInt();
        for (int32_t i = 0; i < numTypeIndexRecords; ++i)
        {
            std::unique_ptr<DIType> type = ReadType(reader, nullptr);
            sngxml::dom::Element* typeElement = TypeToXmlElement(type.get());
            typesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement));
        }
        projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typesElement));
        cmdbElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectElement));
    }
    return doc;
}

void WriteXmlDebugInfo(sngxml::dom::Document* debugInfoXmlDoc, const std::string& xmlDebugInfoFilePath)
{
    std::ofstream strm(xmlDebugInfoFilePath);
    CodeFormatter formatter(strm);
    formatter.SetIndentSize(1);
    debugInfoXmlDoc->Write(formatter);
}

} } // namespace cmajor::debug
