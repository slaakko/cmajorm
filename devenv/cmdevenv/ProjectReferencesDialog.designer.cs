namespace cmdevenv
{
    partial class ProjectReferencesDialog
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
            this.referencesListView = new System.Windows.Forms.ListView();
            this.removeButton = new System.Windows.Forms.Button();
            this.okButton = new System.Windows.Forms.Button();
            this.addProjectRefereceButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // referencesListView
            // 
            this.referencesListView.Dock = System.Windows.Forms.DockStyle.Top;
            this.referencesListView.FullRowSelect = true;
            this.referencesListView.Location = new System.Drawing.Point(0, 0);
            this.referencesListView.MultiSelect = false;
            this.referencesListView.Name = "referencesListView";
            this.referencesListView.Size = new System.Drawing.Size(576, 185);
            this.referencesListView.TabIndex = 0;
            this.referencesListView.UseCompatibleStateImageBehavior = false;
            this.referencesListView.View = System.Windows.Forms.View.List;
            this.referencesListView.SelectedIndexChanged += new System.EventHandler(this.referencesListView_SelectedIndexChanged);
            // 
            // removeButton
            // 
            this.removeButton.Enabled = false;
            this.removeButton.Location = new System.Drawing.Point(409, 191);
            this.removeButton.Name = "removeButton";
            this.removeButton.Size = new System.Drawing.Size(75, 23);
            this.removeButton.TabIndex = 2;
            this.removeButton.Text = "Remove...";
            this.removeButton.UseVisualStyleBackColor = true;
            this.removeButton.Click += new System.EventHandler(this.removeButton_Click);
            // 
            // okButton
            // 
            this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.okButton.Location = new System.Drawing.Point(490, 191);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(75, 23);
            this.okButton.TabIndex = 3;
            this.okButton.Text = "OK";
            this.okButton.UseVisualStyleBackColor = true;
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // addProjectRefereceButton
            // 
            this.addProjectRefereceButton.Location = new System.Drawing.Point(12, 191);
            this.addProjectRefereceButton.Name = "addProjectRefereceButton";
            this.addProjectRefereceButton.Size = new System.Drawing.Size(165, 23);
            this.addProjectRefereceButton.TabIndex = 1;
            this.addProjectRefereceButton.Text = "Add project reference...";
            this.addProjectRefereceButton.UseVisualStyleBackColor = true;
            this.addProjectRefereceButton.Click += new System.EventHandler(this.addButton_Click);
            // 
            // ProjectReferencesDialog
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(576, 220);
            this.ControlBox = false;
            this.Controls.Add(this.addProjectRefereceButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.removeButton);
            this.Controls.Add(this.referencesListView);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ProjectReferencesDialog";
            this.Text = "Project References";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView referencesListView;
        private System.Windows.Forms.Button removeButton;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button addProjectRefereceButton;
    }
}