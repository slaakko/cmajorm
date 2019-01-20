using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;
using master.util.pattern;
using master.xml;

namespace cmdevenv
{
    public class RecentProject : Class
    {
        public RecentProject()
            : base("recentProject")
        {
            path = new StringField("path", Fields);
            openFiles = new Array<StringField>("openFiles", "openFile", Fields);
        }
        public string Path
        {
            get { return path.Value; }
            set { path.Value = value; }
        }
        public Array<StringField> OpenFiles
        {
            get { return openFiles; }
        }
        private StringField path;
        private Array<StringField> openFiles;
    }

    public class Configuration : Singleton<Configuration>
    {
        private class Config : Class
        {
            public Config()
                : base("cmdevenv.configuration")
            {
                cmc = new StringField("cmc", Fields);
                cmc.Value = "cmc.exe";
                tabSize = new IntField("tabSize", Fields);
                tabSize.Value = 4;
                infoDelaySecs = new IntField("infoDelaySecs", Fields);
                infoDelaySecs.Value = 10;
                formSizeX = new IntField("formSizeX", Fields);
                formSizeX.Value = 800;
                formSizeY = new IntField("formSizeY", Fields);
                formSizeY.Value = 600;
                formTop = new IntField("formTop", Fields);
                formTop.Value = 0;
                formLeft = new IntField("formLeft", Fields);
                formLeft.Value = 0;
                textVisX = new master.xml.IntField("textVisX", Fields);
                textVisX.Value = 200;
                textVisY = new master.xml.IntField("textVisY", Fields);
                textVisY.Value = 200;
                textVisW = new master.xml.IntField("textVisW", Fields);
                textVisW.Value = 200;
                textVisH = new master.xml.IntField("textVisH", Fields);
                textVisH.Value = 200;
                toolColWidth = new IntField("toolColWidth", Fields);
                categoryColWidth = new IntField("categoryColWidth", Fields);
                descriptionColWidth = new IntField("descriptionColWidth", Fields);
                fileColWidth = new IntField("fileColWidth", Fields);
                lineColWidth = new IntField("lineColWidth", Fields);
                projectColWidth = new IntField("projectColWidth", Fields);
                textColWidth = new IntField("textColWidth", Fields);
                findFileColWidth = new IntField("findFileColWidth", Fields);
                findLineColWidth = new IntField("findLineColWidth", Fields);
                findProjectColWidth = new IntField("findProjectColWidth", Fields);
                findTextColWidth = new IntField("findTextColWidth", Fields);
                horizontalSplitterPos = new IntField("horizontalSplitterPos", Fields);
                horizontalSplitterPos.Value = 240;
                verticalSplitterPos = new IntField("verticalSplitterPos", Fields);
                verticalSplitterPos.Value = 140;
                windowState = new StringField("windowState", Fields);
                windowState.Value = "Normal";
                maxRecentProjects = new IntField("maxRecentProjects", Fields);
                maxRecentProjects.Value = 10;
                recentProjects = new Array<RecentProject>("recentProjects", "recentProject", Fields);
                strictNothrow = new BooleanField("strictNothrow", Fields);
                strictNothrow.Value = false;
                emitLlvm = new BooleanField("emitLlvm", Fields);
                emitLlvm.Value = false;
                emitOptLlvm = new BooleanField("emitOptLlvm", Fields);
                emitOptLlvm.Value = false;
                linkWithDebugRuntime = new BooleanField("linkWithDebugRuntime", Fields);
                linkWithDebugRuntime.Value = false;
                linkUsingMsLink = new BooleanField("linkUsingMsLink", Fields);
                linkUsingMsLink.Value = false;
                doNotUseModuleCache = new BooleanField("doNotUseModuleCache", Fields);
                doNotUseModuleCache.Value = false;
                numBuildThreads = new IntField("numBuildThreads", Fields);
                numBuildThreads.Value = 0;
            }
            public string CmcPath
            {
                get { return cmc.Value; }
                set { cmc.Value = value; }
            }
            public int TabSize
            {
                get { return tabSize.Value; }
                set { tabSize.Value = value; }
            }
            public int InfoDelaySecs
            {
                get { return infoDelaySecs.Value; }
                set { infoDelaySecs.Value = value; }
            }
            public int FormSizeX
            {
                get { return formSizeX.Value; }
                set { formSizeX.Value = value; }
            }
            public int FormSizeY
            {
                get { return formSizeY.Value; }
                set { formSizeY.Value = value; }
            }
            public int FormTop
            {
                get { return formTop.Value; }
                set { formTop.Value = value; }
            }
            public int FormLeft
            {
                get { return formLeft.Value; }
                set { formLeft.Value = value; }
            }
            public int TextVisX
            {
                get { return textVisX.Value; }
                set { textVisX.Value = value; }
            }
            public int TextVisY
            {
                get { return textVisY.Value; }
                set { textVisY.Value = value; }
            }
            public int TextVisW
            {
                get { return textVisW.Value; }
                set { textVisW.Value = value; }
            }
            public int TextVisH
            {
                get { return textVisH.Value; }
                set { textVisH.Value = value; }
            }
            public int ToolColWidth
            {
                get { return toolColWidth.Value; }
                set { toolColWidth.Value = value; }
            }
            public int CategoryColWidth
            {
                get { return categoryColWidth.Value; }
                set { categoryColWidth.Value = value; }
            }
            public int DescriptionColWidth
            {
                get { return descriptionColWidth.Value; }
                set { descriptionColWidth.Value = value; }
            }
            public int FileColWidth
            {
                get { return fileColWidth.Value; }
                set { fileColWidth.Value = value; }
            }
            public int LineColWidth
            {
                get { return lineColWidth.Value; }
                set { lineColWidth.Value = value; }
            }
            public int ProjectColWidth
            {
                get { return projectColWidth.Value; }
                set { projectColWidth.Value = value; }
            }
            public int TextColWidth
            {
                get { return textColWidth.Value;  }
                set { textColWidth.Value = value; }
            }
            public int FindFileColWidth
            {
                get { return findFileColWidth.Value; }
                set { findFileColWidth.Value = value; }
            }
            public int FindLineColWidth
            {
                get { return findLineColWidth.Value; }
                set { findLineColWidth.Value = value; }
            }
            public int FindProjectColWidth
            {
                get { return findProjectColWidth.Value; }
                set { findProjectColWidth.Value = value; }
            }
            public int FindTextColWidth
            {
                get { return findTextColWidth.Value; }
                set { findTextColWidth.Value = value; }
            }
            public int HorizontalSplitterPos
            {
                get { return horizontalSplitterPos.Value; }
                set { horizontalSplitterPos.Value = value; }
            }
            public int VerticalSplitterPos
            {
                get { return verticalSplitterPos.Value; }
                set { verticalSplitterPos.Value = value; }
            }
            public string WindowState
            {
                get { return windowState.Value; }
                set { windowState.Value = value; }
            }
            public int MaxRecentProjects
            {
                get { return maxRecentProjects.Value; }
                set { maxRecentProjects.Value = value; }
            }
            public Array<RecentProject> RecentProjects
            {
                get { return recentProjects; }
            }
            public bool StrictNothrow
            {
                get { return strictNothrow.Value; }
                set { strictNothrow.Value = value; }
            }
            public bool EmitLlvm
            {
                get { return emitLlvm.Value; }
                set { emitLlvm.Value = value; }
            }
            public bool EmitOptLlvm
            {
                get { return emitOptLlvm.Value; }
                set { emitOptLlvm.Value = value; }
            }
            public bool LinkWithDebugRuntime
            {
                get { return linkWithDebugRuntime.Value; }
                set { linkWithDebugRuntime.Value = value; }
            }
            public bool LinkUsingMsLink
            {
                get { return linkUsingMsLink.Value; }
                set { linkUsingMsLink.Value = value; }
            }
            public bool DoNotUseModuleCache
            {
                get { return doNotUseModuleCache.Value; }
                set { doNotUseModuleCache.Value = value; }
            }
            public int NumBuildThreads
            {
                get { return numBuildThreads.Value; }
                set { numBuildThreads.Value = value; }
            }
            private StringField cmc;
            private IntField tabSize;
            private IntField infoDelaySecs;
            private IntField formSizeX;
            private IntField formSizeY;
            private IntField formTop;
            private IntField formLeft;
            private IntField textVisX;
            private IntField textVisY;
            private IntField textVisW;
            private IntField textVisH;
            private IntField toolColWidth;
            private IntField categoryColWidth;
            private IntField descriptionColWidth;
            private IntField fileColWidth;
            private IntField lineColWidth;
            private IntField projectColWidth;
            private IntField textColWidth;
            private IntField findFileColWidth;
            private IntField findLineColWidth;
            private IntField findProjectColWidth;
            private IntField findTextColWidth;
            private StringField windowState;
            private IntField horizontalSplitterPos;
            private IntField verticalSplitterPos;
            private IntField maxRecentProjects;
            private Array<RecentProject> recentProjects;
            private BooleanField strictNothrow;
            private BooleanField emitLlvm;
            private BooleanField emitOptLlvm;
            private BooleanField linkWithDebugRuntime;
            private BooleanField linkUsingMsLink;
            private BooleanField doNotUseModuleCache;
            private IntField numBuildThreads;
        }
        public Configuration()
        {
            config = new Config();
            Registry.Instance.Register(typeof(Config), "cmdevenv.configuration");
            string configDir = Environment.GetEnvironmentVariable("CMAJOR_ROOT") + "\\config";
            if (!Directory.Exists(configDir))
            {
                Directory.CreateDirectory(configDir);
            }
            configFilePath = configDir + "\\devenv.config.xml";
            ReadWriteConfig();
        }
        public string CmcPath
        {
            get { return config.CmcPath; }
            set { config.CmcPath = value; }
        }
        public int TabSize
        {
            get { return config.TabSize; }
            set { config.TabSize = value; }
        }
        public int InfoDelaySecs
        {
            get { return config.InfoDelaySecs; }
            set { config.InfoDelaySecs = value; }
        }
        public int FormSizeX
        {
            get { return config.FormSizeX; }
            set { config.FormSizeX = value; }
        }
        public int FormSizeY
        {
            get { return config.FormSizeY; }
            set { config.FormSizeY = value; }
        }
        public int FormTop
        {
            get { return config.FormTop; }
            set { config.FormTop = value; }
        }
        public int FormLeft
        {
            get { return config.FormLeft; }
            set { config.FormLeft = value; }
        }
        public int TextVisX
        {
            get { return config.TextVisX; }
            set { config.TextVisX = value; }
        }
        public int TextVisY
        {
            get { return config.TextVisY; }
            set { config.TextVisY = value; }
        }
        public int TextVisW
        {
            get { return config.TextVisW; }
            set { config.TextVisW = value; }
        }
        public int TextVisH
        {
            get { return config.TextVisH; }
            set { config.TextVisH = value; }
        }
        public int ToolColWidth
        {
            get { return config.ToolColWidth; }
            set { config.ToolColWidth = value; }
        }
        public int CategoryColWidth
        {
            get { return config.CategoryColWidth; }
            set { config.CategoryColWidth = value; }
        }
        public int DescriptionColWidth
        {
            get { return config.DescriptionColWidth; }
            set { config.DescriptionColWidth = value; }
        }
        public int FileColWidth
        {
            get { return config.FileColWidth; }
            set { config.FileColWidth = value; }
        }
        public int LineColWidth
        {
            get { return config.LineColWidth; }
            set { config.LineColWidth = value; }
        }
        public int ProjectColWidth
        {
            get { return config.ProjectColWidth; }
            set { config.ProjectColWidth = value; }
        }
        public int TextColWidth
        {
            get { return config.TextColWidth; }
            set { config.TextColWidth = value; }
        }
        public int FindFileColWidth
        {
            get { return config.FindFileColWidth; }
            set { config.FindFileColWidth = value; }
        }
        public int FindLineColWidth
        {
            get { return config.FindLineColWidth; }
            set { config.FindLineColWidth = value; }
        }
        public int FindProjectColWidth
        {
            get { return config.FindProjectColWidth; }
            set { config.FindProjectColWidth = value; }
        }
        public int FindTextColWidth
        {
            get { return config.FindTextColWidth; }
            set { config.FindTextColWidth = value; }
        }
        public int HorizontalSplitterPos
        {
            get { return config.HorizontalSplitterPos; }
            set { config.HorizontalSplitterPos = value; }
        }
        public int VerticalSplitterPos
        {
            get { return config.VerticalSplitterPos; }
            set { config.VerticalSplitterPos = value; }
        }
        public string WindowState
        {
            get { return config.WindowState; }
            set { config.WindowState = value; }
        }
        public int MaxRecentProjects
        {
            get { return config.MaxRecentProjects; }
            set { config.MaxRecentProjects = value; }
        }
        public List<string> GetRecentProjectPaths()
        {
            List<string> recentProjects = new List<string>();
            foreach (RecentProject project in config.RecentProjects.Items)
            {
                recentProjects.Add(project.Path);
            }
            return recentProjects;
        }
        public bool StrictNothrow
        {
            get { return config.StrictNothrow; }
            set { config.StrictNothrow = value; }
        }
        public bool EmitLlvm
        {
            get { return config.EmitLlvm; }
            set { config.EmitLlvm = value; }
        }
        public bool EmitOptLlvm
        {
            get { return config.EmitOptLlvm; }
            set { config.EmitOptLlvm = value; }
        }
        public bool LinkWithDebugRuntime
        {
            get { return config.LinkWithDebugRuntime; }
            set { config.LinkWithDebugRuntime = value; }
        }
        public bool LinkUsingMsLink
        {
            get { return config.LinkUsingMsLink; }
            set { config.LinkUsingMsLink = value; }
        }
        public bool DoNotUseModuleCache
        {
            get { return config.DoNotUseModuleCache; }
            set { config.DoNotUseModuleCache = value; }
        }
        public int NumBuildThreads
        {
            get { return config.NumBuildThreads; }
            set { config.NumBuildThreads = value; }
        }
        public void RemoveRecentProjectPath(string recentProjectPath)
        {
            foreach (RecentProject project in config.RecentProjects.Items)
            {
                if (project.Path == recentProjectPath)
                {
                    config.RecentProjects.Remove(project);
                    break;
                }
            }
        }
        public int NumRecentProjects
        {
            get { return config.RecentProjects.Items.Count; }
        }
        public void ClearRecentProjects()
        {
            config.RecentProjects.Clear();
            Save();
        }
        public string GetRecentProjectPath(int index)
        {
            return config.RecentProjects.Items[index].Path;
        }
        public RecentProject GetRecentProject(int index)
        {
            return config.RecentProjects.Items[index];
        }
        private int IndexOfRecentProject(string recentProject)
        {
            int n = config.RecentProjects.Items.Count;
            for (int i = 0; i < n; ++i)
            {
                string project = config.RecentProjects.Items[i].Path;
                if (project == recentProject)
                {
                    return i;
                }
            }
            return -1;
        }
        private void BringRecentProjectFront(int index)
        {
            RecentProject top = config.RecentProjects.Items[index];
            for (int i = index - 1; i >= 0; --i)
            {
                config.RecentProjects.Items[i + 1] = config.RecentProjects.Items[i];
            }
            config.RecentProjects.Items[0] = top;
        }
        public void AddRecentProject(string recentProject)
        {
            int index = IndexOfRecentProject(recentProject);
            if (index == -1)
            {
                while (config.RecentProjects.Items.Count >= MaxRecentProjects)
                {
                    config.RecentProjects.Items.RemoveAt(config.RecentProjects.Items.Count - 1);
                }
                RecentProject project = new RecentProject();
                project.Name = "recentProject";
                project.Path = recentProject;
                project.Contained = true;
                config.RecentProjects.Items.Insert(0, project);
            }
            else if (index > 0)
            {
                BringRecentProjectFront(index);
            }
        }
        public void Save()
        {
            WriteConfig();
        }
        private void ReadWriteConfig()
        {
            ReadConfig();
            WriteConfig();
        }
        private void ReadConfig()
        {
            try
            {
                if (File.Exists(configFilePath))
                {
                    using (XmlTextReader reader = new XmlTextReader(configFilePath))
                    {
                        config = (Config)Reader.Read(reader);
                    }
                }
            }
            catch
            {
            }
        }
        private void WriteConfig()
        {
            try
            {
                using (XmlTextWriter writer = new XmlTextWriter(configFilePath, Encoding.UTF8))
                {
                    writer.Formatting = Formatting.Indented;
                    writer.Indentation = 4;
                    config.Write(writer);
                }
            }
            catch
            {
            }
        }
        private Config config;
        private string configFilePath;
    }
}
