using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Diagnostics;
using System.Windows.Forms;
using devcore;

namespace server
{
    public delegate void ProfileReady(string errorMessage, int exitCode);

    public class Profiler : RequestHandler
    {
        public Profiler()
        {
            this.requestQueue = new Queue<Request>();
            this.requestWaiting = new ManualResetEvent(false);
            this.exit = new ManualResetEvent(false);
            this.profileThread = new Thread(new ThreadStart(ProcessRequests));
            this.profileThread.Start();
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
        public void SetWriteMethod(Control writer, WriteLineToOutputWindow writeMethod)
        {
            writeControl = writer;
            writeDelegate = writeMethod;
        }
        public void SetHandleProfileResultMethod(Control profileResultHandler, ProfileReady readyMethod)
        {
            this.profileResultHandler = profileResultHandler;
            this.profileReady = readyMethod;
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
        public void AbortProfile()
        {
            if (profileProcess != null)
            {
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, "Abort profile pending...\n");
                }
                profileProcess.Kill();
                profileAborted = true;
                profileProcess = null;
            }
        }
        public void DoProfile(bool rebuildSys, bool rebuildApp, bool inclusive, bool exclusive, bool count, bool emitLlvm, bool emitOptLlvm, bool linkWithDebugRuntime, bool linkUsingMsLink, int top,
            string outFile, string args, string project)
        {
            Request request = new ProfileRequest(rebuildSys, rebuildApp, inclusive, exclusive, count, emitLlvm, emitOptLlvm, linkWithDebugRuntime, linkUsingMsLink, top, outFile, args, project);
            lock (requestQueue)
            {
                requestQueue.Enqueue(request);
            }
            requestWaiting.Set();
        }
        public override void HandleProfileRequest(ProfileRequest request)
        {
            Profile(request.rebuildSys, request.rebuildApp, request.inclusive, request.exclusive, request.count, request.emitLlvm, request.emitOptLlvm, request.linkWithDebugRuntime, request.linkUsingMsLink,
                request.top, request.outFile, request.args, request.project);
        }
        private void Profile(bool rebuildSys, bool rebuildApp, bool inclusive, bool exclusive, bool count, bool emitLlvm, bool emitOptLlvm, bool linkWithDebugRuntime, bool linkUsingMsLink, int top,
            string outFile, string args, string project)
        {
            try
            {
                StringBuilder arguments = new StringBuilder();
                arguments.Append("--verbose");
                if (rebuildSys)
                {
                    arguments.Append(" --rebuild-sys");
                }
                if (rebuildApp)
                {
                    arguments.Append(" --rebuild-app");
                }
                if (inclusive)
                {
                    arguments.Append(" --inclusive");
                }
                if (exclusive)
                {
                    arguments.Append(" --exclusive");
                }
                if (count)
                {
                    arguments.Append(" --count");
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
                if (top == 0)
                {
                    arguments.Append(" --top=*");
                }
                else
                {
                    arguments.Append(" --top=" + top.ToString());
                }
                arguments.Append(" --out=\"" + outFile + "\"");
                if (!string.IsNullOrEmpty(args))
                {
                    arguments.Append(" --args=\"\"" + args + "\"\"");
                }
                arguments.Append(" \"" + project + "\"");
                ProcessStartInfo startInfo = new ProcessStartInfo("cmprof", arguments.ToString());
                startInfo.UseShellExecute = false;
                startInfo.CreateNoWindow = true;
                startInfo.RedirectStandardOutput = true;
                startInfo.RedirectStandardError = true;
                profileAborted = false;
                Process cmprof = Process.Start(startInfo);
                profileProcess = cmprof;
                List<byte> stdoutBytes = new List<byte>();
                List<byte> stderrBytes = new List<byte>();
                int readResult = cmprof.StandardOutput.BaseStream.ReadByte();
                while (readResult != -1)
                {
                    stdoutBytes.Add((byte)readResult);
                    readResult = cmprof.StandardOutput.BaseStream.ReadByte();
                    if ((char)readResult == '\n')
                    {
                        if (writeControl != null && writeDelegate != null)
                        {
                            writeControl.Invoke(writeDelegate, UTF8.Decode(stdoutBytes.ToArray()));
                        }
                        stdoutBytes.Clear();
                    }
                    if (profileAborted)
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
                    if (profileAborted)
                    {
                        break;
                    }
                    readResult = cmprof.StandardOutput.BaseStream.ReadByte();
                    while (readResult != -1)
                    {
                        stdoutBytes.Add((byte)readResult);
                        readResult = cmprof.StandardOutput.BaseStream.ReadByte();
                    }
                }
                if (profileAborted)
                {
                    writeControl.Invoke(writeDelegate, "Profile aborted by the user.");
                    return;
                }
                cmprof.WaitForExit();
                profileProcess = null;
                int stderrReadResult = cmprof.StandardError.BaseStream.ReadByte();
                while (stderrReadResult != -1)
                {
                    stderrBytes.Add((byte)stderrReadResult);
                    stderrReadResult = cmprof.StandardError.BaseStream.ReadByte();
                }
                string profileResultString = UTF8.Decode(stderrBytes.ToArray());
                if (profileResultHandler != null && profileReady != null)
                {
                    profileResultHandler.Invoke(profileReady, profileResultString, cmprof.ExitCode);
                }
            }
            catch (Exception ex)
            {
                profileProcess = null;
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, ex.ToString());
                }
                if (profileResultHandler != null && profileReady != null)
                {
                    profileResultHandler.Invoke(profileReady, "internal error: " + ex.Message, 1);
                }
            }
        }
        private Queue<Request> requestQueue;
        private ManualResetEvent requestWaiting;
        private Thread profileThread;
        private bool exiting;
        private ManualResetEvent exit;
        private Control writeControl;
        private WriteLineToOutputWindow writeDelegate;
        private Control profileResultHandler;
        private ProfileReady profileReady;
        private bool profileAborted;
        private Process profileProcess;
    }
}
