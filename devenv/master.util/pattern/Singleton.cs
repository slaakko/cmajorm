/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;

namespace master.util.pattern
{
    public class Singleton<T> where T : class, new()
    {
        public static T Instance
        {
            get
            {
                return Creator.instance;
            }
        }
        private class Creator
        {
            static Creator()
            {
            }
            internal static readonly T instance = new T();
        }
    }
}
