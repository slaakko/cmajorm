using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace devcore
{
    public class SourceLineFormatter : SourceFormatter
    {
        public SourceLineFormatter(RichTextBox textBox)
        {
            this.textBox = textBox;
        }
        public int StartIndex
        {
            get { return startIndex; }
            set { startIndex = value; }
        }
        public override void BeginFormat()
        {
            blockComment = false;
        }
        public bool ContainsBlockComment()
        {
            return blockComment;
        }
        public override void Keyword(string kw)
        {
            textBox.SelectionStart = startIndex;
            textBox.SelectionLength = kw.Length;
            textBox.SelectionFont = KeywordFont;
            textBox.SelectionColor = KeywordColor;
            startIndex += kw.Length;
        }
        public override void Id(string id)
        {
            textBox.SelectionStart = startIndex;
            textBox.SelectionLength = id.Length;
            textBox.SelectionFont = IdentifierFont;
            textBox.SelectionColor = IdentifierColor;
            startIndex += id.Length;
        }
        public override void Char(string chr)
        {
            textBox.SelectionStart = startIndex;
            textBox.SelectionLength = chr.Length;
            textBox.SelectionFont = CharFont;
            textBox.SelectionColor = CharColor;
            startIndex += chr.Length;
        }
        public override void String(string str_)
        {
            textBox.SelectionStart = startIndex;
            textBox.SelectionLength = str_.Length;
            textBox.SelectionFont = StringFont;
            textBox.SelectionColor = StringColor;
            startIndex += str_.Length;
        }
        public override void Number(string num)
        {
            textBox.SelectionStart = startIndex;
            textBox.SelectionLength = num.Length;
            textBox.SelectionFont = NumberFont;
            textBox.SelectionColor = NumberColor;
            startIndex += num.Length;
        }
        public override void Spaces(string spc)
        {
            textBox.SelectionStart = startIndex;
            textBox.SelectionLength = spc.Length;
            textBox.SelectionFont = NormalFont;
            textBox.SelectionColor = NormalColor;
            startIndex += spc.Length;
        }
        public override void LineComment(string cmt)
        {
            textBox.SelectionStart = startIndex;
            textBox.SelectionLength = cmt.Length;
            textBox.SelectionFont = CommentFont;
            textBox.SelectionColor = CommentColor;
            startIndex += cmt.Length;
        }
        public override void BlockCommentStartOrEnd()
        {
            blockComment = true;
        }
        public override void Other(string other)
        {
            textBox.SelectionStart = startIndex;
            textBox.SelectionLength = other.Length;
            textBox.SelectionFont = NormalFont;
            textBox.SelectionColor = NormalColor;
            startIndex += other.Length;
        }
        private int startIndex;
        private RichTextBox textBox;
        private bool blockComment;
    }
}
