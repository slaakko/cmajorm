#include "Rules.hpp"

std::vector<const char*> rules = {
"NothrowAttributeParser.Attributes"
, "NothrowAttributeParser.Attribute"
, "NothrowBasicTypeParser.BasicType"
, "NothrowClassParser.Class"
, "NothrowClassParser.DefiningClassId"
, "NothrowClassParser.InheritanceAndInterfaces"
, "NothrowClassParser.BaseClassOrInterface"
, "NothrowClassParser.ClassContent"
, "NothrowClassParser.ClassMember"
, "NothrowClassParser.SyncMember"
, "NothrowClassParser.StaticConstructor"
, "NothrowClassParser.Constructor"
, "NothrowClassParser.Destructor"
, "NothrowClassParser.Initializer"
, "NothrowClassParser.MemberFunction"
, "NothrowClassParser.ConversionFunction"
, "NothrowClassParser.MemberVariable"
, "NothrowClassParser.DefiningMemberVariableId"
, "NothrowClassMemberParser.ClassMemberMain"
, "NothrowCompileUnitParser.CompileUnit"
, "NothrowCompileUnitParser.NamespaceContent"
, "NothrowCompileUnitParser.UsingDirectives"
, "NothrowCompileUnitParser.UsingDirective"
, "NothrowCompileUnitParser.UsingAliasDirective"
, "NothrowCompileUnitParser.UsingNamespaceDirective"
, "NothrowCompileUnitParser.Definitions"
, "NothrowCompileUnitParser.Definition"
, "NothrowCompileUnitParser.SyncDef"
, "NothrowCompileUnitParser.NamespaceDefinition"
, "NothrowCompileUnitParser.DefiningNamespaceId"
, "NothrowCompileUnitParser.TypedefDeclaration"
, "NothrowCompileUnitParser.ConceptDefinition"
, "NothrowCompileUnitParser.FunctionDefinition"
, "NothrowCompileUnitParser.ClassDefinition"
, "NothrowCompileUnitParser.InterfaceDefinition"
, "NothrowCompileUnitParser.EnumTypeDefinition"
, "NothrowCompileUnitParser.ConstantDefinition"
, "NothrowCompileUnitParser.DelegateDefinition"
, "NothrowCompileUnitParser.ClassDelegateDefinition"
, "NothrowCompileUnitParser.GlobalVariableDefinition"
, "NothrowConceptParser.Concept"
, "NothrowConceptParser.DefiningConceptId"
, "NothrowConceptParser.Refinement"
, "NothrowConceptParser.ConceptBody"
, "NothrowConceptParser.ConceptBodyConstraint"
, "NothrowConceptParser.TypeNameConstraint"
, "NothrowConceptParser.SignatureConstraint"
, "NothrowConceptParser.ConstructorConstraint"
, "NothrowConceptParser.DestructorConstraint"
, "NothrowConceptParser.MemberFunctionConstraint"
, "NothrowConceptParser.FunctionConstraint"
, "NothrowConceptParser.EmbeddedConstraint"
, "NothrowConceptParser.WhereConstraint"
, "NothrowConceptParser.ConstraintExpr"
, "NothrowConceptParser.DisjunctiveConstraintExpr"
, "NothrowConceptParser.ConjunctiveConstraintExpr"
, "NothrowConceptParser.PrimaryConstraintExpr"
, "NothrowConceptParser.AtomicConstraintExpr"
, "NothrowConceptParser.PredicateConstraint"
, "NothrowConceptParser.IsConstraint"
, "NothrowConceptParser.ConceptOrTypeName"
, "NothrowConceptParser.MultiParamConstraint"
, "NothrowConceptParser.Axiom"
, "NothrowConceptParser.AxiomBody"
, "NothrowConceptParser.AxiomStatement"
, "NothrowConstantParser.Constant"
, "NothrowDelegateParser.Delegate"
, "NothrowDelegateParser.DefiningDelegateId"
, "NothrowDelegateParser.ClassDelegate"
, "NothrowDelegateParser.DefiningClassDelegateId"
, "NothrowEnumerationParser.EnumType"
, "NothrowEnumerationParser.DefiningEnumTypeId"
, "NothrowEnumerationParser.UnderlyingType"
, "NothrowEnumerationParser.EnumConstants"
, "NothrowEnumerationParser.EnumConstant"
, "NothrowExpressionParser.Expression"
, "NothrowExpressionParser.Equivalence"
, "NothrowExpressionParser.Implication"
, "NothrowExpressionParser.Disjunction"
, "NothrowExpressionParser.Conjunction"
, "NothrowExpressionParser.BitOr"
, "NothrowExpressionParser.BitXor"
, "NothrowExpressionParser.BitAnd"
, "NothrowExpressionParser.Equality"
, "NothrowExpressionParser.Relational"
, "NothrowExpressionParser.Shift"
, "NothrowExpressionParser.Additive"
, "NothrowExpressionParser.Multiplicative"
, "NothrowExpressionParser.Prefix"
, "NothrowExpressionParser.Postfix"
, "NothrowExpressionParser.Primary"
, "NothrowExpressionParser.SizeOfExpr"
, "NothrowExpressionParser.TypeNameExpr"
, "NothrowExpressionParser.TypeIdExpr"
, "NothrowExpressionParser.CastExpr"
, "NothrowExpressionParser.ConstructExpr"
, "NothrowExpressionParser.NewExpr"
, "NothrowExpressionParser.ArgumentList"
, "NothrowExpressionParser.ExpressionList"
, "NothrowExpressionParser.InvokeExpr"
, "NothrowFunctionParser.Function"
, "NothrowFunctionParser.FunctionGroupId"
, "NothrowFunctionParser.OperatorFunctionGroupId"
, "NothrowGlobalVariableParser.GlobalVariable"
, "NothrowGlobalVariableParser.DefininigGlobalVariableId"
, "NothrowIdentifierParser.Identifier"
, "NothrowIdentifierParser.QualifiedId"
, "NothrowInterfaceParser.Interface"
, "NothrowInterfaceParser.DefiningInterfaceId"
, "NothrowInterfaceParser.InterfaceContent"
, "NothrowInterfaceParser.InterfaceMemFun"
, "NothrowInterfaceParser.InterfaceFunctionGroupId"
, "NothrowLiteralParser.Literal"
, "NothrowLiteralParser.SimpleLiteral"
, "NothrowLiteralParser.ComplexLiteral"
, "NothrowLiteralParser.ArrayLiteral"
, "NothrowLiteralParser.StructuredLiteral"
, "NothrowParameterParser.ParameterList"
, "NothrowParameterParser.Parameter"
, "NothrowSpecifierParser.Specifiers"
, "NothrowSpecifierParser.Specifier"
, "NothrowStatementParser.Statement"
, "NothrowStatementParser.SyncStmt"
, "NothrowStatementParser.Label"
, "NothrowStatementParser.LabeledStatement"
, "NothrowStatementParser.ControlStatement"
, "NothrowStatementParser.CompoundStatement"
, "NothrowStatementParser.ReturnStatement"
, "NothrowStatementParser.IfStatement"
, "NothrowStatementParser.WhileStatement"
, "NothrowStatementParser.DoStatement"
, "NothrowStatementParser.ForStatement"
, "NothrowStatementParser.ForInitStatement"
, "NothrowStatementParser.ForLoopStatementExpr"
, "NothrowStatementParser.RangeForStatement"
, "NothrowStatementParser.DefiningRangeForId"
, "NothrowStatementParser.BreakStatement"
, "NothrowStatementParser.ContinueStatement"
, "NothrowStatementParser.GotoStatement"
, "NothrowStatementParser.SwitchStatement"
, "NothrowStatementParser.CaseStatement"
, "NothrowStatementParser.DefaultStatement"
, "NothrowStatementParser.GotoCaseStatement"
, "NothrowStatementParser.GotoDefaultStatement"
, "NothrowStatementParser.AssignmentStatementExpr"
, "NothrowStatementParser.AssignmentStatement"
, "NothrowStatementParser.ConstructionStatement"
, "NothrowStatementParser.DefiningLocalVariableId"
, "NothrowStatementParser.DeleteStatement"
, "NothrowStatementParser.DestroyStatement"
, "NothrowStatementParser.ExpressionStatement"
, "NothrowStatementParser.EmptyStatement"
, "NothrowStatementParser.ThrowStatement"
, "NothrowStatementParser.TryStatement"
, "NothrowStatementParser.Catch"
, "NothrowStatementParser.AssertStatement"
, "NothrowStatementParser.ConditionalCompilationStatement"
, "NothrowStatementParser.ConditionalCompilationExpression"
, "NothrowStatementParser.ConditionalCompilationDisjunction"
, "NothrowStatementParser.ConditionalCompilationConjunction"
, "NothrowStatementParser.ConditionalCompilationPrefix"
, "NothrowStatementParser.ConditionalCompilationPrimary"
, "NothrowTemplateParser.TemplateId"
, "NothrowTemplateParser.TemplateParameter"
, "NothrowTemplateParser.TemplateParameterList"
, "NothrowTypedefParser.Typedef"
, "NothrowTypedefParser.DefiningTypedefId"
, "NothrowTypeExprParser.TypeExpr"
, "NothrowTypeExprParser.PrefixTypeExpr"
, "NothrowTypeExprParser.PostfixTypeExpr"
, "NothrowTypeExprParser.PrimaryTypeExpr"
};

std::vector<const char*>* GetRuleNameVecPtr()
{
    return &rules;
}
