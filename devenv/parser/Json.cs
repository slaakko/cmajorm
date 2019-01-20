using System;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using soul.parsing;
using devcore;

namespace parser
{
    public class JsonGrammar : soul.parsing.Grammar
    {
        public JsonGrammar()
            : base("JsonGrammar", "parser.JsonGrammar")
        {
            Link();
        }
        public JsonGrammar(ParsingDomain parsingDomain)
            : base("JsonGrammar", "parser.JsonGrammar", parsingDomain)
        {
            Link();
        }
        public new JsonValue Parse(string content, int fileIndex, string fileName)
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
            return (JsonValue)stack.Pop();
        }
        public override void CreateRules()
        {
            AddRule(new ValueRuleParser("Value", "parser.JsonGrammar.Value",
                new AlternativeParser(
                    new AlternativeParser(
                        new AlternativeParser(
                            new AlternativeParser(
                                new AlternativeParser(
                                    new AlternativeParser(
                                        new ActionParser("A0",
                                            new NonTerminalParser("String", "String", "String", 0)),
                                        new ActionParser("A1",
                                            new NonTerminalParser("Number", "Number", "Number", 0))),
                                    new ActionParser("A2",
                                        new NonTerminalParser("Object", "Object", "Object", 0))),
                                new ActionParser("A3",
                                    new NonTerminalParser("Array", "Array", "Array", 0))),
                            new ActionParser("A4",
                                new KeywordParser("true"))),
                        new ActionParser("A5",
                            new KeywordParser("false"))),
                    new ActionParser("A6",
                        new KeywordParser("null")))));
            AddRule(new ObjectRuleParser("Object", "parser.JsonGrammar.Object",
                new SequenceParser(
                    new SequenceParser(
                        new ActionParser("A0",
                            new CharParser('{')),
                        new OptionalParser(
                            new ListParser(
                                new ActionParser("A1",
                                    new SequenceParser(
                                        new SequenceParser(
                                            new NonTerminalParser("name", "String.name", "String", 0),
                                            new CharParser(':')),
                                        new NonTerminalParser("val", "Value.val", "Value", 0))),
                                new CharParser(',')))),
                    new CharParser('}'))));
            AddRule(new ArrayRuleParser("Array", "parser.JsonGrammar.Array",
                new SequenceParser(
                    new SequenceParser(
                        new ActionParser("A0",
                            new CharParser('[')),
                        new OptionalParser(
                            new ListParser(
                                new ActionParser("A1",
                                    new NonTerminalParser("item", "Value.item", "Value", 0)),
                                new CharParser(',')))),
                    new CharParser(']'))));
            AddRule(new StringRuleParser("String", "parser.JsonGrammar.String",
                new TokenParser(
                    new SequenceParser(
                        new SequenceParser(
                            new ActionParser("A0",
                                new CharParser('\"')),
                            new KleeneStarParser(
                                new AlternativeParser(
                                    new AlternativeParser(
                                        new ActionParser("A1",
                                            new CharSetParser("\"\\", true)),
                                        new SequenceParser(
                                            new StringParser("\\u"),
                                            new ActionParser("A2",
                                                new SequenceParser(
                                                    new SequenceParser(
                                                        new SequenceParser(
                                                            new HexDigitParser(),
                                                            new HexDigitParser()),
                                                        new HexDigitParser()),
                                                    new HexDigitParser())))),
                                    new ActionParser("A3",
                                        new NonTerminalParser("escape", "escape", "escape", 0))))),
                        new CharParser('\"')))));
            AddRule(new NumberRuleParser("Number", "parser.JsonGrammar.Number",
                new ActionParser("A0",
                    new NonTerminalParser("number", "number", "number", 0))));
            SkipRuleName = "spaces";
        }
        public override void GetReferencedGrammars()
        {
            ReferencedGrammars.Add(
                ParsingDomain.GetGrammar("soul.parsing.stdlib", null,
                    Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)));
        }
        private class ValueRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public JsonValue value;
                public JsonString fromString;
                public JsonNumber fromNumber;
                public JsonObject fromObject;
                public JsonArray fromArray;
            }
            public ValueRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "JsonValue";
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
                NonTerminalParser stringNonTerminalParser = GetNonTerminal("String");
                stringNonTerminalParser.PostCall = PostString;
                NonTerminalParser numberNonTerminalParser = GetNonTerminal("Number");
                numberNonTerminalParser.PostCall = PostNumber;
                NonTerminalParser objectNonTerminalParser = GetNonTerminal("Object");
                objectNonTerminalParser.PostCall = PostObject;
                NonTerminalParser arrayNonTerminalParser = GetNonTerminal("Array");
                arrayNonTerminalParser.PostCall = PostArray;
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
                context.value = context.fromString;
            }
            private void A1Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = context.fromNumber;
            }
            private void A2Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = context.fromObject;
            }
            private void A3Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = context.fromArray;
            }
            private void A4Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = new JsonBool(true);
            }
            private void A5Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = new JsonBool(false);
            }
            private void A6Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = new JsonNull();
            }
            private void PostString(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromString = (JsonString)stack.Pop();
                }
            }
            private void PostNumber(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromNumber = (JsonNumber)stack.Pop();
                }
            }
            private void PostObject(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromObject = (JsonObject)stack.Pop();
                }
            }
            private void PostArray(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromArray = (JsonArray)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class ObjectRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public JsonObject value;
                public JsonString fromname;
                public JsonValue fromval;
            }
            public ObjectRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "JsonObject";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                ActionParser a1ActionParser = GetAction("A1");
                a1ActionParser.Action = A1Action;
                NonTerminalParser nameNonTerminalParser = GetNonTerminal("name");
                nameNonTerminalParser.PostCall = Postname;
                NonTerminalParser valNonTerminalParser = GetNonTerminal("val");
                valNonTerminalParser.PostCall = Postval;
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
                context.value = new JsonObject();
            }
            private void A1Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value.AddField(context.fromname.Value, context.fromval);
            }
            private void Postname(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromname = (JsonString)stack.Pop();
                }
            }
            private void Postval(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromval = (JsonValue)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class ArrayRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public JsonArray value;
                public JsonValue fromitem;
            }
            public ArrayRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "JsonArray";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                ActionParser a1ActionParser = GetAction("A1");
                a1ActionParser.Action = A1Action;
                NonTerminalParser itemNonTerminalParser = GetNonTerminal("item");
                itemNonTerminalParser.PostCall = Postitem;
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
                context.value = new JsonArray();
            }
            private void A1Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value.AddItem(context.fromitem);
            }
            private void Postitem(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromitem = (JsonValue)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class StringRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public JsonString value;
                public char fromescape;
            }
            public StringRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "JsonString";
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
                context.value = new JsonString();
            }
            private void A1Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value.Append(match[0]);
            }
            private void A2Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value.Append(char.ConvertFromUtf32(Convert.ToInt32(match, 16)));
            }
            private void A3Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value.Append(context.fromescape);
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
                public JsonNumber value;
                public double fromnumber;
            }
            public NumberRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "JsonNumber";
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
                context.value = new JsonNumber(context.fromnumber);
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
    }
}
