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
    public enum ProjectType
    {
        consoleApp, winguiapp, winapp, library, winlib, unitTestProject, blankSolution
    }
    public partial class NewProjectDialog : Form
    {
        public NewProjectDialog(bool solution, bool solutionOpen)
        {
            InitializeComponent();
            projectItemsListView.SelectedIndices.Add(0);
            selectedProjectType = ProjectType.consoleApp;
            this.solution = solution;
            if (solution)
            {
                Text = "New Project";
                projectItemsListView.Items.Add(new ListViewItem("Blank Solution", 0));
                solutionComboBox.SelectedIndex = 0;
                if (solutionOpen)
                {
                    solutionComboBox.Items.Add("Add to solution");
                }
                solutionLabel.Show();
                solutionComboBox.Show();
            }
            SetButtonState();
        }
        private void okButton_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.OK;
        }
        public string ProjectName
        {
            get { return nameTextBox.Text; }
        }
        public string ProjectLocation
        {
            get { return locationTextBox.Text; }
            set { locationTextBox.Text = value; }
        }
        private void projectItemsListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (projectItemsListView.SelectedIndices.Count > 0)
            {
                selectedProjectType = (ProjectType)projectItemsListView.SelectedIndices[0];
            }
        }
        public ProjectType SelectedProjectType
        {
            get { return selectedProjectType; }
        }
        public bool NewSolution
        {
            get { return solutionComboBox.SelectedIndex == 0; }
        }
        private void SetButtonState()
        {
            okButton.Enabled = !string.IsNullOrEmpty(nameTextBox.Text) && !string.IsNullOrEmpty(locationTextBox.Text);
        }
        private void nameTextBox_TextChanged(object sender, EventArgs e)
        {
            SetButtonState();
        }
        private void locationTextBox_TextChanged(object sender, EventArgs e)
        {
            SetButtonState();
        }
        private void browseButton_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(locationTextBox.Text))
            {
                folderBrowserDialog.SelectedPath = locationTextBox.Text;
            }
            DialogResult result = folderBrowserDialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                locationTextBox.Text = folderBrowserDialog.SelectedPath;
            }
        }
        private bool solution;
        private ProjectType selectedProjectType;
    }
}
