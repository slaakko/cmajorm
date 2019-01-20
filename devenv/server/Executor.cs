using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using devcore;

namespace server
{
    public delegate void WriteMethod(string chars);
    public delegate void ExecuteReady();

    public class Executor : RequestHandler
    {
        public Executor()
        {
            requestQueue = new Queue<Request>();
            requestWaiting = new ManualResetEvent(false);
            exit = new ManualResetEvent(false);
            executeThread = new Thread(new ThreadStart(ProcessRequests));
            executeThread.Start();
            processStdin = null;
            runningProcess = null;
            processKilled = false;
        }
        public void SetWriteMethod(Control writer, WriteMethod writeMethod)
        {
            writeControl = writer;
            writeDelegate = writeMethod;
        }
        public void SetExecuteReadyMethod(Control executeReadyControl, ExecuteReady executeReady)
        {
            this.executeReadyControl = executeReadyControl;
            this.executeReady = executeReady;
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
        public void DoExecute(string executablePath, string arguments)
        {
            Request request = new ExecuteRequest(executablePath, arguments);
            lock (requestQueue)
            {
                requestQueue.Enqueue(request);
            }
            requestWaiting.Set();
        }
        public void TerminateRunningProcess()
        {
            if (runningProcess != null)
            {
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, "Terminate user process pending...");
                }
                runningProcess.Kill();
                processKilled = true;
                runningProcess = null;
            }
        }
        public bool InputExpected()
        {
            return processStdin != null;
        }
        public string WriteLineToProcessStandardInput(string line)
        {
            string bufferedOutput = "";
            if (processStdin != null)
            {
                if (stdoutBytes != null && stdoutBytes.Count > 0)
                {
                    bufferedOutput = UTF8.Decode(stdoutBytes.ToArray());
                    stdoutBytes.Clear();
                }
                byte[] bytes = UTF8.Encode(line);
                foreach (byte b in bytes)
                {
                    processStdin.BaseStream.WriteByte(b);
                }
                processStdin.WriteLine();
            }
            else
            {
                throw new Exception("standard input of the process is closed, because of process exit");
            }
            return bufferedOutput;
        }
        public string CloseProcessStandardInput()
        {
            string bufferedOutput = "";
            if (processStdin != null)
            {
                if (stdoutBytes != null && stdoutBytes.Count > 0)
                {
                    bufferedOutput = UTF8.Decode(stdoutBytes.ToArray());
                    stdoutBytes.Clear();
                }
                processStdin.Close();
            }
            return bufferedOutput;
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
        }
        public override void HandleCleanRequest(CleanRequest request)
        {
        }
        public override void HandleExecuteRequest(ExecuteRequest request)
        {
            Execute(request.ExecutablePath, request.Arguments);
        }
        private void Execute(string executablePath, string arguments)
        {
            try
            {
                if (writeControl != null && writeDelegate != null)
                {
                    string line = "Running '" + executablePath + (!string.IsNullOrEmpty(arguments) ? "' with arguments: [" + arguments + "]" : "'") + ":\n\n";
                    writeControl.Invoke(writeDelegate, line);
                }
                ProcessStartInfo startInfo = new ProcessStartInfo(executablePath, arguments);
                startInfo.UseShellExecute = false;
                startInfo.CreateNoWindow = true;
                startInfo.RedirectStandardInput = true;
                startInfo.RedirectStandardOutput = true;
                startInfo.RedirectStandardError = true;
                startInfo.WorkingDirectory = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(executablePath), "../.."));
                processKilled = false;
                Process process = Process.Start(startInfo);
                processStdin = process.StandardInput;
                runningProcess = process;
                int stdoutByte = process.StandardOutput.BaseStream.ReadByte();
                stdoutBytes = new List<byte>();
                stderrBytes = new List<byte>();
                while (stdoutByte != -1)
                {
                    stdoutBytes.Add((byte)stdoutByte);
                    if ((char)stdoutByte == '\n')
                    {
                        if (writeControl != null && writeDelegate != null)
                        {
                            writeControl.Invoke(writeDelegate, UTF8.Decode(stdoutBytes.ToArray()));
                        }
                        stdoutBytes.Clear();
                    }
                    stdoutByte = process.StandardOutput.BaseStream.ReadByte();
                }
                int stderrByte = process.StandardError.BaseStream.ReadByte();
                while (stderrByte != -1)
                {
                    stderrBytes.Add((byte)stderrByte);
                    if ((char)stderrByte == '\n')
                    {
                        if (writeControl != null && writeDelegate != null)
                        {
                            writeControl.Invoke(writeDelegate, UTF8.Decode(stderrBytes.ToArray()));
                        }
                        stderrBytes.Clear();
                    }
                    stderrByte = process.StandardError.BaseStream.ReadByte();
                }
                if (stdoutBytes.Count > 0)
                {
                    if (writeControl != null && writeDelegate != null)
                    {
                        writeControl.Invoke(writeDelegate, UTF8.Decode(stdoutBytes.ToArray()));
                    }
                }
                if (stderrBytes.Count > 0)
                {
                    if (writeControl != null && writeDelegate != null)
                    {
                        writeControl.Invoke(writeDelegate, UTF8.Decode(stderrBytes.ToArray()));
                    }
                }
                process.WaitForExit();
                runningProcess = null;
                processStdin = null;
                if (writeControl != null && writeDelegate != null)
                {
                    if (processKilled)
                    {
                        writeControl.Invoke(writeDelegate, "\nProcess terminated by the user.\n");
                        processKilled = false;
                    }
                    writeControl.Invoke(writeDelegate, "\nProcess returned exit code " + process.ExitCode.ToString() + ".\n");
                }
                executeReadyControl.Invoke(executeReady);
                stdoutBytes = null;
                stderrBytes = null;
            }
            catch (Exception ex)
            {
                runningProcess = null;
                if (writeControl != null && writeDelegate != null)
                {
                    writeControl.Invoke(writeDelegate, ex.ToString());
                }
            }
        }
        private Queue<Request> requestQueue;
        private ManualResetEvent requestWaiting;
        private Thread executeThread;
        private bool exiting;
        private ManualResetEvent exit;
        private Control writeControl;
        private WriteMethod writeDelegate;
        private Control executeReadyControl;
        private ExecuteReady executeReady;
        private Process runningProcess;
        private StreamWriter processStdin;
        private List<byte> stdoutBytes;
        private List<byte> stderrBytes;
        private bool processKilled;
    }
}
