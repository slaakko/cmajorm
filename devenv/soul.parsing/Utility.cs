/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace soul.parsing
{
    public static class Utility
    {
        public static string GetErrorLines(string content, Position position)
        {
            int lineStartIndex = Math.Min(position.Index, content.Length);
            while (lineStartIndex > 0 && (lineStartIndex == content.Length || content[lineStartIndex] != '\n'))
            {
                --lineStartIndex;
            }
            if (lineStartIndex < content.Length && content[lineStartIndex] == '\n')
            {
                ++lineStartIndex;
            }
            int lineEndIndex = Math.Min(position.Index, content.Length);
            while (lineEndIndex < content.Length && content[lineEndIndex] != '\n' && content[lineEndIndex] != '\r')
            {
                ++lineEndIndex;
            }
            string errorLine = content.Substring(lineStartIndex, lineEndIndex - lineStartIndex);
            StringBuilder spaceLine = new StringBuilder();
            int n = Math.Min(position.Index, content.Length);
            for (int i = lineStartIndex; i < n; ++i)
            {
                spaceLine.Append(content[i] == '\t' ? '\t' : ' ');
            }
            return errorLine + Environment.NewLine + spaceLine.ToString() + "^";
        }
    }
}
