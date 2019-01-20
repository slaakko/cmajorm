/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;

namespace master.util.io
{
    public static class PathUtil
    {
        public static string GetRelativePath(string absolutePath, string relativeRoot)
        {
            string winAbsolutePath = Path.GetFullPath(absolutePath); // makes \ path
            string winRelativeRoot = Path.GetFullPath(relativeRoot); // makes \ path
            StringBuilder resultPath = new StringBuilder(1024);
            const uint FILE_ATTRIBUTE_DIRECTORY = 0x10;
            if (PathRelativePathTo(resultPath, winRelativeRoot, FILE_ATTRIBUTE_DIRECTORY, winAbsolutePath, 0))
            {
                return MakeSlashPath(resultPath.ToString());
            }
            else
            {
                throw new Exception("Paths do not have common root");
            }
        }
        [DllImport("shlwapi.dll", CharSet = CharSet.Unicode)]
        private static extern bool PathRelativePathTo(StringBuilder resultPath, string from, uint dwFrom, string to, uint dwTo);

        public static string MakeSlashPath(string path)
        {
            StringBuilder result = new StringBuilder();
            for (int i = 0; i < path.Length; ++i)
            {
                if (path[i] == '\\')
                {
                    result.Append('/');
                }
                else
                {
                    result.Append(path[i]);
                }
            }
            string r = result.ToString();
            if (r.StartsWith("./"))
            {
                return r.Substring(2);
            }
            return r;
        }

        public static string MakeCanonicalPath(string path)
        {
            path = Path.GetFullPath(path);
            return MakeSlashPath(path);
        }
    }
}
