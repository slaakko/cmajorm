using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace devcore
{
    public abstract class SourceFormatter
    {
        public const int normal = 0;
        public const int keyword = 1;
        public const int identifier = 2;
        public const int character = 3;
        public const int str = 4;
        public const int number = 5;
        public const int comment = 6;
        public const int numStyles = 7;

        public SourceFormatter()
        {
            colors = new Color[numStyles];
            fontNames = new string[numStyles];
            fontStyles = new FontStyle[numStyles];
            Font normalFont = new Font(new FontFamily("Courier New"), 10);
            NormalFont = normalFont;
            KeywordFont = normalFont;
            IdentifierFont = normalFont;
            CharFont = normalFont;
            StringFont = normalFont;
            NumberFont = normalFont;
            CommentFont = normalFont;
        }

        public virtual void BeginFormat() { }
        public virtual void EndFormat() { }
        public virtual void Keyword(string kw) { }
        public virtual void Id(string id) { }
        public virtual void Char(string chr) { }
        public virtual void String(string str_) { }
        public virtual void Number(string num) { }
        public virtual void Spaces(string spc) { }
        public virtual void LineComment(string cmt) { }
        public virtual void BlockComment(string cmt) { }
        public virtual void BlockCommentStartOrEnd() { }
        public virtual void Other(string other) { }
        public virtual void NewLine() { }

        public Color NormalColor
        {
            get { return colors[normal]; }
            set { colors[normal] = value; }
        }
        public Font NormalFont
        {
            get { return normalFont; }
            set
            {
                normalFont = value;
                fontNames[normal] = normalFont.Name;
                fontStyles[normal] = normalFont.Style;
            }
        }
        public Color KeywordColor
        {
            get { return colors[keyword]; }
            set { colors[keyword] = value; }
        }
        public Font KeywordFont
        {
            get { return keywordFont; }
            set
            {
                keywordFont = value;
                fontNames[keyword] = keywordFont.Name;
                fontStyles[keyword] = keywordFont.Style;
            }
        }
        public Color IdentifierColor
        {
            get { return colors[identifier]; }
            set { colors[identifier] = value; }
        }
        public Font IdentifierFont
        {
            get { return idFont; }
            set
            {
                idFont = value;
                fontNames[identifier] = idFont.Name;
                fontStyles[identifier] = idFont.Style;
            }
        }
        public Color CharColor
        {
            get { return colors[character]; }
            set { colors[character] = value; }
        }
        public Font CharFont
        {
            get { return chrFont; }
            set
            {
                chrFont = value;
                fontNames[character] = chrFont.Name;
                fontStyles[character] = chrFont.Style;
            }
        }
        public Color StringColor
        {
            get { return colors[str]; }
            set { colors[str] = value; }
        }
        public Font StringFont
        {
            get { return strFont; }
            set
            {
                strFont = value;
                fontNames[str] = strFont.Name;
                fontStyles[str] = strFont.Style;
            }
        }
        public Color NumberColor
        {
            get { return colors[number]; }
            set { colors[number] = value; }
        }
        public Font NumberFont
        {
            get { return numFont; }
            set
            {
                numFont = value;
                fontNames[number] = numFont.Name;
                fontStyles[number] = numFont.Style;
            }
        }
        public Color CommentColor
        {
            get { return colors[comment]; }
            set { colors[comment] = value; }
        }
        public Font CommentFont
        {
            get { return commentFont; }
            set
            {
                commentFont = value;
                fontNames[comment] = commentFont.Name;
                fontStyles[comment] = commentFont.Style;
            }
        }

        public Color[] Colors
        {
            get { return colors; }
        }
        public string[] FontNames
        {
            get { return fontNames; }
        }
        public FontStyle[] FontStyles
        {
            get { return fontStyles; }
        }
        private Font normalFont;
        private Font keywordFont;
        private Font idFont;
        private Font chrFont;
        private Font strFont;
        private Font numFont;
        private Font commentFont;
        private Color[] colors;
        private string[] fontNames;
        private FontStyle[] fontStyles;
    }
}
