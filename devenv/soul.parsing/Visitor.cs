/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.Collections.Generic;
using System.Text;

namespace soul.parsing
{
    public class Visitor
    {
        public virtual void Visit(CharParser parser) { }
        public virtual void Visit(StringParser parser) { }
        public virtual void Visit(CharSetParser parser) { }
        public virtual void Visit(EmptyParser parser) { }
        public virtual void Visit(SpaceParser parser) { }
        public virtual void Visit(LetterParser parser) { }
        public virtual void Visit(DigitParser parser) { }
        public virtual void Visit(HexDigitParser parser) { }
        public virtual void Visit(PunctuationParser parser) { }
        public virtual void Visit(AnyCharParser parser) { }
        public virtual void Visit(KeywordParser parser) { }
        public virtual void Visit(KeywordListParser parser) { }
        public virtual void Visit(KeywordRuleListParser parser) { }
        public virtual void Visit(NonTerminalParser parser) { }
        public virtual void BeginVisit(Grammar grammar) { }
        public virtual void EndVisit(Grammar grammar) { }
        public virtual void BeginVisit(RuleParser parser) { }
        public virtual void EndVisit(RuleParser parser) { }
        public virtual void BeginVisit(OptionalParser parser) { }
        public virtual void EndVisit(OptionalParser parser) { }
        public virtual void BeginVisit(PositiveParser parser) { }
        public virtual void EndVisit(PositiveParser parser) { }
        public virtual void BeginVisit(KleeneStarParser parser) { }
        public virtual void EndVisit(KleeneStarParser parser) { }
        public virtual void BeginVisit(ActionParser parser) { }
        public virtual void EndVisit(ActionParser parser) { }
        public virtual void BeginVisit(ExpectationParser parser) { }
        public virtual void EndVisit(ExpectationParser parser) { }
        public virtual void BeginVisit(TokenParser parser) { }
        public virtual void EndVisit(TokenParser parser) { }
        public virtual void BeginVisit(SequenceParser parser) { }
        public virtual void Visit(SequenceParser parser) { }
        public virtual void EndVisit(SequenceParser parser) { }
        public virtual void BeginVisit(AlternativeParser parser) { }
        public virtual void Visit(AlternativeParser parser) { }
        public virtual void EndVisit(AlternativeParser parser) { }
        public virtual void BeginVisit(DifferenceParser parser) { }
        public virtual void Visit(DifferenceParser parser) { }
        public virtual void EndVisit(DifferenceParser parser) { }
        public virtual void BeginVisit(ExclusiveOrParser parser) { }
        public virtual void Visit(ExclusiveOrParser parser) { }
        public virtual void EndVisit(ExclusiveOrParser parser) { }
        public virtual void BeginVisit(IntersectionParser parser) { }
        public virtual void Visit(IntersectionParser parser) { }
        public virtual void EndVisit(IntersectionParser parser) { }
        public virtual void BeginVisit(ListParser parser) { }
        public virtual void Visit(ListParser parser) { }
        public virtual void EndVisit(ListParser parser) { }
    }
}
