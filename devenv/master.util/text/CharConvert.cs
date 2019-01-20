/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.Text;

namespace master.util.text
{
    public static class CharConvert
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
                case '\'': return "\\'";
                case '\"': return "\\\"";
                case '\\': return "\\\\";
                case '\a': return "\\a";
                case '\b': return "\\b";
                case '\f': return "\\f";
                case '\n': return "\\n";
                case '\r': return "\\r";
                case '\t': return "\\t";
                case '\v': return "\\v";
                case '\0': return "\\0";
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
        public static string StringStr(string s)
        {
            int n = s.Length;
            StringBuilder b = new StringBuilder(n);
            for (int i = 0; i < n; ++i)
            {
                b.Append(CharStr(s[i]));
            }
            return b.ToString();
        }
    }
}
