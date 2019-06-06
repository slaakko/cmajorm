using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace server
{
    public delegate void WriteLineToOutputWindow(string line);

    public abstract class RequestHandler
    {
        public virtual void HandleCompileRequest(CompileRequest request) { }
        public virtual void HandleExitRequest(ExitRequest request) { }
        public virtual void HandleCleanRequest(CleanRequest request) { }
        public virtual void HandleExecuteRequest(ExecuteRequest request) { }
        public virtual void HandleProfileRequest(ProfileRequest request) { }
        public virtual void HandleUnitTestRequest(UnitTestRequest request) { }
    }

    public abstract class Request
    {
        public abstract void Process(RequestHandler handler);
    }

    public class ExitRequest : Request
    {
        public override void Process(RequestHandler handler)
        {
            handler.HandleExitRequest(this);
        }
    }

    public class CompileRequest : Request
    {
        public CompileRequest(string filePath, string config, bool strictNothrow, bool emitLlvm, bool emitOptLlvm, bool linkWithDebugRuntime, bool linkUsingMsLink, 
            int optimizationLevel, int numBuildThreads, bool rebuild)
        {
            this.filePath = filePath;
            this.config = config;
            this.strictNothrow = strictNothrow;
            this.emitLlvm = emitLlvm;
            this.emitOptLlvm = emitOptLlvm;
            this.linkWithDebugRuntime = linkWithDebugRuntime;
            this.linkUsingMsLink = linkUsingMsLink;
            this.optimizationLevel = optimizationLevel;
            this.numBuildThreads = numBuildThreads;
            this.rebuild = rebuild;
        }
        public override void Process(RequestHandler handler)
        {
            handler.HandleCompileRequest(this);
        }
        public string FilePath
        {
            get { return filePath; }
        }
        public string Config
        {
            get { return config; }
        }
        public bool StrictNothrow
        {
            get { return strictNothrow; }
        }
        public bool EmitLlvm
        {
            get { return emitLlvm; }
        }
        public bool EmitOptLlvm
        {
            get { return emitOptLlvm; }
        }
        public bool LinkWithDebugRuntime
        {
            get { return linkWithDebugRuntime; }
        }
        public bool LinkUsingMsLink
        {
            get { return linkUsingMsLink; }
        }
        public int OptimizationLevel
        {
            get { return optimizationLevel; }
        }
        public int NumBuildThreads
        {
            get { return numBuildThreads; }
        }
        public bool Rebuild
        {
            get { return rebuild; }
        }
        private string filePath;
        private string config;
        private bool strictNothrow;
        private bool emitLlvm;
        private bool emitOptLlvm;
        private bool linkWithDebugRuntime;
        private bool linkUsingMsLink;
        private int optimizationLevel;
        private int numBuildThreads;
        private bool rebuild;
    }

    public class CleanRequest : Request
    {
        public CleanRequest(string filePath, string config)
        {
            this.filePath = filePath;
            this.config = config;
        }
        public override void Process(RequestHandler handler)
        {
            handler.HandleCleanRequest(this);
        }
        public string FilePath
        {
            get { return filePath; }
        }
        public string Config
        {
            get { return config; }
        }
        private string filePath;
        private string config;
    }

    public class ExecuteRequest : Request
    {
        public ExecuteRequest(string executablePath, string arguments)
        {
            this.executablePath = executablePath;
            this.arguments = arguments;
        }
        public string ExecutablePath
        {
            get { return executablePath; }
        }
        public string Arguments
        {
            get { return arguments; }
        }
        public override void Process(RequestHandler handler)
        {
            handler.HandleExecuteRequest(this);
        }
        private string executablePath;
        private string arguments;
    }

    public class ProfileRequest : Request
    {
        public ProfileRequest(bool rebuildSys, bool rebuildApp, bool inclusive, bool exclusive, bool count, bool emitLlvm, bool emitOptLlvm, bool linkWithDebugRuntime, bool linkUsingMsLink, int top, 
            string outFile, string args, string project)
        {
            this.rebuildSys = rebuildSys;
            this.rebuildApp = rebuildApp;
            this.inclusive = inclusive;
            this.exclusive = exclusive;
            this.count = count;
            this.emitLlvm = emitLlvm;
            this.emitOptLlvm = emitOptLlvm;
            this.linkWithDebugRuntime = linkWithDebugRuntime;
            this.linkUsingMsLink = linkUsingMsLink;
            this.top = top;
            this.outFile = outFile;
            this.args = args;
            this.project = project;
        }
        public override void Process(RequestHandler handler)
        {
            handler.HandleProfileRequest(this);
        }
        public bool rebuildSys;
        public bool rebuildApp;
        public bool inclusive;
        public bool exclusive;
        public bool count;
        public bool emitLlvm;
        public bool emitOptLlvm;
        public bool linkWithDebugRuntime;
        public bool linkUsingMsLink;
        public int top;
        public string outFile;
        public string args;
        public string project;
    }

    public class UnitTestRequest : Request
    {
        public UnitTestRequest(string projectOrSolutionFilePath, string config, string file, bool linkWithDebugRuntime, bool linkUsingMsLink, string outFile)
        {
            this.projectOrSolutionFilePath = projectOrSolutionFilePath;
            this.config = config;
            this.file = file;
            this.linkWithDebugRuntime = linkWithDebugRuntime;
            this.linkUsingMsLink = linkUsingMsLink;
            this.outFile = outFile;
        }
        public override void Process(RequestHandler handler)
        {
            handler.HandleUnitTestRequest(this);
        }
        public string projectOrSolutionFilePath;
        public string config;
        public string file;
        public bool linkWithDebugRuntime;
        public bool linkUsingMsLink;
        public string outFile;
    }
}
