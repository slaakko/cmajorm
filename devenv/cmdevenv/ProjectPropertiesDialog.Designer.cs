namespace cmdevenv
{
    partial class ProjectPropertiesDialog
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
            this.configurationComboBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.commandLineArgumentsTextBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.conditionalCompilationSymbolsTextBox = new System.Windows.Forms.TextBox();
            this.okButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.SuspendLayout();
            // 
            // configurationComboBox
            // 
            this.configurationComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.configurationComboBox.FormattingEnabled = true;
            this.configurationComboBox.Items.AddRange(new object[] {
            "debug",
            "release",
            "profile"});
            this.configurationComboBox.Location = new System.Drawing.Point(192, 12);
            this.configurationComboBox.Name = "configurationComboBox";
            this.configurationComboBox.Size = new System.Drawing.Size(255, 21);
            this.configurationComboBox.TabIndex = 0;
            this.configurationComboBox.SelectedIndexChanged += new System.EventHandler(this.configurationComboBox_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(72, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Configuration:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 42);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(133, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Command Line Arguments:";
            // 
            // commandLineArgumentsTextBox
            // 
            this.commandLineArgumentsTextBox.Location = new System.Drawing.Point(192, 39);
            this.commandLineArgumentsTextBox.Name = "commandLineArgumentsTextBox";
            this.commandLineArgumentsTextBox.Size = new System.Drawing.Size(255, 20);
            this.commandLineArgumentsTextBox.TabIndex = 3;
            this.commandLineArgumentsTextBox.TextChanged += new System.EventHandler(this.commandLineArgumentsTextBox_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 68);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(161, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "Conditional Compilation Symbols:";
            // 
            // conditionalCompilationSymbolsTextBox
            // 
            this.conditionalCompilationSymbolsTextBox.Location = new System.Drawing.Point(192, 65);
            this.conditionalCompilationSymbolsTextBox.Name = "conditionalCompilationSymbolsTextBox";
            this.conditionalCompilationSymbolsTextBox.Size = new System.Drawing.Size(255, 20);
            this.conditionalCompilationSymbolsTextBox.TabIndex = 5;
            this.toolTip1.SetToolTip(this.conditionalCompilationSymbolsTextBox, "Conditional compilation symbols separated by space characters.");
            this.conditionalCompilationSymbolsTextBox.TextChanged += new System.EventHandler(this.conditionalCompilationSymbolsTextBox_TextChanged);
            // 
            // okButton
            // 
            this.okButton.Location = new System.Drawing.Point(291, 101);
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
            this.cancelButton.Location = new System.Drawing.Point(372, 101);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 7;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // ProjectPropertiesDialog
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(460, 139);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.conditionalCompilationSymbolsTextBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.commandLineArgumentsTextBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.configurationComboBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ProjectPropertiesDialog";
            this.Text = "Project Properties";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox configurationComboBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox commandLineArgumentsTextBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox conditionalCompilationSymbolsTextBox;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.ToolTip toolTip1;
    }
}