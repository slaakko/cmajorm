using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Windows.Forms;
using server;

namespace cmdevenv
{
    public class ConsoleWindow : RichTextBox
    {
        public ConsoleWindow(Executor executor)
        {
            this.executor = executor;
            BackColor = Color.FromKnownColor(KnownColor.Control);
            ReadOnly = true;
            Dock = DockStyle.Fill;
            Font = new Font(new FontFamily("Consolas"), 8.25f);
            inputStartPos = 0;
            inputLines = new List<string>();
            inputLineIndex = -1;
        }
        public void Write(string chars)
        {
            AppendText(chars);
            ScrollToCaret();
            inputStartPos = SelectionStart;
        }
        protected override void OnKeyDown(KeyEventArgs e)
        {
            try
            {
                if (executor.InputExpected())
                {
                    if (e.KeyCode == Keys.Enter)
                    {
                        SelectionStart = Text.Length;
                        string line = Text.Substring(inputStartPos, SelectionStart - inputStartPos);
                        SelectionStart = inputStartPos;
                        SelectionLength = Text.Length - inputStartPos;
                        SelectedText = "";
                        inputLines.Add(line);
                        inputLineIndex = inputLines.Count;
                        string bufferedOutput = executor.WriteLineToProcessStandardInput(line);
                        Write(bufferedOutput + line);
                        inputStartPos = GetFirstCharIndexOfCurrentLine();
                    }
                    else if (e.KeyCode == Keys.Z && e.Control)
                    {
                        string bufferedOutput = executor.CloseProcessStandardInput();
                        Text = Text + bufferedOutput + "<EOF>\n";
                    }
                    else if (e.KeyCode == Keys.Home)
                    {
                        SelectionStart = inputStartPos;
                        e.Handled = true;
                    }
                    else if (e.KeyCode == Keys.Left)
                    {
                        if (SelectionStart <= inputStartPos)
                        {
                            e.Handled = true;
                            Console.Beep();
                        }
                    }
                    else if (e.KeyCode == Keys.Back)
                    {
                        if (SelectionStart <= inputStartPos)
                        {
                            e.Handled = true;
                            Console.Beep();
                        }
                    }
                    else if (e.KeyCode == Keys.Up)
                    {
                        e.Handled = true;
                        if (inputLineIndex <= 0 || inputLineIndex > inputLines.Count)
                        {
                            Console.Beep();
                        }
                        else
                        {
                            --inputLineIndex;
                            SelectionStart = inputStartPos;
                            SelectionLength = Text.Length - inputStartPos;
                            SelectedText = inputLines[inputLineIndex];
                        }
                    }
                    else if (e.KeyCode == Keys.Down)
                    {
                        e.Handled = true;
                        if (inputLineIndex < 0 || inputLineIndex >= inputLines.Count)
                        {
                            Console.Beep();
                        }
                        else
                        {
                            ++inputLineIndex;
                            SelectionStart = inputStartPos;
                            SelectionLength = Text.Length - inputStartPos;
                            if (inputLineIndex < inputLines.Count)
                            {
                                SelectedText = inputLines[inputLineIndex];
                            }
                            else
                            {
                                SelectedText = "";
                            }
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
        private Executor executor;
        private int inputStartPos;
        private List<string> inputLines;
        private int inputLineIndex;
    }
}
