using System;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using soul.parsing;
using devcore;

namespace parser
{
    public class SourceLine : soul.parsing.Grammar
    {
        public SourceLine()
            : base("SourceLine", "Parsers.SourceLine")
        {
            Link();
        }
        public SourceLine(ParsingDomain parsingDomain)
            : base("SourceLine", "Parsers.SourceLine", parsingDomain)
        {
            Link();
        }
        public void Parse(string content, int fileIndex, string fileName, SourceFormatter formatter)
        {
            Scanner scanner = new Scanner(content, fileIndex, fileName, Skip);
            if (Log != null)
            {
                scanner.Log = new XmlLog(Log, LogLineLength);
                scanner.Log.WriteBeginRule("parse");
            }
            Stack<object> stack = new Stack<object>();
            stack.Push(formatter);
            Match match = Parse(scanner, stack);
            Position stop = scanner.Position;
            if (Log != null)
            {
                scanner.Log.WriteEndRule("parse");
            }
            if (!match.Hit || stop.Index != content.Length)
            {
                if (Start != null)
                {
                    throw new ExpectationFailure(Start.Info, content, scanner.Position, scanner.FileName);
                }
                else
                {
                    throw new ParsingException("grammar '" + Name + "' has no start rule", content, scanner.Position, scanner.FileName);
                }
            }
        }
        public override void CreateRules()
        {
            AddRule(new SourceLineRuleParser("SourceLine", "Parsers.SourceLine.SourceLine",
                new SequenceParser(
                    new SequenceParser(
                        new ActionParser("A0",
                            new EmptyParser()),
                        new KleeneStarParser(
                            new NonTerminalParser("SourceToken", "SourceToken", "SourceToken", 1))),
                    new ActionParser("A1",
                        new EmptyParser()))));
            AddRule(new SourceTokenRuleParser("SourceToken", "Parsers.SourceLine.SourceToken",
                new AlternativeParser(
                    new AlternativeParser(
                        new AlternativeParser(
                            new AlternativeParser(
                                new AlternativeParser(
                                    new AlternativeParser(
                                        new AlternativeParser(
                                            new AlternativeParser(
                                                new ActionParser("A0",
                                                    new NonTerminalParser("kw", "Keyword.kw", "Keyword", 0)),
                                                new ActionParser("A1",
                                                    new NonTerminalParser("id", "QualifiedId.id", "QualifiedId", 0))),
                                            new ActionParser("A2",
                                                new NonTerminalParser("chr", "Char.chr", "Char", 0))),
                                        new ActionParser("A3",
                                            new NonTerminalParser("str", "String.str", "String", 0))),
                                    new ActionParser("A4",
                                        new NonTerminalParser("num", "Number.num", "Number", 0))),
                                new ActionParser("A5",
                                    new NonTerminalParser("spc", "Spaces.spc", "Spaces", 0))),
                            new ActionParser("A6",
                                new NonTerminalParser("cmt", "LineComment.cmt", "LineComment", 0))),
                        new ActionParser("A7",
                            new AlternativeParser(
                                new StringParser("/*"),
                                new StringParser("*/")))),
                    new ActionParser("A8",
                        new NonTerminalParser("other", "Other.other", "Other", 0)))));
        }
        public override void GetReferencedGrammars()
        {
            ReferencedGrammars.Add(
                ParsingDomain.GetGrammar("Parsers.SourceToken", null,
                    Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)));
        }
        private class SourceLineRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public SourceFormatter formatter;
            }
            public SourceLineRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                InheritedAttributes.Add(new AttrOrVariable("SourceFormatter", "formatter"));
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                ActionParser a1ActionParser = GetAction("A1");
                a1ActionParser.Action = A1Action;
                NonTerminalParser sourceTokenNonTerminalParser = GetNonTerminal("SourceToken");
                sourceTokenNonTerminalParser.PreCall = PreSourceToken;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
                context.formatter = (SourceFormatter)stack.Pop();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.formatter.BeginFormat();
            }
            private void A1Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.formatter.EndFormat();
            }
            private void PreSourceToken(Stack<object> stack)
            {
                stack.Push(context.formatter);
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class SourceTokenRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public SourceFormatter formatter;
                public string fromkw;
                public string fromid;
                public string fromchr;
                public string fromstr;
                public string fromnum;
                public string fromspc;
                public string fromcmt;
                public string fromother;
            }
            public SourceTokenRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                InheritedAttributes.Add(new AttrOrVariable("SourceFormatter", "formatter"));
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                ActionParser a1ActionParser = GetAction("A1");
                a1ActionParser.Action = A1Action;
                ActionParser a2ActionParser = GetAction("A2");
                a2ActionParser.Action = A2Action;
                ActionParser a3ActionParser = GetAction("A3");
                a3ActionParser.Action = A3Action;
                ActionParser a4ActionParser = GetAction("A4");
                a4ActionParser.Action = A4Action;
                ActionParser a5ActionParser = GetAction("A5");
                a5ActionParser.Action = A5Action;
                ActionParser a6ActionParser = GetAction("A6");
                a6ActionParser.Action = A6Action;
                ActionParser a7ActionParser = GetAction("A7");
                a7ActionParser.Action = A7Action;
                ActionParser a8ActionParser = GetAction("A8");
                a8ActionParser.Action = A8Action;
                NonTerminalParser kwNonTerminalParser = GetNonTerminal("kw");
                kwNonTerminalParser.PostCall = Postkw;
                NonTerminalParser idNonTerminalParser = GetNonTerminal("id");
                idNonTerminalParser.PostCall = Postid;
                NonTerminalParser chrNonTerminalParser = GetNonTerminal("chr");
                chrNonTerminalParser.PostCall = Postchr;
                NonTerminalParser strNonTerminalParser = GetNonTerminal("str");
                strNonTerminalParser.PostCall = Poststr;
                NonTerminalParser numNonTerminalParser = GetNonTerminal("num");
                numNonTerminalParser.PostCall = Postnum;
                NonTerminalParser spcNonTerminalParser = GetNonTerminal("spc");
                spcNonTerminalParser.PostCall = Postspc;
                NonTerminalParser cmtNonTerminalParser = GetNonTerminal("cmt");
                cmtNonTerminalParser.PostCall = Postcmt;
                NonTerminalParser otherNonTerminalParser = GetNonTerminal("other");
                otherNonTerminalParser.PostCall = Postother;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
                context.formatter = (SourceFormatter)stack.Pop();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.formatter.Keyword(context.fromkw);
            }
            private void A1Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.formatter.Id(context.fromid);
            }
            private void A2Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.formatter.Char(context.fromchr);
            }
            private void A3Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.formatter.String(context.fromstr);
            }
            private void A4Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.formatter.Number(context.fromnum);
            }
            private void A5Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.formatter.Spaces(context.fromspc);
            }
            private void A6Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.formatter.LineComment(context.fromcmt);
            }
            private void A7Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.formatter.BlockCommentStartOrEnd();
            }
            private void A8Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.formatter.Other(context.fromother);
            }
            private void Postkw(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromkw = (string)stack.Pop();
                }
            }
            private void Postid(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromid = (string)stack.Pop();
                }
            }
            private void Postchr(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromchr = (string)stack.Pop();
                }
            }
            private void Poststr(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromstr = (string)stack.Pop();
                }
            }
            private void Postnum(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromnum = (string)stack.Pop();
                }
            }
            private void Postspc(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromspc = (string)stack.Pop();
                }
            }
            private void Postcmt(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromcmt = (string)stack.Pop();
                }
            }
            private void Postother(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromother = (string)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
    }
}
