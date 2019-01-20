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
    public class ParsingException : Exception
    {
        public ParsingException(string message, string content, Position position, string fileName)
            : base(message + " in file '" + fileName + "' at line " + position.Line.ToString() + ":" + Environment.NewLine +
            Utility.GetErrorLines(content, position))
        {
        }
    }

    public class ExpectationFailure : Exception
    {
        public ExpectationFailure(string info, string content, Position position, string fileName)
            : base("Parsing file '" + fileName + "' failed at line " + position.Line.ToString() + ":" + Environment.NewLine + info + " expected:" +
            Environment.NewLine + Utility.GetErrorLines(content, position))
        {
            this.info = info;
            this.content = content;
            this.position = position;
            this.fileName = fileName;
        }
        public string Info
        {
            get { return info; }
        }
        public string Content
        {
            get { return content; }
        }
        public Position Position
        {
            get { return position; }
        }
        public string FileName
        {
            get { return fileName; }
        }
        private string info;
        private string content;
        private Position position;
        private string fileName;
    }

    public class PositionException : Exception
    {
        public PositionException(string message, Position position)
            : base(message)
        {
            this.position = position;
        }
        public Position Position
        {
            get { return position; }
        }
        private Position position;
    }
}
