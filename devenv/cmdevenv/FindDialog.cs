using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace cmdevenv
{
    public enum FindExtent
    {
        entireSolution,
        activeProject,
        currentDocument
    }

    public partial class FindDialog : Form
    {
        public FindDialog(bool editorOpen)
        {
            InitializeComponent();
            lookInComboBox.Items.Add("Entire Solution");
            lookInComboBox.Items.Add("Active Project");
            if (editorOpen)
            {
                lookInComboBox.Items.Add("Current Document");
            }
            lookInComboBox.SelectedIndex = 0;
        }

        public string FindWhat
        {
            get { return findWhatTextBox.Text; }
            set 
            { 
                findWhatTextBox.Text = value;
                if (!string.IsNullOrEmpty(findWhatTextBox.Text))
                {
                    findWhatTextBox.SelectAll();
                }
            }
        }

        public FindExtent LookIn
        {
            get { return (FindExtent)lookInComboBox.SelectedIndex; }
        }

        public bool MatchCase
        {
            get { return matchCaseCheckBox.Checked; }
            set { matchCaseCheckBox.Checked = value; }
        }

        public bool MatchWholeWord
        {
            get { return matchWholeWordCheckBox.Checked; }
            set { matchWholeWordCheckBox.Checked = value; }
        }

        public bool UseRegularExpression
        {
            get { return regularExpressionCheckBox.Checked; }
            set { regularExpressionCheckBox.Checked = value; }
        }

        private void findButton_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.OK;
        }
    }
}
