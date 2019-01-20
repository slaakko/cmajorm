/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.IO;

namespace master.util.text
{
    public class CodeFormatter
    {
        public CodeFormatter(StreamWriter writer)
        {
            this.writer = writer;
            indent = 0;
            indentSize = 4;
            atBeginningOfLine = true;
        }
        public int Indent
        {
            get { return indent; }
        }
        public int IndentSize
        {
            get { return indentSize; }
            set { indentSize = value; }
        }
        public int GetIndentLength()
        {
            return indentSize * indent; 
        }
        public void Write(string text)
        {
            if (atBeginningOfLine)
            {
                if (indent != 0)
                {
                    writer.Write(new string(' ', indentSize * indent));
                    atBeginningOfLine = false;
                }
            }
            writer.Write(text);
        }
        public void WriteLine(string text)
        {
            Write(text);
            NewLine();
        }
        public void NewLine()
        {
            writer.WriteLine();
            atBeginningOfLine = true;
        }
        public void IncIndent()
        {
            ++indent;
        }
        public void DecIndent()
        {
            --indent;
        }
        private StreamWriter writer;
        private int indent;
        private int indentSize;
        private bool atBeginningOfLine;
    }
}
