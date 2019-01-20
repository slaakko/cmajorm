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
    public partial class TextVisualizerForm : Form
    {
        public TextVisualizerForm()
        {
            InitializeComponent();
        }

        public string TextContent
        {
            get { return textRichTextBox.Text; }
            set { textRichTextBox.Text = value; }
        }

        private void closeButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }
    }
}
