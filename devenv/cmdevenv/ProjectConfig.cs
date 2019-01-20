using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Windows.Forms;
using master.util.pattern;
using master.xml;

namespace cmdevenv
{
    public class ProjectConfigurations : Singleton<ProjectConfigurations>
    {
        public ProjectConfigurations()
        {
            Registry.Instance.Register(typeof(ProjectConfig), "cmdevenv.ProjectConfig");
        }
        public ProjectConfig GetProjectConfig(string filePath)
        {
            try
            {
                if (File.Exists(filePath))
                {
                    using (XmlTextReader reader = new XmlTextReader(filePath))
                    {
                        ProjectConfig config = (ProjectConfig)Reader.Read(reader);
                        return config;
                    }
                }
                return new ProjectConfig();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            return null;
        }
        public void SaveProjectConfig(string filePath, ProjectConfig config)
        {
            try
            {
                using (XmlTextWriter writer = new XmlTextWriter(filePath, Encoding.UTF8))
                {
                    writer.Formatting = Formatting.Indented;
                    writer.Indentation = 4;
                    config.Write(writer);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
    }

    public class ProjectConfig : Class
    {
        public ProjectConfig()
        {
            configurations = new Array<Config>("configurations", "configuration", Fields);
        }
        private class Config : Class
        {
            public Config()
            {
                name = new StringField("name", Fields);
                commandLineArguments = new StringField("commandLineArguments", Fields);
                conditionalCompilationSymbols = new StringField("conditionalCompilationSymbols", Fields);
            }
            public string ConfigName
            {
                get { return name.Value; }
                set { name.Value = value; }
            }
            public string CommandLineArguments
            {
                get { return commandLineArguments.Value; }
                set { commandLineArguments.Value = value; }
            }
            public string ConditionalCompilationSymbols
            {
                get { return conditionalCompilationSymbols.Value; }
                set { conditionalCompilationSymbols.Value = value; }
            }
            private StringField name;
            private StringField commandLineArguments;
            private StringField conditionalCompilationSymbols;
        }
        public string GetCommandLineArguments(string configName)
        {
            foreach (Config config in configurations)
            {
                if (config.ConfigName == configName)
                {
                    return config.CommandLineArguments;
                }
            }
            return null;
        }
        public void SetCommandLineArguments(string configName, string commandLineArguments)
        {
            foreach (Config config in configurations)
            {
                if (config.ConfigName == configName)
                {
                    config.CommandLineArguments = commandLineArguments;
                    return;
                }
            }
            Config newConfig = new Config();
            newConfig.ConfigName = configName;
            newConfig.CommandLineArguments = commandLineArguments;
            configurations.Add(newConfig);
        }
        public string GetConditionalCompilationSymbols(string configName)
        {
            foreach (Config config in configurations)
            {
                if (config.ConfigName == configName)
                {
                    return config.ConditionalCompilationSymbols;
                }
            }
            return null;
        }
        public void SetConditionalCompilationSymbols(string configName, string conditionalCompilationSymbols)
        {
            foreach (Config config in configurations)
            {
                if (config.ConfigName == configName)
                {
                    config.ConditionalCompilationSymbols = conditionalCompilationSymbols;
                    return;
                }
            }
            Config newConfig = new Config();
            newConfig.ConfigName = configName;
            newConfig.ConditionalCompilationSymbols = conditionalCompilationSymbols;
            configurations.Add(newConfig);
        }
        private Array<Config> configurations;
    }
}
