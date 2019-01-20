using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace devcore
{
    public class SourceRtfConverter : SourceFormatter
    {
        public SourceRtfConverter(HashSet<int> blockCommentStartEndLineNumbers)
            : base()
        {
            this.blockCommentStartEndLineNumbers = blockCommentStartEndLineNumbers;
            fontSize = 10;
        }
        public string Rtf
        {
            get
            {
                return rtf.ToString();
            }
        }
        public override void BeginFormat()
        {
            rtf = new StringBuilder();
            noCurrentColor = true;
            noCurrentFontStyle = true;
            currentFontName = null;
            EmitPrefix();
            lineNumber = 0;
            blockCommentStartEndLineNumbers.Clear();
        }
        public override void EndFormat()
        {
            EmitSuffix();
        }
        public override void Keyword(string kw)
        {
            Emit(keyword, kw);
        }
        public override void Id(string id)
        {
            Emit(identifier, id);
        }
        public override void Char(string chr)
        {
            Emit(character, chr);
        }
        public override void String(string str_)
        {
            Emit(str, str_);
        }
        public override void Number(string num)
        {
            Emit(number, num);
        }
        public override void Spaces(string spc)
        {
            Emit(normal, spc);
        }
        public override void LineComment(string cmt)
        {
            Emit(comment, cmt);
        }
        public override void BlockComment(string cmt)
        {
            blockCommentStartEndLineNumbers.Add(lineNumber);
            StringBuilder line = new StringBuilder(1024);
            foreach (char c in cmt)
            {
                if (c == '\n')
                {
                    Emit(comment, line.ToString());
                    NewLine();
                    line.Clear();
                }
                else
                {
                    line.Append(c);
                }
            }
            if (line.Length > 0)
            {
                Emit(comment, line.ToString());
            }
            blockCommentStartEndLineNumbers.Add(lineNumber);
        }
        public override void Other(string other)
        {
            Emit(normal, other);
        }
        public override void NewLine()
        {
            rtf.AppendLine("\\par");
            ++lineNumber;
        }
        private void EmitPrefix()
        {
            rtf.AppendLine("{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1035");
            EmitColorTable();
            EmitFontTable();
        }
        private void EmitSuffix()
        {
            NewLine();
            rtf.AppendLine("}");
        }
        private void EmitColorTable()
        {
            rtf.Append("{\\colortbl ");
            for (int i = 0; i < numStyles; ++i)
            {
                EmitColor(Colors[i]);
            }
            rtf.AppendLine("}");
        }
        private void EmitColor(Color col)
        {
            rtf.Append("\\red" + col.R.ToString() + "\\green" + col.G.ToString() + "\\blue" + col.B.ToString() + ";");
        }
        private void EmitColor(int style)
        {
            rtf.Append("\\cf" + style.ToString());
        }
        private void EmitFontTable()
        {
            rtf.Append("{\\fonttbl");
            int n = numStyles;
            for (int i = 0; i < n; ++i)
            {
                EmitFont(i, FontNames[i]);
            }
            rtf.Append("}");
        }
        private void EmitFont(int fontNumber, string fontName)
        {
            rtf.Append("{\\f" + fontNumber.ToString() + "\\fnil\\fcharset0 " + fontName + ";}");
        }
        private void EmitFont(int fontNumber)
        {
            rtf.Append("\\f" + fontNumber.ToString());
            rtf.Append("\\fs" + (fontSize * 2).ToString());
        }
        private void EmitFontStyle(FontStyle fontStyle)
        {
            switch (fontStyle)
            {
                case FontStyle.Bold: rtf.Append("\\b"); break;
                case FontStyle.Italic: rtf.Append("\\i"); break;
                case FontStyle.Regular:
                    {
                        switch (currentFontStyle)
                        {
                            case FontStyle.Bold: rtf.Append("\\b0"); break;
                            case FontStyle.Italic: rtf.Append("\\i0"); break;
                            case FontStyle.Strikeout: rtf.Append("\\strike0"); break;
                            case FontStyle.Underline: rtf.Append("\\ul0"); break;
                        }
                        break;
                    }
                case FontStyle.Strikeout: rtf.Append("\\strike"); break;
                case FontStyle.Underline: rtf.Append("\\ul"); break;
            }
        }
        private void Emit(int style, string content)
        {
            bool changed = false;
            if (currentFontName != FontNames[style])
            {
                changed = true;
                currentFontName = FontNames[style];
                EmitFont(style);
            }
            if (currentFontStyle != FontStyles[style] || noCurrentFontStyle)
            {
                noCurrentFontStyle = false;
                changed = true;
                EmitFontStyle(FontStyles[style]);
                currentFontStyle = FontStyles[style];
            }
            if (currentColor != Colors[style] || noCurrentColor)
            {
                noCurrentColor = false;
                changed = true;
                currentColor = Colors[style];
                EmitColor(style);
            }
            if (changed)
            {
                rtf.Append(" ");
            }
            rtf.Append(RtfEscape(content));
        }
        private string RtfEscape(string content)
        {
            StringBuilder b = new StringBuilder(content.Length);
            foreach (char c in content)
            {
                if (c == '{') b.Append("\\{");
                else if (c == '}') b.Append("\\}");
                else if (c == '\\') b.Append("\\\\");
                else
                {
                    b.Append(c);
                }
            }
            return b.ToString();
        }
        private HashSet<int> blockCommentStartEndLineNumbers;
        private StringBuilder rtf;
        private Color currentColor;
        private bool noCurrentColor;
        private string currentFontName;
        private FontStyle currentFontStyle;
        private bool noCurrentFontStyle;
        private int fontSize;
        private int lineNumber;
    }
}
