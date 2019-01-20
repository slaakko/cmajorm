namespace cmdevenv
{
    partial class ProfileDialog
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
            this.profileButton = new System.Windows.Forms.Button();
            this.topComboBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.argumentsTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.executionCountCheckBox = new System.Windows.Forms.CheckBox();
            this.elapsedTimeExclusiveCheckBox = new System.Windows.Forms.CheckBox();
            this.elapsedTimeInclusiveCheckBox = new System.Windows.Forms.CheckBox();
            this.cancelButton = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.rebuildAppCheckBox = new System.Windows.Forms.CheckBox();
            this.rebuildSysCheckBox = new System.Windows.Forms.CheckBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // profileButton
            // 
            this.profileButton.Location = new System.Drawing.Point(138, 212);
            this.profileButton.Name = "profileButton";
            this.profileButton.Size = new System.Drawing.Size(75, 23);
            this.profileButton.TabIndex = 0;
            this.profileButton.Text = "Profile";
            this.profileButton.UseVisualStyleBackColor = true;
            this.profileButton.Click += new System.EventHandler(this.profileButton_Click);
            // 
            // topComboBox
            // 
            this.topComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.topComboBox.FormattingEnabled = true;
            this.topComboBox.Items.AddRange(new object[] {
            "10",
            "20",
            "50",
            "100",
            "200",
            "500",
            "1000",
            "*"});
            this.topComboBox.Location = new System.Drawing.Point(77, 212);
            this.topComboBox.Name = "topComboBox";
            this.topComboBox.Size = new System.Drawing.Size(55, 21);
            this.topComboBox.TabIndex = 4;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 217);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(37, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "Show:";
            // 
            // argumentsTextBox
            // 
            this.argumentsTextBox.Location = new System.Drawing.Point(77, 181);
            this.argumentsTextBox.Name = "argumentsTextBox";
            this.argumentsTextBox.Size = new System.Drawing.Size(215, 20);
            this.argumentsTextBox.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(11, 184);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(60, 13);
            this.label2.TabIndex = 7;
            this.label2.Text = "Arguments:";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.executionCountCheckBox);
            this.groupBox1.Controls.Add(this.elapsedTimeExclusiveCheckBox);
            this.groupBox1.Controls.Add(this.elapsedTimeInclusiveCheckBox);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(280, 91);
            this.groupBox1.TabIndex = 8;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Report";
            // 
            // executionCountCheckBox
            // 
            this.executionCountCheckBox.AutoSize = true;
            this.executionCountCheckBox.Checked = true;
            this.executionCountCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.executionCountCheckBox.Location = new System.Drawing.Point(6, 65);
            this.executionCountCheckBox.Name = "executionCountCheckBox";
            this.executionCountCheckBox.Size = new System.Drawing.Size(104, 17);
            this.executionCountCheckBox.TabIndex = 6;
            this.executionCountCheckBox.Text = "Execution Count";
            this.executionCountCheckBox.UseVisualStyleBackColor = true;
            // 
            // elapsedTimeExclusiveCheckBox
            // 
            this.elapsedTimeExclusiveCheckBox.AutoSize = true;
            this.elapsedTimeExclusiveCheckBox.Checked = true;
            this.elapsedTimeExclusiveCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.elapsedTimeExclusiveCheckBox.Location = new System.Drawing.Point(6, 42);
            this.elapsedTimeExclusiveCheckBox.Name = "elapsedTimeExclusiveCheckBox";
            this.elapsedTimeExclusiveCheckBox.Size = new System.Drawing.Size(138, 17);
            this.elapsedTimeExclusiveCheckBox.TabIndex = 5;
            this.elapsedTimeExclusiveCheckBox.Text = "Elapsed Time Exclusive";
            this.elapsedTimeExclusiveCheckBox.UseVisualStyleBackColor = true;
            // 
            // elapsedTimeInclusiveCheckBox
            // 
            this.elapsedTimeInclusiveCheckBox.AutoSize = true;
            this.elapsedTimeInclusiveCheckBox.Checked = true;
            this.elapsedTimeInclusiveCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.elapsedTimeInclusiveCheckBox.Location = new System.Drawing.Point(6, 19);
            this.elapsedTimeInclusiveCheckBox.Name = "elapsedTimeInclusiveCheckBox";
            this.elapsedTimeInclusiveCheckBox.Size = new System.Drawing.Size(135, 17);
            this.elapsedTimeInclusiveCheckBox.TabIndex = 4;
            this.elapsedTimeInclusiveCheckBox.Text = "Elapsed Time Inclusive";
            this.elapsedTimeInclusiveCheckBox.UseVisualStyleBackColor = true;
            // 
            // cancelButton
            // 
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(217, 212);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 9;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.rebuildAppCheckBox);
            this.groupBox2.Controls.Add(this.rebuildSysCheckBox);
            this.groupBox2.Location = new System.Drawing.Point(12, 105);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(280, 68);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Rebuild";
            // 
            // rebuildAppCheckBox
            // 
            this.rebuildAppCheckBox.AutoSize = true;
            this.rebuildAppCheckBox.Location = new System.Drawing.Point(6, 42);
            this.rebuildAppCheckBox.Name = "rebuildAppCheckBox";
            this.rebuildAppCheckBox.Size = new System.Drawing.Size(78, 17);
            this.rebuildAppCheckBox.TabIndex = 5;
            this.rebuildAppCheckBox.Text = "Application";
            this.rebuildAppCheckBox.UseVisualStyleBackColor = true;
            // 
            // rebuildSysCheckBox
            // 
            this.rebuildSysCheckBox.AutoSize = true;
            this.rebuildSysCheckBox.Location = new System.Drawing.Point(6, 19);
            this.rebuildSysCheckBox.Name = "rebuildSysCheckBox";
            this.rebuildSysCheckBox.Size = new System.Drawing.Size(60, 17);
            this.rebuildSysCheckBox.TabIndex = 4;
            this.rebuildSysCheckBox.Text = "System";
            this.rebuildSysCheckBox.UseVisualStyleBackColor = true;
            // 
            // ProfileDialog
            // 
            this.AcceptButton = this.profileButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(304, 249);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.argumentsTextBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.topComboBox);
            this.Controls.Add(this.profileButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ProfileDialog";
            this.Text = "Profile";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button profileButton;
        private System.Windows.Forms.ComboBox topComboBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox argumentsTextBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox executionCountCheckBox;
        private System.Windows.Forms.CheckBox elapsedTimeExclusiveCheckBox;
        private System.Windows.Forms.CheckBox elapsedTimeInclusiveCheckBox;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.CheckBox rebuildAppCheckBox;
        private System.Windows.Forms.CheckBox rebuildSysCheckBox;
    }
}