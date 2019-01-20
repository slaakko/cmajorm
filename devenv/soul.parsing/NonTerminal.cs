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
    public delegate void PreCall(Stack<object> stack);
    public delegate void PostCall(Stack<object> stack, bool matched);

    public class NonTerminalParser : Parser, INamedEntity
    {
        public NonTerminalParser(string name, string fullName, string ruleName, int numberOfArguments)
            : base("<" + fullName + ">")
        {
            this.name = name;
            this.fullName = fullName;
            this.ruleName = ruleName;
            this.preCall = null;
            this.postCall = null;
            this.numberOfArguments = numberOfArguments;
        }
        public NonTerminalParser(string name, string fullName, string ruleName, List<CSharpEntity> arguments)
            : base("<" + fullName + ">")
        {
            this.name = name;
            this.fullName = fullName;
            this.ruleName = ruleName;
            this.arguments = arguments;
            this.preCall = null;
            this.postCall = null;
            this.numberOfArguments = arguments != null ? arguments.Count : 0;
        }
        public string Name
        {
            get { return name; }
        }
        public string FullName
        {
            get { return fullName; }
        }
        public string RuleName
        {
            get { return ruleName; }
        }
        public bool Specialized
        {
            get { return numberOfArguments > 0 || name != fullName || rule.TakesOrReturnsValuesOrHasLocals; }
        }
        public int NumberOfArguments
        {
            get { return numberOfArguments; }
        }
        public List<CSharpEntity> Arguments
        {
            get { return arguments; }
            set { arguments = value; }
        }
        public Position Position
        {
            get { return position; }
            set { position = value; }
        }
        public RuleParser Rule
        {
            get { return rule; }
            set { rule = value; }
        }
        public PreCall PreCall
        {
            get { return preCall; }
            set { preCall = value; }
        }
        public PostCall PostCall
        {
            get { return postCall; }
            set { postCall = value; }
        }
        public string PreCallMethodName
        {
            get { return "Pre" + name; }
        }
        public string PostCallMethodName
        {
            get { return "Post" + name; }
        }
        public string ValueFieldName
        {
            get { return "from" + name; }
        }
        public string VariableName
        {
            get { return (name.Length > 0 ? char.ToLower(name[0]) + name.Substring(1) : name) + "NonTerminalParser"; }
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            if (rule != null)
            {
                if (preCall != null)
                {
                    preCall(stack);
                }
                Match match = rule.Parse(scanner, stack);
                if (postCall != null)
                {
                    postCall(stack, match.Hit);
                }
                return match;
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string name;
        private string fullName;
        private string ruleName;
        private RuleParser rule;
        private int numberOfArguments;
        private List<CSharpEntity> arguments;
        private PreCall preCall;
        private PostCall postCall;
        private Position position;
    }
}
