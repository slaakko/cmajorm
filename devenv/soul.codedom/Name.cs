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
    public class Name : CSharpEntity
    {
        public Name(string name)
            : this(name, null)
        {
        }
        public Name(string name, List<CSharpEntity> typeArguments)
            : base(name)
        {
            this.typeArguments = typeArguments;
        }
        public List<CSharpEntity> TypeArguments
        {
            get { return typeArguments; }
        }
        public override int Rank { get { return 15; } }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write(Name);
            if (typeArguments != null && typeArguments.Count > 0)
            {
                formatter.Write("<");
                foreach (CSharpEntity typeArgument in typeArguments)
                {
                    typeArgument.Print(formatter, flags);
                }
                formatter.Write(">");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private List<CSharpEntity> typeArguments;
    }
}
