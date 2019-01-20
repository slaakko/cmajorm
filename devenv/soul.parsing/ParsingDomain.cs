/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Reflection;

namespace soul.parsing
{
    public class ParsingDomain
    {
        public ParsingDomain()
        {
            grammars = new Dictionary<string, Grammar>();
        }
        public Grammar GetGrammar(string grammarFullName, string assemblyFileName, string assemblySearchPath)
        {
            Grammar grammar;
            if (grammars.TryGetValue(grammarFullName, out grammar))
            {
                return grammar;
            }
            throw new Exception("grammar '" + grammarFullName + "' not found");
        }
        public void AddGrammar(Grammar grammar)
        {
            if (!grammars.ContainsKey(grammar.FullName))
            {
                grammars.Add(grammar.FullName, grammar);
            }
        }
        private Dictionary<string, Grammar> grammars;
    }
}