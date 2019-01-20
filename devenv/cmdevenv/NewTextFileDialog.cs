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
    public partial class NewTextFileDialog : Form
    {
        public NewTextFileDialog()
        {
            InitializeComponent();
        }
        public string TextFileName
        {
            get
            {
                string textFileName = textFileNameTextBox.Text;
                return textFileName;
            }
            set
            {
                textFileNameTextBox.Text = value;
                textFileNameTextBox.SelectAll();
            }
        }
        private void textFileNameTextBox_TextChanged(object sender, EventArgs e)
        {
            okButton.Enabled = !string.IsNullOrEmpty(textFileNameTextBox.Text);
        }
    }
}
