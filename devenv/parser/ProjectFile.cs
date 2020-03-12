using System;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using soul.parsing;
using devcore;

namespace parser
{
    public class ProjectFile : soul.parsing.Grammar
    {
        public ProjectFile()
            : base("ProjectFile", "parser.ProjectFile")
        {
            Link();
        }
        public ProjectFile(ParsingDomain parsingDomain)
            : base("ProjectFile", "parser.ProjectFile", parsingDomain)
        {
            Link();
        }
        public new Project Parse(string content, int fileIndex, string fileName)
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
            return (Project)stack.Pop();
        }
        public override void CreateRules()
        {
            AddRule(new ProjectFileRuleParser("ProjectFile", "parser.ProjectFile.ProjectFile",
                new SequenceParser(
                    new ActionParser("A0",
                        new ExpectationParser(
                            new NonTerminalParser("ProjectDeclaration", "ProjectDeclaration", "ProjectDeclaration", 0))),
                    new OptionalParser(
                        new NonTerminalParser("ProjectFileDeclarations", "ProjectFileDeclarations", "ProjectFileDeclarations", 1)))));
            AddRule(new ProjectDeclarationRuleParser("ProjectDeclaration", "parser.ProjectFile.ProjectDeclaration",
                new ActionParser("A0",
                    new SequenceParser(
                        new SequenceParser(
                            new KeywordParser("project"),
                            new ExpectationParser(
                                new NonTerminalParser("name", "qualified_id.name", "qualified_id", 0))),
                        new ExpectationParser(
                            new CharParser(';'))))));
            AddRule(new ProjectFileDeclarationsRuleParser("ProjectFileDeclarations", "parser.ProjectFile.ProjectFileDeclarations",
                new PositiveParser(
                    new NonTerminalParser("ProjectFileDeclaration", "ProjectFileDeclaration", "ProjectFileDeclaration", 1))));
            AddRule(new ProjectFileDeclarationRuleParser("ProjectFileDeclaration", "parser.ProjectFile.ProjectFileDeclaration",
                new AlternativeParser(
                    new AlternativeParser(
                        new AlternativeParser(
                            new AlternativeParser(
                                new ActionParser("A0",
                                    new NonTerminalParser("TargetDeclaration", "TargetDeclaration", "TargetDeclaration", 0)),
                                new SequenceParser(
                                    new ActionParser("A1",
                                        new NonTerminalParser("prf", "ProjectReference.prf", "ProjectReference", 0)),
                                    new ExpectationParser(
                                        new CharParser(';')))),
                            new SequenceParser(
                                new ActionParser("A2",
                                    new NonTerminalParser("src", "SourceFile.src", "SourceFile", 0)),
                                new ExpectationParser(
                                    new CharParser(';')))),
                        new SequenceParser(
                            new ActionParser("A3",
                                new NonTerminalParser("res", "ResourceFile.res", "ResourceFile", 0)),
                            new ExpectationParser(
                                new CharParser(';')))),
                    new SequenceParser(
                        new ActionParser("A4",
                            new NonTerminalParser("txt", "TextFile.txt", "TextFile", 0)),
                        new ExpectationParser(
                            new CharParser(';'))))));
            AddRule(new TargetDeclarationRuleParser("TargetDeclaration", "parser.ProjectFile.TargetDeclaration",
                new SequenceParser(
                    new SequenceParser(
                        new SequenceParser(
                            new KeywordParser("target"),
                            new ExpectationParser(
                                new CharParser('='))),
                        new AlternativeParser(
                            new AlternativeParser(
                                new AlternativeParser(
                                    new AlternativeParser(
                                        new AlternativeParser(
                                            new ActionParser("A0",
                                                new KeywordParser("program")),
                                            new ActionParser("A1",
                                                new KeywordParser("winguiapp"))),
                                        new ActionParser("A2",
                                            new KeywordParser("winapp"))),
                                    new ActionParser("A3",
                                        new KeywordParser("library"))),
                                new ActionParser("A4",
                                    new KeywordParser("winlib"))),
                            new ActionParser("A5",
                                new KeywordParser("unitTest")))),
                    new ExpectationParser(
                        new CharParser(';')))));
            AddRule(new ProjectReferenceRuleParser("ProjectReference", "parser.ProjectFile.ProjectReference",
                new SequenceParser(
                    new KeywordParser("reference"),
                    new ActionParser("A0",
                        new ExpectationParser(
                            new NonTerminalParser("FilePath", "FilePath", "FilePath", 0))))));
            AddRule(new SourceFileRuleParser("SourceFile", "parser.ProjectFile.SourceFile",
                new SequenceParser(
                    new KeywordParser("source"),
                    new ActionParser("A0",
                        new ExpectationParser(
                            new NonTerminalParser("FilePath", "FilePath", "FilePath", 0))))));
            AddRule(new ResourceFileRuleParser("ResourceFile", "parser.ProjectFile.ResourceFile",
                new SequenceParser(
                    new KeywordParser("resource"),
                    new ActionParser("A0",
                        new ExpectationParser(
                            new NonTerminalParser("FilePath", "FilePath", "FilePath", 0))))));
            AddRule(new TextFileRuleParser("TextFile", "parser.ProjectFile.TextFile",
                new SequenceParser(
                    new KeywordParser("text"),
                    new ActionParser("A0",
                        new ExpectationParser(
                            new NonTerminalParser("FilePath", "FilePath", "FilePath", 0))))));
            AddRule(new FilePathRuleParser("FilePath", "parser.ProjectFile.FilePath",
                new TokenParser(
                    new SequenceParser(
                        new SequenceParser(
                            new CharParser('<'),
                            new ActionParser("A0",
                                new PositiveParser(
                                    new CharSetParser(">", true)))),
                        new ExpectationParser(
                            new CharParser('>'))))));
            SkipRuleName = "spaces_and_comments";
        }
        public override void GetReferencedGrammars()
        {
            ReferencedGrammars.Add(
                ParsingDomain.GetGrammar("soul.parsing.stdlib", null,
                    Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)));
        }
        private class ProjectFileRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public Project value;
                public Project fromProjectDeclaration;
            }
            public ProjectFileRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "Project";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser projectDeclarationNonTerminalParser = GetNonTerminal("ProjectDeclaration");
                projectDeclarationNonTerminalParser.PostCall = PostProjectDeclaration;
                NonTerminalParser projectFileDeclarationsNonTerminalParser = GetNonTerminal("ProjectFileDeclarations");
                projectFileDeclarationsNonTerminalParser.PreCall = PreProjectFileDeclarations;
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
                context.value = context.fromProjectDeclaration;
            }
            private void PostProjectDeclaration(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromProjectDeclaration = (Project)stack.Pop();
                }
            }
            private void PreProjectFileDeclarations(Stack<object> stack)
            {
                stack.Push(context.value);
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class ProjectDeclarationRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public Project value;
                public string fromname;
            }
            public ProjectDeclarationRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "Project";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser nameNonTerminalParser = GetNonTerminal("name");
                nameNonTerminalParser.PostCall = Postname;
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
                context.value = new Project(context.fromname, fileName);
            }
            private void Postname(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromname = (string)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class ProjectFileDeclarationsRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public Project project;
            }
            public ProjectFileDeclarationsRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                InheritedAttributes.Add(new AttrOrVariable("Project", "project"));
            }
            public override void Link()
            {
                NonTerminalParser projectFileDeclarationNonTerminalParser = GetNonTerminal("ProjectFileDeclaration");
                projectFileDeclarationNonTerminalParser.PreCall = PreProjectFileDeclaration;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
                context.project = (Project)stack.Pop();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                context = contextStack.Pop();
            }
            private void PreProjectFileDeclaration(Stack<object> stack)
            {
                stack.Push(context.project);
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class ProjectFileDeclarationRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public Project project;
                public Target fromTargetDeclaration;
                public string fromprf;
                public string fromsrc;
                public string fromres;
                public string fromtxt;
            }
            public ProjectFileDeclarationRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                InheritedAttributes.Add(new AttrOrVariable("Project", "project"));
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
                NonTerminalParser targetDeclarationNonTerminalParser = GetNonTerminal("TargetDeclaration");
                targetDeclarationNonTerminalParser.PostCall = PostTargetDeclaration;
                NonTerminalParser prfNonTerminalParser = GetNonTerminal("prf");
                prfNonTerminalParser.PostCall = Postprf;
                NonTerminalParser srcNonTerminalParser = GetNonTerminal("src");
                srcNonTerminalParser.PostCall = Postsrc;
                NonTerminalParser resNonTerminalParser = GetNonTerminal("res");
                resNonTerminalParser.PostCall = Postres;
                NonTerminalParser txtNonTerminalParser = GetNonTerminal("txt");
                txtNonTerminalParser.PostCall = Posttxt;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
                context.project = (Project)stack.Pop();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.project.Target = context.fromTargetDeclaration;
            }
            private void A1Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.project.AddReference(context.fromprf);
            }
            private void A2Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.project.AddSourceFile(context.fromsrc, SourceFile.Kind.cm);
            }
            private void A3Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.project.AddSourceFile(context.fromres, SourceFile.Kind.xml);
            }
            private void A4Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.project.AddSourceFile(context.fromtxt, SourceFile.Kind.text);
            }
            private void PostTargetDeclaration(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromTargetDeclaration = (Target)stack.Pop();
                }
            }
            private void Postprf(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromprf = (string)stack.Pop();
                }
            }
            private void Postsrc(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromsrc = (string)stack.Pop();
                }
            }
            private void Postres(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromres = (string)stack.Pop();
                }
            }
            private void Posttxt(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromtxt = (string)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class TargetDeclarationRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public Target value;
            }
            public TargetDeclarationRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "Target";
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
                context.value = Target.program;
            }
            private void A1Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = Target.winguiapp;
            }
            private void A2Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = Target.winapp;
            }
            private void A3Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = Target.library;
            }
            private void A4Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = Target.winlib;
            }
            private void A5Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                context.value = Target.unitTest;
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class ProjectReferenceRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public string fromFilePath;
            }
            public ProjectReferenceRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser filePathNonTerminalParser = GetNonTerminal("FilePath");
                filePathNonTerminalParser.PostCall = PostFilePath;
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
                context.value = context.fromFilePath;
            }
            private void PostFilePath(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromFilePath = (string)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class SourceFileRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public string fromFilePath;
            }
            public SourceFileRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser filePathNonTerminalParser = GetNonTerminal("FilePath");
                filePathNonTerminalParser.PostCall = PostFilePath;
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
                context.value = context.fromFilePath;
            }
            private void PostFilePath(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromFilePath = (string)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class ResourceFileRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public string fromFilePath;
            }
            public ResourceFileRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser filePathNonTerminalParser = GetNonTerminal("FilePath");
                filePathNonTerminalParser.PostCall = PostFilePath;
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
                context.value = context.fromFilePath;
            }
            private void PostFilePath(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromFilePath = (string)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class TextFileRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public string fromFilePath;
            }
            public TextFileRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "string";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser filePathNonTerminalParser = GetNonTerminal("FilePath");
                filePathNonTerminalParser.PostCall = PostFilePath;
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
                context.value = context.fromFilePath;
            }
            private void PostFilePath(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromFilePath = (string)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class FilePathRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
            }
            public FilePathRuleParser(string name, string fullName, Parser definition)
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
