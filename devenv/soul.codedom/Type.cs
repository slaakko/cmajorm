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
    public abstract class Type : CSharpEntity
    {
        public Type(string name)
            : base(name)
        {
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write(Name);
        }
    }

    public abstract class ClassType : Type
    {
        public ClassType(string name)
            : base(name)
        {
        }
    }

    public class ObjectType : ClassType
    {
        public ObjectType()
            : base("object")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class StringType : ClassType
    {
        public StringType()
            : base("string")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public abstract class SimpleType : Type
    {
        public SimpleType(string name)
            : base(name)
        {
        }
    }

    public abstract class NumericType : SimpleType
    {
        public NumericType(string name)
            : base(name)
        {
        }
    }

    public class DecimalType : NumericType
    {
        public DecimalType()
            : base("decimal")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public abstract class FloatingPointType : NumericType
    {
        public FloatingPointType(string name)
            : base(name)
        {
        }
    }

    public class DoubleType : FloatingPointType
    {
        public DoubleType()
            : base("double")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class FloatType : FloatingPointType
    {
        public FloatType()
            : base("float")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public abstract class IntegralType : NumericType
    {
        public IntegralType(string name)
            : base(name)
        {
        }
    }

    public class SByteType : IntegralType
    {
        public SByteType()
            : base("sbyte")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class ByteType : IntegralType
    {
        public ByteType()
            : base("byte")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class ShortType : IntegralType
    {
        public ShortType()
            : base("short")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class UShortType : IntegralType
    {
        public UShortType()
            : base("ushort")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class IntType : IntegralType
    {
        public IntType()
            : base("int")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class UIntType : IntegralType
    {
        public UIntType()
            : base("uint")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class LongType : IntegralType
    {
        public LongType()
            : base("long")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class ULongType : IntegralType
    {
        public ULongType()
            : base("ulong")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class CharType : IntegralType
    {
        public CharType()
            : base("char")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class BoolType : SimpleType
    {
        public BoolType()
            : base("bool")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public abstract class ComplexType : Type
    {
        public ComplexType(string name)
            : base(name)
        {
        }
    }

    public class GenericType : ComplexType
    {
        public GenericType(CSharpEntity baseType, List<CSharpEntity> typeArguments)
            : base(Utility.MakeGenericTypeName(baseType, typeArguments))
        {
            this.baseType = baseType;
            this.typeArguments = typeArguments;
        }
        public CSharpEntity BaseType
        {
            get { return baseType; }
        }
        public List<CSharpEntity> TypeArguments
        {
            get { return typeArguments; }
        }
        public override void Print(CodeFormatter formatter, PrintFlags flags)
        {
            formatter.Write(Name);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity baseType;
        private List<CSharpEntity> typeArguments;
    }

    public class NullableType : ComplexType
    {
        public NullableType(CSharpEntity baseType)
            : base(baseType.Name + "?")
        {
            this.baseType = baseType;
        }
        public CSharpEntity BaseType
        {
            get { return baseType; }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private CSharpEntity baseType;
    }

    public class ArrayType : ComplexType
    {
        public ArrayType(CSharpEntity baseType, List<int> rankSpecifiers)
            : base(Utility.MakeArrayTypeName(baseType, rankSpecifiers))
        {
            this.rankSpecifiers = rankSpecifiers;
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
        private List<int> rankSpecifiers;
    }

    public class VoidType : SimpleType
    {
        public VoidType()
            : base("void")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class VarType : Type
    {
        public VarType()
            : base("var")
        {
        }
        public override void Accept(Visitor visitor)
        {
            visitor.Visit(this);
        }
    }

    public class PredefinedTypeMap
    {
        static PredefinedTypeMap()
        {
            map = new Dictionary<string, Type>();
            map.Add("bool", new BoolType());
            map.Add("sbyte", new SByteType());
            map.Add("byte", new ByteType());
            map.Add("short", new ShortType());
            map.Add("ushort", new UShortType());
            map.Add("int", new IntType());
            map.Add("uint", new UIntType());
            map.Add("long", new LongType());
            map.Add("ulong", new ULongType());
            map.Add("double", new DoubleType());
            map.Add("float", new FloatType());
            map.Add("decimal", new DecimalType());
            map.Add("char", new CharType());
            map.Add("object", new ObjectType());
            map.Add("string", new StringType());
            map.Add("void", new VoidType());
        }
        public static Type GetType(string typeName)
        {
            return map[typeName];
        }
        private static Dictionary<string, Type> map;
    }
}
