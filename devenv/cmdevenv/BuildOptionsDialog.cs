using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using devcore;
using server;

namespace cmdevenv
{
    public partial class BuildOptionsDialog : Form
    {
        public BuildOptionsDialog(CompilerLib compilerLib)
        {
            InitializeComponent();
            this.compilerLib = compilerLib;
        }
        public bool StrictNothrow
        {
            get { return strictNothrowCheckBox.Checked; }
            set { strictNothrowCheckBox.Checked = value; }
        }
        public bool EmitLlvm
        {
            get { return emitLlvmCheckBox.Checked; }
            set { emitLlvmCheckBox.Checked = value; }
        }
        public bool EmitOptLlvm
        {
            get { return emitOptLlvmCheckBox.Checked; }
            set { emitOptLlvmCheckBox.Checked = value; }
        }
        public bool LinkWithDebugRuntime
        {
            get { return linkWithDebugRuntimeCheckBox.Checked; }
            set { linkWithDebugRuntimeCheckBox.Checked = value; }
        }
        public bool LinkUsingMsLink
        {
            get { return linkUsingMsLinkCheckBox.Checked; }
            set { linkUsingMsLinkCheckBox.Checked = value; }
        }
        public bool DoNotUseModuleCache
        {
            get { return doNotUseModuleCacheCheckBox.Checked; }
            set { doNotUseModuleCacheCheckBox.Checked = value; }
        }
        public int NumBuildThreads
        {
            get { return buildThreadsComboBox.SelectedIndex; }
            set { buildThreadsComboBox.SelectedIndex = value; }
        }
        private void okButton_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.OK;
        }
        private void resetModuleCacheButton_Click(object sender, EventArgs e)
        {
            compilerLib.DoResetModuleCache();
            MessageBox.Show("Module cache reset");
        }
        private CompilerLib compilerLib;
    }
}
