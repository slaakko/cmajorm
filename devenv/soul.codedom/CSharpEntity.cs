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
using master.util.text;

namespace soul.codedom
{
    [Flags]
    public enum PrintFlags
    {
        none = 0, omitNewLine = 1, printSemicolon = 2
    }

    public abstract class CSharpEntity
    {
        public CSharpEntity(string name)
        {
            this.name = name;
        }
        public string Name
        {
            get { return name; }
            set { name = value; }
        }
        public abstract void Print(CodeFormatter formatter, PrintFlags flags);
        public abstract void Accept(Visitor visitor);
        public virtual int Rank { get { return 100; } }
        public override string ToString()
        {
            return name;
        }
        private string name;
    }
}
