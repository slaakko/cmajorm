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
using System.Globalization;

namespace soul.codedom
{
    public abstract class Object : CSharpEntity
    {
        public Object(string name)
            : base(name)
        {
        }
        public override int Rank { get { return 15; } }
    }

    public abstract class ValueObject<ValueType> : Object
    {
        public ValueObject(string name)
            : base(name)
        {
        }
        public ValueObject(string name, ValueType value)
            : base(name)
        {
            this.value = value;
        }
        public ValueType Value
        {
            get { return this.value; }
        }
        public override string ToString()
        {
            return value.ToString();
        }
        private ValueType value;
    }

    public class BoolObject : ValueObject<bool>
    {
        public BoolObject()
            : base("bool_object")
        {
        }
        public BoolObject(bool value)
            : base("bool_object", value)
        {
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (Value)
            {
                formatter.Write("true");
            }
            else
            {
                formatter.Write("false");
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class IntegerObject : ValueObject<long>
    {
        public IntegerObject()
            : base("integer_object")
        {
        }
        public IntegerObject(long value, string suffix)
            : base("integer_object", value)
        {
            this.suffix = suffix;
        }
        public IntegerObject(long value, string suffix, bool hex)
            : this(value, suffix)
        {
            this.hex = hex;
        }
        public string Suffix
        {
            get { return suffix; }
        }
        public bool Hex
        {
            get { return hex; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            if (hex)
            {
                formatter.Write("0x" + Convert.ToString(Value, 16));
            }
            else
            {
                formatter.Write(Value.ToString());
            }
            if (!string.IsNullOrEmpty(suffix))
            {
                formatter.Write(suffix);
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private string suffix;
        private bool hex;
    }

    public class DoubleObject : ValueObject<double>
    {
        public DoubleObject()
            : base("double_object")
        {
        }
        public DoubleObject(double value, string suffix)
            : base("double_object", value)
        {
            this.suffix = suffix;
        }
        public string Suffix
        {
            get { return suffix; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write(Value.ToString(CultureInfo.InvariantCulture));
            if (!string.IsNullOrEmpty(suffix))
            {
                formatter.Write(suffix);
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        public override string ToString()
        {
            return Value.ToString(CultureInfo.InvariantCulture);
        }
        private string suffix;
    }

    public class CharObject : ValueObject<char>
    {
        public CharObject()
            : base("char_object")
        {
        }
        public CharObject(char value)
            : base("char_object", value)
        {
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("'" + CharConvert.CharStr(Value) + "'");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class StringObject : ValueObject<string>
    {
        public StringObject()
            : base("string_object")
        {
        }
        public StringObject(string value)
            : base("string_object", value)
        {
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("\"" + CharConvert.StringStr(Value) + "\"");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class NullObject : ValueObject<object>
    {
        public NullObject()
            : base("null", 0)
        {
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write("null");
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }
}
