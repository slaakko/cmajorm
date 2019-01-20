namespace cmdevenv
{
    partial class FindDialog
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
            this.label1 = new System.Windows.Forms.Label();
            this.findWhatTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.lookInComboBox = new System.Windows.Forms.ComboBox();
            this.findButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.matchWholeWordCheckBox = new System.Windows.Forms.CheckBox();
            this.matchCaseCheckBox = new System.Windows.Forms.CheckBox();
            this.regularExpressionCheckBox = new System.Windows.Forms.CheckBox();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Find what:";
            // 
            // findWhatTextBox
            // 
            this.findWhatTextBox.Location = new System.Drawing.Point(15, 25);
            this.findWhatTextBox.Name = "findWhatTextBox";
            this.findWhatTextBox.Size = new System.Drawing.Size(249, 20);
            this.findWhatTextBox.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 48);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(45, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Look in:";
            // 
            // lookInComboBox
            // 
            this.lookInComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.lookInComboBox.FormattingEnabled = true;
            this.lookInComboBox.Location = new System.Drawing.Point(15, 64);
            this.lookInComboBox.Name = "lookInComboBox";
            this.lookInComboBox.Size = new System.Drawing.Size(249, 21);
            this.lookInComboBox.TabIndex = 3;
            // 
            // findButton
            // 
            this.findButton.Location = new System.Drawing.Point(189, 231);
            this.findButton.Name = "findButton";
            this.findButton.Size = new System.Drawing.Size(75, 23);
            this.findButton.TabIndex = 4;
            this.findButton.Text = "Find";
            this.findButton.UseVisualStyleBackColor = true;
            this.findButton.Click += new System.EventHandler(this.findButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(189, 260);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 5;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.regularExpressionCheckBox);
            this.groupBox1.Controls.Add(this.matchWholeWordCheckBox);
            this.groupBox1.Controls.Add(this.matchCaseCheckBox);
            this.groupBox1.Location = new System.Drawing.Point(15, 91);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(249, 98);
            this.groupBox1.TabIndex = 6;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Find options:";
            // 
            // matchWholeWordCheckBox
            // 
            this.matchWholeWordCheckBox.AutoSize = true;
            this.matchWholeWordCheckBox.Location = new System.Drawing.Point(6, 42);
            this.matchWholeWordCheckBox.Name = "matchWholeWordCheckBox";
            this.matchWholeWordCheckBox.Size = new System.Drawing.Size(113, 17);
            this.matchWholeWordCheckBox.TabIndex = 1;
            this.matchWholeWordCheckBox.Text = "Match whole word";
            this.matchWholeWordCheckBox.UseVisualStyleBackColor = true;
            // 
            // matchCaseCheckBox
            // 
            this.matchCaseCheckBox.AutoSize = true;
            this.matchCaseCheckBox.Location = new System.Drawing.Point(6, 19);
            this.matchCaseCheckBox.Name = "matchCaseCheckBox";
            this.matchCaseCheckBox.Size = new System.Drawing.Size(82, 17);
            this.matchCaseCheckBox.TabIndex = 0;
            this.matchCaseCheckBox.Text = "Match case";
            this.matchCaseCheckBox.UseVisualStyleBackColor = true;
            // 
            // regularExpressionCheckBox
            // 
            this.regularExpressionCheckBox.AutoSize = true;
            this.regularExpressionCheckBox.Location = new System.Drawing.Point(6, 65);
            this.regularExpressionCheckBox.Name = "regularExpressionCheckBox";
            this.regularExpressionCheckBox.Size = new System.Drawing.Size(133, 17);
            this.regularExpressionCheckBox.TabIndex = 2;
            this.regularExpressionCheckBox.Text = "Use regular expression";
            this.regularExpressionCheckBox.UseVisualStyleBackColor = true;
            // 
            // FindDialog
            // 
            this.AcceptButton = this.findButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(276, 295);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.findButton);
            this.Controls.Add(this.lookInComboBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.findWhatTextBox);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FindDialog";
            this.Text = "Find";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox findWhatTextBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox lookInComboBox;
        private System.Windows.Forms.Button findButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox matchWholeWordCheckBox;
        private System.Windows.Forms.CheckBox matchCaseCheckBox;
        private System.Windows.Forms.CheckBox regularExpressionCheckBox;
    }
}