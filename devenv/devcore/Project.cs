using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Threading;
using master.util.io;

namespace devcore
{
    public enum Target
    {
        program, winguiapp, winapp, library, winlib, unitTest
    }

    public class Project : IComparable<Project>
    {
        public Project(string name, string filePath)
        {
            this.name = name;
            this.filePath = PathUtil.MakeCanonicalPath(filePath);
            basePath = PathUtil.MakeCanonicalPath(Path.GetDirectoryName(this.filePath));
            sourceFiles = new List<SourceFile>();
            references = new List<string>();
            ready = new ManualResetEvent(false);
            built = false;
        }
        public int CompareTo(Project that)
        {
            return name.CompareTo(that.name);
        }
        public void Save()
        {
            sourceFiles.Sort();
            using (StreamWriter writer = File.CreateText(filePath))
            {
                writer.WriteLine("project " + name + ";");
                string targetStr = "program";
                if (target == Target.winguiapp)
                {
                    targetStr = "winguiapp";
                }
                else if (target == Target.winapp)
                {
                    targetStr = "winapp";
                }
                else if (target == Target.library)
                {
                    targetStr = "library";
                }
                else if (target == Target.winlib)
                {
                    targetStr = "winlib";
                }
                else if (target == Target.unitTest)
                {
                    targetStr = "unitTest";
                }
                writer.WriteLine("target=" + targetStr + ";");
                foreach (string reference in references)
                {
                    string referenceFilePath = Path.IsPathRooted(reference) ? reference : PathUtil.GetRelativePath(Path.GetFullPath(Path.Combine(basePath, reference)), basePath);
                    writer.WriteLine("reference <" + referenceFilePath + ">;"); 
                }
                foreach (SourceFile sourceFile in sourceFiles)
                {
                    switch (sourceFile.GetKind())
                    {
                        case SourceFile.Kind.cm: writer.WriteLine("source <" + PathUtil.GetRelativePath(sourceFile.FilePath, basePath) + ">;"); break;
                        case SourceFile.Kind.xml: writer.WriteLine("resource <" + PathUtil.GetRelativePath(sourceFile.FilePath, basePath) + ">;"); break;
                        case SourceFile.Kind.text: writer.WriteLine("text <" + PathUtil.GetRelativePath(sourceFile.FilePath, basePath) + ">;"); break;
                    }
                }
            }
        }
        public void Rename(string newName)
        {
            name = newName;
            string newFilePath = PathUtil.MakeCanonicalPath(Path.Combine(basePath, newName.ToLower() + ".cmp"));
            File.Move(filePath, newFilePath);
            filePath = newFilePath;
        }
        public string Name
        {
            get { return name; }
        }
        public string FilePath
        {
            get { return filePath; }
        }
        public string BasePath
        {
            get { return basePath; }
        }
        public Target Target
        {
            get { return target; }
            set { target = value; }
        }
        public List<SourceFile> SourceFiles
        {
            get { return sourceFiles; }
        }
        public SourceFile AddSourceFile(string sourceFilePath, SourceFile.Kind kind)
        {
            string filePath = PathUtil.MakeCanonicalPath(Path.IsPathRooted(sourceFilePath) ? sourceFilePath : Path.Combine(basePath, sourceFilePath));
            foreach (SourceFile sf in sourceFiles)
            {
                if (Path.GetFileName(sf.FilePath) == Path.GetFileName(filePath))
                {
                    throw new Exception("Project '" + name + "' already contains source file named '" + Path.GetFileName(sf.FilePath) + "'");
                }
            }
            SourceFile sourceFile = new SourceFile(Path.GetFileName(sourceFilePath), filePath, kind);
            sourceFile.Project = this;
            sourceFiles.Add(sourceFile);
            return sourceFile;
        }
        public SourceFile GetSourceFile(string sourceFileName)
        {
            foreach (SourceFile sourceFile in sourceFiles)
            {
                if (sourceFile.Name == sourceFileName)
                {
                    return sourceFile;
                }
            }
            return null;
        }
        public SourceFile GetSourceFileByPath(string sourceFilePath)
        {
            foreach (SourceFile sourceFile in sourceFiles)
            {
                if (Path.GetFullPath(sourceFile.FilePath) == Path.GetFullPath(sourceFilePath))
                {
                    return sourceFile;
                }
            }
            return null;
        }
        public List<string> References
        {
            get { return references; }
        }
        public void AddReference(string reference)
        {
            references.Add(reference);
        }
        public ManualResetEvent Ready
        {
            get { return ready; }
        }
        public bool Built
        {
            get { return built; }
            set { built = value; }
        }
        public string GetModuleFilePath(string config)
        {
            return PathUtil.MakeCanonicalPath(Path.Combine(Path.Combine(Path.Combine(basePath, "lib"), config), name + ".cmm"));
        }
        public string DefinesFilePath(string config)
        {
            return PathUtil.MakeCanonicalPath(Path.Combine(Path.Combine(Path.Combine(basePath, "lib"), config), "defines.txt"));
        }
        public string GetExecutablePath(string config)
        {
            return PathUtil.MakeCanonicalPath(Path.Combine(Path.Combine(Path.Combine(basePath, "bin"), config), name + ".exe"));
        }
        public string GetConfigurationFilePath()
        {
            return PathUtil.MakeCanonicalPath(Path.Combine(basePath, name + ".usr"));
        }
        private string name;
        private string filePath;
        private string basePath;
        private Target target;
        private List<SourceFile> sourceFiles;
        private List<string> references;
        private ManualResetEvent ready;
        private bool built;
    }
}
