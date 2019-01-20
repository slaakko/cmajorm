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
    public class Scanner
    {
        public Scanner(string content, int file, string fileName, Parser skipper)
        {
            this.content = content;
            position.Index = 0;
            position.File = file;
            position.Line = 1;
            position.Column = 1;
            this.fileName = fileName;
            tokenCounter = 0;
            skipping = false;
            this.skipper = skipper;
        }
        public string Content
        {
            get { return content; }
        }
        public Position Position
        {
            get { return position; }
            set { position = value; }
        }
        public string FileName
        {
            get { return fileName; }
        }
        public int TokenCounter
        {
            get { return tokenCounter; }
        }
        public bool Skipping
        {
            get { return skipping; }
        }
        public Parser Skipper
        {
            get { return skipper; }
            set { skipper = value; }
        }
        public char Char
        {
            get { return content[position.Index]; }
        }
        public string Match(int startIndex)
        {
            return content.Substring(startIndex, position.Index - startIndex);
        }
        public bool AtEnd
        {
            get { return position.Index >= content.Length; }
        }
        public static Scanner operator ++(Scanner scanner)
        {
            char c = scanner.Char;
            ++scanner.position.Index;
            if (c == '\n')
            {
                ++scanner.position.Line;
                scanner.position.Column = 1;
            }
            else if (c != '\r')
            {
                ++scanner.position.Column;
            }
            return scanner;
        }
        public void BeginToken()
        {
            ++tokenCounter;
        }
        public void EndToken()
        {
            --tokenCounter;
        }
        public void Skip()
        {
            if (tokenCounter == 0 && skipper != null)
            {
                Position save = position;
                BeginToken();
                skipping = true;
                Match match = skipper.Parse(this, null);
                skipping = false;
                EndToken();
                if (!match.Hit)
                {
                    position = save;
                }
            }
        }
        public XmlLog Log
        {
            get { return log; }
            set { log = value; }
        }
        public int LineEndIndex
        {
            get
            {
                int lineEndIndex = position.Index;
                while (lineEndIndex < content.Length && (content[lineEndIndex] != '\r' && content[lineEndIndex] != '\n'))
                {
                    ++lineEndIndex;
                }
                return lineEndIndex;
            }
        }
        public string RestOfLine
        {
            get { return content.Substring(position.Index, LineEndIndex - position.Index);}
        }
        private string content;
        private Position position;
        private string fileName;
        private int tokenCounter;
        private bool skipping;
        private Parser skipper;
        private XmlLog log;
    }
}
