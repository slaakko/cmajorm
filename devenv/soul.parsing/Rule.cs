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
    public class AttrOrVariable
    {
        public AttrOrVariable(string typeName, string name)
        {
            this.typeName = typeName;
            this.name = name;
        }
        public string TypeName
        {
            get { return typeName; }
        }
        public string Name
        {
            get { return name; }
        }
        private string typeName;
        private string name;
    }

    public class RuleParser : Parser, INamedEntity
    {
        public RuleParser(string name, string fullName, bool external)
            : base("<" + name + ">")
        {
            this.name = name;
            this.fullName = fullName;
            this.external = external;
            definition = null;
            inheritedAttributes = new List<AttrOrVariable>();
            valueTypeName = null;
            localVariables = new List<AttrOrVariable>();
            actions = new List<ActionParser>();
            actionScope = new Scope<ActionParser>(name + ".actions", "Action");
            nonTerminals = new List<NonTerminalParser>();
            nonTerminalScope = new Scope<NonTerminalParser>(name + ".nonterminals", "NonTerminal");
        }
        public RuleParser(string name, string fullName, Parser definition)
            : this(name, fullName, false)
        {
            this.definition = definition;
        }
        public string Name
        {
            get { return name; }
        }
        public string FullName
        {
            get { return fullName; }
        }
        public bool IsExternal
        {
            get { return external; }
        }
        public Parser Definition
        {
            get { return definition; }
            set { definition = value; }
        }
        public virtual List<AttrOrVariable> InheritedAttributes
        {
            get { return inheritedAttributes; }
        }
        public int NumberOfParameters
        {
            get { return InheritedAttributes != null ? InheritedAttributes.Count : 0; }
        }
        public virtual string ValueTypeName
        {
            get { return valueTypeName; }
            set { valueTypeName = value; }
        }
        public virtual List<AttrOrVariable> LocalVariables
        {
            get { return localVariables; }
        }
        public void AddAction(ActionParser action)
        {
            actionScope.Add(action);
            actions.Add(action);
        }
        public ActionParser GetAction(string actionName)
        {
            return actionScope.Get(actionName);
        }
        public List<ActionParser> Actions
        {
            get { return actions; }
        }
        public void AddNonTerminal(NonTerminalParser nonTerminal)
        {
            nonTerminalScope.Add(nonTerminal);
            nonTerminals.Add(nonTerminal);
        }
        public NonTerminalParser GetNonTerminal(string nonTerminalName)
        {
            return nonTerminalScope.Get(nonTerminalName);
        }
        public List<NonTerminalParser> NonTerminals
        {
            get { return nonTerminals; }
        }
        public bool TakesOrReturnsValuesOrHasLocals
        {
            get { return InheritedAttributes.Count > 0 || LocalVariables.Count > 0 || !string.IsNullOrEmpty(ValueTypeName); }
        }
        public bool Specialized
        {
            get { return TakesOrReturnsValuesOrHasLocals || !actionScope.IsEmpty; }
        }
        public string SpecializedTypeName
        {
            get { return Specialized ? name + "RuleParser" : "RuleParser"; }
        }
        public virtual void Link()
        {
        }
        public virtual void ExpandCode()
        {
            Dictionary<string, string> expandingDictionary = new Dictionary<string, string>();
            expandingDictionary = new Dictionary<string, string>();
            expandingDictionary.Add("value", "context.value");
            foreach (NonTerminalParser parser in nonTerminals)
            {
                expandingDictionary.Add(parser.Name, "context." + parser.ValueFieldName);
            }
            foreach (AttrOrVariable attr in inheritedAttributes)
            {
                expandingDictionary.Add(attr.Name, "context." + attr.Name);
            }
            foreach (AttrOrVariable variable in localVariables)
            {
                expandingDictionary.Add(variable.Name, "context." + variable.Name);
            }
            CodeExpandingVisitor visitor = new CodeExpandingVisitor(expandingDictionary);
            foreach (ActionParser action in actions)
            {
                action.SuccessCode.Accept(visitor);
                if (action.FailCode != null)
                {
                    action.FailCode.Accept(visitor);
                }
            }
            foreach (NonTerminalParser parser in nonTerminals)
            {
                if (parser.Arguments != null)
                {
                    foreach (CSharpEntity argument in parser.Arguments)
                    {
                        argument.Accept(visitor);
                    }
                }
            }
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            bool writeToLog = !scanner.Skipping && scanner.Log != null;
            if (writeToLog)
            {
                scanner.Log.WriteBeginRule(name);
                scanner.Log.IncIndent();
                scanner.Log.WriteTry(scanner.RestOfLine);
                scanner.Log.IncIndent();
            }
            int startIndex = scanner.Position.Index;
            Enter(stack);
            Match match = definition != null ? definition.Parse(scanner, stack) : Match.Nothing();
            Leave(stack, match.Hit);
            if (writeToLog)
            {
                scanner.Log.DecIndent();
                if (match.Hit)
                {
                    scanner.Log.WriteSuccess(scanner.Match(startIndex));
                }
                else
                {
                    scanner.Log.WriteFail();
                }
                scanner.Log.DecIndent();
                scanner.Log.WriteEndRule(name);
            }
            return match;
        }
        public virtual void Enter(Stack<object> stack) { }
        public virtual void Leave(Stack<object> stack, bool matched) { }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            if (definition != null)
            {
                definition.Accept(visitor);
            }
            visitor.EndVisit(this);
        }
        private string name;
        private string fullName;
        private bool external;
        private Parser definition;
        private List<AttrOrVariable> inheritedAttributes;
        private string valueTypeName;
        private List<AttrOrVariable> localVariables;
        private List<ActionParser> actions;
        private Scope<ActionParser> actionScope;
        private List<NonTerminalParser> nonTerminals;
        private Scope<NonTerminalParser> nonTerminalScope;
    }
}
