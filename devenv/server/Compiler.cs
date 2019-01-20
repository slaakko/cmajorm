using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;
using System.Diagnostics;
using devcore;
using parser;

namespace server
{
    public delegate void CompileReady(CompileResult compileResult);

    public class CompileResult
    {
        public CompileResult()
        {
            diagnostics = new List<Diagnostic>();
            success = true;
        }
        public CompileResult(bool success)
        {
            diagnostics = new List<Diagnostic>();
            this.success = success;
        }
        public bool Success
        {
            get { return success; }
        }
        public void AddDiagnostic(Diagnostic diagnostic)
        {
            diagnostics.Add(diagnostic);
        }
        public List<Diagnostic> Diagnostics
        {
            get { return diagnostics; }
        }
        private bool success;
        private List<Diagnostic> diagnostics;
    }

/*
    public class CompileResult
    {
        public CompileResult(bool success)
        {
            this.success = success;
            this.diagnostics = null;
            this.warnings = new List<Warning>();
        }
        public CompileResult(bool success, Diagnostics diagnostics)
        {
            this.success = success;
            this.diagnostics = diagnostics;
            this.warnings = new List<Warning>();
        }
        public CompileResult(JsonValue json)
        {
            this.warnings = new List<Warning>();
            JsonObject resultObject = json as JsonObject;
            if (resultObject != null)
            {
                JsonValue successValue = resultObject.GetField("success");
                if (successValue != null)
                {
                    JsonBool successBool = successValue as JsonBool;
                    if (successBool != null)
                    {
                        success = successBool.Value;
                    }
                }
                JsonValue diagnosticsValue = resultObject.GetField("diagnostics");
                if (diagnosticsValue != null)
                {
                    diagnostics = new Diagnostics(diagnosticsValue);
                }
                JsonValue warningsValue = resultObject.GetField("warnings");
                if (warningsValue != null)
                {
                    JsonArray warningsArray = warningsValue as JsonArray;
                    if (warningsArray != null)
                    {
                        int n = warningsArray.Count;
                        for (int i = 0; i < n; ++i)
                        {
                            JsonValue warningValue = warningsArray[i];
                            Warning warning = new Warning(warningValue);
                            warnings.Add(warning);
                        }
                    }
                }
            }
        }
        public bool Success
        {
            get { return success; }
        }
        public Diagnostics Diagnostics
        {
            get { return diagnostics; }
        }
        public List<Warning> Warnings
        {
            get { return warnings; }
        }
        private bool success;
        private Diagnostics diagnostics;
        private List<Warning> warnings;
    }

    public class Diagnostics
    {
        public Diagnostics(string tool, string kind, string project, string message)
        {
            this.tool = tool;
            this.kind = kind;
            this.project = project;
            this.message = message;
            this.references = new List<Reference>();
        }
        public Diagnostics(JsonValue json)
        {
            this.references = new List<Reference>();
            JsonObject diagnostics = json as JsonObject;
            if (diagnostics != null)
            {
                JsonValue toolValue = diagnostics.GetField("tool");
                if (toolValue != null)
                {
                    JsonString s = toolValue as JsonString;
                    if (s != null)
                    {
                        tool = s.Value;
                    }
                }
                JsonValue kindValue = diagnostics.GetField("kind");
                if (kindValue != null)
                {
                    JsonString s = kindValue as JsonString;
                    if (s != null)
                    {
                        kind = s.Value;
                    }
                }
                JsonValue projectValue = diagnostics.GetField("project");
                if (projectValue != null)
                {
                    JsonString s = projectValue as JsonString;
                    if (s != null)
                    {
                        project = s.Value;
                    }
                }
                JsonValue messageValue = diagnostics.GetField("message");
                if (messageValue != null)
                {
                    JsonString s = messageValue as JsonString;
                    if (s != null)
                    {
                        message = s.Value;
                    }
                }
                JsonValue referencesValue = diagnostics.GetField("references");
                if (referencesValue != null)
                {
                    JsonArray referencesArray = referencesValue as JsonArray;
                    if (referencesArray != null)
                    {
                        int n = referencesArray.Count;
                        for (int i = 0; i < n; ++i)
                        {
                            JsonValue referenceValue = referencesArray[i];
                            if (referenceValue != null)
                            {
                                references.Add(new Reference(referenceValue));
                            }
                        }
                    }
                }
            }
        }
        public string Tool
        {
            get { return tool; }
        }
        public string Kind
        {
            get { return kind; }
        }
        public string Project
        {
            get { return project; }
        }
        public string Message
        {
            get { return message; }
        }
        public List<Reference> References
        {
            get { return references; }
        }
        private string tool;
        private string kind;
        private string project;
        private string message;
        private List<Reference> references;
    }

    public class Warning
    {
        public Warning(string project, string message)
        {
            this.project = project;
            this.message = message;
            this.references = new List<Reference>();
        }
        public Warning(JsonValue json)
        {
            this.references = new List<Reference>();
            if (json is JsonObject)
            {
                JsonObject warning = (JsonObject)json;
                JsonValue projectValue = warning.GetField("project");
                if (projectValue != null)
                {
                    JsonString s = projectValue as JsonString;
                    if (s != null)
                    {
                        project = s.Value;
                    }
                }
                JsonValue messageValue = warning.GetField("message");
                if (messageValue != null)
                {
                    JsonString s = messageValue as JsonString;
                    if (s != null)
                    {
                        message = s.Value;
                    }
                }
                JsonValue referencesValue = warning.GetField("references");
                if (referencesValue != null)
                {
                    JsonArray referencesArray = referencesValue as JsonArray;
                    if (referencesArray != null)
                    {
                        int n = referencesArray.Count;
                        for (int i = 0; i < n; ++i)
                        {
                            JsonValue referenceValue = referencesArray[i];
                            if (referenceValue != null)
                            {
                                references.Add(new Reference(referenceValue));
                            }
                        }
                    }
                }
            }
        }
        public string Project
        {
            get { return project; }
        }
        public string Message
        {
            get { return message; }
        }
        public List<Reference> References
        {
            get { return references; }
        }
        private string project;
        private string message;
        private List<Reference> references;
    }

    public class Reference
    {
        public Reference(string file, int line, int startCol, int endCol, string text)
        {
            this.file = file;
            this.line = line;
            this.startCol = startCol;
            this.endCol = endCol;
            this.text = text;
        }
        public Reference(JsonValue json)
        {
            JsonObject referenceObject = json as JsonObject;
            if (referenceObject != null)
            {
                JsonValue fileValue = referenceObject.GetField("file");
                if (fileValue != null)
                {
                    JsonString s = fileValue as JsonString;
                    if (s != null)
                    {
                        file = s.Value;
                    }
                }
                JsonValue lineValue = referenceObject.GetField("line");
                if (lineValue != null)
                {
                    JsonString s = lineValue as JsonString;
                    if (s != null)
                    {
                        line = int.Parse(s.Value);
                    }
                }
                JsonValue startColValue = referenceObject.GetField("startCol");
                if (startColValue != null)
                {
                    JsonString s = startColValue as JsonString;
                    if (s != null)
                    {
                        startCol = int.Parse(s.Value);
                    }
                }
                JsonValue endColValue = referenceObject.GetField("endCol");
                if (endColValue != null)
                {
                    JsonString s = endColValue as JsonString;
                    if (s != null)
                    {
                        endCol = int.Parse(s.Value);
                    }
                }
                JsonValue textValue = referenceObject.GetField("text");
                if (textValue != null)
                {
                    JsonString s = textValue as JsonString;
                    if (s != null)
                    {
                        text = s.Value;
                    }
                }
            }
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

    public class Compiler : RequestHandler
    {
        public Compiler()
        {
            this.cmcPath = "cmc.exe";
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
        public string CmcPath
        {
            get { return cmcPath; }
            set { cmcPath = value; }
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
        private void Compile(string filePath, string config, bool strictNothrow, bool emitLlvm, bool emitOptLlvm, bool linkWithDebugRuntime, bool linkUsingMsLink, 
            int optimizationLevel, int numBuildThreads)
        {
            try
            {
                StringBuilder arguments = new StringBuilder();
                arguments.Append("--config=").Append(config);
                arguments.Append(" --ide");
                arguments.Append(" --verbose");
                arguments.Append(" --time");
                if (strictNothrow)
                {
                    arguments.Append(" --strict-nothrow");
                }
                if (emitLlvm)
                {
                    arguments.Append(" --emit-llvm");
                }
                if (emitOptLlvm)
                {
                    arguments.Append(" --emit-opt-llvm");
                }
                if (linkWithDebugRuntime)
                {
                    arguments.Append(" --link-with-debug-runtime");
                }
                if (linkUsingMsLink)
                {
                    arguments.Append(" --link-using-ms-link");
                }
                if (optimizationLevel != -1)
                {
                    arguments.Append(" --optimization-level=" + optimizationLevel.ToString());
                }
                if (numBuildThreads != 0)
                {
                    arguments.Append(" --build-threads=" + numBuildThreads.ToString());
                }
                arguments.Append(" \"").Append(filePath + "\"");
                ProcessStartInfo startInfo = new ProcessStartInfo(cmcPath, arguments.ToString());
                startInfo.UseShellExecute = false;
                startInfo.CreateNoWindow = true;
                startInfo.RedirectStandardOutput = true;
                startInfo.RedirectStandardError = true;
                compileAborted = false;
                Process cmc = Process.Start(startInfo);
                compileProcess = cmc;
                List<byte> stdoutBytes = new List<byte>();
                List<byte> stderrBytes = new List<byte>();
                int readResult = cmc.StandardOutput.BaseStream.ReadByte();
                while (readResult != -1)
                {
                    stdoutBytes.Add((byte)readResult);
                    readResult = cmc.StandardOutput.BaseStream.ReadByte();
                    if ((char)readResult == '\n')
                    {
                        if (writeControl != null && writeDelegate != null)
                        {
                            writeControl.Invoke(writeDelegate, UTF8.Decode(stdoutBytes.ToArray()));
                        }
                        stdoutBytes.Clear();
                    }
                    if (compileAborted)
                    {
                        break;
                    }
                }
                while (stdoutBytes.Count() > 0)
                {
                    if (writeControl != null && writeDelegate != null)
                    {
                        writeControl.Invoke(writeDelegate, UTF8.Decode(stdoutBytes.ToArray()));
                    }
                    stdoutBytes.Clear();
                    if (compileAborted)
                    {
                        break;
                    }
                    readResult = cmc.StandardOutput.BaseStream.ReadByte();
                    while (readResult != -1)
                    {
                        stdoutBytes.Add((byte)readResult);
                        readResult = cmc.StandardOutput.BaseStream.ReadByte();
                    }
                }
                if (compileAborted)
                {
                    writeControl.Invoke(writeDelegate, "Compile aborted by the user.");
                    return;
                }
                cmc.WaitForExit();
                compileProcess = null;
                int stderrReadResult = cmc.StandardError.BaseStream.ReadByte();
                while (stderrReadResult != -1)
                {
                    stderrBytes.Add((byte)stderrReadResult);
                    stderrReadResult = cmc.StandardError.BaseStream.ReadByte();
                }
                string compileResultString = UTF8.Decode(stderrBytes.ToArray());
                if (compileResultHandler != null && compileReady != null)
                {
                    CompileResult compileResult = ParseCompileResult(compileResultString);
                    compileResultHandler.Invoke(compileReady, compileResult);
                }
            }
            catch (Exception ex)
            {
                compileProcess = null;
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, ex.ToString());
                }
                if (compileResultHandler != null && compileReady != null)
                {
                    CompileResult compileResult = new CompileResult(false, new Diagnostics("cmdevenv", "internal error", "", ex.ToString()));
                    compileResultHandler.Invoke(compileReady, compileResult);
                }
            }
        }
        private CompileResult ParseCompileResult(string compileResultString)
        {
            try
            {
                if (!string.IsNullOrEmpty(compileResultString))
                {
                    JsonValue json = ParserRepository.Instance.JsonParser.Parse(compileResultString, 0, "");
                    CompileResult compileResult = new CompileResult(json);
                    return compileResult;
                }
                else
                {
                    CompileResult compileResult = new CompileResult(false, new Diagnostics("cmdevenv", "internal error", "", "empty compile result received"));
                    return compileResult;
                }
            }
            catch (Exception ex)
            {
                CompileResult compileResult = new CompileResult(false, new Diagnostics("cmdevenv", "internal error", "", ex.ToString()));
                return compileResult;
            }
        }
        public void AbortCompile()
        {
            if (compileProcess != null)
            {
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, "Abort compile pending...");
                }
                compileProcess.Kill();
                compileAborted = true;
                compileProcess = null;
            }
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
        public override void HandleCleanRequest(CleanRequest request)
        {
            Clean(request.FilePath, request.Config);
        }
        private void Clean(string solutionOrProjectFilePath, string config)
        {
            try
            {
                StringBuilder arguments = new StringBuilder();
                arguments.Append("--config=").Append(config);
                arguments.Append(" --ide");
                arguments.Append(" --verbose");
                arguments.Append(" --clean");
                arguments.Append(" \"" + solutionOrProjectFilePath + "\"");
                ProcessStartInfo startInfo = new ProcessStartInfo(cmcPath, arguments.ToString());
                startInfo.UseShellExecute = false;
                startInfo.CreateNoWindow = true;
                startInfo.RedirectStandardOutput = true;
                startInfo.RedirectStandardError = true;
                compileAborted = false;
                Process cmc = Process.Start(startInfo);
                compileProcess = cmc;
                List<byte> bytes = new List<byte>();
                int readResult = cmc.StandardOutput.Read();
                while (readResult != -1)
                {
                    bytes.Add((byte)readResult);
                    readResult = cmc.StandardOutput.Read();
                }
                while (bytes.Count() > 0)
                {
                    if (writeControl != null && writeDelegate != null)
                    {
                        writeControl.Invoke(writeDelegate, UTF8.Decode(bytes.ToArray()));
                    }
                    bytes.Clear();
                    readResult = cmc.StandardOutput.Read();
                    while (readResult != -1)
                    {
                        bytes.Add((byte)readResult);
                        readResult = cmc.StandardOutput.Read();
                    }
                }
                cmc.WaitForExit();
                compileProcess = null;
                if (compileAborted)
                {
                    writeControl.Invoke(writeDelegate, "Clean aborted by the user.");
                    return;
                }
                List<byte> stderrBytes = new List<byte>();
                int stderrReadResult = cmc.StandardError.Read();
                while (stderrReadResult != -1)
                {
                    stderrBytes.Add((byte)stderrReadResult);
                    stderrReadResult = cmc.StandardError.Read();
                }
                string cleanResultString = UTF8.Decode(stderrBytes.ToArray());
                if (cleanResultString != null && compileReady != null)
                {
                    CompileResult compileResult = ParseCompileResult(cleanResultString);
                    compileResultHandler.Invoke(compileReady, compileResult);
                }
            }
            catch (Exception ex)
            {
                compileProcess = null;
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, ex.ToString());
                }
                if (compileResultHandler != null && compileReady != null)
                {
                    CompileResult compileResult = new CompileResult(false, new Diagnostics("cmdevenv", "internal error", "", ex.ToString()));
                    compileResultHandler.Invoke(compileReady, compileResult);
                }
            }
        }
        private string cmcPath;
        private Queue<Request> requestQueue;
        private ManualResetEvent requestWaiting;
        private Thread compileThread;
        private bool exiting;
        private ManualResetEvent exit;
        private Control writeControl;
        private WriteLineToOutputWindow writeDelegate;
        private Control compileResultHandler;
        private CompileReady compileReady;
        private Process compileProcess;
        private bool compileAborted;
    }
*/
}
