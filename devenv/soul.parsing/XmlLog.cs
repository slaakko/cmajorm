/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace soul.parsing
{
    public class XmlLog
    {
        public XmlLog(TextWriter writer, int maxLineLength)
        {
            this.writer = writer;
            this.maxLineLength = maxLineLength;
            indent = 0;
        }
        public void IncIndent()
        {
            ++indent;
        }
        public void DecIndent()
        {
            --indent;
        }
        public void WriteBeginRule(string ruleName)
        {
            Write("<" + ruleName + ">");
        }
        public void WriteEndRule(string ruleName)
        {
            Write("</" + ruleName + ">");
        }
        public void WriteTry(string s)
        {
            WriteElement("try", s);
        }
        public void WriteSuccess(string match)
        {
            WriteElement("success", match);
        }
        public void WriteFail()
        {
            Write("<fail/>");
        }
        public void WriteElement(string element, string content)
        {
            string converted = Convert(content);
            int convertedLength = converted.Length;
            int lineLength = indent + 2 * element.Length + 5 + convertedLength;
            string s = converted;
            if (lineLength > maxLineLength)
            {
                lineLength += 3;
                s = converted.Substring(0, Math.Max(0, convertedLength - (lineLength - maxLineLength))) + "...";
            }
            Write("<" + element + ">" + s + "</" + element + ">");
        }
        public void Write(string s)
        {
            if (indent > 0)
            {
                writer.Write(new string(' ', indent));
            }
            writer.WriteLine(s);
        }
        private string Convert(string s)
        {
            StringBuilder b = new StringBuilder(2 * s.Length);
            foreach (char c in s)
            {
                b.Append(CharConvert.CharStr(c));
            }
            return b.ToString();
        }
        private static class CharConvert
        {
            public static string HexEscape(char c)
            {
                int code = (int)c;
                return "\\x" + code.ToString("x4");
            }
            public static string CharStr(char c)
            {
                switch (c)
                {
                    case '&': return "&amp;";
                    case '<': return "&lt;";
                    case '>': return "&gt;";
                    case '\a': return "\\a";
                    case '\b': return "\\b";
                    case '\f': return "\\f";
                    case '\n': return "\\n";
                    case '\r': return "\\r";
                    case '\t': return "\\t";
                    case '\v': return "\\v";
                    default:
                        {
                            if (((int)c >= 32 && (int)c <= 126) || char.IsLetterOrDigit(c) || char.IsPunctuation(c))
                            {
                                return c.ToString();
                            }
                            else
                            {
                                return HexEscape(c);
                            }
                        }
                }
            }
        }
        private TextWriter writer;
        private int maxLineLength;
        private int indent;
    }
}
