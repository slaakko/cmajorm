using System;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using soul.parsing;
using System.Globalization;
using System.Text;

namespace soul.parsing
{
    public class stdlib : soul.parsing.Grammar
    {
        public stdlib()
            : base("stdlib", "soul.parsing.stdlib")
        {
            Link();
        }
        public stdlib(ParsingDomain parsingDomain)
            : base("stdlib", "soul.parsing.stdlib", parsingDomain)
        {
            Link();
        }
        public override void CreateRules()
        {
            AddRule(new RuleParser("spaces", "soul.parsing.stdlib.spaces",
                new PositiveParser(
                    new SpaceParser())));
            AddRule(new RuleParser("newline", "soul.parsing.stdlib.newline",
                new AlternativeParser(
                    new AlternativeParser(
                        new StringParser("\r\n"),
                        new StringParser("\n")),
                    new StringParser("\r"))));
            AddRule(new RuleParser("comment", "soul.parsing.stdlib.comment",
                new AlternativeParser(
                    new NonTerminalParser("line_comment", "line_comment", "line_comment", 0),
                    new NonTerminalParser("block_comment", "block_comment", "block_comment", 0))));
            AddRule(new RuleParser("line_comment", "soul.parsing.stdlib.line_comment",
                new SequenceParser(
                    new SequenceParser(
                        new StringParser("//"),
                        new KleeneStarParser(
                            new CharSetParser("\n\r", true))),
                    new NonTerminalParser("newline", "newline", "newline", 0))));
            AddRule(new RuleParser("block_comment", "soul.parsing.stdlib.block_comment",
                new SequenceParser(
                    new SequenceParser(
                        new StringParser("/*"),
                        new KleeneStarParser(
                            new AlternativeParser(
                                new AlternativeParser(
                                    new NonTerminalParser("string", "string", "string", 0),
                                    new NonTerminalParser("char", "char", "char", 0)),
                                new DifferenceParser(
                                    new AnyCharParser(),
                                    new StringParser("*/"))))),
                    new StringParser("*/"))));
            AddRule(new RuleParser("spaces_and_comments", "soul.parsing.stdlib.spaces_and_comments",
                new PositiveParser(
                    new AlternativeParser(
                        new SpaceParser(),
                        new NonTerminalParser("comment", "comment", "comment", 0)))));
            AddRule(new RuleParser("digit_sequence", "soul.parsing.stdlib.digit_sequence",
                new TokenParser(
                    new PositiveParser(
                        new DigitParser()))));
            AddRule(new RuleParser("sign", "soul.parsing.stdlib.sign",
                new AlternativeParser(
                    new CharParser('+'),
                    new CharParser('-'))));
            AddRule(new intRuleParser("int", "soul.parsing.stdlib.int",
                new ActionParser("int",
                    new TokenParser(
                        new SequenceParser(
                            new OptionalParser(
                                new NonTerminalParser("sign", "sign", "sign", 0)),
                            new NonTerminalParser("digit_sequence", "digit_sequence", "digit_sequence", 0))))));
            AddRule(new uintRuleParser("uint", "soul.parsing.stdlib.uint",
                new ActionParser("uint",
                    new NonTerminalParser("digit_sequence", "digit_sequence", "digit_sequence", 0))));
            AddRule(new longRuleParser("long", "soul.parsing.stdlib.long",
                new ActionParser("long",
                    new TokenParser(
                        new SequenceParser(
                            new OptionalParser(
                                new NonTerminalParser("sign", "sign", "sign", 0)),
                            new NonTerminalParser("digit_sequence", "digit_sequence", "digit_sequence", 0))))));
            AddRule(new ulongRuleParser("ulong", "soul.parsing.stdlib.ulong",
                new ActionParser("ulong",
                    new NonTerminalParser("digit_sequence", "digit_sequence", "digit_sequence", 0))));
            AddRule(new hexRuleParser("hex", "soul.parsing.stdlib.hex",
                new ActionParser("hex",
                    new TokenParser(
                        new PositiveParser(
                            new HexDigitParser())))));
            AddRule(new hex_literalRuleParser("hex_literal", "soul.parsing.stdlib.hex_literal",
                new ActionParser("hex_literal",
                    new TokenParser(
                        new SequenceParser(
                            new AlternativeParser(
                                new StringParser("0x"),
                                new StringParser("0X")),
                            new ExpectationParser(
                                new NonTerminalParser("hex", "hex", "hex", 0)))))));
            AddRule(new realRuleParser("real", "soul.parsing.stdlib.real",
                new ActionParser("real",
                    new TokenParser(
                        new SequenceParser(
                            new OptionalParser(
                                new NonTerminalParser("sign", "sign", "sign", 0)),
                            new AlternativeParser(
                                new NonTerminalParser("fractional_real", "fractional_real", "fractional_real", 0),
                                new NonTerminalParser("exponent_real", "exponent_real", "exponent_real", 0)))))));
            AddRule(new urealRuleParser("ureal", "soul.parsing.stdlib.ureal",
                new ActionParser("ureal",
                    new AlternativeParser(
                        new NonTerminalParser("fractional_real", "fractional_real", "fractional_real", 0),
                        new NonTerminalParser("exponent_real", "exponent_real", "exponent_real", 0)))));
            AddRule(new RuleParser("fractional_real", "soul.parsing.stdlib.fractional_real",
                new TokenParser(
                    new SequenceParser(
                        new SequenceParser(
                            new SequenceParser(
                                new OptionalParser(
                                    new NonTerminalParser("digit_sequence", "digit_sequence", "digit_sequence", 0)),
                                new CharParser('.')),
                            new NonTerminalParser("digit_sequence", "digit_sequence", "digit_sequence", 0)),
                        new OptionalParser(
                            new NonTerminalParser("exponent_part", "exponent_part", "exponent_part", 0))))));
            AddRule(new RuleParser("exponent_real", "soul.parsing.stdlib.exponent_real",
                new TokenParser(
                    new SequenceParser(
                        new NonTerminalParser("digit_sequence", "digit_sequence", "digit_sequence", 0),
                        new NonTerminalParser("exponent_part", "exponent_part", "exponent_part", 0)))));
            AddRule(new RuleParser("exponent_part", "soul.parsing.stdlib.exponent_part",
                new TokenParser(
                    new SequenceParser(
                        new SequenceParser(
                            new CharSetParser("Ee"),
                            new OptionalParser(
                                new NonTerminalParser("sign", "sign", "sign", 0))),
                        new NonTerminalParser("digit_sequence", "digit_sequence", "digit_sequence", 0)))));
            AddRule(new numberRuleParser("number", "soul.parsing.stdlib.number",
                new AlternativeParser(
                    new AlternativeParser(
                        new ActionParser("A0",
                            new NonTerminalParser("real", "real", "real", 0)),
                        new ActionParser("A1",
                            new NonTerminalParser("u", "ulong.u", "ulong", 0))),
                    new ActionParser("A2",
                        new NonTerminalParser("l", "long.l", "long", 0)))));
            AddRule(new boolRuleParser("bool", "soul.parsing.stdlib.bool",
                new AlternativeParser(
                    new ActionParser("true",
                        new KeywordParser("true")),
                    new ActionParser("false",
                        new KeywordParser("false")))));
            AddRule(new identifierRuleParser("identifier", "soul.parsing.stdlib.identifier",
                new ActionParser("A0",
                    new TokenParser(
                        new SequenceParser(
                            new AlternativeParser(
                                new LetterParser(),
                                new CharParser('_')),
                            new KleeneStarParser(
                                new AlternativeParser(
                                    new AlternativeParser(
                                        new LetterParser(),
                                        new DigitParser()),
                                    new CharParser('_'))))))));
            AddRule(new qualified_idRuleParser("qualified_id", "soul.parsing.stdlib.qualified_id",
                new ActionParser("A0",
                    new TokenParser(
                        new SequenceParser(
                            new NonTerminalParser("id1", "identifier.id1", "identifier", 0),
                            new KleeneStarParser(
                                new SequenceParser(
                                    new CharParser('.'),
                                    new NonTerminalParser("id2", "identifier.id2", "identifier", 0))))))));
            AddRule(new escapeRuleParser("escape", "soul.parsing.stdlib.escape",
                new TokenParser(
                    new SequenceParser(
                        new CharParser('\\'),
                        new AlternativeParser(
                            new AlternativeParser(
                                new SequenceParser(
                                    new CharSetParser("Xx"),
                                    new ActionParser("hexEscape",
                                        new NonTerminalParser("hex", "hex", "hex", 0))),
                                new SequenceParser(
                                    new CharSetParser("Dd"),
                                    new ActionParser("A0",
                                        new NonTerminalParser("decEscape", "uint.decEscape", "uint", 0)))),
                            new ActionParser("charEscape",
                                new CharSetParser("DXdx", true)))))));
            AddRule(new charRuleParser("char", "soul.parsing.stdlib.char",
                new TokenParser(
                    new SequenceParser(
                        new SequenceParser(
                            new CharParser('\''),
                            new AlternativeParser(
                                new ActionParser("char",
                                    new CharSetParser("\n\r\\", true)),
                                new ActionParser("esc",
                                    new NonTerminalParser("escape", "escape", "escape", 0)))),
                        new ExpectationParser(
                            new CharParser('\''))))));
            AddRule(new stringRuleParser("string", "soul.parsing.stdlib.string",
                new ActionParser("string",
                    new TokenParser(
                        new SequenceParser(
                            new SequenceParser(
                                new ActionParser("init",
                                    new CharParser('\"')),
                                new KleeneStarParser(
                                    new AlternativeParser(
                                        new ActionParser("stringChars",
                                            new PositiveParser(
                                                new CharSetParser("\n\r\"\\", true))),
                                        new ActionParser("esc",
                                            new NonTerminalParser("escape", "escape", "escape", 0))))),
                            new ExpectationParser(
                                new CharParser('\"')))))));
            AddRule(new RuleParser("next_semicolon_or_closing_brace", "soul.parsing.stdlib.next_semicolon_or_closing_brace",
                new TokenParser(
                    new SequenceParser(
                        new KleeneStarParser(
                            new CharSetParser(";}", true)),
                        new AlternativeParser(
                            new CharParser(';'),
                            new CharParser('}'))))));
        }
        public override void GetReferencedGrammars()
        {
        }
        private class intRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public int value;
            }
            public intRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "int";
            }
            public override void Link()
            {
                ActionParser intActionParser = GetAction("int");
                intActionParser.Action = intAction;
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
            private void intAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = int.Parse(match);
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class uintRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public uint value;
            }
            public uintRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "uint";
            }
            public override void Link()
            {
                ActionParser uintActionParser = GetAction("uint");
                uintActionParser.Action = uintAction;
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
            private void uintAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = uint.Parse(match);
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class longRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public long value;
            }
            public longRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "long";
            }
            public override void Link()
            {
                ActionParser longActionParser = GetAction("long");
                longActionParser.Action = longAction;
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
            private void longAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = long.Parse(match);
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class ulongRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public ulong value;
            }
            public ulongRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "ulong";
            }
            public override void Link()
            {
                ActionParser ulongActionParser = GetAction("ulong");
                ulongActionParser.Action = ulongAction;
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
            private void ulongAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = ulong.Parse(match);
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class hexRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public uint value;
            }
            public hexRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "uint";
            }
            public override void Link()
            {
                ActionParser hexActionParser = GetAction("hex");
                hexActionParser.Action = hexAction;
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
            private void hexAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = Convert.ToUInt32(match, 16);
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class hex_literalRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public uint value;
                public uint fromhex;
            }
            public hex_literalRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "uint";
            }
            public override void Link()
            {
                ActionParser hex_literalActionParser = GetAction("hex_literal");
                hex_literalActionParser.Action = hex_literalAction;
                NonTerminalParser hexNonTerminalParser = GetNonTerminal("hex");
                hexNonTerminalParser.PostCall = Posthex;
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
            private void hex_literalAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = context.fromhex;
            }
            private void Posthex(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromhex = (uint)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class realRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public double value;
            }
            public realRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "double";
            }
            public override void Link()
            {
                ActionParser realActionParser = GetAction("real");
                realActionParser.Action = realAction;
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
            private void realAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = double.Parse(match, CultureInfo.InvariantCulture);
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class urealRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public double value;
            }
            public urealRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "double";
            }
            public override void Link()
            {
                ActionParser urealActionParser = GetAction("ureal");
                urealActionParser.Action = urealAction;
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
            private void urealAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = double.Parse(match, CultureInfo.InvariantCulture);
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class numberRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public double value;
                public double fromreal;
                public ulong fromu;
                public long froml;
            }
            public numberRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "double";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                ActionParser a1ActionParser = GetAction("A1");
                a1ActionParser.Action = A1Action;
                ActionParser a2ActionParser = GetAction("A2");
                a2ActionParser.Action = A2Action;
                NonTerminalParser realNonTerminalParser = GetNonTerminal("real");
                realNonTerminalParser.PostCall = Postreal;
                NonTerminalParser uNonTerminalParser = GetNonTerminal("u");
                uNonTerminalParser.PostCall = Postu;
                NonTerminalParser lNonTerminalParser = GetNonTerminal("l");
                lNonTerminalParser.PostCall = Postl;
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
                context.value = context.fromreal;
            }
            private void A1Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = context.fromu;
            }
            private void A2Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = context.froml;
            }
            private void Postreal(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromreal = (double)stack.Pop();
                }
            }
            private void Postu(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromu = (ulong)stack.Pop();
                }
            }
            private void Postl(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.froml = (long)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class boolRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public bool value;
            }
            public boolRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "bool";
            }
            public override void Link()
            {
                ActionParser trueActionParser = GetAction("true");
                trueActionParser.Action = trueAction;
                ActionParser falseActionParser = GetAction("false");
                falseActionParser.Action = falseAction;
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
            private void trueAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = true;
            }
            private void falseAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = false;
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class identifierRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
            }
            public identifierRuleParser(string name, string fullName, Parser definition)
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
        private class qualified_idRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public string fromid1;
                public string fromid2;
            }
            public qualified_idRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser id1NonTerminalParser = GetNonTerminal("id1");
                id1NonTerminalParser.PostCall = Postid1;
                NonTerminalParser id2NonTerminalParser = GetNonTerminal("id2");
                id2NonTerminalParser.PostCall = Postid2;
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
            private void Postid1(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromid1 = (string)stack.Pop();
                }
            }
            private void Postid2(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromid2 = (string)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class escapeRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public char value;
                public uint fromhex;
                public uint fromdecEscape;
            }
            public escapeRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "char";
            }
            public override void Link()
            {
                ActionParser hexEscapeActionParser = GetAction("hexEscape");
                hexEscapeActionParser.Action = hexEscapeAction;
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                ActionParser charEscapeActionParser = GetAction("charEscape");
                charEscapeActionParser.Action = charEscapeAction;
                NonTerminalParser hexNonTerminalParser = GetNonTerminal("hex");
                hexNonTerminalParser.PostCall = Posthex;
                NonTerminalParser decEscapeNonTerminalParser = GetNonTerminal("decEscape");
                decEscapeNonTerminalParser.PostCall = PostdecEscape;
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
            private void hexEscapeAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = (char)context.fromhex;
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = (char)context.fromdecEscape;
            }
            private void charEscapeAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                char c = match[0];
                switch (c)
                {
                    case 'a': 
                        context.value = '\a';
                        break;
                    case 'b': 
                        context.value = '\b';
                        break;
                    case 'f': 
                        context.value = '\f';
                        break;
                    case 'n': 
                        context.value = '\n';
                        break;
                    case 'r': 
                        context.value = '\r';
                        break;
                    case 't': 
                        context.value = '\t';
                        break;
                    case 'v': 
                        context.value = '\v';
                        break;
                    case '0': 
                        context.value = '\0';
                        break;
                    default: 
                        context.value = c;
                        break;
                }
            }
            private void Posthex(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromhex = (uint)stack.Pop();
                }
            }
            private void PostdecEscape(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromdecEscape = (uint)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class charRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public char value;
                public char fromescape;
            }
            public charRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "char";
            }
            public override void Link()
            {
                ActionParser charActionParser = GetAction("char");
                charActionParser.Action = charAction;
                ActionParser escActionParser = GetAction("esc");
                escActionParser.Action = escAction;
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
            private void charAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = match[0];
            }
            private void escAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = context.fromescape;
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
        private class stringRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public StringBuilder s;
                public char fromescape;
            }
            public stringRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
                LocalVariables.Add(new AttrOrVariable("StringBuilder", "s"));
            }
            public override void Link()
            {
                ActionParser stringActionParser = GetAction("string");
                stringActionParser.Action = stringAction;
                ActionParser initActionParser = GetAction("init");
                initActionParser.Action = initAction;
                ActionParser stringCharsActionParser = GetAction("stringChars");
                stringCharsActionParser.Action = stringCharsAction;
                ActionParser escActionParser = GetAction("esc");
                escActionParser.Action = escAction;
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
            private void stringAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = context.s.ToString();
            }
            private void initAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.s = new StringBuilder();
            }
            private void stringCharsAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.s.Append(match);
            }
            private void escAction(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.s.Append(context.fromescape);
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
    }
}
