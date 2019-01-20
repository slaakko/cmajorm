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
    public delegate void UnitTestingReady(string errorMessage, int exitCode);

    public class UnitTester : RequestHandler
    {
        public UnitTester()
        {
            this.requestQueue = new Queue<Request>();
            this.requestWaiting = new ManualResetEvent(false);
            this.exit = new ManualResetEvent(false);
            this.unitTestingThread = new Thread(new ThreadStart(ProcessRequests));
            this.unitTestingThread.Start();
            this.unitTestingAborted = false;
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
        public void SetHandleUnitTestingResultMethod(Control unitTestingResultHandler, UnitTestingReady readyMethod)
        {
            this.unitTestResultHandler = unitTestingResultHandler;
            this.unitTestingReady = readyMethod;
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
        public void AbortUnitTesting()
        {
            if (unitTestingProcess != null)
            {
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, "Abort unit testing pending...\n");
                }
                unitTestingProcess.Kill();
                unitTestingAborted = true;
                unitTestingProcess = null;
            }
        }
        public void DoUnitTest(string projectOrSolutionFilePath, string config, string file, bool linkWithDebugRuntime, bool linkUsingMsLink, string outFile)
        {
            Request request = new UnitTestRequest(projectOrSolutionFilePath, config, file, linkWithDebugRuntime, linkUsingMsLink, outFile);
            lock (requestQueue)
            {
                requestQueue.Enqueue(request);
            }
            requestWaiting.Set();
        }
        public override void HandleUnitTestRequest(UnitTestRequest request)
        {
            UnitTest(request.projectOrSolutionFilePath, request.config, request.file, request.linkWithDebugRuntime, request.linkUsingMsLink, request.outFile);
        }
        private void UnitTest(string projectOrSolutionFilePath, string config, string file, bool linkWithDebugRuntime, bool linkUsingMsLink, string outFile)
        {
            try
            {
                StringBuilder arguments = new StringBuilder();
                arguments.Append("--verbose");
                if (linkWithDebugRuntime)
                {
                    arguments.Append(" --link-with-debug-runtime");
                }
                if (linkUsingMsLink)
                {
                    arguments.Append(" --link-using-ms-link");
                }
                arguments.Append(" --config=").Append(config);
                if (!string.IsNullOrEmpty(file))
                {
                    arguments.Append(" --file=\"").Append(file).Append("\"");
                }
                arguments.Append(" --out=\"" + outFile + "\"");
                arguments.Append(" \"" + projectOrSolutionFilePath + "\"");
                ProcessStartInfo startInfo = new ProcessStartInfo("cmunit", arguments.ToString());
                startInfo.UseShellExecute = false;
                startInfo.CreateNoWindow = true;
                startInfo.RedirectStandardOutput = true;
                startInfo.RedirectStandardError = true;
                unitTestingAborted = false;
                Process cmunit = Process.Start(startInfo);
                unitTestingProcess = cmunit;
                List<byte> stdoutBytes = new List<byte>();
                List<byte> stderrBytes = new List<byte>();
                int readResult = cmunit.StandardOutput.BaseStream.ReadByte();
                while (readResult != -1)
                {
                    stdoutBytes.Add((byte)readResult);
                    readResult = cmunit.StandardOutput.BaseStream.ReadByte();
                    if ((char)readResult == '\n')
                    {
                        if (writeControl != null && writeDelegate != null)
                        {
                            writeControl.Invoke(writeDelegate, UTF8.Decode(stdoutBytes.ToArray()));
                        }
                        stdoutBytes.Clear();
                    }
                    if (unitTestingAborted)
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
                    if (unitTestingAborted)
                    {
                        break;
                    }
                    readResult = cmunit.StandardOutput.BaseStream.ReadByte();
                    while (readResult != -1)
                    {
                        stdoutBytes.Add((byte)readResult);
                        readResult = cmunit.StandardOutput.BaseStream.ReadByte();
                    }
                }
                if (unitTestingAborted)
                {
                    writeControl.Invoke(writeDelegate, "Unit testing aborted by the user.");
                    return;
                }
                cmunit.WaitForExit();
                unitTestingProcess = null;
                int stderrReadResult = cmunit.StandardError.BaseStream.ReadByte();
                while (stderrReadResult != -1)
                {
                    stderrBytes.Add((byte)stderrReadResult);
                    stderrReadResult = cmunit.StandardError.BaseStream.ReadByte();
                }
                string unitTestingResultString = UTF8.Decode(stderrBytes.ToArray());
                if (unitTestResultHandler != null && unitTestingReady != null)
                {
                    unitTestResultHandler.Invoke(unitTestingReady, unitTestingResultString, cmunit.ExitCode);
                }
            }
            catch (Exception ex)
            {
                unitTestingProcess = null;
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, ex.ToString());
                }
                if (unitTestResultHandler != null && unitTestingReady != null)
                {
                    unitTestResultHandler.Invoke(unitTestingReady, "internal error: " + ex.Message, 1);
                }
            }
        }
        private Queue<Request> requestQueue;
        private ManualResetEvent requestWaiting;
        private Thread unitTestingThread;
        private bool exiting;
        private ManualResetEvent exit;
        private Control writeControl;
        private WriteLineToOutputWindow writeDelegate;
        private Control unitTestResultHandler;
        private UnitTestingReady unitTestingReady;
        private bool unitTestingAborted;
        private Process unitTestingProcess;
    }
}
