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
    public partial class NewResourceFileDialog : Form
    {
        public NewResourceFileDialog()
        {
            InitializeComponent();
        }

        public string ResourceFileName
        {
            get
            {
                string resourceFileName = sourceFileNameTextBox.Text;
                return resourceFileName.EndsWith(".xml") ? resourceFileName : resourceFileName + ".xml";
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
