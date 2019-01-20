/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.Collections.Generic;
using System.Text;

namespace master.util.encoding
{
    public static class Hex
    {
        public static byte[] Decode(string hexString)
        {
            int state = 0;
            byte b = 0;
            List<byte> bytes = new List<byte>(hexString.Length / 2);
            foreach (char c in hexString)
            {
                switch (state)
                {
                    case 0:
                    {
                        if (IsHexChar(c))
                        {
                            b = Nibble(c);
                            state = 1;
                        }
                        break;
                    }
                    case 1:
                    {
                        if (IsHexChar(c))
                        {
                            b = (byte)(16 * b + Nibble(c));
                            bytes.Add(b);
                            state = 0;
                        }
                        break;
                    }
                }
            }
            if (state == 1)
            {
                bytes.Add((byte)(16 * b));
            }
            return bytes.ToArray();
        }
        public static string Encode(byte[] bytes)
        {
            StringBuilder s = new StringBuilder(bytes.Length * 2);
            foreach (byte b in bytes)
            {
                s.Append(HexChar((byte)(b / 16)));
                s.Append(HexChar((byte)(b % 16)));
            }
            return s.ToString();
        }
        private static bool IsHexChar(char c)
        {
            return c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F';
        }
        private static byte Nibble(char hexChar)
        {
            return (byte)(hexChar >= '0' && hexChar <= '9' ?
                hexChar - '0' :
                hexChar >= 'a' && hexChar <= 'f' ?
                    10 + hexChar - 'a' :
                    10 + hexChar - 'A');
        }
        private static char HexChar(byte nibble)
        {
            const string hexChars = "0123456789ABCDEF";
            return hexChars[nibble];
        }

    }
}
