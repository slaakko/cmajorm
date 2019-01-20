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

namespace soul.codedom
{
    public static class Utility
    {
        public static string MakeGenericTypeName(CSharpEntity baseType, List<CSharpEntity> typeArgumentList)
        {
            StringBuilder s = new StringBuilder();
            s.Append(baseType.Name);
            s.Append("<");
            bool first = true;
            foreach (CSharpEntity type in typeArgumentList)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    s.Append(", ");
                }
                s.Append(type.Name);
            }
            s.Append(">");
            return s.ToString();
        }
        public static string MakeArrayTypeName(CSharpEntity baseType, List<int> rankSpecifiers)
        {
            StringBuilder s = new StringBuilder();
            s.Append(baseType.Name);
            foreach (int rankSpecifier in rankSpecifiers)
            {
                s.Append('[');
                s.Append(new string(',', rankSpecifier));
                s.Append(']');
            }
            return s.ToString();
        }
    }
}
