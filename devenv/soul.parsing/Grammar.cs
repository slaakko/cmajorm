/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace soul.parsing
{
    public class LinkerVisitor : Visitor
    {
        public LinkerVisitor(Grammar grammar)
        {
            this.grammar = grammar;
        }
        public override void BeginVisit(RuleParser parser)
        {
            currentRule = parser;
            actionNumber = 0;
        }
        public override void EndVisit(RuleParser parser)
        {
            currentRule.Link();
        }
        public override void BeginVisit(ActionParser parser)
        {
            if (string.IsNullOrEmpty(parser.Name))
            {
                parser.Name = "A" + actionNumber.ToString();
                ++actionNumber;
            }
            currentRule.AddAction(parser);
        }
        public override void Visit(NonTerminalParser parser)
        {
            try
            {
                string ruleName = parser.RuleName;
                parser.Rule = grammar.GetRule(ruleName);
                if (parser.Specialized)
                {
                    currentRule.AddNonTerminal(parser);
                }
                if (parser.NumberOfArguments != parser.Rule.NumberOfParameters)
                {
                    throw new Exception("Rule '" + parser.RuleName + "' takes " + parser.Rule.NumberOfParameters.ToString() + " parameters (" +
                        parser.NumberOfArguments.ToString() + " arguments supplied)");
                }

            }
            catch (Exception ex)
            {
                if (parser.Position.Line != 0)
                {
                    throw new PositionException(ex.Message, parser.Position);
                }
                else
                {
                    throw ex;
                }
            }
        }
        public override void Visit(KeywordParser parser)
        {
            string continuationRuleName = parser.ContinuationRuleName;
            if (!string.IsNullOrEmpty(continuationRuleName))
            {
                parser.SetContinuationRule(grammar.GetRule(continuationRuleName));
            }
        }
        public override void Visit(KeywordListParser parser)
        {
            string selectorRuleName = parser.SelectorRuleName;
            parser.SetSelectorRule(grammar.GetRule(selectorRuleName));
        }
        public override void Visit(KeywordRuleListParser parser)
        {
            string selectorRuleName = parser.SelectorRuleName;
            parser.SetSelectorRule(grammar.GetRule(selectorRuleName));
            foreach (KeywordRule keywordRule in parser.KeywordRuleList)
            {
                parser.SetKeywordRule(keywordRule.Keyword, grammar.GetRule(keywordRule.RuleName));
            }
        }
        private Grammar grammar;
        private RuleParser currentRule;
        private int actionNumber;
    }

    public class GrammarReference
    {
        public GrammarReference(string grammarFullName, string assemblyFileName)
        {
            this.grammarFullName = grammarFullName;
            this.assemblyFileName = assemblyFileName;
        }
        public string GrammarFullName
        {
            get { return grammarFullName; }
        }
        public string AssemblyFileName
        {
            get { return assemblyFileName; }
        }
        private string grammarFullName;
        private string assemblyFileName;
    }

    public class Grammar : RuleParser
    {
        public Grammar(string name, string fullName)
            : this(name, fullName, new ParsingDomain())
        {
        }
        public Grammar(string name, string fullName, ParsingDomain parsingDomain)
            : base(name, fullName, false)
        {
            this.parsingDomain = parsingDomain;
            parsingDomain.AddGrammar(this);
            startRuleName = null;
            start = null;
            skipRuleName = null;
            skip = null;
            grammarReferences = new List<GrammarReference>();
            referencedGrammars = new List<Grammar>();
            rules = new List<RuleParser>();
            ruleScope = new Scope<RuleParser>(name + ".rules", "Rule");
            linking = false;
            linked = false;
            CreateRules();
            maxLogLineLength = 79;
        }
        public ParsingDomain ParsingDomain
        {
            get { return parsingDomain; }
        }
        public List<RuleParser> Rules
        {
            get { return rules; }
        }
        public Scope<RuleParser> RuleScope
        {
            get { return ruleScope; }
        }
        public string StartRuleName
        {
            get { return startRuleName; }
            set { startRuleName = value; }
        }
        public RuleParser Start
        {
            get { return start; }
            set { start = value; }
        }
        public string SkipRuleName
        {
            get { return skipRuleName; }
            set { skipRuleName = value; }
        }
        public RuleParser Skip
        {
            get { return skip; }
            set { skip = value; }
        }
        public string SynchronizationRuleName
        {
            get { return synchronizationRuleName; }
            set { synchronizationRuleName = value; }
        }
        public RuleParser SynchronizationRule
        {
            get { return synchronizationRule; }
            set { synchronizationRule = value; }
        }
        public TextWriter Log
        {
            get { return log; }
            set { log = value; }
        }
        public int LogLineLength
        {
            get { return maxLogLineLength; }
            set { maxLogLineLength = value; }
        }
        public List<GrammarReference> GrammarReferences
        {
            get { return grammarReferences; }
        }
        public List<Grammar> ReferencedGrammars
        {
            get { return referencedGrammars; }
        }
        public void AddGrammarReference(GrammarReference grammarReference)
        {
            grammarReferences.Add(grammarReference);
        }
        public virtual void CreateRules()
        {
        }
        public virtual void GetReferencedGrammars()
        {
        }
        public override List<AttrOrVariable> InheritedAttributes
        {
            get { return start != null ? start.InheritedAttributes : base.InheritedAttributes; }
        }
        public override string ValueTypeName
        {
            get { return start != null ? start.ValueTypeName : base.ValueTypeName; }
            set { if (start != null) start.ValueTypeName = value; else base.ValueTypeName = value; }
        }
        public override List<AttrOrVariable> LocalVariables
        {
            get { return start != null ? start.LocalVariables : base.LocalVariables; }
        }
        public void AddRule(RuleParser rule)
        {
            ruleScope.Add(rule);
            rules.Add(rule);
        }
        public RuleParser GetRule(string ruleName)
        {
            return ruleScope.Get(ruleName);
        }
        public override void Link()
        {
            if (!linked && !linking)
            {
                linking = true;
                GetReferencedGrammars();
                foreach (Grammar grammar in referencedGrammars)
                {
                    grammar.Link();
                }
                MergeRuleScopes();
                LinkerVisitor linkerVisitor = new LinkerVisitor(this);
                Accept(linkerVisitor);
                ResolveStart();
                ResolveSkip();
                ResolveSynchronizationRule();
                linking = false;
                linked = true;
            }
        }
        public override void ExpandCode()
        {
            foreach (RuleParser rule in rules)
            {
                rule.ExpandCode();
            }
        }
        private void ResolveStart()
        {
            if (string.IsNullOrEmpty(startRuleName))
            {
                foreach (RuleParser rule in rules)
                {
                    if (!rule.IsExternal)
                    {
                        start = rule;
                        return;
                    }
                }
            }
            else
            {
                start = ruleScope.Get(startRuleName);
            }
        }
        private void ResolveSkip()
        {
            if (!string.IsNullOrEmpty(skipRuleName))
            {
                skip = ruleScope.Get(skipRuleName);
            }
        }
        private void ResolveSynchronizationRule()
        {
            if (!string.IsNullOrEmpty(synchronizationRuleName))
            {
                synchronizationRule = ruleScope.Get(synchronizationRuleName);
            }
        }
        private void MergeRuleScopes()
        {
            foreach (Grammar grammar in referencedGrammars)
            {
                ruleScope.Merge(grammar.ruleScope);
            }
        }
        public void Parse(string content, int fileIndex, string fileName)
        {
            Scanner scanner = new Scanner(content, fileIndex, fileName, skip);
            if (log != null)
            {
                scanner.Log = new XmlLog(log, maxLogLineLength);
                scanner.Log.WriteBeginRule("parse");
            }
            Stack<object> stack = new Stack<object>();
            Match match = Parse(scanner, stack);
            Position stop = scanner.Position;
            if (log != null)
            {
                scanner.Log.WriteEndRule("parse");
            }
            if (!match.Hit || stop.Index != content.Length)
            {
                if (start != null)
                {
                    throw new ExpectationFailure(start.Info, content, scanner.Position, scanner.FileName);
                }
                else
                {
                    throw new ParsingException("grammar '" + Name + "' has no start rule", content, scanner.Position, scanner.FileName);
                }
            }
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            if (start != null)
            {
                if (content == null)
                {
                    if (skip != null)
                    {
                        content = new SequenceParser(
                            new SequenceParser(
                                new OptionalParser(skip),
                                start),
                            new OptionalParser(skip));
                    }
                    else
                    {
                        content = start;
                    }
                }
                return content.Parse(scanner, stack);
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            foreach (RuleParser rule in rules)
            {
                rule.Accept(visitor);
            }
            visitor.EndVisit(this);
        }
        private ParsingDomain parsingDomain;
        private string startRuleName;
        private RuleParser start;
        private string skipRuleName;
        private RuleParser skip;
        private string synchronizationRuleName;
        private RuleParser synchronizationRule;
        private List<GrammarReference> grammarReferences;
        private List<Grammar> referencedGrammars;
        private List<RuleParser> rules;
        private Scope<RuleParser> ruleScope;
        private bool linked;
        private bool linking;
        private TextWriter log;
        private int maxLogLineLength;
        private Parser content;
    }
}
