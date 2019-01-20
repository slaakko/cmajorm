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

namespace soul.parsing
{
    public class KeywordParser : Parser
    {
        static KeywordParser()
        {
            identifierCharsRule = new RuleParser("identifier_chars", "soul.parsing.keyword.identifier_chars",
                new PositiveParser(
                    new AlternativeParser(
                        new AlternativeParser(
                            new AlternativeParser(
                                new LetterParser(),
                                new DigitParser()),
                            new CharParser('_')),
                        new CharParser('.'))));
        }
        public KeywordParser(string keyword)
            : this(keyword, null)
        {
        }
        public KeywordParser(string keyword, string continuationRuleName)
            : base("\"" + keyword + "\"")
        {
            this.keyword = keyword;
            this.continuationRuleName = continuationRuleName;
            continuationRule = identifierCharsRule;
            keywordStringParser = new StringParser(keyword);
            CreateKeywordRule();
        }
        public string Keyword
        {
            get { return keyword; }
        }
        public string ContinuationRuleName
        {
            get { return continuationRuleName; }
            set { continuationRuleName = value; }
        }
        public void SetContinuationRule(RuleParser continuationRule)
        {
            if (continuationRule != null)
            {
                this.continuationRule = continuationRule;
            }
            CreateKeywordRule();
        }
        private void CreateKeywordRule()
        {
            keywordRule = new RuleParser(keyword, "soul.parsing.keyword." + keyword,
                new DifferenceParser(
                    keywordStringParser,
                    new TokenParser(
                        new SequenceParser(
                            keywordStringParser,
                            continuationRule))));
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            return keywordRule.Parse(scanner, stack);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private static RuleParser identifierCharsRule;
        private string keyword;
        private string continuationRuleName;
        StringParser keywordStringParser;
        RuleParser keywordRule;
        RuleParser continuationRule;
    }

    public class KeywordListParser : Parser
    {
        public KeywordListParser(string selectorRuleName, IEnumerable<string> keywordList)
            : base("keyword_list")
        {
            this.selectorRuleName = selectorRuleName;
            keywords = new HashSet<string>(keywordList);
        }
        public HashSet<string> Keywords
        {
            get { return keywords; }
        }
        public string SelectorRuleName
        {
            get { return selectorRuleName; }
        }
        public void SetSelectorRule(RuleParser selectorRule)
        {
            this.selectorRule = selectorRule;
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Position save = scanner.Position;
            Match match = selectorRule.Parse(scanner, stack);
            if (match.Hit)
            {
                string keyword = scanner.Match(save.Index);
                if (keywords.Contains(keyword))
                {
                    return match;
                }
                else
                {
                    scanner.Position = save;
                    return Match.Nothing();
                }
            }
            return match;
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string selectorRuleName;
        private RuleParser selectorRule;
        private HashSet<string> keywords;
    }

    public class KeywordRule
    {
        public KeywordRule(string keyword, string ruleName)
        {
            Keyword = keyword;
            RuleName = ruleName;
        }
        public string Keyword { get; set; }
        public string RuleName { get; set; }
        public RuleParser Rule { get; set; }
    }

    public class KeywordRuleListParser : Parser
    {
        public KeywordRuleListParser(string selectorRuleName, List<KeywordRule> keywordRuleList)
            : base("keyword_rule_list")
        {
            this.keywordRuleList = keywordRuleList;
            this.selectorRuleName = selectorRuleName;
            keywordRuleMap = new Dictionary<string, KeywordRule>();
            foreach (KeywordRule keywordRule in keywordRuleList)
            {
                keywordRuleMap[keywordRule.Keyword] = keywordRule;
            }
        }
        public List<KeywordRule> KeywordRuleList
        {
            get { return keywordRuleList; }
        }
        public string SelectorRuleName
        {
            get { return selectorRuleName; }
        }
        public void SetSelectorRule(RuleParser selectorRule)
        {
            this.selectorRule = selectorRule;
        }
        public List<string> GetRuleNames()
        {
            List<string> ruleNames = new List<string>(keywordRuleMap.Keys);
            return ruleNames;
        }
        public void SetKeywordRule(string keyword, RuleParser rule)
        {
            KeywordRule keywordRule;
            if (keywordRuleMap.TryGetValue(keyword, out keywordRule))
            {
                keywordRule.Rule = rule;
            }
            else
            {
                throw new Exception("Keyword rule for keyword '" + keyword + "' not found");
            }
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Position save = scanner.Position;
            Match match = selectorRule.Parse(scanner, stack);
            if (match.Hit)
            {
                string keyword = scanner.Match(save.Index);
                KeywordRule keywordRule;
                if (keywordRuleMap.TryGetValue(keyword, out keywordRule))
                {
                    Match ruleMatch = keywordRule.Rule.Parse(scanner, stack);
                    if (ruleMatch.Hit)
                    {
                        match.Concatenate(ruleMatch);
                        return match;
                    }
                }
                scanner.Position = save;
                return Match.Nothing();
            }
            return match;
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string selectorRuleName;
        private RuleParser selectorRule;
        private List<KeywordRule> keywordRuleList;
        private Dictionary<string, KeywordRule> keywordRuleMap;
    }
}
