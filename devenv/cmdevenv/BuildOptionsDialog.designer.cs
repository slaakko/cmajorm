namespace cmdevenv
{
    partial class BuildOptionsDialog
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.okButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.emitOptLlvmCheckBox = new System.Windows.Forms.CheckBox();
            this.emitLlvmCheckBox = new System.Windows.Forms.CheckBox();
            this.linkWithDebugRuntimeCheckBox = new System.Windows.Forms.CheckBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.doNotUseModuleCacheCheckBox = new System.Windows.Forms.CheckBox();
            this.linkUsingMsLinkCheckBox = new System.Windows.Forms.CheckBox();
            this.strictNothrowCheckBox = new System.Windows.Forms.CheckBox();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.buildThreadsComboBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.resetModuleCacheButton = new System.Windows.Forms.Button();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // okButton
            // 
            this.okButton.Location = new System.Drawing.Point(92, 248);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(75, 23);
            this.okButton.TabIndex = 6;
            this.okButton.Text = "OK";
            this.okButton.UseVisualStyleBackColor = true;
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(173, 248);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 7;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            // 
            // emitOptLlvmCheckBox
            // 
            this.emitOptLlvmCheckBox.AutoSize = true;
            this.emitOptLlvmCheckBox.Location = new System.Drawing.Point(17, 74);
            this.emitOptLlvmCheckBox.Name = "emitOptLlvmCheckBox";
            this.emitOptLlvmCheckBox.Size = new System.Drawing.Size(211, 17);
            this.emitOptLlvmCheckBox.TabIndex = 1;
            this.emitOptLlvmCheckBox.Text = "Emit optimized LLVM intermediate code";
            this.toolTip1.SetToolTip(this.emitOptLlvmCheckBox, "Emit optimized LLVM intermediate code to .opt.ll files while compiling.");
            this.emitOptLlvmCheckBox.UseVisualStyleBackColor = true;
            // 
            // emitLlvmCheckBox
            // 
            this.emitLlvmCheckBox.AutoSize = true;
            this.emitLlvmCheckBox.Location = new System.Drawing.Point(17, 51);
            this.emitLlvmCheckBox.Name = "emitLlvmCheckBox";
            this.emitLlvmCheckBox.Size = new System.Drawing.Size(164, 17);
            this.emitLlvmCheckBox.TabIndex = 0;
            this.emitLlvmCheckBox.Text = "Emit LLVM intermediate code";
            this.toolTip1.SetToolTip(this.emitLlvmCheckBox, "Emit LLVM intermediate code to .ll files while compiling.");
            this.emitLlvmCheckBox.UseVisualStyleBackColor = true;
            // 
            // linkWithDebugRuntimeCheckBox
            // 
            this.linkWithDebugRuntimeCheckBox.AutoSize = true;
            this.linkWithDebugRuntimeCheckBox.Location = new System.Drawing.Point(17, 97);
            this.linkWithDebugRuntimeCheckBox.Name = "linkWithDebugRuntimeCheckBox";
            this.linkWithDebugRuntimeCheckBox.Size = new System.Drawing.Size(138, 17);
            this.linkWithDebugRuntimeCheckBox.TabIndex = 0;
            this.linkWithDebugRuntimeCheckBox.Text = "Link with debug runtime";
            this.toolTip1.SetToolTip(this.linkWithDebugRuntimeCheckBox, "Link with the debug version of the runtime library cmrt200d.dll.");
            this.linkWithDebugRuntimeCheckBox.UseVisualStyleBackColor = true;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.doNotUseModuleCacheCheckBox);
            this.groupBox3.Controls.Add(this.linkUsingMsLinkCheckBox);
            this.groupBox3.Controls.Add(this.linkWithDebugRuntimeCheckBox);
            this.groupBox3.Controls.Add(this.emitOptLlvmCheckBox);
            this.groupBox3.Controls.Add(this.strictNothrowCheckBox);
            this.groupBox3.Controls.Add(this.emitLlvmCheckBox);
            this.groupBox3.Location = new System.Drawing.Point(14, 13);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(236, 173);
            this.groupBox3.TabIndex = 14;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Compiling options";
            // 
            // doNotUseModuleCacheCheckBox
            // 
            this.doNotUseModuleCacheCheckBox.AutoSize = true;
            this.doNotUseModuleCacheCheckBox.Location = new System.Drawing.Point(17, 143);
            this.doNotUseModuleCacheCheckBox.Name = "doNotUseModuleCacheCheckBox";
            this.doNotUseModuleCacheCheckBox.Size = new System.Drawing.Size(131, 17);
            this.doNotUseModuleCacheCheckBox.TabIndex = 3;
            this.doNotUseModuleCacheCheckBox.Text = "Disable module cache";
            this.toolTip1.SetToolTip(this.doNotUseModuleCacheCheckBox, "Don\'t cache recently built modules.");
            this.doNotUseModuleCacheCheckBox.UseVisualStyleBackColor = true;
            // 
            // linkUsingMsLinkCheckBox
            // 
            this.linkUsingMsLinkCheckBox.AutoSize = true;
            this.linkUsingMsLinkCheckBox.Location = new System.Drawing.Point(17, 120);
            this.linkUsingMsLinkCheckBox.Name = "linkUsingMsLinkCheckBox";
            this.linkUsingMsLinkCheckBox.Size = new System.Drawing.Size(197, 17);
            this.linkUsingMsLinkCheckBox.TabIndex = 2;
            this.linkUsingMsLinkCheckBox.Text = "Use Microsoft\'s link.exe as the linker";
            this.toolTip1.SetToolTip(this.linkUsingMsLinkCheckBox, "Use link.exe as the linker instead of LLVM\'s lld-link.exe. \r\nThen link.exe must b" +
        "e found from PATH.\r\n");
            this.linkUsingMsLinkCheckBox.UseVisualStyleBackColor = true;
            // 
            // strictNothrowCheckBox
            // 
            this.strictNothrowCheckBox.AutoSize = true;
            this.strictNothrowCheckBox.Location = new System.Drawing.Point(17, 28);
            this.strictNothrowCheckBox.Name = "strictNothrowCheckBox";
            this.strictNothrowCheckBox.Size = new System.Drawing.Size(91, 17);
            this.strictNothrowCheckBox.TabIndex = 0;
            this.strictNothrowCheckBox.Text = "Strict nothrow";
            this.toolTip1.SetToolTip(this.strictNothrowCheckBox, "Treat calling a function that can throw from a nothrow function as an error.\r\n");
            this.strictNothrowCheckBox.UseVisualStyleBackColor = true;
            // 
            // toolTip1
            // 
            this.toolTip1.ShowAlways = true;
            // 
            // buildThreadsComboBox
            // 
            this.buildThreadsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.buildThreadsComboBox.FormattingEnabled = true;
            this.buildThreadsComboBox.Items.AddRange(new object[] {
            "Default",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "11",
            "12",
            "13",
            "14",
            "15",
            "16"});
            this.buildThreadsComboBox.Location = new System.Drawing.Point(135, 221);
            this.buildThreadsComboBox.Name = "buildThreadsComboBox";
            this.buildThreadsComboBox.Size = new System.Drawing.Size(113, 21);
            this.buildThreadsComboBox.TabIndex = 3;
            this.toolTip1.SetToolTip(this.buildThreadsComboBox, "Default is 2 x number of cores");
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 224);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(122, 13);
            this.label1.TabIndex = 15;
            this.label1.Text = "Number of build threads:";
            // 
            // resetModuleCacheButton
            // 
            this.resetModuleCacheButton.Location = new System.Drawing.Point(135, 192);
            this.resetModuleCacheButton.Name = "resetModuleCacheButton";
            this.resetModuleCacheButton.Size = new System.Drawing.Size(115, 23);
            this.resetModuleCacheButton.TabIndex = 16;
            this.resetModuleCacheButton.Text = "Reset module cache";
            this.resetModuleCacheButton.UseVisualStyleBackColor = true;
            this.resetModuleCacheButton.Click += new System.EventHandler(this.resetModuleCacheButton_Click);
            // 
            // BuildOptionsDialog
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(260, 284);
            this.Controls.Add(this.resetModuleCacheButton);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.buildThreadsComboBox);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "BuildOptionsDialog";
            this.Text = "Build Options";
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.CheckBox emitOptLlvmCheckBox;
        private System.Windows.Forms.CheckBox emitLlvmCheckBox;
        private System.Windows.Forms.CheckBox linkWithDebugRuntimeCheckBox;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.CheckBox strictNothrowCheckBox;
        private System.Windows.Forms.CheckBox linkUsingMsLinkCheckBox;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.ComboBox buildThreadsComboBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox doNotUseModuleCacheCheckBox;
        private System.Windows.Forms.Button resetModuleCacheButton;
    }
}