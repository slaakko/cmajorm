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
    public partial class ProfileDialog : Form
    {
        public ProfileDialog()
        {
            InitializeComponent();
            topComboBox.SelectedIndex = 0;
        }

        private void profileButton_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.OK;
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }

        public bool RebuildSys
        {
            get { return rebuildSysCheckBox.Checked; }
        }
        public bool RebuildApp
        {
            get { return rebuildAppCheckBox.Checked; }
        }
        public bool Inclusive
        {
            get { return elapsedTimeInclusiveCheckBox.Checked; }
        }
        public bool Exclusive
        {
            get { return elapsedTimeExclusiveCheckBox.Checked; }
        }
        public bool Count
        {
            get { return executionCountCheckBox.Checked; }
        }
        public string Arguments
        {
            get { return argumentsTextBox.Text; }
        }
        public new int Top
        {
            get
            {
                if (topComboBox.Text == "*")
                {
                    return 0;
                }
                else
                {
                    return int.Parse(topComboBox.Text);
                }
            }
        }
    }
}
