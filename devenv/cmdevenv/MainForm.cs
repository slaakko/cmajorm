using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;
using devcore;
using parser;
using server;

namespace cmdevenv
{
    public enum State
    {
        editing, compiling, running, debugging, profiling, unitTesting
    }

    public partial class MainForm : Form, IMessageFilter
    {
        public MainForm()
        {
            InitializeComponent();
            Application.AddMessageFilter(this);
            progressChars = "|/-\\";
            progressIndex = 0;
            editorTabControl = new XTabControl();
            editorTabControl.Dock = System.Windows.Forms.DockStyle.Fill;
            editorTabControl.Location = new System.Drawing.Point(0, 0);
            editorTabControl.Name = "editorTabControl";
            editorTabControl.SelectedIndex = 0;
            editorTabControl.Size = new System.Drawing.Size(400, 325);
            editorTabControl.TabIndex = 0;
            editorTabControl.TabClosing += editorTabControl_TabClosing;
            editorTabControl.Selected += EditorTabControl_Selected;
            editModuleBuilt = false;
            editorSplitContainer.Panel1.Controls.Add(editorTabControl);
            configComboBox.SelectedIndex = 0;
            //compiler = new Compiler();
            //compiler.CmcPath = Configuration.Instance.CmcPath;
            compiler = new CompilerLib();
            compiler.SetWriteMethod(this, WriteOutputLine);
            compiler.SetHandleCompileResultMethod(this, HandleCompileResult);
            profiler = new Profiler();
            profiler.SetWriteMethod(this, WriteOutputLine);
            profiler.SetHandleProfileResultMethod(this, HandleProfileResult);
            unitTester = new UnitTester();
            unitTester.SetWriteMethod(this, WriteOutputLine);
            unitTester.SetHandleUnitTestingResultMethod(this, HandleUnitTestingResult);
            executor = new Executor();
            executor.SetWriteMethod(this, WriteToConsole);
            executor.SetExecuteReadyMethod(this, ProjectRun);
            console = new ConsoleWindow(executor);
            consoleTabPage.Controls.Add(console);
            processRunning = false;
            buildInProgress = false;
            compileAborted = false;
            profileAborted = false;
            unitTestingAborted = false;
            strictNothrow = Configuration.Instance.StrictNothrow;
            emitLlvm = Configuration.Instance.EmitLlvm;
            emitOptLlvm = Configuration.Instance.EmitOptLlvm;
            linkWithDebugRuntime = Configuration.Instance.LinkWithDebugRuntime;
            linkUsingMsLink = Configuration.Instance.LinkUsingMsLink;
            doNotUseModuleCache = Configuration.Instance.DoNotUseModuleCache;
            compiler.DoSetUseModuleCache(!doNotUseModuleCache);
            numBuildThreads = Configuration.Instance.NumBuildThreads;
            optimizationLevel = -1;
            errorListView.ContextMenuStrip = errorsListViewContextMenuStrip;
            string[] args = Environment.GetCommandLineArgs();
            if (args.Length > 1)
            {
                OpenProjectOrSolution(args[1]);
            }
        }

        private void EditorTabControl_Selected(object sender, TabControlEventArgs e)
        {
            try
            {
                TabPage selectedTab = editorTabControl.SelectedTab;
                if (selectedTab != null)
                {
                    Editor editor = (Editor)selectedTab.Tag;
                    if (editor.SourceFile.Project != null)
                    {
                        string projectFilePath = editor.SourceFile.Project.FilePath;
                        string sourceFilePath = editor.SourceFile.FilePath;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        public bool PreFilterMessage(ref Message m)
        {
            try
            {
                const int WM_KEYDOWN = 0x100;
                if (m.Msg == WM_KEYDOWN)
                {
                    int w = (int)m.WParam;
                    Keys key = (Keys)w & Keys.KeyCode;
                    if (key == Keys.Tab)
                    {
                        bool controlPressed = KeyboardUtil.KeyPressed((int)Keys.ControlKey);
                        if (controlPressed)
                        {
                            if (editorTabControl.TabCount > 1)
                            {
                                bool shiftPressed = KeyboardUtil.KeyPressed((int)Keys.ShiftKey);
                                if (shiftPressed)
                                {
                                    if (editorTabControl.SelectedIndex > 0)
                                    {
                                        --editorTabControl.SelectedIndex;
                                    }
                                    else
                                    {
                                        editorTabControl.SelectedIndex = editorTabControl.TabCount - 1;
                                    }
                                }
                                else
                                {
                                    if (editorTabControl.SelectedIndex < editorTabControl.TabCount - 1)
                                    {
                                        ++editorTabControl.SelectedIndex;
                                    }
                                    else
                                    {
                                        editorTabControl.SelectedIndex = 0;
                                    }
                                }
                            }
                            return true;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            return false;
        }
        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                AboutBox aboutBox = new AboutBox();
                aboutBox.ShowDialog();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void openProjectSolutionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (openProjectDialog.ShowDialog() == DialogResult.OK)
            {
                await OpenProjectOrSolution(openProjectDialog.FileName);
            }
        }
        private async Task OpenProjectOrSolution(string projectOrSolutionName)
        {
            try
            {
                if (solution != null)
                {
                    if (!await CloseSolution())
                    {
                        return;
                    }
                }
                string solutionFileName = projectOrSolutionName.EndsWith(".cms") ? projectOrSolutionName : projectOrSolutionName + ".cms";
                string solutionDir = Path.GetDirectoryName(solutionFileName);
                Directory.SetCurrentDirectory(solutionDir);
                if (File.Exists(solutionFileName))
                {
                    Configuration.Instance.AddRecentProject(solutionFileName);
                    Configuration.Instance.Save();
                    SetupRecentProjectMenu();
                    SolutionFile solutionFileParser = ParserRepository.Instance.SolutionFileParser;
                    solution = solutionFileParser.Parse(File.ReadAllText(solutionFileName), 0, solutionFileName);
                }
                else if (projectOrSolutionName.EndsWith(".cmp"))
                {
                    ProjectFile projectFileParser = ParserRepository.Instance.ProjectFileParser;
                    Project project = projectFileParser.Parse(File.ReadAllText(projectOrSolutionName), 0, projectOrSolutionName);
                    solution = new Solution(project.Name, Path.ChangeExtension(project.FilePath, ".cms"));
                    solution.AddProject(project);
                    solution.Save();
                    Configuration.Instance.AddRecentProject(solution.FilePath);
                    Configuration.Instance.Save();
                    SetupRecentProjectMenu();
                }
                await SetupSolutionExplorer(null);
                SetMenuItemStatus();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async Task SetupSolutionExplorer(Project visibleProject)
        {
            TreeNode visibleProjectNode = null;
            solutionExplorerTreeView.Nodes.Clear();
            int n = solution.Projects.Count;
            if (n > 0 && solution.ActiveProject == null)
            {
                solution.ActiveProject = solution.Projects[0];
            }
            TreeNode solutionNode = solutionExplorerTreeView.Nodes.Add("Solution '" + solution.Name + "' (" + n.ToString() + " project" + (n == 1 ? ")" : "s)"));
            solutionNode.ContextMenuStrip = solutionContextMenuStrip;
            foreach (Project project in solution.Projects)
            {
                TreeNode projectNode = solutionNode.Nodes.Add(project.Name);
                if (project == visibleProject)
                {
                    visibleProjectNode = projectNode;
                }
                projectNode.Tag = project;
                projectNode.ContextMenuStrip = projectContextMenuStrip;
                if (project == solution.ActiveProject)
                {
                    projectNode.NodeFont = new Font(solutionExplorerTreeView.Font, FontStyle.Bold);
                }
                foreach (SourceFile sourceFile in project.SourceFiles)
                {
                    TreeNode sourceNode = projectNode.Nodes.Add(sourceFile.Name);
                    sourceNode.Tag = sourceFile;
                    sourceNode.ContextMenuStrip = sourceFileContextMenuStrip;
                }
            }
            solutionExplorerTreeView.ExpandAll();
            if (visibleProjectNode != null)
            {
                visibleProjectNode.EnsureVisible();
            }
            else
            {
                solutionNode.EnsureVisible();
            }
        }
        private bool editorTabControl_TabClosing(EventArgs e)
        {
            try
            {
                if (editorTabControl.SelectedIndex != -1)
                {
                    TabPage selectedTab = editorTabControl.SelectedTab;
                    Editor editor = (Editor)selectedTab.Tag;
                    DialogResult result = editor.Close();
                    if (result == DialogResult.Cancel)
                    {
                        return false;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            return true;
        }
        public void SetMenuItemStatus()
        {
            bool editing = state == State.editing;
            bool debugging = state == State.debugging;
            bool editorOpen = editorTabControl.TabPages.Count > 0;
            bool solutionOpen = solution != null;
            bool activeProjectSet = solution != null && solution.ActiveProject != null;
            bool solutionHasProjects = solution != null && solution.Projects.Count > 0;
            runActiveProjectToolStripMenuItem.Enabled = activeProjectSet;
            closeToolStripMenuItem.Enabled = editing && editorOpen;
            closeSolutionToolStripMenuItem.Enabled = editing && solutionOpen;
            saveToolStripMenuItem.Enabled = editing && editorOpen;
            saveAllToolStripMenuItem.Enabled = editing && solutionOpen;
            buildSolutionToolStripMenuItem.Enabled = editing && solutionHasProjects;
            buildToolStripMenuItem1.Enabled = editing && solutionHasProjects;
            buildActiveProjectToolStripMenuItem.Enabled = editing && activeProjectSet &&
                (solution.ActiveProject.Target == Target.program || solution.ActiveProject.Target == Target.winapp ||
                solution.ActiveProject.Target == Target.library || solution.ActiveProject.Target == Target.winlib);
            rebuildToolStripMenuItem.Enabled = editing && solutionHasProjects;
            rebuildActiveProjcectToolStripMenuItem.Enabled = editing && activeProjectSet &&
                (solution.ActiveProject.Target == Target.program || solution.ActiveProject.Target == Target.winapp || 
                solution.ActiveProject.Target == Target.library || solution.ActiveProject.Target == Target.winlib);
            rebuildToolStripMenuItem1.Enabled = editing && activeProjectSet &&
                (solution.ActiveProject.Target == Target.program || solution.ActiveProject.Target == Target.winapp ||
                solution.ActiveProject.Target == Target.library || solution.ActiveProject.Target == Target.winlib);
            runUnitTestsInCurrentSolutionToolStripMenuItem.Enabled = editing && solutionHasProjects;
            profileActiveProjectToolStripMenuItem1.Enabled = editing && activeProjectSet &&
                (solution.ActiveProject.Target == Target.program || solution.ActiveProject.Target == Target.winapp);
            runUnitTestsInActiveProjectToolStripMenuItem.Enabled = editing && activeProjectSet && solution.ActiveProject.Target == Target.unitTest;
            buildSolutionToolStripMenuItem.Enabled = editing && solutionHasProjects;
            rebuildSolutionToolStripMenuItem.Enabled = editing && solutionHasProjects;
            cleanSolutionToolStripMenuItem.Enabled = editing && solutionHasProjects;
            cleanSolutionToolStripMenuItem2.Enabled = editing && solutionHasProjects;
            cleanActiveProjectToolStripMenuItem.Enabled = editing && activeProjectSet;
            cleanProjectToolStripMenuItem.Enabled = editing && solutionHasProjects;
            optionsToolStripMenuItem.Enabled = editing && solutionOpen;
            setAsActiveProjectToolStripMenuItem.Enabled = editing && solutionHasProjects;
            gotoLineToolStripMenuItem.Enabled = editorOpen;
            formatContentToolStripMenuItem.Enabled = editing && editorOpen;
            findToolStripMenuItem.Enabled = activeProjectSet && solution.ActiveProject.SourceFiles.Count > 0;
        }
        private async Task<bool> CloseSolution()
        {
            try
            {
                solution.Save();
                foreach (TabPage editorTab in editorTabControl.TabPages)
                {
                    Editor editor = (Editor)editorTab.Tag;
                    DialogResult result = editor.Close();
                    if (result == DialogResult.Cancel)
                    {
                        return false;
                    }
                    editorTabControl.TabPages.Remove(editorTab);
                }
                solutionExplorerTreeView.Nodes.Clear();
                solution = null;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            return true;
        }
        private void SetState(State state)
        {
            this.state = state;
            SetMenuItemStatus();
        }
        private void MainForm_Shown(object sender, EventArgs e)
        {
            try
            {
                alterConfig = false;
                SetState(State.editing);
                Top = Math.Max(0, Configuration.Instance.FormTop);
                Left = Math.Max(0, Configuration.Instance.FormLeft);
                Width = Math.Max(0, Configuration.Instance.FormSizeX);
                Height = Math.Max(0, Configuration.Instance.FormSizeY);
                string windowState = Configuration.Instance.WindowState;
                WindowState = windowState == "Normal" ? FormWindowState.Normal : windowState == "Maximized" ? FormWindowState.Maximized : FormWindowState.Normal;
                if (Configuration.Instance.ToolColWidth != 0) toolColumnHeader.Width = Configuration.Instance.ToolColWidth;
                if (Configuration.Instance.CategoryColWidth != 0) categoryColumnHeader.Width = Configuration.Instance.CategoryColWidth;
                if (Configuration.Instance.DescriptionColWidth != 0) descriptionColumnHeader.Width = Configuration.Instance.DescriptionColWidth;
                if (Configuration.Instance.FileColWidth != 0) fileColumnHeader.Width = Configuration.Instance.FileColWidth;
                if (Configuration.Instance.LineColWidth != 0) lineColumnHeader.Width = Configuration.Instance.LineColWidth;
                if (Configuration.Instance.ProjectColWidth != 0) projectColumnHeader.Width = Configuration.Instance.ProjectColWidth;
                if (Configuration.Instance.TextColWidth != 0) textColumnHeader.Width = Configuration.Instance.TextColWidth;
                if (Configuration.Instance.FindFileColWidth != 0) findFileColumnHeader.Width = Configuration.Instance.FindFileColWidth;
                if (Configuration.Instance.FindLineColWidth != 0) findLineColumnHeader.Width = Configuration.Instance.FindLineColWidth;
                if (Configuration.Instance.FindProjectColWidth != 0) findProjectColumnHeader.Width = Configuration.Instance.FindProjectColWidth;
                if (Configuration.Instance.FindTextColWidth != 0) findTextColumnHeader.Width = Configuration.Instance.FindTextColWidth;
                editorSplitContainer.SplitterDistance = Configuration.Instance.HorizontalSplitterPos;
                splitter1.SplitPosition = Configuration.Instance.VerticalSplitterPos;
                Resize += new EventHandler(MainForm_Resize);
                Move += new EventHandler(MainForm_Move);
                SetupRecentProjectMenu();
                infoTimer.Interval = 1000 * Configuration.Instance.InfoDelaySecs;
                infoTimer.Tick += infoTimer_Tick;
                alterConfig = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void SetupRecentProjectMenu()
        {
            try
            {
                recentProjectsToolStripMenuItem.DropDownItems.Clear();
                List<string> recentProjects = Configuration.Instance.GetRecentProjectPaths();
                int n = recentProjects.Count;
                recentProjectsToolStripMenuItem.Enabled = n != 0;
                clearRecentProjectsToolStripMenuItem.Enabled = n != 0;
                for (int i = 0; i < n; ++i)
                {
                    ToolStripMenuItem recentProjectItem = new ToolStripMenuItem((i + 1).ToString() + " " + recentProjects[i]);
                    recentProjectItem.Tag = recentProjects[i];
                    recentProjectItem.Click += recentProjectItem_Click;
                    recentProjectsToolStripMenuItem.DropDownItems.Add(recentProjectItem);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void recentProjectItem_Click(object sender, EventArgs e)
        {
            try
            {
                string recentProjectFilePath = (string)((ToolStripMenuItem)sender).Tag;
                if (File.Exists(recentProjectFilePath))
                {
                    await OpenProjectOrSolution(recentProjectFilePath);
                }
                else
                {
                    recentProjectsToolStripMenuItem.DropDownItems.Remove((ToolStripMenuItem)sender);
                    Configuration.Instance.RemoveRecentProjectPath(recentProjectFilePath);
                    Configuration.Instance.Save();
                    throw new Exception("project file '" + recentProjectFilePath + "' does not exist");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void MainForm_Resize(object sender, EventArgs e)
        {
            try
            {
                if (alterConfig)
                {
                    Configuration.Instance.WindowState = WindowState == FormWindowState.Maximized || WindowState == FormWindowState.Normal ? WindowState.ToString() : "Normal";
                    if (WindowState == FormWindowState.Normal)
                    {
                        Configuration.Instance.FormSizeX = Width;
                        Configuration.Instance.FormSizeY = Height;
                    }
                    Configuration.Instance.Save();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void MainForm_Move(object sender, EventArgs e)
        {
            try
            {
                if (alterConfig)
                {
                    Configuration.Instance.FormTop = Math.Max(0, Top);
                    Configuration.Instance.FormLeft = Math.Max(0, Left);
                    Configuration.Instance.Save();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void infoTimer_Tick(object sender, EventArgs e)
        {
            infoTimer.Stop();
            infoLabel.Text = "";
        }
        private void splitter1_SplitterMoved(object sender, SplitterEventArgs e)
        {
            try
            {
                if (alterConfig)
                {
                    Configuration.Instance.VerticalSplitterPos = splitter1.SplitPosition;
                    Configuration.Instance.Save();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void editorSplitContainer_SplitterMoved(object sender, SplitterEventArgs e)
        {
            try
            {
                if (alterConfig)
                {
                    Configuration.Instance.HorizontalSplitterPos = editorSplitContainer.SplitterDistance;
                    Configuration.Instance.Save();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                SourceFile sourceFile = (SourceFile)solutionExplorerTreeView.SelectedNode.Tag;
                EditSourceFile(sourceFile);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

        }
        private Editor EditSourceFile(SourceFile sourceFile)
        {
            fileLabel.Text = sourceFile.FilePath;
            foreach (TabPage editorTabPage in editorTabControl.TabPages)
            {
                Editor currentEditor = (Editor)editorTabPage.Tag;
                if (currentEditor.SourceFile == sourceFile)
                {
                    editorTabControl.SelectedTab = editorTabPage;
                    currentEditor.Mode = EditorMode.editing;
                    currentEditor.Focus();
                    return currentEditor;
                }
            }
            TabPage editorTab = new TabPage(sourceFile.Name);
            Editor editor = new Editor(sourceFile, this);
            editor.Mode = EditorMode.editing;
            editor.TabSize = Configuration.Instance.TabSize;
            editorTab.Tag = editor;
            editor.PositionChanged += editor_PositionChanged;
            editor.DirtyStatusChanged += editor_DirtyStatusChanged;
            editor.Closed += editor_Closed;
            editorTab.Controls.Add(editor);
            editorTabControl.TabPages.Add(editorTab);
            editorTabControl.SelectedTab = editorTab;
            SetMenuItemStatus();
            editor.Focus();
            return editor;
        }
        void editor_PositionChanged(object sender, PositionEventArgs e)
        {
            lineLabel.Text = "Line " + e.Line.ToString();
            colLabel.Text = "Col " + e.Col.ToString();
        }
        void editor_DirtyStatusChanged(object sender, EventArgs e)
        {
            Editor editor = (Editor)sender;
            editorTabControl.SelectedTab.Text = editor.SourceFile.Name + (editor.Dirty ? "*" : "");
        }
        void editor_Closed(object sender, EventArgs e)
        {
            fileLabel.Text = "";
            lineLabel.Text = "";
            colLabel.Text = "";
        }
        private void solutionExplorerTreeView_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            try
            {
                if (e.Node.Level == 0)  // renaming a solution node
                {
                    e.CancelEdit = true;
                    if (!string.IsNullOrEmpty(e.Label))
                    {
                        solution.Rename(e.Label);
                        solution.Save();
                        SetSolutionNodeText();
                    }
                }
                else if (e.Node.Level == 1) // renaming a project node
                {
                    if (!string.IsNullOrEmpty(e.Label))
                    {
                        string projectName = e.Node.Text;
                        Project project = solution.GetProject(projectName);
                        if (project != null)
                        {
                            try
                            {
                                solution.RenameProject(project, e.Label);
                            }
                            catch (Exception ex)
                            {
                                e.CancelEdit = true;
                                throw;
                            }
                            solution.Save();
                        }
                        else
                        {
                            e.CancelEdit = true;
                        }
                    }
                    else
                    {
                        e.CancelEdit = true;
                    }
                }
                else if (e.Node.Level == 2) // renaming a source file node
                {
                    if (!string.IsNullOrEmpty(e.Label))
                    {
                        if (Path.GetExtension(e.Label) != ".cm")
                        {
                            e.CancelEdit = true;
                            throw new Exception("'.cm' extension must be supplied");
                        }
                        Project project = solution.GetProject(e.Node.Parent.Text);
                        if (project != null)
                        {
                            SourceFile file = project.GetSourceFile(e.Node.Text);
                            if (file != null)
                            {
                                file.Rename(e.Label);
                                foreach (TabPage editorTab in editorTabControl.TabPages)
                                {
                                    Editor editor = (Editor)editorTab.Tag;
                                    if (editor.SourceFile == file)
                                    {
                                        editorTab.Text = file.Name;
                                        if (editorTab == editorTabControl.SelectedTab)
                                        {
                                            fileLabel.Text = file.FilePath;
                                        }
                                    }
                                }
                                project.Save();
                            }
                            else
                            {
                                e.CancelEdit = true;
                            }
                        }
                    }
                    else
                    {
                        e.CancelEdit = true;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Cmajor Development Environment");
            }
        }
        private void SetSolutionNodeText()
        {
            int n = solution.Projects.Count;
            solutionExplorerTreeView.Nodes[0].Text = "Solution '" + solution.Name + "' (" + n.ToString() + " project" + (n == 1 ? ")" : "s)");
        }
        private void solutionExplorerTreeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            try
            {
                solutionExplorerTreeView.SelectedNode = e.Node;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void solutionExplorerTreeView_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            try
            {
                if (e.Node.Level == 2)
                {
                    SourceFile sourceFile = e.Node.Tag as SourceFile;
                    if (sourceFile != null)
                    {
                        EditSourceFile(sourceFile);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void UneditSourceFile(SourceFile sourceFile)
        {
            foreach (TabPage editorTabPage in editorTabControl.TabPages)
            {
                Editor currentEditor = (Editor)editorTabPage.Tag;
                if (currentEditor.SourceFile == sourceFile)
                {
                    editorTabControl.TabPages.Remove(editorTabPage);
                    return;
                }
            }
        }
        private async void removeToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            try
            {
                TreeNode sourceFileNode = solutionExplorerTreeView.SelectedNode;
                SourceFile sourceFile = (SourceFile)sourceFileNode.Tag;
                if (MessageBox.Show("'" + sourceFile.Name + "' will be removed.", "Cmajor Development Environment", MessageBoxButtons.OKCancel) == DialogResult.OK)
                {
                    UneditSourceFile(sourceFile);
                    TreeNode projectNode = sourceFileNode.Parent;
                    Project project = (Project)projectNode.Tag;
                    project.SourceFiles.Remove(sourceFile);
                    project.Save();
                    await SetupSolutionExplorer(project);
                    SetMenuItemStatus();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void renameToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            solutionExplorerTreeView.SelectedNode.BeginEdit();
        }
        private async void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                TreeNode sourceFileNode = solutionExplorerTreeView.SelectedNode;
                SourceFile sourceFile = (SourceFile)sourceFileNode.Tag;
                if (MessageBox.Show("'" + sourceFile.FilePath + "' will be deleted.", "Cmajor Development Environment", MessageBoxButtons.OKCancel) == DialogResult.OK)
                {
                    UneditSourceFile(sourceFile);
                    TreeNode projectNode = sourceFileNode.Parent;
                    Project project = (Project)projectNode.Tag;
                    project.SourceFiles.Remove(sourceFile);
                    project.Save();
                    File.Delete(sourceFile.FilePath);
                    await SetupSolutionExplorer(project);
                    SetMenuItemStatus();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void closeSolutionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (solution != null)
                {
                    await CloseSolution();
                }
                SetMenuItemStatus();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void WriteOutputLine(string line)
        {
            outputRichTextBox.AppendText(line + Environment.NewLine);
            outputRichTextBox.ScrollToCaret();
        }
        private void WriteToConsole(string chars)
        {
            console.Write(chars);
        }
        private void ProjectRun()
        {
            abortToolStripMenuItem.Enabled = false;
            processRunning = false;
            console.ReadOnly = true;
            SetState(State.editing);
        }
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                Configuration.Instance.Save();
                Application.Exit();
                SetMenuItemStatus();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                foreach (TabPage editorTab in editorTabControl.TabPages)
                {
                    Editor editor = (Editor)editorTab.Tag;
                    DialogResult result = editor.Close();
                    if (result == DialogResult.Cancel)
                    {
                        e.Cancel = true;
                        return;
                    }
                }
                compiler.DoExit();
                compiler.WaitForExit();
                executor.DoExit();
                executor.WaitForExit();
                profiler.DoExit();
                profiler.WaitForExit();
                unitTester.DoExit();
                unitTester.WaitForExit();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void SaveAll()
        {
            try
            {
                if (solution != null)
                {
                    solution.Save();
                    foreach (TabPage editorTab in editorTabControl.TabPages)
                    {
                        Editor editor = (Editor)editorTab.Tag;
                        editor.Save();
                    }
                }
                Configuration.Instance.Save();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void newProjectSolutionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                NewProjectDialog dialog = new NewProjectDialog(true, solution != null);
                string cmajorProjectsDir = Environment.GetEnvironmentVariable("CMAJOR_ROOT") + "\\projects";
                if (!Directory.Exists(cmajorProjectsDir))
                {
                    Directory.CreateDirectory(cmajorProjectsDir);
                }
                dialog.ProjectLocation = solution != null ? solution.BasePath : cmajorProjectsDir;
                DialogResult result = dialog.ShowDialog();
                if (result == DialogResult.OK)
                {
                    if (dialog.NewSolution || dialog.SelectedProjectType == ProjectType.blankSolution)
                    {
                        if (solution != null)
                        {
                            if (!await CloseSolution())
                            {
                                return;
                            }
                        }
                    }
                    if (dialog.SelectedProjectType == ProjectType.blankSolution)
                    {
                        string solutionName = Path.GetFileNameWithoutExtension(dialog.ProjectName);
                        string solutionFilePath = Path.Combine(Path.Combine(dialog.ProjectLocation, solutionName), solutionName + ".cms");
                        Directory.CreateDirectory(Path.GetDirectoryName(solutionFilePath));
                        solution = new Solution(solutionName, solutionFilePath);
                        await SetupSolutionExplorer(null);
                        Configuration.Instance.AddRecentProject(solutionFilePath);
                        Configuration.Instance.Save();
                        SetupRecentProjectMenu();
                    }
                    else
                    {
                        string projectName = dialog.ProjectName;
                        if (projectName.EndsWith(".cmp") || projectName.EndsWith(".cms"))
                        {
                            projectName = projectName.Remove(projectName.Length - 4);
                        }
                        string solutionDirectory = Path.Combine(dialog.ProjectLocation, projectName);
                        Directory.CreateDirectory(solutionDirectory);
                        if (dialog.NewSolution)
                        {
                            string solutionName = projectName;
                            string solutionFilePath = Path.Combine(solutionDirectory, solutionName + ".cms");
                            solution = new Solution(solutionName, solutionFilePath);
                            await SetupSolutionExplorer(null);
                            Configuration.Instance.AddRecentProject(solutionFilePath);
                            Configuration.Instance.Save();
                            SetupRecentProjectMenu();
                        }
                        Project project = new Project(projectName, Path.Combine(solutionDirectory, projectName + ".cmp"));
                        if (dialog.SelectedProjectType == ProjectType.consoleApp)
                        {
                            project.Target = Target.program;
                        }
                        else if (dialog.SelectedProjectType == ProjectType.winapp)
                        {
                            project.Target = Target.winapp;
                        }
                        else if (dialog.SelectedProjectType == ProjectType.library)
                        {
                            project.Target = Target.library;
                        }
                        else if (dialog.SelectedProjectType == ProjectType.winlib)
                        {
                            project.Target = Target.winlib;
                        }
                        else if (dialog.SelectedProjectType == ProjectType.unitTestProject)
                        {
                            project.Target = Target.unitTest;
                        }
                        solution.AddProject(project);
                        TreeNode solutionNode = solutionExplorerTreeView.Nodes[0];
                        TreeNode projectNode = solutionNode.Nodes.Add(project.Name);
                        projectNode.Tag = project;
                        projectNode.ContextMenuStrip = projectContextMenuStrip;
                    }
                    await SetupSolutionExplorer(null);
                    solution.Save();
                }
                SetMenuItemStatus();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void newSourceFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                Project project = (Project)solutionExplorerTreeView.SelectedNode.Tag;
                NewSourceFileDialog dialog = new NewSourceFileDialog();
                dialog.SourceFileName = "file1.cm";
                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    string sourceFileName = dialog.SourceFileName;
                    SourceFile.Kind kind = SourceFile.Kind.cm;
                    if (!sourceFileName.EndsWith(".cm"))
                    {
                        kind = SourceFile.Kind.text;
                    }
                    SourceFile sourceFile = project.AddSourceFile(sourceFileName, kind);
                    using (StreamWriter writer = File.CreateText(sourceFile.FilePath))
                    {
                    }
                    solution.Save();
                    await SetupSolutionExplorer(project);
                    SetMenuItemStatus();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void buildSolutionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BuildSolution(false);
        }
        private void rebuildSolutionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BuildSolution(true);
        }
        private void buildToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            BuildSolution(false);
        }
        private void rebuildToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BuildSolution(true);
        }
        private void buildActiveProjectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BuildProject(solution.ActiveProject, false);
        }
        private void rebuildActiveProjcectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BuildProject(solution.ActiveProject, true);
        }
        private void buildToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            TreeNode selectedNode = solutionExplorerTreeView.SelectedNode;
            if (selectedNode != null)
            {
                Project selectedProject = selectedNode.Tag as Project;
                if (selectedProject != null)
                {
                    BuildProject(selectedProject, false);
                }
            }
        }
        private void rebuildToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            TreeNode selectedNode = solutionExplorerTreeView.SelectedNode;
            if (selectedNode != null)
            {
                Project selectedProject = selectedNode.Tag as Project;
                if (selectedProject != null)
                {
                    BuildProject(selectedProject, true);
                }
            }
        }
        private void SetDefinesFor(Project project, string config)
        {
            try
            {
                string definesFilePath = project.DefinesFilePath(config);
                Directory.CreateDirectory(Path.GetDirectoryName(definesFilePath));
                if (File.Exists(definesFilePath))
                {
                    File.Delete(definesFilePath);
                }
                ProjectConfig projectConfig = ProjectConfigurations.Instance.GetProjectConfig(project.GetConfigurationFilePath());
                string condionalCompilationSymbolsString = projectConfig.GetConditionalCompilationSymbols(config);
                if (!string.IsNullOrEmpty(condionalCompilationSymbolsString))
                {
                    condionalCompilationSymbolsString = condionalCompilationSymbolsString.Trim();
                    if (!string.IsNullOrEmpty(condionalCompilationSymbolsString))
                    {
                        string[] conditionalCompilationSymbols = condionalCompilationSymbolsString.Split(' ');
                        if (conditionalCompilationSymbols.Length > 0)
                        {
                            bool empty = true;
                            foreach (string conditionalCompilationSymbol in conditionalCompilationSymbols)
                            {
                                if (!string.IsNullOrEmpty(conditionalCompilationSymbol))
                                {
                                    empty = false;
                                    break;
                                }
                            }
                            if (!empty)
                            {
                                using (StreamWriter writer = File.CreateText(definesFilePath))
                                {
                                    foreach (string conditionalCompilationSymbol in conditionalCompilationSymbols)
                                    {
                                        if (!string.IsNullOrEmpty(conditionalCompilationSymbol))
                                        {
                                            writer.WriteLine(conditionalCompilationSymbol);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void BuildSolution(bool rebuild)
        {
            try
            {
                if (processRunning)
                {
                    throw new Exception("Cannot build while process is running");
                }
                if (state == State.debugging)
                {
                    throw new Exception("Stop debugging before building");
                }
                SaveAll();
                progressTimer.Start();
                outputRichTextBox.Clear();
                errorListView.Items.Clear();
                showErrorDescriptionInTextWindowToolStripMenuItem.Enabled = false;
                buildSolutionToolStripMenuItem.Enabled = false;
                buildToolStripMenuItem1.Enabled = false;
                buildActiveProjectToolStripMenuItem.Enabled = false;
                profileActiveProjectToolStripMenuItem1.Enabled = false;
                runUnitTestsInActiveProjectToolStripMenuItem.Enabled = false;
                runUnitTestsInCurrentSolutionToolStripMenuItem.Enabled = false;
                outputTabControl.SelectedTab = outputTabPage;
                if (solution != null && solution.Projects.Count > 0)
                {
                    string config = configComboBox.Text;
                    compileStartTime = DateTime.Now;
                    compileTimer.Start();
                    cancelToolStripMenuItem.Enabled = true;
                    buildInProgress = true;
                    SetState(State.compiling);
                    foreach (Project project in solution.Projects)
                    {
                        SetDefinesFor(project, config);
                    }
                    compiler.DoCompile(solution.FilePath, config, strictNothrow, emitLlvm, emitOptLlvm, linkWithDebugRuntime, linkUsingMsLink, optimizationLevel, numBuildThreads, rebuild);
                    infoTimer.Stop();
                    infoLabel.Text = "Building";
                }
            }
            catch (Exception ex)
            {
                SetState(State.editing);
                MessageBox.Show(ex.Message);
            }
        }
        private void BuildProject(Project project, bool rebuild)
        {
            try
            {
                if (processRunning)
                {
                    throw new Exception("Cannot build while process is running");
                }
                SaveAll();
                progressTimer.Start();
                outputRichTextBox.Clear();
                errorListView.Items.Clear();
                showErrorDescriptionInTextWindowToolStripMenuItem.Enabled = false;
                buildSolutionToolStripMenuItem.Enabled = false;
                buildToolStripMenuItem1.Enabled = false;
                buildActiveProjectToolStripMenuItem.Enabled = false;
                profileActiveProjectToolStripMenuItem1.Enabled = false;
                runUnitTestsInActiveProjectToolStripMenuItem.Enabled = false;
                runUnitTestsInCurrentSolutionToolStripMenuItem.Enabled = false;
                outputTabControl.SelectedTab = outputTabPage;
                string config = configComboBox.Text;
                compileStartTime = DateTime.Now;
                compileTimer.Start();
                cancelToolStripMenuItem.Enabled = true;
                buildInProgress = true;
                SetState(State.compiling);
                SetDefinesFor(project, config);
                compiler.DoCompile(project.FilePath, config, strictNothrow, emitLlvm, emitOptLlvm, linkWithDebugRuntime, linkUsingMsLink, optimizationLevel, numBuildThreads, rebuild);
                infoTimer.Stop();
                infoLabel.Text = "Building";
            }
            catch (Exception ex)
            {
                SetState(State.editing);
                MessageBox.Show(ex.Message);
            }
        }
        private void HandleCompileResult(CompileResult compileResult)
        {
            cancelToolStripMenuItem.Enabled = false;
            buildInProgress = false;
            compileTimer.Stop();
            compileTimeStatusLabel.Text = "";
            progressTimer.Stop();
            progressLabel.Text = "";
            bool hasErrors = false;
            bool hasWarningsOrInfos = false;
            if (compileResult != null)
            {
                hasErrors = !compileResult.Success;
                hasWarningsOrInfos = compileResult.Diagnostics.Count > 0;
            }
            if (hasErrors || hasWarningsOrInfos)
            {
                if (hasErrors)
                {
                    if (compiling)
                    {
                        compiling = false;
                        infoTimer.Stop();
                        infoLabel.Text = "Compile failed";
                    }
                    else if (cleaning)
                    {
                        cleaning = false;
                        infoTimer.Stop();
                        infoLabel.Text = "Clean failed";
                    }
                    else
                    {
                        infoTimer.Stop();
                        infoLabel.Text = "Build failed";
                    }
                }
                else
                {
                    if (compileAborted)
                    {
                        compileAborted = false;
                        infoTimer.Stop();
                        infoLabel.Text = "Compile aborted";
                    }
                    else if (compiling)
                    {
                        compiling = false;
                        infoTimer.Stop();
                        infoLabel.Text = "Compile succeeded";
                    }
                    else if (cleaning)
                    {
                        cleaning = false;
                        infoTimer.Stop();
                        infoLabel.Text = "Clean succeeded";
                    }
                    else
                    {
                        infoTimer.Stop();
                        infoLabel.Text = "Build succeeded";
                    }
                }
                if (compileResult != null)
                {
                    foreach (Diagnostic diagnostic in compileResult.Diagnostics)
                    {
                        if (diagnostic != null)
                        {
                            string tool = diagnostic.Tool;
                            string category = diagnostic.Category;
                            string message = diagnostic.Message;
                            string project = diagnostic.Project;
                            string file = "";
                            string line = "";
                            string text = "";
                            if (diagnostic.Span != null)
                            {
                                file = diagnostic.Span.File;
                                if (diagnostic.Span.Line != 0)
                                {
                                    line = diagnostic.Span.Line.ToString();
                                }
                                text = diagnostic.Span.Text;
                            }
                            ListViewItem item = new ListViewItem(new string[] { tool, category, message, file, line, project, text });
                            item.Tag = diagnostic.Span;
                            errorListView.Items.Add(item);
                        }
                        showErrorDescriptionInTextWindowToolStripMenuItem.Enabled = true;
                    }
                }
                outputTabControl.SelectedTab = errorTabPage;
            }
            else
            {
                if (compileAborted)
                {
                    compileAborted = false;
                    infoTimer.Stop();
                    infoLabel.Text = "Compile aborted";
                }
                else if (compiling)
                {
                    compiling = false;
                    infoTimer.Stop();
                    infoLabel.Text = "Compile succeeded";
                }
                else if (cleaning)
                {
                    cleaning = false;
                    infoTimer.Stop();
                    infoLabel.Text = "Clean succeeded";
                }
                else
                {
                    infoTimer.Stop();
                    infoLabel.Text = "Build succeeded";
                }
            }
            infoTimer.Start();
            SetState(State.editing);
        }
        private void ProfileProject(Project project)
        {
            try
            {
                if (processRunning)
                {
                    throw new Exception("Cannot profile while process is running");
                }
                SaveAll();
                ProfileDialog profileDialog = new ProfileDialog();
                DialogResult dialogResult = profileDialog.ShowDialog();
                if (dialogResult == DialogResult.OK)
                {
                    buildActiveProjectToolStripMenuItem.Enabled = false;
                    profileActiveProjectToolStripMenuItem1.Enabled = false;
                    runUnitTestsInActiveProjectToolStripMenuItem.Enabled = false;
                    runUnitTestsInCurrentSolutionToolStripMenuItem.Enabled = false;
                    progressTimer.Start();
                    outputRichTextBox.Clear();
                    outputTabControl.SelectedTab = outputTabPage;
                    SetState(State.profiling);
                    profileOutFile = Path.Combine(Path.GetDirectoryName(project.FilePath), "cmprof-" + DateTime.Now.ToString("yyyyMMddTHHmmss") + ".html");
                    cancelToolStripMenuItem.Enabled = true;
                    profiler.DoProfile(profileDialog.RebuildSys, profileDialog.RebuildApp, profileDialog.Inclusive, profileDialog.Exclusive, profileDialog.Count, emitLlvm, emitOptLlvm, linkWithDebugRuntime,
                        linkUsingMsLink, profileDialog.Top, profileOutFile, profileDialog.Arguments, project.FilePath);
                    infoTimer.Stop();
                    infoLabel.Text = "Profiling";
                }
            }
            catch (Exception ex)
            {
                SetState(State.editing);
                MessageBox.Show(ex.Message);
            }
        }
        private void HandleProfileResult(string errorMessage, int exitCode)
        {
            cancelToolStripMenuItem.Enabled = false;
            progressTimer.Stop();
            progressLabel.Text = "";
            if (profileAborted)
            {
                profileAborted = false;
                infoTimer.Stop();
                infoLabel.Text = "Profile aborted";
            }
            if (exitCode == 0)
            {
                infoTimer.Stop();
                infoLabel.Text = "Profile succeeded";
                System.Diagnostics.Process.Start(profileOutFile);
            }
            else
            {
                infoTimer.Stop();
                infoLabel.Text = "Profile failed";
                WriteOutputLine(errorMessage);
                WriteOutputLine("exit code = " + exitCode.ToString());
            }
            infoTimer.Start();
            SetState(State.editing);
        }
        private void RunUnitTests(Solution solution)
        {
            try
            {
                if (processRunning)
                {
                    throw new Exception("Cannot unit test while process is running");
                }
                SaveAll();
                buildActiveProjectToolStripMenuItem.Enabled = false;
                profileActiveProjectToolStripMenuItem1.Enabled = false;
                runUnitTestsInActiveProjectToolStripMenuItem.Enabled = false;
                runUnitTestsInCurrentSolutionToolStripMenuItem.Enabled = false;
                progressTimer.Start();
                outputRichTextBox.Clear();
                outputTabControl.SelectedTab = outputTabPage;
                SetState(State.unitTesting);
                unitTestOutFile = Path.Combine(Path.GetDirectoryName(solution.FilePath), "cmunit-" + DateTime.Now.ToString("yyyyMMddTHHmmss") + ".html");
                cancelToolStripMenuItem.Enabled = true;
                infoTimer.Stop();
                infoLabel.Text = "Unit testing";
                string config = configComboBox.Text;
                unitTester.DoUnitTest(solution.FilePath, config, null, linkWithDebugRuntime, linkUsingMsLink, unitTestOutFile);
            }
            catch (Exception ex)
            {
                SetState(State.editing);
                MessageBox.Show(ex.Message);
            }
        }
        private void RunUnitTests(Project project, string file)
        {
            try
            {
                if (processRunning)
                {
                    throw new Exception("Cannot unit test while process is running");
                }
                if (project.Target != Target.unitTest)
                {
                    throw new Exception("Project '" + project.Name + "' is not a unit test project");
                }
                SaveAll();
                buildActiveProjectToolStripMenuItem.Enabled = false;
                profileActiveProjectToolStripMenuItem1.Enabled = false;
                runUnitTestsInActiveProjectToolStripMenuItem.Enabled = false;
                runUnitTestsInCurrentSolutionToolStripMenuItem.Enabled = false;
                progressTimer.Start();
                outputRichTextBox.Clear();
                outputTabControl.SelectedTab = outputTabPage;
                SetState(State.unitTesting);
                unitTestOutFile = Path.Combine(Path.GetDirectoryName(project.FilePath), "cmunit-" + DateTime.Now.ToString("yyyyMMddTHHmmss") + ".html");
                cancelToolStripMenuItem.Enabled = true;
                infoTimer.Stop();
                infoLabel.Text = "Unit testing";
                string config = configComboBox.Text;
                unitTester.DoUnitTest(project.FilePath, config, file, linkWithDebugRuntime, linkUsingMsLink, unitTestOutFile);
            }
            catch (Exception ex)
            {
                SetState(State.editing);
                MessageBox.Show(ex.Message);
            }
        }
        private void HandleUnitTestingResult(string errorMessage, int exitCode)
        {
            cancelToolStripMenuItem.Enabled = false;
            progressTimer.Stop();
            progressLabel.Text = "";
            if (unitTestingAborted)
            {
                unitTestingAborted = false;
                infoTimer.Stop();
                infoLabel.Text = "Unit testing aborted";
            }
            if (exitCode == 0)
            {
                infoTimer.Stop();
                infoLabel.Text = "Unit testing succeeded";
                System.Diagnostics.Process.Start(unitTestOutFile);
            }
            else
            {
                infoTimer.Stop();
                infoLabel.Text = "Unit testing failed";
                WriteOutputLine(errorMessage);
                WriteOutputLine("exit code = " + exitCode.ToString());
            }
            infoTimer.Start();
            SetState(State.editing);
        }
        private void profileActiveProjectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (solution.ActiveProject.Target == Target.program)
                {
                    ProfileProject(solution.ActiveProject);
                }
                else
                {
                    throw new Exception("project '" + solution.ActiveProject.Name + "' is not a program");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void showErrorDescriptionInTextWindowToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (errorListView.SelectedItems.Count > 0)
                {
                    ListViewItem item = errorListView.SelectedItems[0];
                    string description = item.SubItems[2].Text;
                    TextVisualizerForm form = new TextVisualizerForm();
                    form.Text = "Error Description";
                    form.Left = Configuration.Instance.TextVisX;
                    form.Top = Configuration.Instance.TextVisY;
                    form.Width = Configuration.Instance.TextVisW;
                    form.Height = Configuration.Instance.TextVisH;
                    form.TextContent = description;
                    form.ShowDialog();
                    Configuration.Instance.TextVisX = form.Left;
                    Configuration.Instance.TextVisY = form.Top;
                    Configuration.Instance.TextVisW = form.Width;
                    Configuration.Instance.TextVisH = form.Height;
                    Configuration.Instance.Save();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void errorListView_ItemActivate(object sender, EventArgs e)
        {
            try
            {
                if (errorListView.SelectedItems.Count > 0)
                {
                    ListViewItem selectedItem = errorListView.SelectedItems[0];
                    string projectName = selectedItem.SubItems[5].Text;
                    Span span = (Span)selectedItem.Tag;
                    if (span != null)
                    {
                        string filePath = span.File;
                        if (!string.IsNullOrEmpty(filePath))
                        {
                            SourceFile sourceFile = solution.GetSourceFileByPath(filePath);
                            if (sourceFile == null)
                            {
                                sourceFile = new SourceFile(Path.GetFileName(filePath), filePath, (filePath.EndsWith(".cm") ? SourceFile.Kind.cm : SourceFile.Kind.text));
                            }
                            Editor editor = EditSourceFile(sourceFile);
                            int line = span.Line;
                            if (line != 0)
                            {
                                int startCol = span.StartCol;
                                int endCol = span.EndCol;
                                if (startCol != 0 && endCol == 0 || startCol == endCol)
                                {
                                    endCol = startCol + 1;
                                }
                                LineCol startLineCol = new LineCol(line, startCol);
                                LineCol endLineCol = new LineCol(line, endCol);
                                editor.SetCursorPos(line, 1);
                                editor.SetHilite(startLineCol, endLineCol, Editor.Hilite.errorPos);
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void progressTimer_Tick(object sender, EventArgs e)
        {
            progressLabel.Text = new string(progressChars[progressIndex], 1);
            progressIndex = (progressIndex + 1) % progressChars.Length;
        }
        private void errorListView_ColumnWidthChanged(object sender, ColumnWidthChangedEventArgs e)
        {
            try
            {
                switch (e.ColumnIndex)
                {
                    case 0: Configuration.Instance.ToolColWidth = errorListView.Columns[0].Width; break;
                    case 1: Configuration.Instance.CategoryColWidth = errorListView.Columns[1].Width; break;
                    case 2: Configuration.Instance.DescriptionColWidth = errorListView.Columns[2].Width; break;
                    case 3: Configuration.Instance.FileColWidth = errorListView.Columns[3].Width; break;
                    case 4: Configuration.Instance.LineColWidth = errorListView.Columns[4].Width; break;
                    case 5: Configuration.Instance.ProjectColWidth = errorListView.Columns[5].Width; break;
                    case 6: Configuration.Instance.TextColWidth = errorListView.Columns[6].Width; break;
                }
                Configuration.Instance.Save();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void saveAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                SaveAll();
                SetMenuItemStatus();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                TabPage editorTab = editorTabControl.SelectedTab;
                if (editorTab != null)
                {
                    Editor editor = (Editor)editorTab.Tag;
                    editor.Save();
                }
                SetMenuItemStatus();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                TabPage editorTab = editorTabControl.SelectedTab;
                if (editorTab != null)
                {
                    Editor editor = (Editor)editorTab.Tag;
                    DialogResult result = editor.Close();
                    if (result == DialogResult.Cancel)
                    {
                        return;
                    }
                    editorTabControl.TabPages.RemoveAt(editorTabControl.SelectedIndex);
                }
                SetMenuItemStatus();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void optionsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                BuildOptionsDialog dialog = new BuildOptionsDialog(compiler);
                strictNothrow = Configuration.Instance.StrictNothrow;
                emitLlvm = Configuration.Instance.EmitLlvm;
                emitOptLlvm = Configuration.Instance.EmitOptLlvm;
                linkWithDebugRuntime = Configuration.Instance.LinkWithDebugRuntime;
                linkUsingMsLink = Configuration.Instance.LinkUsingMsLink;
                doNotUseModuleCache = Configuration.Instance.DoNotUseModuleCache;
                numBuildThreads = Configuration.Instance.NumBuildThreads;
                dialog.StrictNothrow = strictNothrow;
                dialog.EmitLlvm = emitLlvm;
                dialog.EmitOptLlvm = emitOptLlvm;
                dialog.LinkWithDebugRuntime = linkWithDebugRuntime;
                dialog.LinkUsingMsLink = linkUsingMsLink;
                dialog.DoNotUseModuleCache = doNotUseModuleCache;
                dialog.NumBuildThreads = numBuildThreads;
                if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    strictNothrow = dialog.StrictNothrow;
                    emitLlvm = dialog.EmitLlvm;
                    emitOptLlvm = dialog.EmitOptLlvm;
                    linkWithDebugRuntime = dialog.LinkWithDebugRuntime;
                    linkUsingMsLink = dialog.LinkUsingMsLink;
                    doNotUseModuleCache = dialog.DoNotUseModuleCache;
                    compiler.DoSetUseModuleCache(!doNotUseModuleCache);
                    numBuildThreads = dialog.NumBuildThreads;
                    Configuration.Instance.StrictNothrow = strictNothrow;
                    Configuration.Instance.EmitLlvm = emitLlvm;
                    Configuration.Instance.EmitOptLlvm = emitOptLlvm;
                    Configuration.Instance.LinkWithDebugRuntime = linkWithDebugRuntime;
                    Configuration.Instance.LinkUsingMsLink = linkUsingMsLink;
                    Configuration.Instance.DoNotUseModuleCache = doNotUseModuleCache;
                    Configuration.Instance.NumBuildThreads = numBuildThreads;
                    Configuration.Instance.Save();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void intermediateCodeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                SourceFile sourceFile = (SourceFile)solutionExplorerTreeView.SelectedNode.Tag;
                string intermediateCodePath = sourceFile.GetIntermediateCodePath(configComboBox.Text);
                EditSourceFile(new SourceFile(Path.GetFileName(intermediateCodePath), intermediateCodePath, SourceFile.Kind.text));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void optimizedIntermediateCodeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                SourceFile sourceFile = (SourceFile)solutionExplorerTreeView.SelectedNode.Tag;
                string optimizedIntermediateCodePath = sourceFile.GetOptimizedIntermediateCodePath(configComboBox.Text);
                EditSourceFile(new SourceFile(Path.GetFileName(optimizedIntermediateCodePath), optimizedIntermediateCodePath, SourceFile.Kind.text));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void viewFileToolStripMenuItem_DropDownOpened(object sender, EventArgs e)
        {
            try
            {
                SourceFile sourceFile = (SourceFile)solutionExplorerTreeView.SelectedNode.Tag;
                string intermediateCodePath = sourceFile.GetIntermediateCodePath(configComboBox.Text);
                intermediateCodeToolStripMenuItem.Enabled = File.Exists(intermediateCodePath);
                string optimizedIntermediateCodePath = sourceFile.GetOptimizedIntermediateCodePath(configComboBox.Text);
                optimizedIntermediateCodeToolStripMenuItem.Enabled = File.Exists(optimizedIntermediateCodePath);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void cleanSolutionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            CleanProjectOrSolution(solution.FilePath);
        }
        private void cleanSolutionToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            CleanProjectOrSolution(solution.FilePath);
        }
        private void CleanProjectOrSolution(string solutionOrProjectFilePath)
        {
            try
            {
                if (state == State.debugging)
                {
                    throw new Exception("Stop debugging before cleaning");
                }
                string config = configComboBox.Text;
                SaveAll();
                progressTimer.Start();
                outputRichTextBox.Clear();
                errorListView.Items.Clear();
                showErrorDescriptionInTextWindowToolStripMenuItem.Enabled = false;
                buildSolutionToolStripMenuItem.Enabled = false;
                buildToolStripMenuItem1.Enabled = false;
                buildActiveProjectToolStripMenuItem.Enabled = false;
                profileActiveProjectToolStripMenuItem1.Enabled = false;
                runUnitTestsInActiveProjectToolStripMenuItem.Enabled = false;
                runUnitTestsInCurrentSolutionToolStripMenuItem.Enabled = false;
                outputTabControl.SelectedTab = outputTabPage;
                cleaning = true;
                cancelToolStripMenuItem.Enabled = true;
                buildInProgress = true;
                SetState(State.compiling);
                compiler.DoClean(solutionOrProjectFilePath, config);
                infoTimer.Stop();
                infoLabel.Text = "Cleaning";
            }
            catch (Exception ex)
            {
                SetState(State.editing);
                MessageBox.Show(ex.Message);
            }
        }
        private void cleanActiveProjectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            CleanProjectOrSolution(solution.ActiveProject.FilePath);
        }
        private void cleanProjectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TreeNode selectedNode = solutionExplorerTreeView.SelectedNode;
            if (selectedNode != null)
            {
                Project selectedProject = selectedNode.Tag as Project;
                if (selectedProject != null)
                {
                    CleanProjectOrSolution(selectedProject.FilePath);
                }
            }
        }
        private void closeAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                foreach (TabPage editorTab in editorTabControl.TabPages)
                {
                    Editor editor = (Editor)editorTab.Tag;
                    DialogResult result = editor.Close();
                    if (result == DialogResult.Cancel)
                    {
                        break;
                    }
                    editorTabControl.TabPages.Remove(editorTab);
                }
                SetMenuItemStatus();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void existingSourceFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                Project project = (Project)solutionExplorerTreeView.SelectedNode.Tag;
                addExistingSourceFileDialog.InitialDirectory = project.BasePath;
                if (addExistingSourceFileDialog.ShowDialog() == DialogResult.OK)
                {
                    string fileName = addExistingSourceFileDialog.FileName;
                    SourceFile.Kind kind = SourceFile.Kind.cm;
                    if (!fileName.EndsWith(".cm"))
                    {
                        kind = SourceFile.Kind.text;
                    }
                    project.AddSourceFile(fileName, kind);
                    solution.Save();
                    await SetupSolutionExplorer(project);
                    SetMenuItemStatus();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void existingTextFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                Project project = (Project)solutionExplorerTreeView.SelectedNode.Tag;
                addExistingTextFileDialog.InitialDirectory = project.BasePath;
                if (addExistingTextFileDialog.ShowDialog() == DialogResult.OK)
                {
                    string fileName = addExistingTextFileDialog.FileName;
                    if (fileName.EndsWith("cm"))
                    {
                        throw new Exception("cannot add files ending with .cm with this dialog");
                    }
                    SourceFile.Kind kind = SourceFile.Kind.text;
                    project.AddSourceFile(fileName, kind);
                    solution.Save();
                    await SetupSolutionExplorer(project);
                    SetMenuItemStatus();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void newProjectToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            try
            {
                NewProjectDialog dialog = new NewProjectDialog(false, solution != null);
                dialog.ProjectLocation = solution.BasePath;
                DialogResult result = dialog.ShowDialog();
                if (result == DialogResult.OK)
                {
                    string projectName = dialog.ProjectName;
                    if (projectName.EndsWith(".cmp"))
                    {
                        projectName = projectName.Remove(projectName.Length - 4);
                    }
                    Project project = new Project(projectName, Path.Combine(Path.Combine(dialog.ProjectLocation, projectName), projectName + ".cmp"));
                    Directory.CreateDirectory(Path.GetDirectoryName(project.FilePath));
                    if (dialog.SelectedProjectType == ProjectType.consoleApp)
                    {
                        project.Target = Target.program;
                    }
                    else if (dialog.SelectedProjectType == ProjectType.winapp)
                    {
                        project.Target = Target.winapp;
                    }
                    else if (dialog.SelectedProjectType == ProjectType.library)
                    {
                        project.Target = Target.library;
                    }
                    else if (dialog.SelectedProjectType == ProjectType.winlib)
                    {
                        project.Target = Target.winlib;
                    }
                    else if (dialog.SelectedProjectType == ProjectType.unitTestProject)
                    {
                        project.Target = Target.unitTest;
                    }
                    solution.AddProject(project);
                    TreeNode solutionNode = solutionExplorerTreeView.Nodes[0];
                    TreeNode projectNode = solutionNode.Nodes.Add(project.Name);
                    projectNode.Tag = project;
                    projectNode.ContextMenuStrip = projectContextMenuStrip;
                    solution.Save();
                    await SetupSolutionExplorer(null);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void existingProjectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                addExistingProjectFileDialog.InitialDirectory = solution.BasePath;
                if (addExistingProjectFileDialog.ShowDialog() == DialogResult.OK)
                {
                    ProjectFile projectFileParser = ParserRepository.Instance.ProjectFileParser;
                    Project project = projectFileParser.Parse(File.ReadAllText(addExistingProjectFileDialog.FileName), 0, addExistingProjectFileDialog.FileName);
                    solution.AddProject(project);
                    TreeNode solutionNode = solutionExplorerTreeView.Nodes[0];
                    TreeNode projectNode = solutionNode.Nodes.Add(project.Name);
                    projectNode.Tag = project;
                    projectNode.ContextMenuStrip = projectContextMenuStrip;
                    solution.Save();
                    await SetupSolutionExplorer(null);
                    SetMenuItemStatus();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void renameToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            solutionExplorerTreeView.SelectedNode.BeginEdit();
        }
        private async void removeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                TreeNode node = solutionExplorerTreeView.SelectedNode;
                Project toRemove = (Project)node.Tag;
                if (MessageBox.Show("'" + toRemove.Name + "' will be removed.", "Cmajor Development Environment", MessageBoxButtons.OKCancel) == DialogResult.OK)
                {
                    solution.RemoveProject(toRemove);
                    solution.Save();
                    await SetupSolutionExplorer(null);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void moduleContentToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                TreeNode selectedNode = solutionExplorerTreeView.SelectedNode;
                if (selectedNode != null)
                {
                    Project selectedProject = selectedNode.Tag as Project;
                    if (selectedProject != null)
                    {
                        string moduleFilePath = selectedProject.GetModuleFilePath(configComboBox.Text);
                        System.Diagnostics.ProcessStartInfo startInfo = new System.Diagnostics.ProcessStartInfo("cmmdump.exe", moduleFilePath);
                        startInfo.UseShellExecute = false;
                        startInfo.CreateNoWindow = true;
                        startInfo.RedirectStandardOutput = true;
                        startInfo.RedirectStandardError = true;
                        System.Diagnostics.Process cmlDump = System.Diagnostics.Process.Start(startInfo);
                        string output = cmlDump.StandardOutput.ReadToEnd();
                        string errorText = cmlDump.StandardError.ReadToEnd();
                        cmlDump.WaitForExit();
                        if (cmlDump.ExitCode != 0)
                        {
                            throw new Exception(errorText);
                        }
                        else
                        {
                            string cmmTextFilePath = moduleFilePath + ".txt";
                            using (StreamWriter writer = File.CreateText(cmmTextFilePath))
                            {
                                writer.Write(output);
                            }
                            EditSourceFile(new SourceFile(selectedProject.Name + ".cmm", cmmTextFilePath, SourceFile.Kind.text));
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void newTextFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                Project project = (Project)solutionExplorerTreeView.SelectedNode.Tag;
                NewTextFileDialog dialog = new NewTextFileDialog();
                dialog.TextFileName = "file1.txt";
                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    string textFileName = dialog.TextFileName;
                    SourceFile.Kind kind = SourceFile.Kind.text;
                    SourceFile sourceFile = project.AddSourceFile(textFileName, kind);
                    using (StreamWriter writer = File.CreateText(sourceFile.FilePath))
                    {
                    }
                    solution.Save();
                    await SetupSolutionExplorer(project);
                    SetMenuItemStatus();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void clearRecentProjectsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                Configuration.Instance.ClearRecentProjects();
                SetupRecentProjectMenu();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void systemLibraryToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                string cmajorRootDir = Environment.GetEnvironmentVariable("CMAJOR_ROOT");
                await OpenProjectOrSolution(Path.Combine(Path.Combine(Path.Combine(Path.Combine(cmajorRootDir, "system"), "platform"), "windows"), "System.cms"));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void formatContentToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (editorTabControl.SelectedTab != null)
                {
                    Editor currentEditor = (Editor)editorTabControl.SelectedTab.Tag;
                    if (currentEditor != null)
                    {
                        if (currentEditor.SourceFile.GetKind() == SourceFile.Kind.cm)
                        {
                            currentEditor.FormatContent();
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void runActiveProjectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (buildInProgress)
                {
                    throw new Exception("cannot run while build is in progress");
                }
                if (solution != null)
                {
                    Project activeProject = solution.ActiveProject;
                    if (activeProject != null)
                    {
                        if (activeProject.Target != Target.program && activeProject.Target != Target.winapp)
                        {
                            throw new Exception("cannot execute: active project is not a program");
                        }
                        console.ReadOnly = false;
                        console.Clear();
                        outputTabControl.SelectedTab = consoleTabPage;
                        console.Focus();
                        ProjectConfig config = ProjectConfigurations.Instance.GetProjectConfig(activeProject.GetConfigurationFilePath());
                        string arguments = config.GetCommandLineArguments(configComboBox.Text);
                        string executablePath = activeProject.GetExecutablePath(configComboBox.Text);
                        if (!File.Exists(executablePath))
                        {
                            throw new Exception("executable '" + executablePath + "' not found");
                        }
                        abortToolStripMenuItem.Enabled = true;
                        processRunning = true;
                        SetState(State.running);
                        executor.DoExecute(executablePath, arguments);
                    }
                }
                else
                {
                    throw new Exception("no solution open");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void abortToolStripMenuItem_Click(object sender, EventArgs e)
        {
            batchBuilding = false;
            if (processRunning)
            {
                executor.TerminateRunningProcess();
            }
            else if (buildInProgress)
            {
                compiler.AbortCompile();
                compileAborted = true;
                HandleCompileResult(null);
            }
            else if (state == State.profiling)
            {
                profiler.AbortProfile();
                profileAborted = true;
                HandleProfileResult("profile aborted", 1);
            }
            else if (state == State.unitTesting)
            {
                unitTester.AbortUnitTesting();
                unitTestingAborted = true;
                HandleUnitTestingResult("unit testing aborted", 1);
            }
        }
        private void cancelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            batchBuilding = false;
            if (processRunning)
            {
                executor.TerminateRunningProcess();
            }
            else if (buildInProgress)
            {
                compiler.AbortCompile();
                compileAborted = true;
                HandleCompileResult(null);
            }
            else if (state == State.profiling)
            {
                profiler.AbortProfile();
                profileAborted = true;
                HandleProfileResult("profile aborted", 1);
            }
            else if (state == State.unitTesting)
            {
                unitTester.AbortUnitTesting();
                unitTestingAborted = true;
                HandleUnitTestingResult("unit testing aborted", 1);
            }
        }
        private async void closeToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            await CloseSolution();
        }
        private void renameToolStripMenuItem_Click(object sender, EventArgs e)
        {
            solutionExplorerTreeView.SelectedNode.BeginEdit();
        }
        private void projectReferencesItem_Click(object sender, EventArgs e)
        {
            try
            {
                TreeNode selectedNode = solutionExplorerTreeView.SelectedNode;
                if (selectedNode != null)
                {
                    Project selectedProject = selectedNode.Tag as Project;
                    if (selectedProject != null)
                    {
                        ProjectReferencesDialog dialog = new ProjectReferencesDialog(addProjectReferenceDialog, solution, selectedProject);
                        dialog.ShowDialog();
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void findToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                SourceFile currentSourceFile = null;
                TabPage selectedTab = editorTabControl.SelectedTab;
                if (selectedTab != null)
                {
                    Editor currentEditor = (Editor)selectedTab.Tag;
                    if (currentEditor != null)
                    {
                        currentSourceFile = currentEditor.SourceFile;
                    }
                }
                FindDialog findDialog = new FindDialog(currentSourceFile != null);
                findDialog.FindWhat = findWhatText;
                findDialog.MatchCase = matchCase;
                findDialog.MatchWholeWord = matchWholeWord;
                findDialog.UseRegularExpression = useRegularExpression;
                if (findDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    outputTabControl.SelectedTab = findResultsTabPage;
                    findWhatText = findDialog.FindWhat;
                    matchCase = findDialog.MatchCase;
                    matchWholeWord = findDialog.MatchWholeWord;
                    useRegularExpression = findDialog.UseRegularExpression;
                    Solution findSolution = null;
                    Project findProject = null;
                    SourceFile findSourceFile = null;
                    if (findDialog.LookIn == FindExtent.entireSolution)
                    {
                        findSolution = solution;
                    }
                    else if (findDialog.LookIn == FindExtent.activeProject)
                    {
                        findProject = solution.ActiveProject;
                    }
                    else if (findDialog.LookIn == FindExtent.currentDocument)
                    {
                        findSourceFile = currentSourceFile;
                    }
                    Cursor prevCursor = Cursor.Current;
                    try
                    {
                        Cursor.Current = Cursors.WaitCursor;
                        findResultsListView.Items.Clear();
                        infoTimer.Stop();
                        infoLabel.Text = "Searching '" + findWhatText + "'...";
                        List<FindResult> results = Finder.Find(findSolution, findProject, findSourceFile, findDialog.MatchCase, findDialog.MatchWholeWord, findDialog.UseRegularExpression, findWhatText);
                        foreach (FindResult result in results)
                        {
                            ListViewItem item = new ListViewItem(new string[] { result.File.FilePath, result.LineNumber.ToString(), result.File.Project.Name, result.Line });
                            item.Tag = result;
                            findResultsListView.Items.Add(item);
                        }
                        infoTimer.Start();
                        infoLabel.Text = results.Count.ToString() + " matching lines found";
                    }
                    finally
                    {
                        Cursor.Current = prevCursor;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void gotoLineToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                GotoDialog gotoDialog = new GotoDialog();
                if (gotoDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    int line = gotoDialog.Line;
                    if (editorTabControl.SelectedTab != null)
                    {
                        Editor currentEditor = (Editor)editorTabControl.SelectedTab.Tag;
                        if (currentEditor != null)
                        {
                            currentEditor.SetCursorPos(line, 1);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void findResultsListView_ItemActivate(object sender, EventArgs e)
        {
            if (findResultsListView.SelectedItems.Count > 0)
            {
                ListViewItem selectedItem = findResultsListView.SelectedItems[0];
                FindResult result = (FindResult)selectedItem.Tag;
                SourceFile sourceFile = result.File;
                Editor editor = EditSourceFile(sourceFile);
                editor.SetCursorPos(result.LineNumber, 1);
                editor.SelectLine(result.LineNumber);
            }
        }
        private void findResultsListView_ColumnWidthChanged(object sender, ColumnWidthChangedEventArgs e)
        {
            try
            {
                switch (e.ColumnIndex)
                {
                    case 0: Configuration.Instance.FindFileColWidth = findResultsListView.Columns[0].Width; break;
                    case 1: Configuration.Instance.FindLineColWidth = findResultsListView.Columns[1].Width; break;
                    case 2: Configuration.Instance.FindProjectColWidth = findResultsListView.Columns[2].Width; break;
                    case 3: Configuration.Instance.FindTextColWidth = findResultsListView.Columns[3].Width; break;
                }
                Configuration.Instance.Save();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void setAsActiveProjectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (solution != null)
                {
                    if (state == State.debugging)
                    {
                        throw new Exception("Cannot set active project while debugging");
                    }
                    TreeNode selectedNode = solutionExplorerTreeView.SelectedNode;
                    if (selectedNode != null)
                    {
                        Project project = (Project)selectedNode.Tag;
                        solution.ActiveProject = project;
                        solution.Save();
                        await SetupSolutionExplorer(project);
                    }
                }
                SetMenuItemStatus();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void toolStripMenuItem17_Click(object sender, EventArgs e)
        {
            try
            {
                string cmajorRoot = Environment.GetEnvironmentVariable("CMAJOR_ROOT");
                if (string.IsNullOrEmpty(cmajorRoot))
                {
                    throw new Exception("CMAJOR_ROOT environment variable not set, please set it to contain /path/to/cmajor directory");
                }
                System.Diagnostics.Process.Start(Path.Combine(cmajorRoot, Path.Combine("doc", "index.html")));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void toolStripMenuItem7_Click(object sender, EventArgs e)
        {
            try
            {
                System.Diagnostics.Process.Start("http://slaakko.github.io/cmajor/");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void languageReferenceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                string cmajorRoot = Environment.GetEnvironmentVariable("CMAJOR_ROOT");
                if (string.IsNullOrEmpty(cmajorRoot))
                {
                    throw new Exception("CMAJOR_ROOT environment variable not set, please set it to contain /path/to/cmajor directory");
                }
                System.Diagnostics.Process.Start(Path.Combine(cmajorRoot, Path.Combine("doc", Path.Combine("langref", "langref.html"))));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void libraryDocumentationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                string cmajorRoot = Environment.GetEnvironmentVariable("CMAJOR_ROOT");
                if (string.IsNullOrEmpty(cmajorRoot))
                {
                    throw new Exception("CMAJOR_ROOT environment variable not set, please set it to contain /path/to/cmajor directory");
                }
                System.Diagnostics.Process.Start(Path.Combine(cmajorRoot, Path.Combine("doc", Path.Combine("system", "index.html"))));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void projectPropertiesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                TreeNode selectedNode = solutionExplorerTreeView.SelectedNode;
                if (selectedNode != null)
                {
                    Project selectedProject = selectedNode.Tag as Project;
                    if (selectedProject != null)
                    {
                        ProjectPropertiesDialog dialog = new ProjectPropertiesDialog(selectedProject);
                        dialog.ShowDialog();
                    }
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void examplesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                string cmajorRootDir = Environment.GetEnvironmentVariable("CMAJOR_ROOT");
                await OpenProjectOrSolution(Path.Combine(Path.Combine(Path.Combine(cmajorRootDir, "projects"), "examples"), "examples.cms"));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void parserGeneratorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                string cmajorRootDir = Environment.GetEnvironmentVariable("CMAJOR_ROOT");
                await OpenProjectOrSolution(Path.Combine(Path.Combine(Path.Combine(Path.Combine(cmajorRootDir, "projects"), "tools"), "cmparsergen"), "cmparsergen.cms"));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private async void systemXToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                string cmajorRootDir = Environment.GetEnvironmentVariable("CMAJOR_ROOT");
                await OpenProjectOrSolution(Path.Combine(Path.Combine(Path.Combine(cmajorRootDir, "projects"), "cmsx"), "cmsx.cms"));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

        }
        private void profileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                TreeNode selectedNode = solutionExplorerTreeView.SelectedNode;
                if (selectedNode != null)
                {
                    Project selectedProject = selectedNode.Tag as Project;
                    if (selectedProject != null)
                    {
                        if (selectedProject.Target == Target.program)
                        {
                            ProfileProject(selectedProject);
                        }
                        else
                        {
                            throw new Exception("project '" + selectedProject.Name + "' is not a program");
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void projectContextMenuStrip_Opening(object sender, CancelEventArgs e)
        {
            profileToolStripMenuItem.Enabled = false;
            runUnitTestsToolStripMenuItem1.Enabled = false;
            buildToolStripMenuItem2.Enabled = false;
            TreeNode selectedNode = solutionExplorerTreeView.SelectedNode;
            if (selectedNode != null)
            {
                Project selectedProject = selectedNode.Tag as Project;
                if (selectedProject != null)
                {
                    if (selectedProject.Target == Target.program)
                    {
                        profileToolStripMenuItem.Enabled = true;
                    }
                    else if (selectedProject.Target == Target.unitTest)
                    {
                        runUnitTestsToolStripMenuItem1.Enabled = true;
                    }
                    if (selectedProject.Target == Target.program || selectedProject.Target == Target.library)
                    {
                        buildToolStripMenuItem2.Enabled = true;
                    }
                }
            }
        }
        private async void unitTestsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                string cmajorRootDir = Environment.GetEnvironmentVariable("CMAJOR_ROOT");
                await OpenProjectOrSolution(Path.Combine(Path.Combine(Path.Combine(cmajorRootDir, "unitTest"), "cmajor"), "cmajor.cms"));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void runUnitTestsToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            try
            {
                SourceFile sourceFile = (SourceFile)solutionExplorerTreeView.SelectedNode.Tag;
                if (sourceFile != null)
                {
                    string file = sourceFile.FilePath;
                    Project project = sourceFile.Project;
                    if (project != null)
                    {
                        RunUnitTests(project, file);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void runUnitTestsToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            try
            {
                TreeNode selectedNode = solutionExplorerTreeView.SelectedNode;
                if (selectedNode != null)
                {
                    Project selectedProject = selectedNode.Tag as Project;
                    if (selectedProject != null)
                    {
                        RunUnitTests(selectedProject, null);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void runUnitTestsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (solution != null)
                {
                    RunUnitTests(solution);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void runUnitTestsInSolutionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (solution != null)
                {
                    RunUnitTests(solution);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void runUnitTestsIsActiveProjectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (solution != null && solution.ActiveProject != null)
                {
                    RunUnitTests(solution.ActiveProject, null);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void sourceFileContextMenuStrip_Opening(object sender, CancelEventArgs e)
        {
            try
            {
                runUnitTestsToolStripMenuItem2.Enabled = false;
                TreeNode selectedNode = solutionExplorerTreeView.SelectedNode;
                if (selectedNode != null)
                {
                    SourceFile selectedSourceFile = selectedNode.Tag as SourceFile;
                    if (selectedSourceFile != null)
                    {
                        Project selectedProject = selectedSourceFile.Project;
                        if (selectedProject != null)
                        {
                            if (selectedProject.Target == Target.unitTest)
                            {
                                runUnitTestsToolStripMenuItem2.Enabled = true;
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        public bool EditModuleBuilt
        {
            get { return editModuleBuilt; }
        }
        private Solution solution;
        private XTabControl editorTabControl;
        private State state;
        private bool alterConfig;
        private CompilerLib compiler;
        private Profiler profiler;
        private Executor executor;
        private UnitTester unitTester;
        private ConsoleWindow console;
        private bool buildInProgress;
        private bool batchBuilding;
        private string progressChars;
        private int progressIndex;
        private bool processRunning;
        private bool compileAborted;
        private bool compiling;
        private bool profileAborted;
        private bool unitTestingAborted;
        private bool cleaning;
        private bool strictNothrow;
        private bool emitLlvm;
        private bool emitOptLlvm;
        private bool linkWithDebugRuntime;
        private bool linkUsingMsLink;
        private bool doNotUseModuleCache;
        private int numBuildThreads;
        private int optimizationLevel;
        private DateTime compileStartTime;
        private string findWhatText;
        private bool matchCase;
        private bool matchWholeWord;
        private bool useRegularExpression;
        private string profileOutFile;
        private string unitTestOutFile;
        private bool editModuleBuilt;
    }

    public static class KeyboardUtil
    {
        [DllImport("user32.dll", EntryPoint = "GetKeyState")]
        private static extern short GetKeyState(int virtualKey);
        public static bool KeyPressed(int virtualKey)
        {
            return (GetKeyState(virtualKey) & 0x8000) != 0;
        }
    }
}
