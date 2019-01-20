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
    public partial class GotoDialog : Form
    {
        public GotoDialog()
        {
            InitializeComponent();
        }
        private void okButton_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.OK;
        }
        private void lineTextBox_TextChanged(object sender, EventArgs e)
        {
            try
            {
                line = int.Parse(lineTextBox.Text);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        public int Line
        {
            get { return line; }
        }
        private int line;
    }
}
