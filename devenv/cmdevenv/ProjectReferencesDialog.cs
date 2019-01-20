using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using master.util.io;
using System.IO;
using devcore;

namespace cmdevenv
{
    public partial class ProjectReferencesDialog : Form
    {
        public ProjectReferencesDialog(OpenFileDialog addProjectReferenceDialog, Solution solution, Project project)
        {
            InitializeComponent();
            this.addProjectReferenceDialog = addProjectReferenceDialog;
            this.solution = solution;
            this.project = project;
            foreach (string reference in project.References)
            {
                ListViewItem item = new ListViewItem(reference);
                item.Tag = reference;
                referencesListView.Items.Add(item);
            }
        }
        private void referencesListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (referencesListView.SelectedIndices.Count > 0)
            {
                removeButton.Enabled = true;
            }
            else
            {
                removeButton.Enabled = false;
            }
        }
        private void addButton_Click(object sender, EventArgs e)
        {
            try
            {
                addProjectReferenceDialog.InitialDirectory = solution.BasePath;
                DialogResult result = addProjectReferenceDialog.ShowDialog();
                if (result == System.Windows.Forms.DialogResult.OK)
                {
                    string referencedProjectFilePath = PathUtil.GetRelativePath(addProjectReferenceDialog.FileName, project.BasePath);
                    ListViewItem item = new ListViewItem(referencedProjectFilePath);
                    referencesListView.Items.Add(item);
                    project.AddReference(referencedProjectFilePath);
                    project.Save();
                    solution.Save();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void removeButton_Click(object sender, EventArgs e)
        {
            try
            {
                ListViewItem item = referencesListView.SelectedItems[0];
                string reference = item.Text;
                if (MessageBox.Show("Project Reference '" + reference + "' will be removed", "Project References", MessageBoxButtons.OKCancel) == System.Windows.Forms.DialogResult.OK)
                {
                    project.References.Remove(reference);
                    referencesListView.Items.Remove(item);
                    project.Save();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void okButton_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.OK;
        }
        private OpenFileDialog addProjectReferenceDialog;
        private Solution solution;
        private Project project;
    }
}
