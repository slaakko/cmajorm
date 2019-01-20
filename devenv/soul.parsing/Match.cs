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
    public struct Match
    {
        public Match(bool hit, int length)
        {
            this.hit = hit;
            this.length = length;
        }
        public bool Hit
        {
            get { return hit; }
        }
        public int Length
        {
            get { return length; }
        }
        public void Concatenate(Match that)
        {
            length += that.length;
        }
        public static Match Empty()
        {
            return new Match(true, 0);
        }
        public static Match One()
        {
            return new Match(true, 1);
        }
        public static Match Nothing()
        {
            return new Match(false, -1);
        }
        private bool hit;
        private int length;
    }
}
