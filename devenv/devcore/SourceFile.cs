using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using master.util.io;

namespace devcore
{
    public class SourceFile : IComparable<SourceFile>
    {
        public enum Kind
        {
            cm, xml, text
        }
        public SourceFile(string name, string filePath, Kind kind)
        {
            this.name = name;
            this.filePath = PathUtil.MakeCanonicalPath(filePath);
            this.kind = kind;
        }
        public int CompareTo(SourceFile that)
        {
            return name.CompareTo(that.name);
        }
        public string Name
        {
            get { return name; }
        }
        public string FilePath
        {
            get { return filePath; }
        }
        public Kind GetKind()
        {
            return kind; 
        }
        public Project Project
        {
            get { return project; }
            set { project = value; }
        }
        public void Rename(string newName)
        {
            name = newName;
            string newFilePath = PathUtil.MakeCanonicalPath(Path.Combine(Path.GetDirectoryName(filePath), newName));
            File.Move(filePath, newFilePath);
            filePath = master.util.io.PathUtil.MakeCanonicalPath(newFilePath);
        }
        public string GetIntermediateCodePath(string config)
        {
            return Path.ChangeExtension(Path.Combine(Path.Combine(Path.Combine(project.BasePath, "lib"), config), name), ".ll");
        }
        public string GetOptimizedIntermediateCodePath(string config)
        {
            return Path.ChangeExtension(Path.Combine(Path.Combine(Path.Combine(project.BasePath, "lib"), config), name), ".opt.ll");
        }
        private string name;
        private string filePath;
        private Kind kind;
        private Project project;
    }
}
