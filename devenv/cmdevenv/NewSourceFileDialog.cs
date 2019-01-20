using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace cmdevenv
{
    public partial class NewSourceFileDialog : Form
    {
        public NewSourceFileDialog()
        {
            InitializeComponent();
        }
        public string SourceFileName
        {
            get 
            {
                string sourceFileName = sourceFileNameTextBox.Text;
                return sourceFileName.EndsWith(".cm") ? sourceFileName : sourceFileName + ".cm";
            }
            set 
            { 
                sourceFileNameTextBox.Text = value; 
                sourceFileNameTextBox.SelectAll();  
            }
        }
        private void sourceFileNameTextBox_TextChanged(object sender, EventArgs e)
        {
            okButton.Enabled = !string.IsNullOrEmpty(sourceFileNameTextBox.Text);
        }
    }
}
