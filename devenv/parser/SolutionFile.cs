using System;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using soul.parsing;
using devcore;

namespace parser
{
    public class SolutionFile : soul.parsing.Grammar
    {
        public SolutionFile()
            : base("SolutionFile", "parser.SolutionFile")
        {
            Link();
        }
        public SolutionFile(ParsingDomain parsingDomain)
            : base("SolutionFile", "parser.SolutionFile", parsingDomain)
        {
            Link();
        }
        public new Solution Parse(string content, int fileIndex, string fileName)
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
            return (Solution)stack.Pop();
        }
        public override void CreateRules()
        {
            AddRule(new SolutionFileRuleParser("SolutionFile", "parser.SolutionFile.SolutionFile",
                new SequenceParser(
                    new SequenceParser(
                        new ActionParser("A0",
                            new ExpectationParser(
                                new NonTerminalParser("SolutionDeclaration", "SolutionDeclaration", "SolutionDeclaration", 0))),
                        new OptionalParser(
                            new NonTerminalParser("ProjectFileDeclarations", "ProjectFileDeclarations", "ProjectFileDeclarations", 1))),
                    new OptionalParser(
                        new NonTerminalParser("ActiveProject", "ActiveProject", "ActiveProject", 1)))));
            AddRule(new SolutionDeclarationRuleParser("SolutionDeclaration", "parser.SolutionFile.SolutionDeclaration",
                new ActionParser("A0",
                    new SequenceParser(
                        new SequenceParser(
                            new KeywordParser("solution"),
                            new ExpectationParser(
                                new NonTerminalParser("name", "qualified_id.name", "qualified_id", 0))),
                        new ExpectationParser(
                            new CharParser(';'))))));
            AddRule(new ProjectFileDeclarationsRuleParser("ProjectFileDeclarations", "parser.SolutionFile.ProjectFileDeclarations",
                new PositiveParser(
                    new ActionParser("A0",
                        new NonTerminalParser("projectFilePath", "ProjectFileDeclaration.projectFilePath", "ProjectFileDeclaration", 0)))));
            AddRule(new ProjectFileDeclarationRuleParser("ProjectFileDeclaration", "parser.SolutionFile.ProjectFileDeclaration",
                new ActionParser("A0",
                    new SequenceParser(
                        new SequenceParser(
                            new KeywordParser("project"),
                            new ExpectationParser(
                                new NonTerminalParser("FilePath", "FilePath", "FilePath", 0))),
                        new CharParser(';')))));
            AddRule(new ActiveProjectRuleParser("ActiveProject", "parser.SolutionFile.ActiveProject",
                new ActionParser("A0",
                    new SequenceParser(
                        new SequenceParser(
                            new KeywordParser("activeProject"),
                            new ExpectationParser(
                                new NonTerminalParser("projectName", "qualified_id.projectName", "qualified_id", 0))),
                        new CharParser(';')))));
            AddRule(new FilePathRuleParser("FilePath", "parser.SolutionFile.FilePath",
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
        private class SolutionFileRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public Solution value;
                public Solution fromSolutionDeclaration;
            }
            public SolutionFileRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "Solution";
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser solutionDeclarationNonTerminalParser = GetNonTerminal("SolutionDeclaration");
                solutionDeclarationNonTerminalParser.PostCall = PostSolutionDeclaration;
                NonTerminalParser projectFileDeclarationsNonTerminalParser = GetNonTerminal("ProjectFileDeclarations");
                projectFileDeclarationsNonTerminalParser.PreCall = PreProjectFileDeclarations;
                NonTerminalParser activeProjectNonTerminalParser = GetNonTerminal("ActiveProject");
                activeProjectNonTerminalParser.PreCall = PreActiveProject;
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
                context.value = context.fromSolutionDeclaration;
            }
            private void PostSolutionDeclaration(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromSolutionDeclaration = (Solution)stack.Pop();
                }
            }
            private void PreProjectFileDeclarations(Stack<object> stack)
            {
                stack.Push(context.value);
            }
            private void PreActiveProject(Stack<object> stack)
            {
                stack.Push(context.value);
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class SolutionDeclarationRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public Solution value;
                public string fromname;
            }
            public SolutionDeclarationRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                ValueTypeName = "Solution";
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
                context.value = new Solution(context.fromname, fileName);
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
                public Solution solution;
                public string fromprojectFilePath;
            }
            public ProjectFileDeclarationsRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                InheritedAttributes.Add(new AttrOrVariable("Solution", "solution"));
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser projectFilePathNonTerminalParser = GetNonTerminal("projectFilePath");
                projectFilePathNonTerminalParser.PostCall = PostprojectFilePath;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
                context.solution = (Solution)stack.Pop();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                ProjectFile projectFileParser = ParserRepository.Instance.ProjectFileParser;
                Project project = projectFileParser.Parse(File.ReadAllText(Path.Combine(context.solution.BasePath, context.fromprojectFilePath)), 0, Path.GetFullPath(context.fromprojectFilePath));
                context.solution.Projects.Add(project);
            }
            private void PostprojectFilePath(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromprojectFilePath = (string)stack.Pop();
                }
            }
            private Context context;
            private Stack<Context> contextStack;
        }
        private class ProjectFileDeclarationRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public string value;
                public string fromFilePath;
            }
            public ProjectFileDeclarationRuleParser(string name, string fullName, Parser definition)
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
        private class ActiveProjectRuleParser : soul.parsing.RuleParser
        {
            private class Context
            {
                public Solution solution;
                public string fromprojectName;
            }
            public ActiveProjectRuleParser(string name, string fullName, Parser definition)
                : base(name, fullName, definition)
            {
                contextStack = new Stack<Context>();
                InheritedAttributes.Add(new AttrOrVariable("Solution", "solution"));
            }
            public override void Link()
            {
                ActionParser a0ActionParser = GetAction("A0");
                a0ActionParser.Action = A0Action;
                NonTerminalParser projectNameNonTerminalParser = GetNonTerminal("projectName");
                projectNameNonTerminalParser.PostCall = PostprojectName;
            }
            public override void Enter(Stack<object> stack)
            {
                contextStack.Push(context);
                context = new Context();
                context.solution = (Solution)stack.Pop();
            }
            public override void Leave(Stack<object> stack, bool matched)
            {
                context = contextStack.Pop();
            }
            private void A0Action(string match, string content, Position position, string fileName, ref bool pass)
            {
                Project activeProject = context.solution.GetProject(context.fromprojectName);
                if (activeProject != null) 
                {
                    context.solution.ActiveProject = activeProject;
                }
            }
            private void PostprojectName(Stack<object> stack, bool matched)
            {
                if (matched)
                {
                    context.fromprojectName = (string)stack.Pop();
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
