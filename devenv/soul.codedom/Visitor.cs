/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace soul.codedom
{
    public class Visitor
    {
        public virtual void Visit(Name entity) { }
        public virtual void Visit(BoolObject entity) { }
        public virtual void Visit(IntegerObject entity) { }
        public virtual void Visit(DoubleObject entity) { }
        public virtual void Visit(CharObject entity) { }
        public virtual void Visit(StringObject entity) { }
        public virtual void Visit(NullObject entity) { }
        public virtual void Visit(ObjectType entity) { }
        public virtual void Visit(StringType entity) { }
        public virtual void Visit(DecimalType entity) { }
        public virtual void Visit(DoubleType entity) { }
        public virtual void Visit(FloatType entity) { }
        public virtual void Visit(SByteType entity) { }
        public virtual void Visit(ByteType entity) { }
        public virtual void Visit(ShortType entity) { }
        public virtual void Visit(UShortType entity) { }
        public virtual void Visit(IntType entity) { }
        public virtual void Visit(UIntType entity) { }
        public virtual void Visit(LongType entity) { }
        public virtual void Visit(ULongType entity) { }
        public virtual void Visit(CharType entity) { }
        public virtual void Visit(BoolType entity) { }
        public virtual void Visit(GenericType entity) { }
        public virtual void Visit(NullableType entity) { }
        public virtual void Visit(ArrayType entity) { }
        public virtual void Visit(VoidType entity) { }
        public virtual void Visit(VarType entity) { }
        public virtual void Visit(ConditionalExpression entity) { } // !
        public virtual void Visit(ConditionalAndExpression entity) { } 
        public virtual void BeginVisit(AssignmentExpression entity) { }
        public virtual void EndVisit(AssignmentExpression entity) { }
        public virtual void BeginVisit(NullCoalescingExpression entity) { }
        public virtual void EndVisit(NullCoalescingExpression entity) { }
        public virtual void BeginVisit(ConditionalOrExpression entity) { }
        public virtual void EndVisit(ConditionalOrExpression entity) { }
        public virtual void BeginVisit(ConditionalAndExpression entity) { }
        public virtual void EndVisit(ConditionalAndExpression entity) { }
        public virtual void BeginVisit(InclusiveOrExpression entity) { }
        public virtual void EndVisit(InclusiveOrExpression entity) { }
        public virtual void BeginVisit(ExclusiveOrExpression entity) { }
        public virtual void EndVisit(ExclusiveOrExpression entity) { }
        public virtual void BeginVisit(AndExpression entity) { }
        public virtual void EndVisit(AndExpression entity) { }
        public virtual void BeginVisit(EqualityExpression entity) { }
        public virtual void EndVisit(EqualityExpression entity) { }
        public virtual void BeginVisit(RelationalExpression entity) { }
        public virtual void EndVisit(RelationalExpression entity) { }
        public virtual void BeginVisit(IsExpression entity) { }
        public virtual void EndVisit(IsExpression entity) { }
        public virtual void BeginVisit(AsExpression entity) { }
        public virtual void EndVisit(AsExpression entity) { }
        public virtual void BeginVisit(ShiftExpression entity) { }
        public virtual void EndVisit(ShiftExpression entity) { }
        public virtual void BeginVisit(AdditiveExpression entity) { }
        public virtual void EndVisit(AdditiveExpression entity) { }
        public virtual void BeginVisit(MultiplicativeExpression entity) { }
        public virtual void EndVisit(MultiplicativeExpression entity) { }
        public virtual void BeginVisit(PreIncrementExpression entity) { }
        public virtual void EndVisit(PreIncrementExpression entity) { }
        public virtual void BeginVisit(PreDecrementExpression entity) { }
        public virtual void EndVisit(PreDecrementExpression entity) { }
        public virtual void BeginVisit(UnaryPlusExpression entity) { }
        public virtual void EndVisit(UnaryPlusExpression entity) { }
        public virtual void BeginVisit(UnaryMinusExpression entity) { }
        public virtual void EndVisit(UnaryMinusExpression entity) { }
        public virtual void BeginVisit(NotExpression entity) { }
        public virtual void EndVisit(NotExpression entity) { }
        public virtual void BeginVisit(NegateExpression entity) { }
        public virtual void EndVisit(NegateExpression entity) { }
        public virtual void BeginVisit(CastExpression entity) { }
        public virtual void EndVisit(CastExpression entity) { }
        public virtual void BeginVisit(PostIncrementExpression entity) { }
        public virtual void EndVisit(PostIncrementExpression entity) { }
        public virtual void BeginVisit(PostDecrementExpression entity) { }
        public virtual void EndVisit(PostDecrementExpression entity) { }
        public virtual void BeginVisit(MemberSelectExpression entity) { }
        public virtual void EndVisit(MemberSelectExpression entity) { }
        public virtual void BeginVisit(InvokeExpression entity) { } // !
        public virtual void EndVisit(InvokeExpression entity) { }
        public virtual void BeginVisit(IndexExpression entity) { }
        public virtual void EndVisit(IndexExpression entity) { }
        public virtual void BeginVisit(RefArgument entity) { }
        public virtual void EndVisit(RefArgument entity) { }
        public virtual void BeginVisit(OutArgument entity) { }
        public virtual void EndVisit(OutArgument entity) { }
        public virtual void Visit(ThisAccess entity) { }
        public virtual void Visit(BaseAccess entity) { }
        public virtual void Visit(BaseIndexAccess entity) { }
        public virtual void Visit(ObjectCreationExpression entity) { } // !
        public virtual void Visit(ExpressionList entity) { } // !
        public virtual void Visit(ArrayCreationExpression entity) { }
        public virtual void Visit(TypeofExpression entity) { }
        public virtual void Visit(CheckedExpression entity) { }
        public virtual void Visit(UncheckedExpression entity) { }
        public virtual void Visit(DefaultExpression entity) { }
        public virtual void Visit(MemberInitializerList entity) { }
        public virtual void Visit(MemberInitializer entity) { }
        public virtual void Visit(LabeledStatement entity) { } // !
        public virtual void Visit(LocalVariableDeclarator entity) { } // !
        public virtual void Visit(LocalVariableDeclarationStatement entity) { } // !
        public virtual void Visit(ConstantDeclarator entity) { } // !
        public virtual void Visit(LocalConstantDeclarationStatement entity) { } // !
        public virtual void Visit(BlockStatement entity) { } // !
        public virtual void Visit(EmptyStatement entity) { }
        public virtual void Visit(ExpressionStatement entity) { } // !
        public virtual void Visit(IfStatement entity) { } // !
        public virtual void Visit(SwitchLabel entity) { }
        public virtual void Visit(SwitchSection entity) { }
        public virtual void Visit(SwitchStatement entity) { }
        public virtual void Visit(WhileStatement entity) { } // !
        public virtual void Visit(DoStatement entity) { } // !
        public virtual void Visit(ForStatement entity) { } // !
        public virtual void Visit(StatementExpressionList entity) { }
        public virtual void Visit(ForEachStatement entity) { } // !
        public virtual void Visit(BreakStatement entity) { }
        public virtual void Visit(ContinueStatement entity) { }
        public virtual void Visit(GotoStatement entity) { }
        public virtual void Visit(GotoCaseStatement entity) { }
        public virtual void Visit(GotoDefaultStatement entity) { }
        public virtual void Visit(ReturnStatement entity) { } // !
        public virtual void Visit(ThrowStatement entity) { } // !
        public virtual void Visit(CatchClause entity) { } // !
        public virtual void Visit(FinallyClause entity) { } // !
        public virtual void Visit(TryStatement entity) { } // !
        public virtual void Visit(CheckedStatement entity) { } // !
        public virtual void Visit(UncheckedStatement entity) { } // !
        public virtual void Visit(LockStatement entity) { } // !
        public virtual void Visit(UsingStatement entity) { } // !
        public virtual void Visit(YieldReturnStatement entity) { } // !
        public virtual void Visit(YieldBreakStatement entity) { }
    }
}
