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
    public abstract class Parser
    {
        public Parser(string info)
        {
            this.info = info;
        }
        public string Info
        {
            get { return info; }
        }
        public abstract Match Parse(Scanner scanner, Stack<object> stack);
        public abstract void Accept(Visitor visitor);
        private string info;
    }
}
