using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using devcore;
using master.xml;
using System.Windows.Forms;

namespace cmdevenv
{
    public partial class ProjectPropertiesDialog : Form
    {
        public ProjectPropertiesDialog(Project project)
        {
            this.project = project;
            projectConfig = ProjectConfigurations.Instance.GetProjectConfig(project.GetConfigurationFilePath());
            InitializeComponent();
            configurationComboBox.SelectedIndex = 0;
            SetTextBoxes();
            dirty = false;
        }
        private void configurationComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (dirty)
            {
                if (MessageBox.Show("Save changes?", "Confirmation", MessageBoxButtons.YesNo) == System.Windows.Forms.DialogResult.Yes)
                {
                    Save();
                }
            }
            SetTextBoxes();
        }
        private void okButton_Click(object sender, EventArgs e)
        {
            try
            {
                if (dirty)
                {
                    Save();
                }
                DialogResult = System.Windows.Forms.DialogResult.OK;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void cancelButton_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.Cancel;
        }
        private void Save()
        {
            try
            {
                string configurationName = configurationComboBox.Text;
                projectConfig.SetCommandLineArguments(configurationName, commandLineArgumentsTextBox.Text);
                projectConfig.SetConditionalCompilationSymbols(configurationName, conditionalCompilationSymbolsTextBox.Text);
                ProjectConfigurations.Instance.SaveProjectConfig(project.GetConfigurationFilePath(), projectConfig);
                dirty = false;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void SetTextBoxes()
        {
            string configurationName = configurationComboBox.Text;
            string commandLineArguments = projectConfig.GetCommandLineArguments(configurationName);
            if (!string.IsNullOrEmpty(commandLineArguments))
            {
                commandLineArgumentsTextBox.Text = commandLineArguments;
            }
            else
            {
                commandLineArgumentsTextBox.Text = "";
            }
            string conditionalCompilationSymbols = projectConfig.GetConditionalCompilationSymbols(configurationName);
            if (!string.IsNullOrEmpty(conditionalCompilationSymbols))
            {
                conditionalCompilationSymbolsTextBox.Text = conditionalCompilationSymbols;
            }
            else
            {
                conditionalCompilationSymbolsTextBox.Text = "";
            }
        }
        private void commandLineArgumentsTextBox_TextChanged(object sender, EventArgs e)
        {
            dirty = true;
        }
        private void conditionalCompilationSymbolsTextBox_TextChanged(object sender, EventArgs e)
        {
            dirty = true;
        }
        private Project project;
        private ProjectConfig projectConfig;
        private bool dirty;
    }
}
