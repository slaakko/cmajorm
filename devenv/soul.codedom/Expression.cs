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
using master.util.text;

namespace soul.codedom
{
    public abstract class Expression : CSharpEntity
    {
        public Expression(string name)
            : base(name)
        {
        }
    }

    public abstract class UnaryExpression : Expression
    {
        public UnaryExpression(string name, CSharpEntity child)
            : base(name)
        {
            this.child = child;
        }
        public CSharpEntity Child
        {
            get { return child; }
        }
        public override void Accept(Visitor visitor)
        {
            child.Accept(visitor);
        }
        private CSharpEntity child;
    }

    public abstract class BinaryExpression : Expression
    {
        public BinaryExpression(string name, CSharpEntity left, CSharpEntity right)
            : base(name)
        {
            this.left = left;
            this.right = right;
        }
        public CSharpEntity Left
        {
            get { return left; }
        }
        public CSharpEntity Right
        {
            get { return right; }
        }
        public override void Accept(Visitor visitor)
        {
            left.Accept(visitor);
            right.Accept(visitor);
        }
        private CSharpEntity left;
        private CSharpEntity right;
    }

    public class AssignmentExpression : BinaryExpression
    {
        public AssignmentExpression(CSharpEntity left, Operator op, CSharpEntity right)
            : base(OperatorMap.Str(op), left, right)
        {
            this.op = op;
        }
        public Operator Op
        {
            get { return op; }
        }
        public override int Rank { get { return 0; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            Left.Print(formatter, flags);
            formatter.Write(" ");
            formatter.Write(Name);
            formatter.Write(" ");
            Right.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
        private Operator op;
    }

    public class ConditionalExpression : Expression
    {
        public ConditionalExpression(CSharpEntity condition, CSharpEntity thenExpr, CSharpEntity elseExpr)
            : base("conditional")
        {
            this.condition = condition;
            this.thenExpr = thenExpr;
            this.elseExpr = elseExpr;
        }
        public CSharpEntity Condition
        {
            get { return condition; }
        }
        public CSharpEntity ThenExpr
        {
            get { return thenExpr; }
        }
        public CSharpEntity ElseExpr
        {
            get { return elseExpr; }
        }
        public override int Rank { get { return 1; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (condition.Rank < Rank)
            {
                formatter.Write("(");
            }
            condition.Print(formatter, flags);
            if (condition.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" ? ");
            if (thenExpr.Rank < Rank)
            {
                formatter.Write("(");
            }
            thenExpr.Print(formatter, flags);
            if (thenExpr.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" : ");
            if (elseExpr.Rank < Rank)
            {
                formatter.Write("(");
            }
            elseExpr.Print(formatter, flags);
            if (elseExpr.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity condition;
        private CSharpEntity thenExpr;
        private CSharpEntity elseExpr;
    }

    public class NullCoalescingExpression : BinaryExpression
    {
        public NullCoalescingExpression(CSharpEntity left, CSharpEntity right)
            : base("??", left, right)
        {
        }
        public override int Rank { get { return 2; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" ?? ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class ConditionalOrExpression : BinaryExpression
    {
        public ConditionalOrExpression(CSharpEntity left, CSharpEntity right)
            : base("||", left, right)
        {
        }
        public override int Rank { get { return 3; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" || ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class ConditionalAndExpression : BinaryExpression
    {
        public ConditionalAndExpression(CSharpEntity left, CSharpEntity right)
            : base("&&", left, right)
        {
        }
        public override int Rank { get { return 4; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" && ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class InclusiveOrExpression : BinaryExpression
    {
        public InclusiveOrExpression(CSharpEntity left, CSharpEntity right)
            : base("|", left, right)
        {
        }
        public override int Rank { get { return 5; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" | ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class ExclusiveOrExpression : BinaryExpression
    {
        public ExclusiveOrExpression(CSharpEntity left, CSharpEntity right)
            : base("^", left, right)
        {
        }
        public override int Rank { get { return 6; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" ^ ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class AndExpression : BinaryExpression
    {
        public AndExpression(CSharpEntity left, CSharpEntity right)
            : base("&", left, right)
        {
        }
        public override int Rank { get { return 7; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" & ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class EqualityExpression : BinaryExpression
    {
        public EqualityExpression(CSharpEntity left, CSharpEntity right, Operator op)
            : base(OperatorMap.Str(op), left, right)
        {
            this.op = op;
        }
        public Operator Op
        {
            get { return op; }
        }
        public override int Rank { get { return 8; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" ");
            formatter.Write(Name);
            formatter.Write(" ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
        Operator op;
    }

    public class RelationalExpression : BinaryExpression
    {
        public RelationalExpression(CSharpEntity left, CSharpEntity right, Operator op)
            : base(OperatorMap.Str(op), left, right)
        {
            this.op = op;
        }
        public Operator Op
        {
            get { return op; }
        }
        public override int Rank { get { return 9; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" ");
            formatter.Write(Name);
            formatter.Write(" ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
        Operator op;
    }

    public class IsExpression : BinaryExpression
    {
        public IsExpression(CSharpEntity left, CSharpEntity right)
            : base("is", left, right)
        {
        }
        public override int Rank { get { return 9; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" is ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class AsExpression : BinaryExpression
    {
        public AsExpression(CSharpEntity left, CSharpEntity right)
            : base("as", left, right)
        {
        }
        public override int Rank { get { return 9; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" as ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class ShiftExpression : BinaryExpression
    {
        public ShiftExpression(CSharpEntity left, CSharpEntity right, Operator op)
            : base(OperatorMap.Str(op), left, right)
        {
            this.op = op;
        }
        public Operator Op
        {
            get { return op; }
        }
        public override int Rank { get { return 10; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" ");
            formatter.Write(Name);
            formatter.Write(" ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
        private Operator op;
    }

    public class AdditiveExpression : BinaryExpression
    {
        public AdditiveExpression(CSharpEntity left, CSharpEntity right, Operator op)
            : base(OperatorMap.Str(op), left, right)
        {
            this.op = op;
        }
        public Operator Op
        {
            get { return op; }
        }
        public override int Rank { get { return 11; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" ");
            formatter.Write(Name);
            formatter.Write(" ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
        private Operator op;
    }

    public class MultiplicativeExpression : BinaryExpression
    {
        public MultiplicativeExpression(CSharpEntity left, CSharpEntity right, Operator op)
            : base(OperatorMap.Str(op), left, right)
        {
            this.op = op;
        }
        public Operator Op
        {
            get { return op; }
        }
        public override int Rank { get { return 12; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Left.Rank < Rank)
            {
                formatter.Write("(");
            }
            Left.Print(formatter, flags);
            if (Left.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(" ");
            formatter.Write(Name);
            formatter.Write(" ");
            if (Right.Rank < Rank)
            {
                formatter.Write("(");
            }
            Right.Print(formatter, flags);
            if (Right.Rank < Rank)
            {
                formatter.Write(")");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
        private Operator op;
    }

    public class PreIncrementExpression : UnaryExpression
    {
        public PreIncrementExpression(CSharpEntity child)
            : base("++", child)
        {
        }
        public override int Rank { get { return 13; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("++");
            Child.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class PreDecrementExpression : UnaryExpression
    {
        public PreDecrementExpression(CSharpEntity child)
            : base("--", child)
        {
        }
        public override int Rank { get { return 13; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("--");
            Child.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class UnaryPlusExpression : UnaryExpression
    {
        public UnaryPlusExpression(CSharpEntity child)
            : base("+", child)
        {
        }
        public override int Rank { get { return 13; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("+");
            Child.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class UnaryMinusExpression : UnaryExpression
    {
        public UnaryMinusExpression(CSharpEntity child)
            : base("-", child)
        {
        }
        public override int Rank { get { return 13; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("-");
            Child.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class NotExpression : UnaryExpression
    {
        public NotExpression(CSharpEntity child)
            : base("!", child)
        {
        }
        public override int Rank { get { return 13; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("!");
            Child.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class NegateExpression : UnaryExpression
    {
        public NegateExpression(CSharpEntity child)
            : base("~", child)
        {
        }
        public override int Rank { get { return 13; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("~");
            Child.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class CastExpression : UnaryExpression
    {
        public CastExpression(CSharpEntity child, CSharpEntity type)
            : base("cast", child)
        {
            this.type = type;
        }
        public CSharpEntity Type
        {
            get { return type; }
        }
        public override int Rank { get { return 13; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("(");
            type.Print(formatter, flags);
            formatter.Write(")");
            Child.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
        private CSharpEntity type;
    }

    public abstract class PostfixExpression : UnaryExpression
    {
        public PostfixExpression(string name, CSharpEntity child)
            : base(name, child)
        {
        }
    }

    public class PostIncrementExpression : PostfixExpression
    {
        public PostIncrementExpression(CSharpEntity child)
            : base("post++", child)
        {
        }
        public override int Rank { get { return 14; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Child.Rank < Rank)
            {
                formatter.Write("(");
            }
            Child.Print(formatter, flags);
            if (Child.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write("++");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class PostDecrementExpression : PostfixExpression
    {
        public PostDecrementExpression(CSharpEntity child)
            : base("post--", child)
        {
        }
        public override int Rank { get { return 14; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Child.Rank < Rank)
            {
                formatter.Write("(");
            }
            Child.Print(formatter, flags);
            if (Child.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write("--");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class MemberSelectExpression : PostfixExpression
    {
        public MemberSelectExpression(CSharpEntity child, string memberName, List<CSharpEntity> typeArgumentList)
            : base(".", child)
        {
            this.memberName = memberName;
            this.typeArgumentList = typeArgumentList;
        }
        public string MemberName
        {
            get { return memberName; }
        }
        public List<CSharpEntity> TypeArgumentList
        {
            get { return typeArgumentList; }
        }
        public override int Rank { get { return 14; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Child.Rank < Rank)
            {
                formatter.Write("(");
            }
            Child.Print(formatter, flags);
            if (Child.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write(".");
            formatter.Write(memberName);
            if (typeArgumentList != null)
            {
                formatter.Write("<");
                bool first = true;
                foreach (CSharpEntity typeArgument in typeArgumentList)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        formatter.Write(", ");
                    }
                    typeArgument.Print(formatter, flags);
                }
                formatter.Write(">");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
        private string memberName;
        private List<CSharpEntity> typeArgumentList;
    }

    public class InvokeExpression : PostfixExpression
    {
        public InvokeExpression(CSharpEntity child, List<CSharpEntity> arguments)
            : base("()", child)
        {
            this.arguments = arguments;
        }
        public List<CSharpEntity> Arguments
        {
            get { return arguments; }
        }
        public override int Rank { get { return 14; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Child.Rank < Rank)
            {
                formatter.Write("(");
            }
            Child.Print(formatter, flags);
            if (Child.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write("(");
            if (arguments != null)
            {
                bool first = true;
                foreach (CSharpEntity argument in arguments)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        formatter.Write(", ");
                    }
                    argument.Print(formatter, flags);
                }
            }
            formatter.Write(")");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
        private List<CSharpEntity> arguments;
    }

    public class IndexExpression : PostfixExpression
    {
        public IndexExpression(CSharpEntity child, CSharpEntity indexExpressions)
            : base("[]", child)
        {
            this.indexExpressions = indexExpressions; 
        }
        public CSharpEntity IndexExpressions
        {
            get { return indexExpressions; }
        }
        public override int Rank { get { return 14; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Child.Rank < Rank)
            {
                formatter.Write("(");
            }
            Child.Print(formatter, flags);
            if (Child.Rank < Rank)
            {
                formatter.Write(")");
            }
            formatter.Write("[");
            indexExpressions.Print(formatter, flags);
            formatter.Write("]");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
        private CSharpEntity indexExpressions;
    }

    public class RefArgument : UnaryExpression
    {
        public RefArgument(CSharpEntity child)
            : base("ref", child)
        {
        }
        public override int Rank { get { return Child.Rank; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("ref ");
            Child.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class OutArgument : UnaryExpression
    {
        public OutArgument(CSharpEntity child)
            : base("out", child)
        {
        }
        public override int Rank { get { return Child.Rank; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("out ");
            Child.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            base.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class ThisAccess : Expression
    {
        public ThisAccess()
            : base("this")
        {
        }
        public override int Rank { get { return 15; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("this");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class BaseAccess : Expression
    {
        public BaseAccess(string baseMember)
            : base("base")
        {
            this.baseMember = baseMember;
        }
        public string BaseMember
        {
            get { return baseMember; }
        }
        public override int Rank { get { return 15; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("base." + baseMember);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string baseMember;
    }

    public class BaseIndexAccess : Expression
    {
        public BaseIndexAccess(ExpressionList indexExpressions)
            : base("base[]")
        {
            this.indexExpressions = indexExpressions;
        }
        public ExpressionList IndexExpressions
        {
            get { return indexExpressions; }
        }
        public override int Rank { get { return 15; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("base[");
            indexExpressions.Print(formatter, flags);
            formatter.Write("]");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private ExpressionList indexExpressions;
    }

    public class ObjectCreationExpression : Expression
    {
        public ObjectCreationExpression(CSharpEntity type, List<CSharpEntity> arguments, CSharpEntity initializer)
            : base("new(object)")
        {
            this.type = type;
            this.arguments = arguments;
            this.initializer = initializer;
        }
        public CSharpEntity Type
        {
            get { return type; }
        }
        public List<CSharpEntity> Arguments
        {
            get { return arguments; }
        }
        public CSharpEntity Initializer
        {
            get { return initializer; }
        }
        public override int Rank { get { return 15; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("new ");
            type.Print(formatter, flags);
            formatter.Write("(");
            if (arguments != null)
            {
                bool first = true;
                foreach (CSharpEntity argument in arguments)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        formatter.Write(", ");
                    }
                    argument.Print(formatter, flags);
                }
            }
            formatter.Write(")");
            if (initializer != null)
            {
                formatter.Write("{ ");
                initializer.Print(formatter, flags);
                formatter.Write(" }");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity type;
        private List<CSharpEntity> arguments;
        private CSharpEntity initializer;
    }

    public class ExpressionList : Expression
    {
        public ExpressionList()
            : base("expressionList")
        {
            expressions = new List<CSharpEntity>();
        }
        public ExpressionList(List<CSharpEntity> expressions)
            : base("expressionList")
        {
            this.expressions = expressions;
        }
        public List<CSharpEntity> Expressions
        {
            get { return expressions; }
        }
        public void Add(CSharpEntity expr)
        {
            expressions.Add(expr);
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            bool first = true;
            foreach (CSharpEntity expression in expressions)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    formatter.Write(", ");
                }
                expression.Print(formatter, flags);
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private List<CSharpEntity> expressions;
    }

    public class MemberInitializerList : CSharpEntity
    {
        public MemberInitializerList()
            : base("memberInitializerList")
        {
            list = new List<MemberInitializer>();
        }
        public void Add(MemberInitializer initializer)
        {
            list.Add(initializer);
        }
        public List<MemberInitializer> Initializers
        {
            get { return list; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            bool first = true;
            foreach (MemberInitializer initializer in list)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    formatter.Write(", ");
                }
                initializer.Print(formatter, flags);
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private List<MemberInitializer> list;
    }

    public class MemberInitializer : CSharpEntity
    {
        public MemberInitializer(string memberName, CSharpEntity memberValue)
            : base("memberInitializer")
        {
            this.memberName = memberName;
            this.memberValue = memberValue;
        }
        public string MemberName
        {
            get { return memberName; }
        }
        public CSharpEntity MemberValue
        {
            get { return memberValue; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write(memberName);
            formatter.Write(" = ");
            memberValue.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string memberName;
        private CSharpEntity memberValue;
    }

    public class ArrayCreationExpression : Expression
    {
        public ArrayCreationExpression(CSharpEntity baseType, CSharpEntity expressionList, List<int> rankSpecifiers, CSharpEntity arrayInitializer)
            : base("new(array)")
        {
            this.baseType = baseType;
            this.expressionList = expressionList;
            this.rankSpecifiers = rankSpecifiers;
            this.arrayInitializer = arrayInitializer;
        }
        public CSharpEntity BaseType
        {
            get { return baseType; }
        }
        public CSharpEntity ExpressionList
        {
            get { return expressionList; }
        }
        public List<int> RankSpecifiers
        {
            get { return rankSpecifiers; }
        }
        public CSharpEntity Initializer
        {
            get { return arrayInitializer; }
        }
        public override int Rank { get { return 15; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("new ");
            if (baseType != null)
            {
                baseType.Print(formatter, flags);
            }
            if (expressionList != null)
            {
                formatter.Write("[");
                expressionList.Print(formatter, flags);
                formatter.Write("]");
            }
            if (rankSpecifiers != null)
            {
                foreach (int rankSpec in rankSpecifiers)
                {
                    formatter.Write("[");
                    if (rankSpec != 0)
                    {
                        formatter.Write(new string(',', rankSpec));                    
                    }
                    formatter.Write("]");
                }
            }
            if (arrayInitializer != null)
            {
                formatter.Write("{ ");
                arrayInitializer.Print(formatter, flags);
                formatter.Write(" }");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity baseType;
        private CSharpEntity expressionList;
        private List<int> rankSpecifiers;
        private CSharpEntity arrayInitializer;
    }

    public class TypeofExpression : UnaryExpression
    {
        public TypeofExpression(CSharpEntity child)
            : base("typeof", child)
        {
        }
        public override int Rank { get { return 15; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("typeof(");
            Child.Print(formatter, flags);
            formatter.Write(")");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class CheckedExpression : UnaryExpression
    {
        public CheckedExpression(CSharpEntity child)
            : base("checked", child)
        {
        }
        public override int Rank { get { return 15; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("checked(");
            Child.Print(formatter, flags);
            formatter.Write(")");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class UncheckedExpression : UnaryExpression
    {
        public UncheckedExpression(CSharpEntity child)
            : base("unchecked", child)
        {
        }
        public override int Rank { get { return 15; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("unchecked(");
            Child.Print(formatter, flags);
            formatter.Write(")");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class DefaultExpression : UnaryExpression
    {
        public DefaultExpression(CSharpEntity child)
            : base("default", child)
        {
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("default(");
            Child.Print(formatter, flags);
            formatter.Write(")");
        }
        public override int Rank { get { return 15; } }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }
}
