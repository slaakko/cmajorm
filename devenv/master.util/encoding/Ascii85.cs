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
    public static class Ascii85
    {
        public class DecodingException : Exception
        {
            public DecodingException(string message)
                : base(message)
            {
            }
        }
        public static byte[] Decode(string ascii85String)
        {
            int state = 0;
            List<byte> bytes = new List<byte>(ascii85String.Length);
            char c1 = '!';
            char c2 = '!';
            char c3 = '!';
            char c4 = '!';
            char c5 = '!';
            foreach (char c in ascii85String)
            {
                switch (state)
                {
                    case 0:
                    {
                        if (IsAscii85Char(c))
                        {
                            c1 = c;
                            state = 1;
                        }
                        else if (c == 'z')
                        {
                            bytes.AddRange(new byte[4] { 0, 0, 0, 0 });
                        }
                        else if (!char.IsWhiteSpace(c))
                        {
                            throw new DecodingException("Ascii85.Decode: invalid character " + c);
                        }
                        break;
                    }
                    case 1:
                    {
                        if (IsAscii85Char(c))
                        {
                            c2 = c;
                            state = 2;
                        }
                        else if (!char.IsWhiteSpace(c))
                        {
                            throw new DecodingException("Ascii85.Decode: invalid character " + c);
                        }
                        break;
                    }
                    case 2:
                    {
                        if (IsAscii85Char(c))
                        {
                            c3 = c;
                            state = 3;
                        }
                        else if (!char.IsWhiteSpace(c))
                        {
                            throw new DecodingException("Ascii85.Decode: invalid character " + c);
                        }
                        break;
                    }
                    case 3:
                    {
                        if (IsAscii85Char(c))
                        {
                            c4 = c;
                            state = 4;
                        }
                        else if (!char.IsWhiteSpace(c))
                        {
                            throw new DecodingException("Ascii85.Decode: invalid character " + c);
                        }
                        break;
                    }
                    case 4:
                    {
                        if (IsAscii85Char(c))
                        {
                            c5 = c;
                            bytes.AddRange(Bytes(Tuple(c1, c2, c3, c4, c5)));
                            state = 0;
                        }
                        else if (!char.IsWhiteSpace(c))
                        {
                            throw new DecodingException("Ascii85.Decode: invalid character " + c);
                        }
                        break;
                    }
                }
            }
            switch (state)
            {
                case 1:
                {
                    throw new DecodingException("Ascii85.Decode: final block cannot contain only one character");
                }
                case 2:
                {
                    byte[] b = Bytes(Tuple(c1, c2, 'u', 'u', 'u'));
                    bytes.Add(b[0]);
                    break;
                }
                case 3:
                {
                    byte[] b = Bytes(Tuple(c1, c2, c3, 'u', 'u'));
                    bytes.Add(b[0]);
                    bytes.Add(b[1]);
                    break;
                }
                case 4:
                {
                    byte[] b = Bytes(Tuple(c1, c2, c3, c4, 'u'));
                    bytes.Add(b[0]);
                    bytes.Add(b[1]);
                    bytes.Add(b[2]);
                    break;
                }
            }
            return bytes.ToArray();
        }
        public const int defaultLineLength = 80;
        public static string Encode(byte[] bytes)
        {
            return Encode(bytes, defaultLineLength);
        }
        public static string Encode(byte[] bytes, int lineLength)
        {
            StringBuilder s = new StringBuilder(5 * bytes.Length / 4);
            int state = 0;
            int linePos = 0;
            byte b1 = 0;
            byte b2 = 0;
            byte b3 = 0;
            byte b4 = 0;
            foreach (byte b in bytes)
            {
                switch (state)
                {
                    case 0: b1 = b; state = 1; break;
                    case 1: b2 = b; state = 2; break;
                    case 2: b3 = b; state = 3; break;
                    case 3:
                    {
                        b4 = b;
                        string t = EncodeBytes(b1, b2, b3, b4);
                        LineBreak(ref linePos, t, s, lineLength);
                        s.Append(t);
                        state = 0;
                        break;
                    }
                }
            }
            switch (state)
            {
                case 1:
                {
                    string t = EncodeBytes(b1, 0, 0, 0, false).Substring(0, 2);
                    LineBreak(ref linePos, t, s, lineLength);
                    s.Append(t);
                    break;
                }
                case 2:
                {
                    string t = EncodeBytes(b1, b2, 0, 0, false).Substring(0, 3);
                    LineBreak(ref linePos, t, s, lineLength);
                    s.Append(t); break;
                }
                case 3:
                {
                    string t = EncodeBytes(b1, b2, b3, 0, false).Substring(0, 4);
                    LineBreak(ref linePos, t, s, lineLength);
                    s.Append(t); 
                    break;
                }
            }
            return s.ToString();
        }
        private static bool IsAscii85Char(char c)
        {
            return c >= '!' && c <= 'u';
        }
        private static uint DecodeChar(char ascii85Char)
        {
            return (uint)(ascii85Char - '!');
        }
        private static uint Tuple(char c1, char c2, char c3, char c4, char c5)
        {
            return 85 * 85 * 85 * 85 * DecodeChar(c1) + 85 * 85 * 85 * DecodeChar(c2) + 85 * 85 * DecodeChar(c3) +
                85 * DecodeChar(c4) + DecodeChar(c5);
        }
        private static char EncodeByte(byte b)
        {
            return (char)(b + '!');
        }
        private static uint Tuple(byte b1, byte b2, byte b3, byte b4)
        {
            return 256 * 256 * 256 * (uint)b1 + 256 * 256 * (uint)b2 + 256 * (uint)b3 + (uint)b4;
        }
        private static byte[] Bytes(uint tuple)
        {
            return new byte[] { (byte)(tuple >> 24 & 255), (byte)(tuple >> 16 & 255), (byte)(tuple >> 8 & 255), (byte)(tuple & 255) };
        }
        private static string EncodeBytes(byte b1, byte b2, byte b3, byte b4, bool z)
        {
            uint tuple = Tuple(b1, b2, b3, b4);
            if (tuple == 0 && z)
            {
                return "z";
            }
            char[] chars = new char[5];
            for (int i = 4; i >= 0; --i)
            {
                chars[i] = EncodeByte((byte)(tuple % 85));
                tuple /= 85;
            }
            return new string(chars);
        }
        private static string EncodeBytes(byte b1, byte b2, byte b3, byte b4)
        {
            return EncodeBytes(b1, b2, b3, b4, true);
        }
        private static void LineBreak(ref int linePos, string t, StringBuilder s, int lineLength)
        {
            if (linePos + t.Length >= lineLength)
            {
                s.Append(Environment.NewLine);
                linePos = 0;
            }
        }
    }
}
