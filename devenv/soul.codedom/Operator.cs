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
    public enum Operator
    {
        assign, plusAssign, minusAssign, mulAssign, divAssign, remAssign, andAssign, orAssign, xorAssing, shiftLeftAssign, shiftRightAssign,
        eq, notEq, less, greater, lessOrEq, greaterOrEq, shiftLeft, shiftRight, plus, minus, mul, div, rem, plusPlus, minusMinus, not, negate
    }

    public static class OperatorMap
    {
        static OperatorMap()
        {
            op = new Dictionary<string, Operator>();
            op.Add("=", Operator.assign);
            op.Add("+=", Operator.plusAssign);
            op.Add("-=", Operator.minusAssign);
            op.Add("*=", Operator.mulAssign);
            op.Add("/=", Operator.divAssign);
            op.Add("%=", Operator.remAssign);
            op.Add("&=", Operator.andAssign);
            op.Add("|=", Operator.orAssign);
            op.Add("^=", Operator.xorAssing);
            op.Add("<<=", Operator.shiftLeftAssign);
            op.Add(">>=", Operator.shiftRightAssign);
            op.Add("==", Operator.eq);
            op.Add("!=", Operator.notEq);
            op.Add("<", Operator.less);
            op.Add(">", Operator.greater);
            op.Add("<=", Operator.lessOrEq);
            op.Add(">=", Operator.greaterOrEq);
            op.Add("<<", Operator.shiftLeft);
            op.Add(">>", Operator.shiftRight);
            op.Add("+", Operator.plus);
            op.Add("-", Operator.minus);
            op.Add("*", Operator.mul);
            op.Add("/", Operator.div);
            op.Add("%", Operator.rem);
            op.Add("++", Operator.plusPlus);
            op.Add("--", Operator.minusMinus);
            op.Add("!", Operator.not);
            op.Add("~", Operator.negate);
            str = new Dictionary<Operator, string>();
            str.Add(Operator.assign, "=");
            str.Add(Operator.plusAssign, "+=");
            str.Add(Operator.minusAssign, "-=");
            str.Add(Operator.mulAssign, "*=");
            str.Add(Operator.divAssign, "/=");
            str.Add(Operator.remAssign, "%=");
            str.Add(Operator.andAssign, "&=");
            str.Add(Operator.orAssign, "|=");
            str.Add(Operator.xorAssing, "^=");
            str.Add(Operator.shiftLeftAssign, "<<=");
            str.Add(Operator.shiftRightAssign, ">>=");
            str.Add(Operator.eq, "==");
            str.Add(Operator.notEq, "!=");
            str.Add(Operator.less, "<");
            str.Add(Operator.greater, ">");
            str.Add(Operator.lessOrEq, "<=");
            str.Add(Operator.greaterOrEq, ">=");
            str.Add(Operator.shiftLeft, "<<");
            str.Add(Operator.shiftRight, ">>");
            str.Add(Operator.plus, "+");
            str.Add(Operator.minus, "-");
            str.Add(Operator.mul, "*");
            str.Add(Operator.div, "/");
            str.Add(Operator.rem, "%");
            str.Add(Operator.plusPlus, "++");
            str.Add(Operator.minusMinus, "--");
            str.Add(Operator.not, "!");
            str.Add(Operator.negate, "~");
        }
        public static Operator Op(string opStr)
        {
            return op[opStr];
        }
        public static string Str(Operator op)
        {
            return str[op];
        }
        private static Dictionary<string, Operator> op;
        private static Dictionary<Operator, string> str;
    }
}
