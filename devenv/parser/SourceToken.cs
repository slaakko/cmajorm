using System;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using soul.parsing;

namespace parser
{
    public class SourceToken : soul.parsing.Grammar
    {
        public SourceToken()
            : base("SourceToken", "Parsers.SourceToken")
        {
            Link();
        }
        public SourceToken(ParsingDomain parsingDomain)
            : base("SourceToken", "Parsers.SourceToken", parsingDomain)
        {
            Link();
        }
        public new string Parse(string content, int fileIndex, string fileName)
        {
            Scanner scanner = new Scanner(content, fileIndex, fileName, Skip);
            if (Log != null)
            {
                scanner.Log = new XmlLog(Log, LogLineLength);
                scanner.Log.WriteBeginRule("parse");
            }
            Stack<object> stack = new Stack<object>();
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
            return (string)stack.Pop();
        }
        public override void CreateRules()
        {
            AddRule(new KeywordRuleParser("Keyword", "parser.SourceToken.Keyword",
                new ActionParser("A0",
                    new AlternativeParser(
                        new SequenceParser(
                            new SequenceParser(
                                new StringParser("#"),
                                new OptionalParser(
                                    new NonTerminalParser("Spaces", "Spaces", "Spaces", 0))),
                            new AlternativeParser(
                                new AlternativeParser(
                                    new AlternativeParser(
                                        new StringParser("assert"),
                                        new StringParser("if")),
                                    new StringParser("elif")),
                                new StringParser("endif"))),
                        new KeywordListParser("identifier", new List<string>(){
                            "bool", "true", "false", "sbyte", "byte", "short", 
                            "ushort", "int", "uint", "long", "ulong", "float", 
                            "double", "char", "wchar", "uchar", "void", "enum", 
                            "cast", "namespace", "using", "static", "extern", 
                            "interface", "is", "as", "explicit", "delegate", 
                            "inline", "constexpr", "cdecl", "nothrow", "public", 
                            "protected", "private", "internal", "virtual", 
                            "abstract", "override", "suppress", "default", 
                            "operator", "class", "return", "if", "else", 
                            "switch", "case", "while", "do", "for", "break", 
                            "continue", "goto", "typedef", "typename", "const", 
                            "null", "this", "base", "construct", "destroy", 
                            "new", "delete", "sizeof", "try", "catch", "throw", 
                            "concept", "where", "axiom", "and", "or", "not", 
                            "unit_test"})
                        ))));
            AddRule(new QualifiedIdRuleParser("QualifiedId", "parser.SourceToken.QualifiedId",
                new ActionParser("A0",
                    new SequenceParser(
                        new NonTerminalParser("Identifier", "Identifier", "Identifier", 0),
                        new KleeneStarParser(
                            new SequenceParser(
                                new CharParser('.'),
                                new NonTerminalParser("Identifier", "Identifier", "Identifier", 0)))))));
            AddRule(new RuleParser("Identifier", "parser.SourceToken.Identifier",
                new DifferenceParser(
                    new NonTerminalParser("identifier", "identifier", "identifier", 0),
                    new NonTerminalParser("Keyword", "Keyword", "Keyword", 0))));
            AddRule(new CharRuleParser("Char", "parser.SourceToken.Char",
                new ActionParser("A0",
                    new TokenParser(
                        new SequenceParser(
                            new SequenceParser(
                                new CharParser('\''),
                                new AlternativeParser(
                                    new CharSetParser("\n\r\'\\", true),
                                    new NonTerminalParser("escape", "escape", "escape", 0))),
                            new CharParser('\''))))));
            AddRule(new StringRuleParser("String", "parser.SourceToken.String",
                new ActionParser("A0",
                    new TokenParser(
                        new SequenceParser(
                            new SequenceParser(
                                new CharParser('\"'),
                                new KleeneStarParser(
                                    new AlternativeParser(
                                        new PositiveParser(
                                            new CharSetParser("\n\r\"", true)),
                                        new NonTerminalParser("escape", "escape", "escape", 0)))),
                            new CharParser('\"'))))));
            AddRule(new NumberRuleParser("Number", "parser.SourceToken.Number",
                new ActionParser("A0",
                    new NonTerminalParser("number", "number", "number", 0))));
            AddRule(new SpacesRuleParser("Spaces", "parser.SourceToken.Spaces",
                new ActionParser("A0",
                    new PositiveParser(
                        new CharSetParser("\t ")))));
            AddRule(new LineCommentRuleParser("LineComment", "parser.SourceToken.LineComment",
                new ActionParser("A0",
                    new SequenceParser(
                        new StringParser("//"),
                        new KleeneStarParser(
                            new CharSetParser("\n\r", true))))));
            AddRule(new BlockCommentRuleParser("BlockComment", "parser.SourceToken.BlockComment",
                new ActionParser("A0",
                    new SequenceParser(
                        new SequenceParser(
                            new StringParser("/*"),
                            new KleeneStarParser(
                                new AlternativeParser(
                                    new AlternativeParser(
                                        new NonTerminalParser("String", "String", "String", 0),
                                        new NonTerminalParser("Char", "Char", "Char", 0)),
                                    new DifferenceParser(
                                        new AnyCharParser(),
                                        new StringParser("*/"))))),
                        new OptionalParser(
                            new StringParser("*/"))))));
            AddRule(new OtherRuleParser("Other", "parser.SourceToken.Other",
                new ActionParser("A0",
                    new CharSetParser("\n\r", true))));
        }
        public override void GetReferencedGrammars()
        {
            ReferencedGrammars.Add(
                ParsingDomain.GetGrammar("soul.parsing.stdlib", null,
                    Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)));
        }
        private class KeywordRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public string fromSpaces;
            }
            public KeywordRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser spacesNonTerminalParser = GetNonTerminal("Spaces");
                spacesNonTerminalParser.PostCall = PostSpaces;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    stack.Push(context.value);
                }
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = match;
            }
            private void PostSpaces(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromSpaces = (string)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class QualifiedIdRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
            }
            public QualifiedIdRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    stack.Push(context.value);
                }
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = match;
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class CharRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public char fromescape;
            }
            public CharRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser escapeNonTerminalParser = GetNonTerminal("escape");
                escapeNonTerminalParser.PostCall = Postescape;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    stack.Push(context.value);
                }
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = match;
            }
            private void Postescape(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromescape = (char)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class StringRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public char fromescape;
            }
            public StringRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser escapeNonTerminalParser = GetNonTerminal("escape");
                escapeNonTerminalParser.PostCall = Postescape;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    stack.Push(context.value);
                }
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = match;
            }
            private void Postescape(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromescape = (char)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class NumberRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public double fromnumber;
            }
            public NumberRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser numberNonTerminalParser = GetNonTerminal("number");
                numberNonTerminalParser.PostCall = Postnumber;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    stack.Push(context.value);
                }
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = match;
            }
            private void Postnumber(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromnumber = (double)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class SpacesRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
            }
            public SpacesRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    stack.Push(context.value);
                }
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = match;
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class LineCommentRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
            }
            public LineCommentRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    stack.Push(context.value);
                }
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = match;
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class BlockCommentRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public string fromString;
                public string fromChar;
            }
            public BlockCommentRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser stringNonTerminalParser = GetNonTerminal("String");
                stringNonTerminalParser.PostCall = PostString;
                NonTerminalParser charNonTerminalParser = GetNonTerminal("Char");
                charNonTerminalParser.PostCall = PostChar;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    stack.Push(context.value);
                }
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = match;
            }
            private void PostString(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromString = (string)stack.Pop();
                }
            }
            private void PostChar(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromChar = (string)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class OtherRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
            }
            public OtherRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    stack.Push(context.value);
                }
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = match;
            }
            private Context context;
            private Stack<Context> contextStack;
        }
    }
}
