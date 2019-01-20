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
    public delegate void ParsingAction(string match, string content, Position position, string fileName, ref bool pass);
    public delegate void FailureAction();

    public class ActionParser : UnaryParser, INamedEntity
    {
        public ActionParser(string name, Parser child)
            : this(name, null, child)
        {
        }
        public ActionParser(string name, BlockStatement sucessCode, Parser child)
            : base(child, child.Info)
        {
            this.name = name;
            this.successCode = sucessCode;
        }
        public ActionParser(string name, BlockStatement successCode, BlockStatement failCode, Parser child)
            : base(child, child.Info)
        {
            this.name = name;
            this.successCode = successCode;
            this.failCode = failCode;
        }
        public string Name
        {
            get { return name; }
            set { name = value; }
        }
        public string FullName
        {
            get { return name; }
        }
        public string MethodName
        {
            get { return name + "Action"; }
        }
        public string VariableName
        {
            get { return (name.Length > 0 ? char.ToLower(name[0]) + name.Substring(1) : name) + "ActionParser"; }
        }
        public BlockStatement SuccessCode
        {
            get { return successCode; }
        }
        public BlockStatement FailCode
        {
            get { return failCode; }
        }
        public ParsingAction Action
        {
            get { return action; }
            set { action = value; }
        }
        public FailureAction FailureAction
        {
            get { return failureAction; }
            set { failureAction = value; }
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Position position = scanner.Position;
            Match match = Child.Parse(scanner, stack);
            if (match.Hit)
            {
                if (action != null)
                {
                    bool pass = true;
                    action(scanner.Match(position.Index), scanner.Content, position, scanner.FileName, ref pass);
                    if (!pass)
                    {
                        return Match.Nothing();
                    }
                }
            }
            else if (failureAction != null)
            {
                failureAction();
            }
            return match;
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            Child.Accept(visitor);
            visitor.EndVisit(this);
        }
        private string name;
        private BlockStatement successCode;
        private BlockStatement failCode;
        private ParsingAction action;
        private FailureAction failureAction;
    }
}
