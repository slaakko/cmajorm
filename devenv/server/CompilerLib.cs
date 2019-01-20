using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Xml;
using System.IO;

namespace server
{
    public class Span
    {
        public Span(string file, int line, int startCol, int endCol, string text)
        {
            this.file = file;
            this.line = line;
            this.startCol = startCol;
            this.endCol = endCol;
            this.text = text;
        }
        public string File
        {
            get { return file; }
        }
        public int Line
        {
            get { return line; }
        }
        public int StartCol
        {
            get { return startCol; }
        }
        public int EndCol
        {
            get { return endCol; }
        }
        public string Text
        {
            get { return text; }
        }
        private string file;
        private int line;
        private int startCol;
        private int endCol;
        private string text;
    }

    public class Diagnostic
    {
        public Diagnostic(string category, string message, string tool, string project)
        {
            this.category = category;
            this.message = message;
            this.tool = tool;
            this.project = project;
            this.span = null;
        }
        public string Category
        {
            get { return category; }
        }
        public string Message
        {
            get { return message; }
        }
        public string Tool
        {
            get { return tool; }
        }
        public string Project
        {
            get { return project; }
        }
        public Span Span
        {
            get { return span; }
            set { span = value; }
        }
        private string category;
        private string message;
        private string tool;
        private string project;
        private Span span;
    }

    public class CompilerLib : RequestHandler
    {
        public CompilerLib()
        {
            Init();
            this.requestQueue = new Queue<Request>();
            this.requestWaiting = new ManualResetEvent(false);
            this.exit = new ManualResetEvent(false);
            this.compileThread = new Thread(new ThreadStart(ProcessRequests));
            this.compileThread.Start();
        }
        private void ProcessRequests()
        {
            while (!exiting)
            {
                requestWaiting.WaitOne();
                requestWaiting.Reset();
                lock (requestQueue)
                {
                    Request request = requestQueue.Dequeue();
                    request.Process(this);
                }
            }
            exit.Set();
        }
        public override void HandleExitRequest(ExitRequest request)
        {
            exiting = true;
        }
        public override void HandleCompileRequest(CompileRequest request)
        {
            Compile(request.FilePath, request.Config, request.StrictNothrow, request.EmitLlvm, request.EmitOptLlvm, request.LinkWithDebugRuntime, request.LinkUsingMsLink,
                request.OptimizationLevel, request.NumBuildThreads);
        }
        public override void HandleCleanRequest(CleanRequest request)
        {
            Clean(request.FilePath, request.Config);
        }
        public void SetWriteMethod(Control writer, WriteLineToOutputWindow writeMethod)
        {
            writeControl = writer;
            writeDelegate = writeMethod;
        }
        public void SetHandleCompileResultMethod(Control compileResultHandler, CompileReady readyMethod)
        {
            this.compileResultHandler = compileResultHandler;
            this.compileReady = readyMethod;
        }
        public void DoResetModuleCache()
        {
            ResetModuleCache();
        }
        public void DoSetUseModuleCache(bool useModuleCache)
        {
            SetUseModuleCache(useModuleCache);
        }
        public void DoExit()
        {
            Request request = new ExitRequest();
            lock (requestQueue)
            {
                requestQueue.Enqueue(request);
            }
            requestWaiting.Set();
        }
        public void WaitForExit()
        {
            exit.WaitOne();
            Done();
        }
        public void DoCompile(string filePath, string config, bool strictNothrow, bool emitLlvm, bool emitOptLlvm, bool linkWithDebugRuntime, bool linkUsingMsLink,
            int optimizationLevel, int numBuildThreads)
        {
            Request request = new CompileRequest(filePath, config, strictNothrow, emitLlvm, emitOptLlvm, linkWithDebugRuntime, linkUsingMsLink, optimizationLevel,
                numBuildThreads);
            lock (requestQueue)
            {
                requestQueue.Enqueue(request);
            }
            requestWaiting.Set();
        }
        public void DoClean(string solutionOrProjectFilePath, string config)
        {
            Request request = new CleanRequest(solutionOrProjectFilePath, config);
            lock (requestQueue)
            {
                requestQueue.Enqueue(request);
            }
            requestWaiting.Set();
        }
        private void LogMessages()
        {
            try
            {
                int result = WaitForLogMessage();
                while (result >= 0)
                {
                    int bufferSize = result + 1;
                    StringBuilder logMessageBuffer = new StringBuilder(bufferSize);
                    result = FetchLogMessage(logMessageBuffer, bufferSize);
                    if (result >= 0)
                    {
                        if (writeControl != null && writeDelegate != null)
                        {
                            writeControl.Invoke(writeDelegate, logMessageBuffer.ToString());
                        }
                    }
                    else
                    {
                        throw new Exception("FetchLogMessage() returned " + result.ToString());
                    }
                    result = WaitForLogMessage();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void Compile(string filePath, string config, bool strictNothrow, bool emitLlvm, bool emitOptLlvm, bool linkWithDebugRuntime, bool linkUsingMsLink,
            int optimizationLevel, int numBuildThreads)
        {
            try
            {
                StartLog();
                loggerThread = new Thread(LogMessages);
                loggerThread.Start();
                XmlDocument compileRequestXmlDoc = new XmlDocument();
                XmlElement compileRequestElement = compileRequestXmlDoc.CreateElement("compileRequest");
                compileRequestElement.SetAttribute("filePath", filePath);
                compileRequestElement.SetAttribute("config", config);
                compileRequestElement.SetAttribute("verbose", "true");
                compileRequestElement.SetAttribute("time", "true");
                if (strictNothrow)
                {
                    compileRequestElement.SetAttribute("strict-nothrow", "true");
                }
                if (emitLlvm)
                {
                    compileRequestElement.SetAttribute("emit-llvm", "true");
                }
                if (emitOptLlvm)
                {
                    compileRequestElement.SetAttribute("emit-opt-llvm", "true");
                }
                if (linkWithDebugRuntime)
                {
                    compileRequestElement.SetAttribute("link-with-debug-runtime", "true");
                }
                if (linkUsingMsLink)
                {
                    compileRequestElement.SetAttribute("link-using-ms-link", "true");
                }
                if (optimizationLevel != -1)
                {
                    compileRequestElement.SetAttribute("optimization-level", optimizationLevel.ToString());
                }
                if (numBuildThreads != 0)
                {
                    compileRequestElement.SetAttribute("build-threads", numBuildThreads.ToString());
                }
                compileRequestXmlDoc.AppendChild(compileRequestElement);
                StringBuilder compileRequest = new StringBuilder();
                StringWriter stringWriter = new StringWriter(compileRequest);
                compileRequestXmlDoc.Save(stringWriter);
                string compileXmlRequest = compileRequest.ToString();
                compileAborted = false;
                int compileResultHandle = Compile(compileXmlRequest);
                if (compileResultHandle < 0)
                {
                    throw new Exception("Compile() returned " + compileResultHandle.ToString());
                }
                if (compileAborted)
                {
                    writeControl.Invoke(writeDelegate, "Compile aborted by the user.");
                    return;
                }
                int compileResultLength = GetCompileResultLength(compileResultHandle);
                if (compileResultLength < 0)
                {
                    throw new Exception("GetCompileResultLength() returned " + compileResultHandle.ToString());
                }
                StringBuilder compileResultStrBuilder = new StringBuilder(compileResultLength + 1);
                int result = GetCompileResult(compileResultHandle, compileResultStrBuilder, compileResultLength + 1);
                if (result < 0)
                {
                    throw new Exception("GetCompileResult() returned " + compileResultHandle.ToString());
                }
                string compileXmlResult = compileResultStrBuilder.ToString();
                XmlDocument compileResultXmlDoc = new XmlDocument();
                StringReader stringReader = new StringReader(compileXmlResult);
                compileResultXmlDoc.Load(stringReader);
                CompileResult compileResult = new CompileResult(compileResultXmlDoc.DocumentElement.GetAttribute("success") == "true");
                XmlNodeList diagnosticNodes = compileResultXmlDoc.SelectNodes("/compileResult/diagnostics/diagnostic");
                foreach (XmlNode diagnosticNode in diagnosticNodes)
                {
                    XmlElement diagnosticElement = diagnosticNode as XmlElement;
                    if (diagnosticElement != null)
                    {
                        string category = diagnosticElement.GetAttribute("category");
                        string message = diagnosticElement.GetAttribute("message");
                        string tool = diagnosticElement.GetAttribute("tool");
                        string project = diagnosticElement.GetAttribute("project");
                        Diagnostic diagnostic = new Diagnostic(category, message, tool, project);
                        XmlNode spanNode = diagnosticElement.SelectSingleNode("span");
                        if (spanNode != null)
                        {
                            XmlElement spanElement = spanNode as XmlElement;
                            if (spanElement != null)
                            {
                                string file = spanElement.GetAttribute("file");
                                int line;
                                if (!int.TryParse(spanElement.GetAttribute("line"), out line))
                                {
                                    line = 0;
                                }
                                int startCol;
                                if (!int.TryParse(spanElement.GetAttribute("startCol"), out startCol))
                                {
                                    startCol = 0;
                                }
                                int endCol;
                                if (!int.TryParse(spanElement.GetAttribute("endCol"), out endCol))
                                {
                                    endCol = 0;
                                }
                                string text = spanElement.GetAttribute("text");
                                Span span = new Span(file, line, startCol, endCol, text);
                                diagnostic.Span = span;
                            }
                        }
                        compileResult.AddDiagnostic(diagnostic);
                    }
                }
                EndLog();
                if (compileResultHandler != null && compileReady != null)
                {
                    compileResultHandler.Invoke(compileReady, compileResult);
                }
            }
            catch (Exception ex)
            {
                EndLog();
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, ex.ToString());
                }
                if (compileResultHandler != null && compileReady != null)
                {
                    CompileResult compileResult = new CompileResult(false);
                    compileResult.AddDiagnostic(new Diagnostic("internal error", ex.ToString(), "cmdevenv", ""));
                    compileResultHandler.Invoke(compileReady, compileResult);
                }
            }
        }
        private void Clean(string solutionOrProjectFilePath, string config)
        {
            try
            {
                StartLog();
                loggerThread = new Thread(LogMessages);
                loggerThread.Start();
                XmlDocument compileRequestXmlDoc = new XmlDocument();
                XmlElement compileRequestElement = compileRequestXmlDoc.CreateElement("compileRequest");
                compileRequestElement.SetAttribute("filePath", solutionOrProjectFilePath);
                compileRequestElement.SetAttribute("config", config);
                compileRequestElement.SetAttribute("verbose", "true");
                compileRequestElement.SetAttribute("clean", "true");
                compileRequestXmlDoc.AppendChild(compileRequestElement);
                StringBuilder compileRequest = new StringBuilder();
                StringWriter stringWriter = new StringWriter(compileRequest);
                compileRequestXmlDoc.Save(stringWriter);
                string compileXmlRequest = compileRequest.ToString();
                compileAborted = false;
                int compileResultHandle = Compile(compileXmlRequest);
                if (compileResultHandle < 0)
                {
                    throw new Exception("Compile() returned " + compileResultHandle.ToString());
                }
                if (compileAborted)
                {
                    writeControl.Invoke(writeDelegate, "Compile aborted by the user.");
                    return;
                }
                int compileResultLength = GetCompileResultLength(compileResultHandle);
                if (compileResultLength < 0)
                {
                    throw new Exception("GetCompileResultLength() returned " + compileResultHandle.ToString());
                }
                StringBuilder compileResultStrBuilder = new StringBuilder(compileResultLength + 1);
                int result = GetCompileResult(compileResultHandle, compileResultStrBuilder, compileResultLength + 1);
                if (result < 0)
                {
                    throw new Exception("GetCompileResult() returned " + compileResultHandle.ToString());
                }
                string compileXmlResult = compileResultStrBuilder.ToString();
                XmlDocument compileResultXmlDoc = new XmlDocument();
                StringReader stringReader = new StringReader(compileXmlResult);
                compileResultXmlDoc.Load(stringReader);
                CompileResult compileResult = new CompileResult(compileResultXmlDoc.DocumentElement.GetAttribute("success") == "true");
                XmlNodeList diagnosticNodes = compileResultXmlDoc.SelectNodes("/compileResult/diagnostics/diagnostic");
                foreach (XmlNode diagnosticNode in diagnosticNodes)
                {
                    XmlElement diagnosticElement = diagnosticNode as XmlElement;
                    if (diagnosticElement != null)
                    {
                        string category = diagnosticElement.GetAttribute("category");
                        string message = diagnosticElement.GetAttribute("message");
                        string tool = diagnosticElement.GetAttribute("tool");
                        string project = diagnosticElement.GetAttribute("project");
                        Diagnostic diagnostic = new Diagnostic(category, message, tool, project);
                        XmlNode spanNode = diagnosticElement.SelectSingleNode("span");
                        if (spanNode != null)
                        {
                            XmlElement spanElement = spanNode as XmlElement;
                            if (spanElement != null)
                            {
                                string file = spanElement.GetAttribute("file");
                                int line;
                                if (!int.TryParse(spanElement.GetAttribute("line"), out line))
                                {
                                    line = 0;
                                }
                                int startCol;
                                if (!int.TryParse(spanElement.GetAttribute("startCol"), out startCol))
                                {
                                    startCol = 0;
                                }
                                int endCol;
                                if (!int.TryParse(spanElement.GetAttribute("endCol"), out endCol))
                                {
                                    endCol = 0;
                                }
                                string text = spanElement.GetAttribute("text");
                                Span span = new Span(file, line, startCol, endCol, text);
                                diagnostic.Span = span;
                            }
                        }
                        compileResult.AddDiagnostic(diagnostic);
                    }
                }
                EndLog();
                if (compileResultHandler != null && compileReady != null)
                {
                    compileResultHandler.Invoke(compileReady, compileResult);
                }
            }
            catch (Exception ex)
            {
                EndLog();
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, ex.ToString());
                }
                if (compileResultHandler != null && compileReady != null)
                {
                    CompileResult compileResult = new CompileResult(false);
                    compileResult.AddDiagnostic(new Diagnostic("internal error", ex.ToString(), "cmdevenv", ""));
                    compileResultHandler.Invoke(compileReady, compileResult);
                }
            }
        }
        public void AbortCompile()
        {
            try
            {
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, "Abort compile pending...");
                }
                StopBuild();
                compileAborted = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private Thread compileThread;
        private Queue<Request> requestQueue;
        private ManualResetEvent requestWaiting;
        private bool exiting;
        private ManualResetEvent exit;
        private Control writeControl;
        private WriteLineToOutputWindow writeDelegate;
        private Control compileResultHandler;
        private CompileReady compileReady;
        private bool compileAborted;
        private Thread loggerThread;

#if (DEBUG)

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void Init();

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void Done();

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern int Compile(string compileXmlRequest);

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern int GetCompileResultLength(int compileResultHandle);

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern int GetCompileResult(int compileResultHandle, StringBuilder compileResult, int size);

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void StopBuild();

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern int WaitForLogMessage();

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern int FetchLogMessage(StringBuilder logMessage, int size);

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void StartLog();

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void EndLog();

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void ResetModuleCache();

        [DllImport("cmcmd.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void SetUseModuleCache(bool useModuleCache);

#else

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void Init();

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void Done();

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern int Compile(string compileXmlRequest);

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern int GetCompileResultLength(int compileResultHandle);

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern int GetCompileResult(int compileResultHandle, StringBuilder compileResult, int size);

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void StopBuild();

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern int WaitForLogMessage();

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern int FetchLogMessage(StringBuilder logMessage, int size);

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void StartLog();

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void EndLog();

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void ResetModuleCache();

        [DllImport("cmcm.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void SetUseModuleCache(bool useModuleCache);

#endif

    }
}
