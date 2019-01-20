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
    public abstract class Statement : CSharpEntity
    {
        public Statement(string name)
            : base(name)
        {
        }
    }

    public class LabeledStatement : Statement
    {
        public LabeledStatement(string label, Statement statement)
            : base("labeledStatement")
        {
            this.label = label;
            this.statement = statement;
        }
        public string Label
        {
            get { return label; }
        }
        public Statement Statement
        {
            get { return statement; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (formatter.Indent > 0)
            {
                formatter.DecIndent();
                formatter.Write(label);
                formatter.WriteLine(":");
                formatter.IncIndent();
            }
            else
            {
                formatter.Write(label);
                formatter.WriteLine(":");
            }
            statement.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string label;
        private Statement statement;
    }

    public abstract class DeclarationStatement : Statement
    {
        public DeclarationStatement(string name)
            : base(name)
        {
        }
    }

    public class LocalVariableDeclarator : CSharpEntity
    {
        public LocalVariableDeclarator(string identifier, CSharpEntity initializer)
            : base("localVariableDeclarator")
        {
            this.identifier = identifier;
            this.initializer = initializer;
        }
        public string Identifer
        {
            get { return identifier; }
        }
        public CSharpEntity Initializer
        {
            get { return initializer; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write(identifier);
            if (initializer != null)
            {
                formatter.Write(" = ");
                initializer.Print(formatter, flags);
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string identifier;
        private CSharpEntity initializer;
    }

    public class LocalVariableDeclarationStatement : DeclarationStatement
    {
        public LocalVariableDeclarationStatement(CSharpEntity type, List<LocalVariableDeclarator> declarators)
            : base("localVariableDeclarationStatement")
        {
            this.type = type;
            this.declarators = declarators;
        }
        public CSharpEntity Type
        {
            get { return type; }
        }
        public List<LocalVariableDeclarator> Declarators
        {
            get { return declarators; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            type.Print(formatter, flags);
            formatter.Write(" ");
            bool first = true;
            foreach (LocalVariableDeclarator declarator in declarators)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    formatter.Write(", ");
                }
                declarator.Print(formatter, flags);
            }
            if ((flags & PrintFlags.omitNewLine) != PrintFlags.none)
            {
                formatter.Write(";");
            }
            else
            {
                formatter.WriteLine(";");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity type;
        private List<LocalVariableDeclarator> declarators;
    }

    public class ConstantDeclarator : CSharpEntity
    {
        public ConstantDeclarator(string identifier, CSharpEntity expression)
            : base("constantDeclarator")
        {
            this.identifier = identifier;
            this.expression = expression;
        }
        public string Identifier
        {
            get { return identifier; }
        }
        public CSharpEntity Expression
        {
            get { return expression; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write(identifier);
            formatter.Write(" = ");
            expression.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string identifier;
        private CSharpEntity expression;
    }

    public class LocalConstantDeclarationStatement : DeclarationStatement
    {
        public LocalConstantDeclarationStatement(CSharpEntity type, List<ConstantDeclarator> declarators)
            : base("localConstantDeclarationStatement")
        {
            this.type = type;
            this.declarators = declarators;
        }
        public CSharpEntity Type
        {
            get { return type; }
        }
        public List<ConstantDeclarator> Declarators
        {
            get { return declarators; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            type.Print(formatter, flags);
            bool first = true;
            foreach (ConstantDeclarator declarator in declarators)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    formatter.Write(", ");
                }
                declarator.Print(formatter, flags);
            }
            formatter.WriteLine(";");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity type;
        private List<ConstantDeclarator> declarators;
    }

    public class BlockStatement : Statement
    {
        public BlockStatement()
            : base("block")
        {
            statements = new List<Statement>();
        }
        public List<Statement> Statements
        {
            get { return statements; }
        }
        public void Add(Statement statement)
        {
            statements.Add(statement);
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine("{");
            formatter.IncIndent();
            foreach (Statement statement in statements)
            {
                statement.Print(formatter, flags);
            }
            formatter.DecIndent();
            formatter.WriteLine("}");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private List<Statement> statements;
    }

    public class EmptyStatement : Statement
    {
        public EmptyStatement()
            : base("empty")
        {
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine(";");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class ExpressionStatement : Statement
    {
        public ExpressionStatement(CSharpEntity expression)
            : base("expressionStatement")
        {
            this.expression = expression;
        }
        public CSharpEntity Expression
        {
            get { return expression; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            expression.Print(formatter, flags);
            formatter.WriteLine(";");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity expression;
    }

    public class IfStatement : Statement
    {
        public IfStatement(CSharpEntity condition, Statement thenStatement, Statement elseStatement)
            : base("if")
        {
            this.condition = condition;
            this.thenStatement = thenStatement;
            this.elseStatement = elseStatement;
        }
        public CSharpEntity Condition
        {
            get { return condition; }
        }
        public Statement ThenStatement
        {
            get { return thenStatement; }
        }
        public Statement ElseStatement
        {
            get { return elseStatement; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("if (");
            condition.Print(formatter, flags);
            formatter.Write(") ");
            if (thenStatement is BlockStatement)
            {
                formatter.NewLine();
            }
            thenStatement.Print(formatter, flags);
            if (elseStatement != null)
            {
                if (!(thenStatement is BlockStatement))
                {
                    formatter.Write(" ");
                }
                formatter.Write("else");
                if (elseStatement is BlockStatement)
                {
                    formatter.NewLine();
                }
                else
                {
                    formatter.Write(" ");
                }
                elseStatement.Print(formatter, flags);
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity condition;
        private Statement thenStatement;
        private Statement elseStatement;
    }

    public class SwitchLabel : CSharpEntity
    {
        public SwitchLabel()
            : base("default")
        {
        }
        public SwitchLabel(CSharpEntity caseExpr)
            : base("case")
        {
            this.caseExpr = caseExpr;
        }
        public CSharpEntity CaseExpr
        {
            get { return caseExpr; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (caseExpr != null)
            {
                formatter.Write("case ");
                caseExpr.Print(formatter, flags);
                formatter.Write(": ");
            }
            else
            {
                formatter.Write("default: ");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity caseExpr;
    }

    public class SwitchSection : CSharpEntity
    {
        public SwitchSection(List<SwitchLabel> labels, List<Statement> statements)
            : base("switchSection")
        {
            this.labels = labels;
            this.statements = statements;
        }
        public List<SwitchLabel> Labels
        {
            get { return labels; }
        }
        public List<Statement> Statements
        {
            get { return statements; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            foreach (SwitchLabel label in labels)
            {
                label.Print(formatter, flags);
                formatter.NewLine();
            }
            formatter.IncIndent();
            foreach (Statement statement in statements)
            {
                statement.Print(formatter, flags);
            }
            formatter.DecIndent();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private List<SwitchLabel> labels;
        private List<Statement> statements;
    }

    public class SwitchStatement : Statement
    {
        public SwitchStatement(CSharpEntity expression, List<SwitchSection> sections)
            : base("switch")
        {
            this.expression = expression;
            this.sections = sections;
        }
        public CSharpEntity Expression
        {
            get { return expression; }
        }
        public List<SwitchSection> Sections
        {
            get { return sections; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("switch (");
            expression.Print(formatter, flags);
            formatter.WriteLine(")");
            formatter.WriteLine("{");
            formatter.IncIndent();
            foreach (SwitchSection section in sections)
            {
                section.Print(formatter, flags);
            }
            formatter.DecIndent();
            formatter.WriteLine("}");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity expression;
        private List<SwitchSection> sections;
    }

    public class WhileStatement : Statement
    {
        public WhileStatement(CSharpEntity condition, Statement statement)
            : base("while")
        {
            this.condition = condition;
            this.statement = statement;
        }
        public CSharpEntity Condition
        {
            get { return condition; }
        }
        public Statement Statement
        {
            get { return statement; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("while (");
            condition.Print(formatter, flags);
            formatter.Write(") ");
            statement.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity condition;
        private Statement statement;
    }

    public class DoStatement : Statement
    {
        public DoStatement(Statement statement, CSharpEntity condition)
            : base("do")
        {
            this.statement = statement;
            this.condition = condition;
        }
        public Statement Statement
        {
            get { return statement; }
        }
        public CSharpEntity Condition
        {
            get { return condition; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("do ");
            statement.Print(formatter, flags);
            formatter.Write("while (");
            condition.Print(formatter, flags);
            formatter.WriteLine(");");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private Statement statement;
        private CSharpEntity condition;
    }

    public class ForStatement : Statement
    {
        public ForStatement(CSharpEntity initializer, CSharpEntity condition, CSharpEntity iterator, Statement statement)
            : base("for")
        {
            this.initializer = initializer;
            this.condition = condition;
            this.iterator = iterator;
            this.statement = statement;
        }
        public CSharpEntity Initializer
        {
            get { return initializer; }
        }
        public CSharpEntity Condition
        {
            get { return condition; }
        }
        public CSharpEntity Iterator
        {
            get { return iterator; }
        }
        public Statement Statement
        {
            get { return statement; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("for (");
            if (initializer != null)
            {
                initializer.Print(formatter, flags | PrintFlags.omitNewLine | PrintFlags.printSemicolon);
            }
            else
            {
                formatter.Write("; ");
            }
            if (condition != null)
            {
                condition.Print(formatter, flags);
            }
            formatter.Write("; ");
            if (iterator != null)
            {
                iterator.Print(formatter, flags);
            }
            if (statement is BlockStatement)
            {
                formatter.WriteLine(")");
            }
            else
            {
                formatter.Write(") ");
            }
            statement.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity initializer;
        private CSharpEntity condition;
        private CSharpEntity iterator;
        private Statement statement;
    }

    public class StatementExpressionList : CSharpEntity
    {
        public StatementExpressionList()
            : base("statementExpressionList")
        {
            expressions = new List<CSharpEntity>();
        }
        public List<CSharpEntity> Expressions
        {
            get { return expressions; }
        }
        public void Add(CSharpEntity expression)
        {
            expressions.Add(expression);
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
            if ((flags & PrintFlags.printSemicolon) != PrintFlags.none)
            {
                formatter.Write(";");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private List<CSharpEntity> expressions;
    }

    public class ForEachStatement : Statement
    {
        public ForEachStatement(CSharpEntity type, string identifier, CSharpEntity expression, Statement statement)
            : base("foreach")
        {
            this.type = type;
            this.identifier = identifier;
            this.expression = expression;
            this.statement = statement;
        }
        public CSharpEntity Type
        {
            get { return type; }
        }
        public string Identifier
        {
            get { return identifier; }
        }
        public CSharpEntity Expression
        {
            get { return expression; }
        }
        public Statement Statement
        {
            get { return statement; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("foreach (");
            type.Print(formatter, flags);
            formatter.Write(" ");
            formatter.Write(identifier);
            formatter.Write(" in ");
            expression.Print(formatter, flags);
            if (statement is BlockStatement)
            {
                formatter.WriteLine(")");
            }
            else
            {
                formatter.Write(") ");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity type;
        private string identifier;
        private CSharpEntity expression;
        private Statement statement;
    }

    public class BreakStatement : Statement
    {
        public BreakStatement()
            : base("break")
        {
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine("break;");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class ContinueStatement : Statement
    {
        public ContinueStatement()
            : base("continue")
        {
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine("continue;");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class GotoStatement : Statement
    {
        public GotoStatement(string target)
            : base("goto")
        {
            this.target = target;
        }
        public string Target
        {
            get { return target; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine("goto " + target + ";");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string target;
    }

    public class GotoCaseStatement : Statement
    {
        public GotoCaseStatement(CSharpEntity caseExpr)
            : base("goto case")
        {
            this.caseExpr = caseExpr;
        }
        public CSharpEntity CaseExpr
        {
            get { return caseExpr; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine("goto case ");
            caseExpr.Print(formatter, flags);
            formatter.WriteLine(";");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity caseExpr;
    }

    public class GotoDefaultStatement : Statement
    {
        public GotoDefaultStatement()
            : base("goto default")
        {
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine("goto default;");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class ReturnStatement : Statement
    {
        public ReturnStatement(CSharpEntity expression)
            : base("return")
        {
            this.expression = expression;
        }
        public CSharpEntity Expression
        {
            get { return expression; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("return");
            if (expression != null)
            {
                formatter.Write(" ");
                expression.Print(formatter, flags);
            }
            formatter.WriteLine(";");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity expression;
    }

    public class ThrowStatement : Statement
    {
        public ThrowStatement(CSharpEntity expression)
            : base("throw")
        {
            this.expression = expression;
        }
        public CSharpEntity Expression
        {
            get { return expression; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("throw");
            if (expression != null)
            {
                formatter.Write(" ");
                expression.Print(formatter, flags);
            }
            formatter.WriteLine(";");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity expression;
    }

    public class CatchClause : Statement
    {
        public CatchClause(CSharpEntity type, string identifier, BlockStatement block)
            : base("catch")
        {
            this.type = type;
            this.identifier = identifier;
            this.block = block;
        }
        public CSharpEntity Type
        {
            get { return type; }
        }
        public string Identifier
        {
            get { return identifier; }
        }
        public BlockStatement Block
        {
            get { return block; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("catch");
            if (type != null)
            {
                formatter.Write("(");
                type.Print(formatter, flags);
                if (identifier != null)
                {
                    formatter.Write(" ");
                    formatter.Write(identifier);
                }
                formatter.WriteLine(")");
            }
            else
            {
                formatter.NewLine();
            }
            block.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        } 
        private CSharpEntity type;
        private string identifier;
        private BlockStatement block;
    }

    public class FinallyClause : Statement
    {
        public FinallyClause(BlockStatement block)
            : base("finally")
        {
            this.block = block;
        }
        public BlockStatement Block
        {
            get { return block; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine("finally");
            block.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private BlockStatement block;
    }

    public class TryStatement : Statement
    {
        public TryStatement(BlockStatement block, List<CatchClause> catchClauses, FinallyClause finallyClause)
            : base("try")
        {
            this.block = block;
            this.catchClauses = catchClauses;
            this.finallyClause = finallyClause;
        }
        public BlockStatement Block
        {
            get { return block; }
        }
        public List<CatchClause> CatchClauses
        {
            get { return catchClauses; }
        }
        public FinallyClause FinallyClause
        {
            get { return finallyClause; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine("try");
            block.Print(formatter, flags);
            if (catchClauses != null && catchClauses.Count > 0)
            {
                foreach (CatchClause catchClause in catchClauses)
                {
                    catchClause.Print(formatter, flags);
                }
            }
            if (finallyClause != null)
            {
                finallyClause.Print(formatter, flags);
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private BlockStatement block;
        private List<CatchClause> catchClauses;
        private FinallyClause finallyClause;
    }

    public class CheckedStatement : Statement
    {
        public CheckedStatement(BlockStatement block)
            : base("checked")
        {
            this.block = block;
        }
        public BlockStatement Block
        {
            get { return block; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine("checked");
            block.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private BlockStatement block;
    }

    public class UncheckedStatement : Statement
    {
        public UncheckedStatement(BlockStatement block)
            : base("unchecked")
        {
            this.block = block;
        }
        public BlockStatement Block
        {
            get { return block; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine("unchecked");
            block.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private BlockStatement block;
    }

    public class LockStatement : Statement
    {
        public LockStatement(CSharpEntity expression, Statement statement)
            : base("lock")
        {
            this.expression = expression;
            this.statement = statement;
        }
        public CSharpEntity Expression
        {
            get { return expression; }
        }
        public Statement Statement
        {
            get { return statement; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("lock (");
            expression.Print(formatter, flags);
            if (statement is BlockStatement)
            {
                formatter.WriteLine(")");
            }
            else
            {
                formatter.Write(") ");
            }
            statement.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity expression;
        private Statement statement;
    }

    public class UsingStatement : Statement
    {
        public UsingStatement(CSharpEntity resourceAcquisition, Statement statement)
            : base("using")
        {
            this.resourceAcquisition = resourceAcquisition;
            this.statement = statement;
        }
        public CSharpEntity ResourceAcquisition
        {
            get { return resourceAcquisition; }
        }
        public Statement Statement
        {
            get { return statement; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("using (");
            resourceAcquisition.Print(formatter, flags);
            if (statement is BlockStatement)
            {
                formatter.WriteLine(")");
            }
            else
            {
                formatter.Write(") ");
            }
            statement.Print(formatter, flags);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity resourceAcquisition;
        private Statement statement;
    }

    public abstract class YieldStatement : Statement
    {
        public YieldStatement(string name)
            : base(name)
        {
        }
    }

    public class YieldReturnStatement : YieldStatement
    {
        public YieldReturnStatement(CSharpEntity expression)
            : base("yield return")
        {
            this.expression = expression;
        }
        public CSharpEntity Expression
        {
            get { return expression; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("yield return ");
            expression.Print(formatter, flags);
            formatter.WriteLine(";");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity expression;
    }

    public class YieldBreakStatement : YieldStatement
    {
        public YieldBreakStatement()
            : base("yield break")
        {
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.WriteLine("yield break;");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }
}
