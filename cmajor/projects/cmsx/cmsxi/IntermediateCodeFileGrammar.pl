namespace cmsx.intermediate
{
    grammar IntermediateCodeFileGrammar
    {
        IntermediateCodeFile(cmsx.intermediate.Context* context);
        CompileUnitHeader(cmsx.intermediate.Context* context, var string compileUnitId, var MDStructRef* mdRef);
        TypeDeclarations(cmsx.intermediate.Context* context);
        DataDefinitions(cmsx.intermediate.Context* context);
        Functions(cmsx.intermediate.Context* context);
        Function(cmsx.intermediate.Context* context);
        FunctionHeader(cmsx.intermediate.Context* context, var bool once) : Function*;
        BasicBlock(cmsx.intermediate.Context* context, Function* function, var BasicBlock* bb);
        EmptyLine;
        InstructionLine(cmsx.intermediate.Context* context, BasicBlock* bb);
        Instruction(cmsx.intermediate.Context* context) : Instruction*;
        StoreInstruction(cmsx.intermediate.Context* context) : Instruction*;
        ArgInstruction(cmsx.intermediate.Context* context) : Instruction*;
        JumpInstruction(cmsx.intermediate.Context* context) : Instruction*;
        BranchInstruction(cmsx.intermediate.Context* context) : Instruction*;
        ProcedureCallInstruction(cmsx.intermediate.Context* context) : Instruction*;
        RetInstruction(cmsx.intermediate.Context* context) : Instruction*;
        SwitchInstruction(cmsx.intermediate.Context* context) : SwitchInstruction*;
        ValueInstruction(cmsx.intermediate.Context* context) : Instruction*;
        Operation(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        UnaryInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        NotInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        NegInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        SignExtendInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        ZeroExtendInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        TruncateInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        BitCastInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        IntToFloatInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        FloatToIntInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        IntToPtrInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        PtrToIntInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        BinaryInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        AddInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        SubInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        MulInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        DivInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        ModInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        AndInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        OrInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        XorInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        ShlInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        ShrInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        EqualInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        LessInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        ParamInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        LocalInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        LoadInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        ElemAddrInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        PtrOffsetInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        PtrDiffInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        FunctionCallInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        TrapInstruction(cmsx.intermediate.Context* context, Value* result) : Instruction*;
        Operand(cmsx.intermediate.Context* context) : Value*;
        Label : uint;
        Value(cmsx.intermediate.Context* context, Type* type) : Value*;
        IdValue(cmsx.intermediate.Context* context, Type* type) : Value*;
        SymbolValue(cmsx.intermediate.Context* context, Type* type) : ConstantValue*;
        LiteralValue(cmsx.intermediate.Context* context, Type* type) : ConstantValue*;
        S;
        Comma;
        TypeDeclaration(cmsx.intermediate.Context* context);
        StructureType(string typeId, cmsx.intermediate.Context* context, var List<string> memberTypeIds);
        ArrayType(string typeId, cmsx.intermediate.Context* context);
        FunctionType(string typeId, cmsx.intermediate.Context* context, var List<string> paramTypeIds);
        StrTypeId(cmsx.intermediate.Context* context) : string;
        TypeExpr(cmsx.intermediate.Context* context) : Type*;
        PostfixTypeExpr(cmsx.intermediate.Context* context) : Type*;
        PrimaryTypeExpr(cmsx.intermediate.Context* context) : Type*;
        TypeId(cmsx.intermediate.Context* context) : Type*;
        PrimitiveType(cmsx.intermediate.Context* context) : Type*;
        VoidType(cmsx.intermediate.Context* context) : Type*;
        BoolType(cmsx.intermediate.Context* context) : Type*;
        SByteType(cmsx.intermediate.Context* context) : Type*;
        ByteType(cmsx.intermediate.Context* context) : Type*;
        ShortType(cmsx.intermediate.Context* context) : Type*;
        UShortType(cmsx.intermediate.Context* context) : Type*;
        IntType(cmsx.intermediate.Context* context) : Type*;
        UIntType(cmsx.intermediate.Context* context) : Type*;
        LongType(cmsx.intermediate.Context* context) : Type*;
        ULongType(cmsx.intermediate.Context* context) : Type*;
        FloatType(cmsx.intermediate.Context* context) : Type*;
        DoubleType(cmsx.intermediate.Context* context) : Type*;
        DataDefinition(cmsx.intermediate.Context* context, var bool once);
        Constant(cmsx.intermediate.Context* context) : ConstantValue*;
        BoolConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        BoolValue : bool;
        SByteConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        SByteValue : sbyte;
        ByteConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        ByteValue : byte;
        ShortConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        ShortValue : short;
        UShortConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        UShortValue : ushort;
        IntConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        IntValue : int;
        UIntConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        UIntValue : uint;
        LongConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        LongValue : long;
        ULongConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        ULongValue : ulong;
        FloatConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        FloatValue : float;
        DoubleConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        DoubleValue : double;
        ConversionConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        ClsIdConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        PtrConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        SymbolConstant(cmsx.intermediate.Context* context) : ConstantValue*;
        ArrayConstant(cmsx.intermediate.Context* context) : ArrayValue*;
        StructureConstant(cmsx.intermediate.Context* context) : StructureValue*;
        StringConstant(cmsx.intermediate.Context* context) : StringValue*;
        StringArrayConstant(cmsx.intermediate.Context* context) : StringArrayValue*;
        StringArrayPrefix : char;
        Metadata(cmsx.intermediate.Context* context);
        MDItem(cmsx.intermediate.Context* context) : MDItem*;
        MDBool(cmsx.intermediate.Context* context) : MDBool*;
        MDLong(cmsx.intermediate.Context* context) : MDLong*;
        MDString(cmsx.intermediate.Context* context) : MDString*;
        MDStructRef(cmsx.intermediate.Context* context) : MDStructRef*;
        MDStruct(cmsx.intermediate.Context* context) : MDStruct*;
        MDField(cmsx.intermediate.Context* context, MDStruct* parent);
    }
}
