using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using System.IO;
using devcore;
using parser;
using System.Runtime.InteropServices;

namespace cmdevenv
{
    public class PositionEventArgs : EventArgs
    {
        public PositionEventArgs(int line, int col)
        {
            Line = line;
            Col = col;
        }
        public int Line { get; set; }
        public int Col { get; set; }
    }

    public enum EditorMode
    {
        editing, debugging
    }

    public partial class Editor : RichTextBox
    {
        public delegate void PositionEventHandler(object sender, PositionEventArgs e);
        public event PositionEventHandler PositionChanged;
        public delegate void DirtyEventHandler(object sender, EventArgs e);
        public event DirtyEventHandler DirtyStatusChanged;
        public delegate void CloseEventHandler(object sender, EventArgs e);
        public event CloseEventHandler Closed;
        public event EventHandler Painted;

        public Editor(SourceFile sourceFile, MainForm mainForm)
        {
            InitializeComponent();
            this.sourceFile = sourceFile;
            this.mainForm = mainForm;
            blockCommentStartEndLineNumbers = new HashSet<int>();
            AcceptsTab = true;
            HideSelection = false;
            WordWrap = false;
            tabSize = 4;
            indentation = 0;
            Dock = DockStyle.Fill;
            normalFont = new Font(new FontFamily("Consolas"), 10);
            normalColor = Color.Black;
            keywordFont = new Font(new FontFamily("Consolas"), 10);
            keywordColor = Color.Blue;
            identifierFont = normalFont;
            identifierColor = normalColor;
            charFont = normalFont;
            charColor = Color.FromArgb(161, 21, 21);
            stringFont = normalFont;
            stringColor = charColor;
            numberFont = normalFont;
            numberColor = normalColor;
            commentFont = normalFont;
            commentColor = Color.Green;
            debugPosHiliteColor = Color.Yellow;
            errorPosHiliteColor = Color.LightGray;
            breakpointColor = Color.FromArgb(200, 0, 0);
            Font = normalFont;
            ForeColor = normalColor;
            rtfConverter = new SourceRtfConverter(blockCommentStartEndLineNumbers);
            rtfConverter.NormalColor = normalColor;
            rtfConverter.NormalFont = normalFont;
            rtfConverter.KeywordColor = keywordColor;
            rtfConverter.KeywordFont = keywordFont;
            rtfConverter.IdentifierColor = identifierColor;
            rtfConverter.IdentifierFont = identifierFont;
            rtfConverter.CharColor = charColor;
            rtfConverter.CharFont = charFont;
            rtfConverter.StringColor = stringColor;
            rtfConverter.StringFont = stringFont;
            rtfConverter.NumberColor = numberColor;
            rtfConverter.NumberFont = numberFont;
            rtfConverter.CommentColor = commentColor;
            rtfConverter.CommentFont = commentFont;
            sourceLineFormatter = new SourceLineFormatter(this);
            sourceLineFormatter.NormalColor = normalColor;
            sourceLineFormatter.NormalFont = normalFont;
            sourceLineFormatter.KeywordColor = keywordColor;
            sourceLineFormatter.KeywordFont = keywordFont;
            sourceLineFormatter.IdentifierColor = identifierColor;
            sourceLineFormatter.IdentifierFont = identifierFont;
            sourceLineFormatter.CharColor = charColor;
            sourceLineFormatter.CharFont = charFont;
            sourceLineFormatter.StringColor = stringColor;
            sourceLineFormatter.StringFont = stringFont;
            sourceLineFormatter.NumberColor = numberColor;
            sourceLineFormatter.NumberFont = numberFont;
            sourceLineFormatter.CommentColor = commentColor;
            sourceLineFormatter.CommentFont = commentFont;
            Text = File.ReadAllText(sourceFile.FilePath, Encoding.UTF8);
            FormatContent();
            dirty = false;
            inOnSelectionChanged = false;
            formatting = false;
            hiliting = false;
            selectingBreakpointLine = false;
            clearingBreakpointLine = false;
            startOfLine = false;
        }
        public EditorMode Mode
        {
            get 
            { 
                return mode; 
            }
            set
            {
                mode = value;
                if (mode == EditorMode.editing)
                {
                    ReadOnly = false;
                }
                else if (mode == EditorMode.debugging)
                {
                    ReadOnly = true;
                }
            }
        }
        public SourceFile SourceFile
        {
            get { return sourceFile; }
        }
        public DialogResult Close()
        {
            if (dirty)
            {
                DialogResult result = MessageBox.Show("Save changes to " + sourceFile.Name + "?", "e", MessageBoxButtons.YesNoCancel);
                if (result == DialogResult.Cancel)
                {
                    return result;
                }
                if (result == DialogResult.Yes)
                {
                    Save();
                }
                return result;
            }
            if (Closed != null)
            {
                Closed(this, new EventArgs());
            }
            return DialogResult.OK;
        }
        public void Save()
        {
            try
            {
                if (dirty)
                {
                    using (StreamWriter writer = new StreamWriter(sourceFile.FilePath, false, new UTF8Encoding(false)))
                    {
                        StringBuilder s = new StringBuilder();
                        foreach (char c in Text)
                        {
                            if (c == '\r')
                            {
                                continue;
                            }
                            if (c == '\n')
                            {
                                s.Append(Environment.NewLine);
                            }
                            else
                            {
                                s.Append(c);
                            }
                        }
                        writer.Write(s.ToString());
                    }
                    dirty = false;
                    FireDirtyStatusChanged();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        public bool Dirty
        {
            get { return dirty; }
        }
        public int TabSize
        {
            get { return tabSize; }
            set { tabSize = value; }
        }
        private void FireDirtyStatusChanged()
        {
            try
            {
                if (DirtyStatusChanged != null)
                {
                    DirtyStatusChanged(this, new EventArgs());
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private const int WM_PAINT = 15;

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);
            if (m.Msg == WM_PAINT)
            {
                FirePainted();
            }
        }
        private void FirePainted()
        {
            if (Painted != null)
            {
                Painted(this, new EventArgs());
            }
        }
        private void FirePositionChanged()
        {
            try
            {
                if (PositionChanged != null)
                {
                    int line = GetLineFromCharIndex(SelectionStart) + 1;
                    int col = SelectionStart - GetFirstCharIndexFromLine(line - 1) + 1;
                    PositionChanged(this, new PositionEventArgs(line, col));
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        public int GetCurrentLineNumber()
        {
            int charIndex = GetFirstCharIndexOfCurrentLine();
            int line = GetLineFromCharIndex(charIndex) + 1;
            return line;
        }
        public void SetCursorPos(int line, int col)
        {
            if (line >= 0 && col >= 0)
            {
                int numLines = Lines.Count();
                line = Math.Max(0, Math.Min(numLines - 1, line - 1));
                int maxCol = 0;
                if (line < numLines)
                {
                    maxCol = Lines[line].Count() - 1;
                }
                col = Math.Max(0, Math.Min(maxCol, col - 1));
                int topLine = Math.Max(0, line - GetNumVisibleLines() / 2);
                int topLineIndex = GetFirstCharIndexFromLine(topLine);
                topLineIndex += col;
                SelectionStart = topLineIndex;
                SelectionLength = 0;
                ScrollToCaret();
                int index = GetFirstCharIndexFromLine(line);
                SelectionStart = index;
                SelectionLength = 0;
                Focus();
            }
        }
        public void SelectLine(int line)
        {
            int start = GetFirstCharIndexFromLine(line - 1);
            int end = GetFirstCharIndexFromLine(line);
            Select(start, end - start);
        }
        public void SelectBreakpointLine(int line)
        {
            selectingBreakpointLine = true;
            SelectionStart = GetFirstCharIndexFromLine(line - 1);
            int selectionStart = SelectionStart;
            int selectionLength = SelectionLength;
            int start = GetFirstCharIndexFromLine(line - 1);
            int end = GetFirstCharIndexFromLine(line);
            while (start < end && Text[start] == ' ')
            {
                ++start;
            }
            while (end > start && Text[end - 1] == ' ')
            {
                --end;
            }
            if (start < end)
            {
                SelectionStart = start;
                SelectionLength = end - start;
                SelectionColor = Color.White;
                SelectionBackColor = breakpointColor;
            }
            SelectionStart = selectionStart;
            SelectionLength = selectionLength;
            selectingBreakpointLine = false;
        }
        public void ClearBreakpointLine(int line)
        {
            clearingBreakpointLine = true;
            SelectionStart = GetFirstCharIndexFromLine(line - 1);
            int selectionStart = SelectionStart;
            int selectionLength = SelectionLength;
            int start = GetFirstCharIndexFromLine(line - 1);
            int end = GetFirstCharIndexFromLine(line);
            while (start < end && Text[start] == ' ')
            {
                ++start;
            }
            while (end > start && Text[end - 1] == ' ')
            {
                --end;
            }
            if (start < end)
            {
                SelectionStart = start;
                SelectionLength = end - start;
                SelectionColor = Color.Black;
                SelectionBackColor = Color.White;
                SelectionStart = selectionStart;
                SelectionLength = selectionLength;
            }
            clearingBreakpointLine = false;
        }

        public enum Hilite
        {
            debugPos, errorPos, none
        }
        public void SetHilite(LineCol start, LineCol end, Hilite hilite)
        {
            hiliting = true;
            int selectionStart = SelectionStart;
            int selectionLength = SelectionLength;
            if (hiliteStart.Line != 0 && hiliteStart.Col != 0 && hiliteEnd.Line != 0 && hiliteEnd.Col != 0)
            {
                int firstCharIndex = GetFirstCharIndexFromLine(hiliteStart.Line - 1) + hiliteStart.Col - 1;
                int lastCharIndex = GetFirstCharIndexFromLine(hiliteEnd.Line - 1) + hiliteEnd.Col - 1;
                if (firstCharIndex >= 0 && lastCharIndex >= 0)
                {
                    SelectionStart = firstCharIndex;
                    SelectionLength = lastCharIndex - firstCharIndex;
                    SelectionBackColor = Color.White;
                }
            }
            hiliteStart = start;
            hiliteEnd = end;
            if (hiliteStart.Line != 0 && hiliteStart.Col != 0 && hiliteEnd.Line != 0 && hiliteEnd.Col != 0)
            {
                int firstCharIndex = GetFirstCharIndexFromLine(hiliteStart.Line - 1) + hiliteStart.Col - 1;
                int lastCharIndex = GetFirstCharIndexFromLine(hiliteEnd.Line - 1) + hiliteEnd.Col - 1;
                if (firstCharIndex >= 0 && lastCharIndex >= 0)
                {
                    SelectionStart = firstCharIndex;
                    SelectionLength = lastCharIndex - firstCharIndex;
                    this.hilite = hilite;
                    switch (hilite)
                    {
                        case Hilite.debugPos: SelectionBackColor = debugPosHiliteColor; break;
                        case Hilite.errorPos: SelectionBackColor = errorPosHiliteColor; break;
                        default: SelectionBackColor = Color.White; break;
                    }
                }
            }
            SelectionStart = selectionStart;
            SelectionLength = selectionLength;
            hiliting = false;
        }
        public int GetNumVisibleLines()
        {
            int numVisibleLines = Height / FontHeight;
            return numVisibleLines;
        }
        public int GetTopLineNumber()
        {
            int charIndex = GetCharIndexFromPosition(new Point(0, FontHeight / 2));
            int zeroBasedTopLineNumber = GetLineFromCharIndex(charIndex);
            return zeroBasedTopLineNumber + 1;
        }
        public int GetBottomLineNumber()
        {
            return GetTopLineNumber() + GetNumVisibleLines();
        }
        public int GetFontHeight()
        {
            return FontHeight - 1;
        }
        protected override void OnClick(EventArgs e)
        {
            try
            {
                base.OnClick(e);
                FirePositionChanged();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        protected override void OnSelectionChanged(EventArgs e)
        {
            if (!inOnSelectionChanged && !hiliting && !selectingBreakpointLine && !clearingBreakpointLine)
            {
                try
                {
                    inOnSelectionChanged = true;
                    FormatContent();
                    base.OnSelectionChanged(e);
                    FirePositionChanged();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
            else
            {
                base.OnSelectionChanged(e);
            }
        }
        protected override async void OnTextChanged(EventArgs e)
        {
            try
            {
                if (!formatting && !hiliting && !selectingBreakpointLine && !clearingBreakpointLine)
                {
                    dirty = true;
                }
                if (formatChangedLine && !selectingBreakpointLine)
                {
                    int lineStart = GetFirstCharIndexOfCurrentLine();
                    int lineIndex = GetLineFromCharIndex(lineStart);
                    int lineNumber = lineIndex + 1;
                    sourceLineFormatter.StartIndex = lineStart;
                    bool formatAll = false;
                    if (blockCommentStartEndLineNumbers.Contains(lineIndex))
                    {
                        formatAll = true;
                    }
                    else
                    {
                        if (Lines.Count() > 0)
                        {
                            string line = Lines[lineIndex];
                            SourceLine sourceLineParser = ParserRepository.Instance.SourceLineParser;
                            int selectionStart = SelectionStart;
                            int selectionLength = SelectionLength;
                            SourcePos pos = new SourcePos(sourceFile.FilePath, lineNumber, new LineCol(), new LineCol());
                            sourceLineParser.Parse(line, 0, sourceFile.FilePath, sourceLineFormatter);
                            SelectionStart = selectionStart;
                            SelectionLength = selectionLength;
                            if (sourceLineFormatter.ContainsBlockComment())
                            {
                                formatAll = true;
                            }
                        }
                    }
                    if (formatAll)
                    {
                        FormatContent();
                    }
                }
                base.OnTextChanged(e);
                FireDirtyStatusChanged();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        protected override void OnKeyDown(KeyEventArgs e)
        {
            try
            {
                if (hilite == Hilite.errorPos)
                {
                    SetHilite(new LineCol(), new LineCol(), Hilite.none);
                }
                keyDownLine = GetLineFromCharIndex(GetFirstCharIndexOfCurrentLine());
                keyDownLineCount = Lines.Count();
                startOfLine = GetFirstCharIndexOfCurrentLine() == SelectionStart;
                indentation = 0;
                if (e.KeyCode == Keys.Tab)
                {
                    int pos = SelectionStart;
                    int col = pos - GetFirstCharIndexOfCurrentLine();
                    if (!e.Shift)
                    {
                        if (SelectionLength > 0 && SelectedText.Contains("\n"))
                        {
                            int selectionStart = SelectionStart;
                            int selectionLength = SelectionLength;
                            string spaces = new string(' ', tabSize);
                            int firstLine = GetLineFromCharIndex(selectionStart);
                            int numLines = Util.LineCount(SelectedText);
                            StringBuilder newSelection = new StringBuilder(2 * SelectedText.Length);
                            for (int i = 0; i < numLines; ++i)
                            {
                                int index = firstLine + i;
                                newSelection.Append(Lines[index].Insert(0, spaces)).Append('\n');
                            }
                            SelectedText = newSelection.ToString();
                            SelectionStart = selectionStart;
                            SelectionLength = selectionLength + numLines * tabSize;
                        }
                        else
                        {
                            int numSpaces = tabSize - (col % tabSize);
                            string spaces = new string(' ', numSpaces);
                            SelectedText = spaces;
                            SelectionStart = pos + numSpaces;
                            SelectionLength = 0;
                        }
                    }
                    else
                    {
                        if (SelectionLength > 0 && SelectedText.Contains("\n"))
                        {
                            int selectionStart = SelectionStart;
                            int selectionLength = SelectionLength;
                            int firstLine = GetLineFromCharIndex(selectionStart);
                            int numLines = Util.LineCount(SelectedText);
                            int numSpaces = tabSize;
                            for (int i = 0; i < numLines; ++i)
                            {
                                string line = Lines[firstLine + i];
                                int numSpacesInLine = 0;
                                for (int j = 0; j < Math.Min(line.Length, tabSize); ++j)
                                {
                                    if (line[j] != ' ')
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        ++numSpacesInLine;
                                    }
                                }
                                if (numSpacesInLine < numSpaces)
                                {
                                    numSpaces = numSpacesInLine;
                                }
                            }
                            StringBuilder newSelection = new StringBuilder(2 * SelectedText.Length);
                            for (int i = 0; i < numLines; ++i)
                            {
                                int index = firstLine + i;
                                newSelection.Append(Lines[index].Remove(0, numSpaces)).Append('\n');
                            }
                            SelectedText = newSelection.ToString();
                            SelectionStart = selectionStart;
                            SelectionLength = selectionLength - numLines * numSpaces;
                        }
                        else
                        {
                            int numSpaces = 0;
                            for (int i = 0; i < tabSize; ++i)
                            {
                                if (Text[pos - i - 1] != ' ')
                                {
                                    break;
                                }
                                else
                                {
                                    ++numSpaces;
                                }
                            }
                            if (pos - numSpaces >= 0)
                            {
                                SelectionStart = pos - numSpaces;
                                SelectionLength = numSpaces;
                                SelectedText = "";
                            }
                        }
                    }
                    e.SuppressKeyPress = true;
                    e.Handled = true;
                }
                else if (e.KeyCode == Keys.Enter)
                {
                    int start = GetFirstCharIndexOfCurrentLine();
                    int end = start;
                    while (end < Text.Length && Text[end] == ' ')
                    {
                        ++end;
                    }
                    if (start != end)
                    {
                        indentation = end - start;
                    }
                    if (end < Text.Length && Text[end] == '{')
                    {
                        if (end + 1 >= Text.Length || Text[end + 1] != '}')
                        {
                            indentation += tabSize;
                        }
                    }
                }
                base.OnKeyDown(e);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        protected override void OnKeyUp(KeyEventArgs e)
        {
            try
            {
                base.OnKeyUp(e);
                FirePositionChanged();
                if (e.KeyCode == Keys.Enter && indentation > 0)
                {
                    SelectedText = new string(' ', indentation);
                    SelectionLength = 0;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            if (e.KeyChar == '}')
            {
                int start = GetFirstCharIndexOfCurrentLine();
                int p = SelectionStart;
                if (p < Text.Length)
                {
                    while (p > start)
                    {
                        char c = Text[p - 1];
                        if (c == '{' || c == '"' || c == '\'')
                        {
                            return;
                        }
                        --p;
                    }
                }
                int prevLineNumber = GetLineFromCharIndex(start) - 1;
                if (prevLineNumber < 0)
                {
                    return;
                }
                int prevLineStart = GetFirstCharIndexFromLine(prevLineNumber);
                int prevLineEnd = prevLineStart;
                while (prevLineEnd < Text.Length && Text[prevLineEnd] == ' ')
                {
                    ++prevLineEnd;
                }
                int end = start;
                while (end < Text.Length && Text[end] == ' ')
                {
                    ++end;
                }
                if (start != end)
                {
                    indentation = end - start;
                    int offset = 0;
                    if (indentation >= tabSize)
                    {
                        if (Text[prevLineEnd] == '{')
                        {
                            offset = prevLineEnd - prevLineStart;
                        }
                        SelectionStart = start + offset;
                        SelectionLength = tabSize;
                        SelectedText = "";
                    }
                }
            }
        }
        public void FormatContent()
        {
            if (sourceFile.GetKind() != devcore.SourceFile.Kind.cm) return;
            formatting = true;
            int selectionStart = SelectionStart;
            int selectionLength = SelectionLength;
            formatChangedLine = false;
            SourceRtf parser = ParserRepository.Instance.SourceRtfParser;
            string text = Text;
            parser.Parse(text, 0, sourceFile.FilePath, rtfConverter);
            Rtf = rtfConverter.Rtf;
            formatChangedLine = true;
            SelectionStart = selectionStart;
            SelectionLength = selectionLength;
            formatting = false;
        }
        private SourceFile sourceFile;
        private SourceRtfConverter rtfConverter;
        private SourceLineFormatter sourceLineFormatter;
        private int tabSize;
        private int indentation;
        private Font normalFont;
        private Color normalColor;
        private Font keywordFont;
        private Color keywordColor;
        private Font identifierFont;
        private Color identifierColor;
        private Font charFont;
        private Color charColor;
        private Font stringFont;
        private Color stringColor;
        private Font numberFont;
        private Color numberColor;
        private Font commentFont;
        private Color commentColor;
        private Color debugPosHiliteColor;
        private Color errorPosHiliteColor;
        private Color breakpointColor;
        private bool formatChangedLine;
        private bool dirty;
        private bool inOnSelectionChanged;
        private bool formatting;
        private HashSet<int> blockCommentStartEndLineNumbers;
        private EditorMode mode;
        private LineCol hiliteStart;
        private LineCol hiliteEnd;
        private bool hiliting;
        private bool selectingBreakpointLine;
        private bool clearingBreakpointLine;
        private int keyDownLine = -1;
        private int keyDownLineCount = -1;
        private bool startOfLine;
        private Hilite hilite;
        private MainForm mainForm;
    }

    public static class Util
    {
        public static int LineCount(string text)
        {
            int lineCount = 0;
            foreach (char c in text)
            {
                if (c == '\n')
                {
                    ++lineCount;
                }
            }
            return lineCount;
        }
    }
}
