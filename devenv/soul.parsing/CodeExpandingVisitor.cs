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
using soul.codedom;

namespace soul.parsing
{
    public class CodeExpandingVisitor : soul.codedom.Visitor
    {
        public CodeExpandingVisitor(Dictionary<string, string> expandingDictionary)
        {
            this.expandingDictionary = expandingDictionary;
        }
        public override void Visit(Name entity)
        {
            string expandedName;
            if (expandingDictionary.TryGetValue(entity.Name, out expandedName))
            {
                entity.Name = expandedName;
            }
            if (entity.TypeArguments != null)
            {
                foreach (CSharpEntity typeArgument in entity.TypeArguments)
                {
                    typeArgument.Accept(this);
                }
            }
        }
        public override void Visit(ConditionalExpression entity)
        {
            entity.Condition.Accept(this);
            entity.ThenExpr.Accept(this);
            entity.ElseExpr.Accept(this);
        }
        public override void BeginVisit(InvokeExpression entity)
        {
            if (entity.Arguments != null)
            {
                foreach (CSharpEntity argument in entity.Arguments)
                {
                    argument.Accept(this);
                }
            }
        }
        public override void BeginVisit(IndexExpression entity)
        {
            entity.IndexExpressions.Accept(this);
        }
        public override void Visit(ObjectCreationExpression entity)
        {
            if (entity.Arguments != null)
            {
                foreach (CSharpEntity argument in entity.Arguments)
                {
                    argument.Accept(this);
                }
            }
            if (entity.Initializer != null)
            {
                entity.Initializer.Accept(this);
            }
        }
        public override void Visit(ExpressionList entity)
        {
            foreach (CSharpEntity expression in entity.Expressions)
            {
                expression.Accept(this);
            }
        }
        public override void Visit(MemberInitializerList entity)
        {
            foreach (MemberInitializer initializer in entity.Initializers)
            {
                initializer.Accept(this);
            }
        }
        public override void Visit(MemberInitializer entity)
        {
            entity.MemberValue.Accept(this);
        }
        public override void Visit(ArrayCreationExpression entity)
        {
            if (entity.ExpressionList != null)
            {
                entity.ExpressionList.Accept(this);
            }
            if (entity.Initializer != null)
            {
                entity.Initializer.Accept(this);
            }
        }
        public override void Visit(LabeledStatement entity)
        {
            entity.Statement.Accept(this);
        }
        public override void Visit(LocalVariableDeclarator entity)
        {
            if (entity.Initializer != null)
            {
                entity.Initializer.Accept(this);
            }
        }
        public override void Visit(LocalVariableDeclarationStatement entity)
        {
            foreach (LocalVariableDeclarator declarator in entity.Declarators)
            {
                declarator.Accept(this);
            }
        }
        public override void Visit(ConstantDeclarator entity)
        {
            entity.Expression.Accept(this);
        }
        public override void Visit(LocalConstantDeclarationStatement entity)
        {
            foreach (ConstantDeclarator declarator in entity.Declarators)
            {
                declarator.Accept(this);
            }
        }
        public override void Visit(BlockStatement entity)
        {
            foreach (Statement statement in entity.Statements)
            {
                statement.Accept(this);
            }
        }
        public override void Visit(ExpressionStatement entity)
        {
            entity.Expression.Accept(this);
        }
        public override void Visit(IfStatement entity)
        {
            entity.Condition.Accept(this);
            entity.ThenStatement.Accept(this);
            if (entity.ElseStatement != null)
            {
                entity.ElseStatement.Accept(this);
            }
        }
        public override void Visit(SwitchLabel label)
        {
            if (label.CaseExpr != null)
            {
                label.CaseExpr.Accept(this);
            }
        }
        public override void Visit(SwitchSection entity)
        {
            foreach (SwitchLabel label in entity.Labels)
            {
                label.Accept(this);
            }
            foreach (Statement statement in entity.Statements)
            {
                statement.Accept(this);
            }
        }
        public override void Visit(SwitchStatement entity)
        {
            entity.Expression.Accept(this);
            foreach (SwitchSection section in entity.Sections)
            {
                section.Accept(this);
            }
        }
        public override void Visit(WhileStatement entity)
        {
            entity.Condition.Accept(this);
            entity.Statement.Accept(this);
        }
        public override void Visit(DoStatement entity)
        {
            entity.Statement.Accept(this);
            entity.Condition.Accept(this);
        }
        public override void Visit(ForStatement entity)
        {
            if (entity.Initializer != null)
            {
                entity.Initializer.Accept(this);
            }
            if (entity.Condition != null)
            {
                entity.Condition.Accept(this);
            }
            if (entity.Initializer != null)
            {
                entity.Iterator.Accept(this);
            }
            entity.Statement.Accept(this);
        }
        public override void Visit(StatementExpressionList entity)
        {
            foreach (CSharpEntity expression in entity.Expressions)
            {
                expression.Accept(this);
            }
        }
        public override void Visit(ForEachStatement entity)
        {
            entity.Expression.Accept(this);
            entity.Statement.Accept(this);
        }
        public override void Visit(GotoCaseStatement entity)
        {
            entity.CaseExpr.Accept(this);
        }
        public override void Visit(ReturnStatement entity)
        {
            if (entity.Expression != null)
            {
                entity.Expression.Accept(this);
            }
        }
        public override void Visit(ThrowStatement entity)
        {
            if (entity.Expression != null)
            {
                entity.Expression.Accept(this);
            }
        }
        public override void Visit(CatchClause entity)
        {
            entity.Block.Accept(this);
        }
        public override void Visit(FinallyClause entity)
        {
            entity.Block.Accept(this);
        }
        public override void Visit(TryStatement entity)
        {
            entity.Block.Accept(this);
            if (entity.CatchClauses != null)
            {
                foreach (CatchClause clause in entity.CatchClauses)
                {
                    clause.Accept(this);
                }
            }
            if (entity.FinallyClause != null)
            {
                entity.FinallyClause.Accept(this);
            }
        }
        public override void Visit(CheckedStatement entity)
        {
            entity.Block.Accept(this);
        }
        public override void Visit(UncheckedStatement entity)
        {
            entity.Block.Accept(this);
        }
        public override void Visit(LockStatement entity)
        {
            entity.Expression.Accept(this);
            entity.Statement.Accept(this);
        }
        public override void Visit(UsingStatement entity)
        {
            entity.ResourceAcquisition.Accept(this);
            entity.Statement.Accept(this);
        }
        public override void Visit(YieldReturnStatement entity)
        {
            entity.Expression.Accept(this);
        }
        private Dictionary<string, string> expandingDictionary;
    }
}
