using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Build.Utilities;
using Microsoft.Build.Framework;
using System.IO;

namespace CmajorTasks
{
    internal static class SystemModuleSet
    {
        static SystemModuleSet()
        {
            systemModules = new HashSet<string>();
            systemModules.Add("System.Core");
            systemModules.Add("System.Runtime");
            systemModules.Add("System.Base");
            systemModules.Add("System.Text.Parsing.CodeDom");
            systemModules.Add("System.Text.Parsing");
            systemModules.Add("System.Net.Sockets");
            systemModules.Add("System.Net.Http");
            systemModules.Add("System.Json");
            systemModules.Add("System.Xml");
            systemModules.Add("System.Dom");
            systemModules.Add("System.Numerics.Multiprecision");
            systemModules.Add("System.IO.Compression");
            systemModules.Add("System.Text.RegularExpressions");
            systemModules.Add("System.XPath");
            systemModules.Add("System");
            systemModules.Add("System.Install");
        }
        public static bool Contains(string projectName)
        {
            return systemModules.Contains(projectName);
        }
        private static HashSet<string> systemModules;
    }

    public class ResolveModuleFilePath : Task
    {
        public override bool Execute()
        {
            if (string.IsNullOrEmpty(ProjectName))
            {
                Log.LogError("'ProjectName' is empty");
                return false;
            }
            if (string.IsNullOrEmpty(ProjectDir))
            {
                Log.LogError("'ProjectDir' is empty");
                return false;
            }
            if (string.IsNullOrEmpty(Configuration))
            {
                Log.LogError("'Configuration' is empty");
                return false;
            }
            int n = 0;
            if (ProjectReferences != null)
            {
                n = ProjectReferences.Length;
            }
            string config = "debug";
            if (Configuration == "Release")
            {
                config = "release";
            }
            List<TaskItem> moduleFilePaths = new List<TaskItem>();
            if (!SystemModuleSet.Contains(ProjectName))
            {
                string cmajorRoot = Environment.GetEnvironmentVariable("CMAJOR_ROOT");
                string systemModuleFilePath = Path.GetFullPath(Path.Combine(Path.Combine(Path.Combine(Path.Combine(cmajorRoot, "system"), "lib"), config), "System.cmm"));
                moduleFilePaths.Add(new TaskItem(systemModuleFilePath));
            }
            for (int i = 0; i < n; ++i)
            {
                ITaskItem projectReferenceTaskITem = ProjectReferences[i];
                string projectReferencePath = projectReferenceTaskITem.ItemSpec;
                string moduleFilePath = Path.GetFullPath(Path.Combine(Path.Combine(Path.Combine(Path.Combine(ProjectDir, Path.GetDirectoryName(projectReferencePath)), "lib"), config), 
                    Path.GetFileNameWithoutExtension(projectReferencePath) + ".cmm"));
                moduleFilePaths.Add(new TaskItem(moduleFilePath));
            }
            ModuleFilePaths = moduleFilePaths.ToArray();
            return true;
        }

        public string ProjectName { get; set; }

        public string ProjectDir { get; set; }

        public string Configuration { get; set; }

        public ITaskItem[] ProjectReferences { get; set; }

        [Output]
        public ITaskItem[] ModuleFilePaths { get; set; }
    }
}
