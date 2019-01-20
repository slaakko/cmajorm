using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using master.util.io;

namespace devcore
{
    public class Solution
    {
        public Solution(string name, string filePath)
        {
            this.name = name;
            this.filePath = PathUtil.MakeCanonicalPath(filePath);
            basePath = PathUtil.MakeCanonicalPath(Path.GetDirectoryName(filePath));
            projects = new List<Project>();
        }
        public string Name
        {
            get { return name; }
            set { name = value; }
        }
        public string FilePath
        {
            get { return filePath; }
        }
        public string BasePath
        {
            get { return basePath; }
        }
        public List<Project> Projects
        {
            get { return projects; }
        }
        public Project ActiveProject
        {
            get { return activeProject; }
            set { activeProject = value; }
        }
        public Project GetProject(string projectName)
        {
            foreach (Project project in projects)
            {
                if (project.Name == projectName)
                {
                    return project;
                }
            }
            return null;
        }
        public Project GetProjectByFilePath(string projectFilePath)
        {
            foreach (Project project in projects)
            {
                if (project.FilePath == projectFilePath)
                {
                    return project;
                }
            }
            return null;
        }
        public string GetConfigurationFilePath()
        {
            return Path.ChangeExtension(filePath, ".cms.usr");
        }
        public void AddProject(Project project)
        {
            foreach (Project p in projects)
            { 
                if (p.Name == project.Name)
                {
                    throw new Exception("solution '" + name + "' already has a project named '" + project.Name + "'");
                }
            }
            projects.Add(project);
        }
        public void Save()
        {
            projects.Sort();
            using (StreamWriter writer = File.CreateText(filePath))
            {
                writer.WriteLine("solution " + name + ";");
                foreach (Project project in projects)
                {
                    writer.WriteLine("project <" + PathUtil.GetRelativePath(project.FilePath, basePath) + ">;");
                    project.Save();
                }
                if (activeProject != null)
                {
                    writer.WriteLine("activeProject " + activeProject.Name + ";");
                }
            }
        }
        public void Rename(string newName)
        {
            name = newName;
            string newFilePath = PathUtil.MakeCanonicalPath(Path.Combine(basePath, newName.ToLower() + ".cms"));
            File.Move(filePath, newFilePath);
            filePath = newFilePath;
        }
        public void RemoveProject(Project toRemove)
        {
            projects.Remove(toRemove);
            if (toRemove == activeProject)
            {
                if (projects.Count > 0)
                {
                    activeProject = projects[0];
                }
                else
                {
                    activeProject = null;
                }
            }
        }
        public void RenameProject(Project project, string newProjectName)
        {
            foreach (Project p in projects)
            {
                if (p.Name == newProjectName)
                {
                    throw new Exception("solution '" + name + "' already has a project named '" + newProjectName + "'");
                }
            }
            string oldProjectName = project.Name;
            project.Rename(newProjectName);
        }
        public SourceFile GetSourceFileByPath(string sourceFilePath)
        {
            foreach (Project project in projects)
            {
                SourceFile sourceFile = project.GetSourceFileByPath(sourceFilePath);
                if (sourceFile != null)
                {
                    return sourceFile;
                }
            }
            return null;
        }
        private string name;
        private string filePath;
        private string basePath;
        private List<Project> projects;
        private Project activeProject;
    }
}
