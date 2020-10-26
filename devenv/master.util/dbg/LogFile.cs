/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.IO;

namespace master.util.debug
{
    public class LogFile
    {
        public LogFile(string logFilePath)
        {
            file = new object();
            this.logFilePath = logFilePath;
        }
        public void Write(string text)
        {
            try
            {
                lock (file)
                {
                    using (StreamWriter logWriter = File.AppendText(logFilePath))
                    {
                        logWriter.WriteLine(DateTime.Now.ToString() + ": " + text);
                    }
                }

            }
            catch
            {
            }
        }
        public string Path
        {
            get { return logFilePath; }
            set { logFilePath = value; }
        }
        private object file;
        private string logFilePath;
    }
}
