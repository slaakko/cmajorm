using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.IO;

namespace devcore
{
    public class FindResult
    {
        public FindResult(SourceFile file, int lineNumber, string line)
        {
            this.file = file;
            this.lineNumber = lineNumber;
            this.line = line;
        }
        public SourceFile File
        {
            get { return file; }
        }
        public int LineNumber
        {
            get { return lineNumber; }
        }
        public string Line
        {
            get { return line; }
        }
        private SourceFile file;
        private int lineNumber;
        private string line;
    }

    public static class Finder
    {
        public static List<FindResult> Find(Solution findSolution, Project findProject, SourceFile findSourceFile, bool matchCase, bool matchWholeWord, bool regExMatch, string findWhat)
        {
            List<FindResult> results = new List<FindResult>();
            List<SourceFile> sourceFiles = new List<SourceFile>();
            if (findSolution != null)
            {
                foreach (Project project in findSolution.Projects)
                {
                    foreach (SourceFile sourceFile in project.SourceFiles)
                    {
                        sourceFiles.Add(sourceFile);
                    }
                }
            }
            else if (findProject != null)
            {
                foreach (SourceFile sourceFile in findProject.SourceFiles)
                {
                    sourceFiles.Add(sourceFile);
                }
            }
            else if (findSourceFile != null)
            {
                sourceFiles.Add(findSourceFile);
            }
            RegexOptions options = RegexOptions.Singleline;
            if (!matchCase)
            {
                options = RegexOptions.IgnoreCase;
            }
            string pattern = matchWholeWord ? "\\b" + (regExMatch ? findWhat : RegExEscape(findWhat)) + "\\b" : (regExMatch ? findWhat : RegExEscape(findWhat));
            Regex regex = new Regex(pattern, options);
            foreach (SourceFile sourceFile in sourceFiles)
            {
                string[] lines = File.ReadAllLines(sourceFile.FilePath);
                int lineNumber = 1;
                foreach (string line in lines)
                {
                    if (regex.IsMatch(line))
                    {
                        results.Add(new FindResult(sourceFile, lineNumber, line));
                    }
                    ++lineNumber;
                }
            }
            return results;
        }
        public static string RegExEscape(string pattern)
        {
            StringBuilder s = new StringBuilder();
            foreach (char c in pattern)
            {
                switch (c)
                {
                    case '\\': s.Append("\\\\"); break;
                    case '(': s.Append("\\("); break;
                    case ')': s.Append("\\)"); break;
                    case '[': s.Append("\\["); break;
                    case ']': s.Append("\\]"); break;
                    case '.': s.Append("\\."); break;
                    case '^': s.Append("\\^"); break;
                    case '$': s.Append("\\$"); break;
                    case '*': s.Append("\\*"); break;
                    case '+': s.Append("\\+"); break;
                    case '?': s.Append("\\?"); break;
                    case '{': s.Append("\\{"); break;
                    case '}': s.Append("\\}"); break;
                    case '|': s.Append("\\|"); break;
                    default: s.Append(c); break;
                }
            }
            return s.ToString();
        }
    }
}
