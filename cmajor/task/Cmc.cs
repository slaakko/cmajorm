using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Build.Utilities;
using Microsoft.Build.Framework;
using System.Xml;

namespace CmajorTasks
{
    public static class UTF8
    {
        public static byte[] Encode(string s)
        {
            return Encoding.UTF8.GetBytes(s.ToArray());
        }
        public static string Decode(byte[] bytes)
        {
            char[] chars = Encoding.UTF8.GetChars(bytes);
            StringBuilder s = new StringBuilder();
            foreach (char c in chars)
            {
                s.Append(c);
            }
            return s.ToString();
        }
    }

    public class Cmc : Task
    {
        public override bool Execute()
        {
            try
            {
                if (string.IsNullOrEmpty(ProjectName))
                {
                    Log.LogError("'ProjectName' is empty");
                    return false;
                }
                if (string.IsNullOrEmpty(ProjectDir))
                {
                    Log.LogError("'ProjectDir' is empty");
                    return false;
                }
                if (string.IsNullOrEmpty(Configuration))
                {
                    Log.LogError("'Configuration' is empty");
                    return false;
                }
                if (string.IsNullOrEmpty(TargetType))
                {
                    Log.LogError("'TargetType' is empty");
                    return false;
                }
                string config = "debug";
                if (Configuration == "Release")
                {
                    config = "release";
                }
                StringBuilder arguments = new StringBuilder();
                arguments.Append("--msbuild");
                arguments.Append(" --verbose");
                arguments.Append(" --config=" + config);
                if (Sources != null)
                {
                    foreach (ITaskItem taskItem in Sources)
                    {
                        string sourceFile = taskItem.ItemSpec;
                        if (Path.GetExtension(sourceFile) == ".xml")
                        {
                            arguments.Append(" --resource=\"").Append(sourceFile).Append("\"");
                        }
                        else
                        {
                            arguments.Append(" \"").Append(sourceFile).Append("\"");
                        }
                    }
                }
                if (References != null)
                {
                    foreach (ITaskItem taskItem in References)
                    {
                        string referenceFile = taskItem.ItemSpec;
                        arguments.Append(" --reference=\"").Append(referenceFile).Append("\"");
                    }
                }
                arguments.Append(" --target=").Append(TargetType);
                arguments.Append(" --name=").Append(ProjectName);
                arguments.Append(" --dir=\"").Append(ProjectDir).Append("\"");
                System.Diagnostics.ProcessStartInfo processStartInfo = new System.Diagnostics.ProcessStartInfo("cmc", arguments.ToString());
                processStartInfo.UseShellExecute = false;
                processStartInfo.CreateNoWindow = true;
                processStartInfo.RedirectStandardOutput = true;
                processStartInfo.RedirectStandardError = true;
                System.Diagnostics.Process cmc = System.Diagnostics.Process.Start(processStartInfo);
                List<byte> stdoutBytes = new List<byte>();
                List<byte> stderrBytes = new List<byte>();
                int readResult = cmc.StandardOutput.BaseStream.ReadByte();
                while (readResult != -1)
                {
                    if ((char)readResult == '\n')
                    {
                        Log.LogMessage(MessageImportance.High, UTF8.Decode(stdoutBytes.ToArray()));
                        stdoutBytes.Clear();
                    }
                    else if ((char)readResult  != '\r')
                    {
                        stdoutBytes.Add((byte)readResult);
                    }
                    readResult = cmc.StandardOutput.BaseStream.ReadByte();
                }
                while (stdoutBytes.Count() > 0)
                {
                    Log.LogMessage(MessageImportance.High, UTF8.Decode(stdoutBytes.ToArray()));
                    stdoutBytes.Clear();
                    readResult = cmc.StandardOutput.BaseStream.ReadByte();
                    while (readResult != -1)
                    {
                        if ((char)readResult != '\n' && (char)readResult != '\r')
                        {
                            stdoutBytes.Add((byte)readResult);
                        }
                        readResult = cmc.StandardOutput.BaseStream.ReadByte();
                    }
                }
                cmc.WaitForExit();
                int stderrReadResult = cmc.StandardError.BaseStream.ReadByte();
                while (stderrReadResult != -1)
                {
                    stderrBytes.Add((byte)stderrReadResult);
                    stderrReadResult = cmc.StandardError.BaseStream.ReadByte();
                }
                string compileResultString = UTF8.Decode(stderrBytes.ToArray());
                XmlTextReader reader = new XmlTextReader(new System.IO.StringReader(compileResultString));
                bool success = false;
                string category = "error";
                string subcategory = "";
                string message = "";
                string file = "";
                int line = 0;
                int startCol = 0;
                int endCol = 0;
                string spanText = "";
                while (reader.Read())
                {
                    switch (reader.NodeType)
                    {
                        case XmlNodeType.Element:
                        {
                            if (reader.Name == "success")
                            {
                                success = reader.ReadElementContentAsBoolean();
                            }
                            else if (reader.Name == "diagnostic")
                            {
                                category = "error";
                                subcategory = "";
                                message = "";
                                file = "";
                                line = 0;
                                startCol = 0;
                                endCol = 0;
                                spanText = "";
                            }
                            else if (reader.Name == "category")
                            {
                                category = reader.ReadElementContentAsString();
                            }
                            else if (reader.Name == "subcategory")
                            {
                                subcategory = reader.ReadElementContentAsString();
                            }
                            else if (reader.Name == "message")
                            {
                                message = reader.ReadElementContentAsString();
                            }
                            else if (reader.Name == "span")
                            {
                                file = "";
                                line = 0;
                                startCol = 0;
                                endCol = 0;
                                spanText = "";
                            }
                            else if (reader.Name == "file")
                            {
                                file = reader.ReadElementContentAsString();
                            }
                            else if (reader.Name == "line")
                            {
                                line = reader.ReadElementContentAsInt();
                            }
                            else if (reader.Name == "startCol")
                            {
                                startCol = reader.ReadElementContentAsInt();
                            }
                            else if (reader.Name == "endCol")
                            {
                                endCol = reader.ReadElementContentAsInt();
                            }
                            else if (reader.Name == "text")
                            {
                                spanText = reader.ReadElementContentAsString();
                            }
                            break;
                        }
                        case XmlNodeType.EndElement:
                        {
                            if (reader.Name == "diagnostic")
                            {
                                if (category == "error")
                                {
                                    Log.LogError(subcategory, "", "", file, line, startCol, line, endCol, message);
                                }
                                else if (category == "warning")
                                {
                                    Log.LogWarning("", "", "", file, line, startCol, line, endCol, message);
                                }
                                else if (category == "info")
                                {
                                    Log.LogMessage("info", "", "", file, line, startCol, line, endCol, MessageImportance.High, message);
                                    if (!string.IsNullOrEmpty(spanText))
                                    {
                                        Log.LogMessage(MessageImportance.High, spanText);
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
                if (!success || cmc.ExitCode != 0)
                {
                    if (cmc.ExitCode != 0)
                    {
                        Log.LogError("cmc returned exit code " + cmc.ExitCode.ToString());
                    }
                    return false;
                }
            }
            catch (Exception ex)
            {
                Log.LogError(ex.ToString());
                return false;
            }
            return true;
        }

        public string ProjectName { get; set; }

        public string ProjectDir { get; set; }

        public ITaskItem[] Sources { get; set; }

        public ITaskItem[] References { get; set; }

        public string Configuration { get; set; }

        public string TargetType { get; set; }
    }
}
