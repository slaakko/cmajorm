/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;

namespace soul.parsing
{
    public class CharParser : Parser
    {
        public CharParser(char c)
            : base("'" + c + "'")
        {
            this.c = c;
        }
        public Char Char
        {
            get { return c; }
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            if (!scanner.AtEnd)
            {
                if (scanner.Char == c)
                {
                    ++scanner;
                    return Match.One();
                }
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private char c;
    }

    public class StringParser : Parser
    {
        public StringParser(string s)
            : base("\"" + s + "\"")
        {
            this.s = s;
        }
        public string String
        {
            get { return s; }
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Match match = Match.Empty();
            int i = 0;
            int n = s.Length;
            while (i < n && !scanner.AtEnd && scanner.Char == s[i])
            {
                ++scanner;
                ++i;
                match.Concatenate(Match.One());
            }
            if (i == n)
            {
                return match;
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string s;
    }

    public struct CharRange
    {
        public CharRange(char c)
            : this(c, c)
        {
        }
        public CharRange(char first, char last)
        {
            this.first = first;
            this.last = last;
        }
        public bool Includes(char c)
        {
            return c >= first && c <= last;
        }
        public char First
        {
            get { return first; }
        }
        public char Last
        {
            get { return last; }
        }
        private char first;
        private char last;
    }

    public class CharSetParser : Parser
    {
        public CharSetParser(string s)
            : this(s, false)
        {
        }
        public CharSetParser(string s, bool inverse)
            : base("[" + s + "]")
        {
            this.s = s;
            this.inverse = inverse;
            ranges = new List<CharRange>();
            InitRanges();
        }
        public string Set
        {
            get { return s; }
        }
        public bool Inverse
        {
            get { return inverse; }
        }
        public List<CharRange> Ranges
        {
            get { return ranges; }
        }
        private void InitRanges()
        {
            BitArray bits = new BitArray(char.MaxValue + 1);
            int i = 0;
            int n = s.Length;
            while (i < n)
            {
                int first = s[i];
                int last = first;
                ++i;
                if (i < n)
                {
                    if (s[i] == '-')
                    {
                        ++i;
                        if (i < n)
                        {
                            last = s[i];
                            ++i;
                        }
                        else
                        {
                            bits.Set(first, true);
                            first = '-';
                            last = '-';
                        }
                    }
                }
                for (int b = first; b <= last; ++b)
                {
                    bits.Set(b, true);
                }
            }
            int state = 0;
            char begin = '\0';
            char end = '\0';
            for (int j = 0; j < char.MaxValue + 1; ++j)
            {
                bool bit = bits[j];
                switch (state)
                {
                    case 0:
                        if (bit)
                        {
                            begin = (char)j;
                            end = begin;
                            state = 1;
                        }
                        break;
                    case 1:
                        if (bit)
                        {
                            end = (char)j;
                        }
                        else
                        {
                            ranges.Add(new CharRange(begin, end));
                            state = 0;
                        }
                        break;
                }
            }
            if (state == 1)
            {
                ranges.Add(new CharRange(begin, end));
            }
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            if (!scanner.AtEnd)
            {
                if (Test(scanner.Char))
                {
                    ++scanner;
                    return Match.One();
                }
            }
            return Match.Nothing();
        }
        private bool Test(char c)
        {
            int n = ranges.Count;
            if (n > 0)
            {
                for (int i = 0; i < n; ++i)
                {
                    if (ranges[i].Includes(c))
                    {
                        return inverse ? false : true;
                    }
                }
            }
            return inverse;
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string s;
        private bool inverse;
        private List<CharRange> ranges;
    }
    
    public class EmptyParser : Parser
    {
        public EmptyParser()
            : base("")
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            return Match.Empty();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class SpaceParser : Parser
    {
        public SpaceParser()
            : base("space")
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            if (!scanner.AtEnd)
            {
                if (char.IsWhiteSpace(scanner.Char))
                {
                    ++scanner;
                    return Match.One();
                }
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class LetterParser : Parser
    {
        public LetterParser()
            : base("letter")
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            if (!scanner.AtEnd)
            {
                if (char.IsLetter(scanner.Char))
                {
                    ++scanner;
                    return Match.One();
                }
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class DigitParser : Parser
    {
        public DigitParser()
            : base("digit")
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            if (!scanner.AtEnd)
            {
                if (char.IsDigit(scanner.Char))
                {
                    ++scanner;
                    return Match.One();
                }
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class HexDigitParser : Parser
    {
        public HexDigitParser()
            : base("hexdigit")
        {
            hexDigit = new CharSetParser("0-9a-fA-F");
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            return hexDigit.Parse(scanner, stack);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CharSetParser hexDigit;
    }

    public class PunctuationParser : Parser
    {
        public PunctuationParser()
            : base("punctuation")
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            if (!scanner.AtEnd)
            {
                if (char.IsPunctuation(scanner.Char))
                {
                    ++scanner;
                    return Match.One();
                }
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class AnyCharParser : Parser
    {
        public AnyCharParser()
            : base("anychar")
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            if (!scanner.AtEnd)
            {
                ++scanner;
                return Match.One();
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

 }