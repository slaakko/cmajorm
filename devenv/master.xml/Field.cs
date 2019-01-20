using System;
using System.Globalization;
using System.Xml;

namespace master.xml
{
//  Field is an abstract serializable class having StringField, IntField, LongField, DoubleField, DecimalField, BooleanField and DateTimeField as its concrete descendants.
//  Each concrete field type contains a single value of its type.
//  XML-serialization for a Field is of the form:
//  <name of the field>value of the field</name of the field>
//  For format of each concrete field type values see: http://www.w3.org/TR/xmlschema-2/

	public abstract class Field : Serializable
	{
        public Field()
            : base("Field")
        {
        }

        public Field(string name)
            : base(name)
        {
        }

        public Field(string name, Container container)
            : base(name, container)
        {
        }

//      If the name of the given reader's current xml element mathces the name of the field, 
//      reads the value of the field from the content of the xml element, otherwise skips the current xml element.

        public override void Read(XmlReader reader)
        {
            if (reader.IsStartElement(Name))
            {
                if (reader.IsEmptyElement)
                {
                    reader.ReadStartElement();
                }
                else
                {
                    reader.ReadStartElement();
                    if (reader.HasValue)
                    {
                        ReadValue(reader);
                    }
                    reader.ReadEndElement();
                }
                present = true;
            }
        }

//      Writes the name and value of the field to the given xml writer if the value differs from the default field value or 
//      if asked to write default values also.

        public override void Write(XmlWriter writer)
        {
            if (present)
            {
                WriteComments(writer);
                writer.WriteStartElement(Name);
                WriteValue(writer);
                writer.WriteEndElement();
            }
        }

//      Each concrete field type implements:

        public abstract void ReadValue(XmlReader reader);

//      Each concrete field type implements:

        public abstract void WriteValue(XmlWriter writer);

        public bool Present
        {
            get { return present; }
            set { present = value; }
        }

        private bool present = false;
    }

//  StringField is a Field that contains an xml string value as defined in http://www.w3.org/TR/xmlschema-2/#string
//  The default value for the StringField is an empty string.

    public class StringField : Field
    {
        public StringField()
            : base("StringField")
        {
            val = "";
        }

        public StringField(string name)
            : base(name)
        {
            val = "";
        }

        public StringField(string name, Container container)
            : base(name, container)
        {
            val = "";
        }

        public string Value
        {
            get
            {
                return val;
            }
            set
            {
                val = value;
                Present = true;
            }
        }

        public override void ReadValue(XmlReader reader)
        {
            val = reader.ReadContentAsString();
            if (val == null)
            {
                val = "";
            }
        }

        public override void WriteValue(XmlWriter writer)
        {
            writer.WriteValue(val);
        }

        private string val;
    }

//  IntField is a Field that contains an xml int value as defined in http://www.w3.org/TR/xmlschema-2/#int
//  The default value for the IntField is 0.

    public class IntField : Field
    {
        public IntField()
            : base("IntField")
        {
            val = 0;
        }

        public IntField(string name)
            : base(name)
        {
            val = 0;
        }

        public IntField(string name, Container container)
            : base(name, container)
        {
            val = 0;
        }

        public int Value
        {
            get
            {
                return val;
            }
            set
            {
                val = value;
                Present = true;
            }
        }

        public override void WriteValue(XmlWriter writer)
        {
            writer.WriteValue(val);
        }

        public override void ReadValue(XmlReader reader)
        {
            val = reader.ReadContentAsInt();
        }

        private int val;
    }

//  LongField is a Field that contains a xml long value as defined in http://www.w3.org/TR/xmlschema-2/#long
//  The default value for the LongField is 0.

    public class LongField : Field
    {
        public LongField()
            : base("LongField")
        {
            val = 0;
        }

        public LongField(string name)
            : base(name)
        {
            val = 0;
        }

        public LongField(string name, Container container)
            : base(name, container)
        {
            val = 0;
        }

        public long Value
        {
            get
            {
                return val;
            }
            set
            {
                val = value;
                Present = true;
            }
        }

        public override void ReadValue(XmlReader reader)
        {
            val = reader.ReadContentAsLong();
        }

        public override void WriteValue(XmlWriter writer)
        {
            writer.WriteValue(val);
        }

        private long val;
    }

//  DoubleField is a Field that contains an xml double precision floating point value as defined in http://www.w3.org/TR/xmlschema-2/#double
//  The default value for the DoubleField is 0.

    public class DoubleField : Field
    {
        public DoubleField()
            : base("DoubleField")
        {
            val = 0;
        }

        public DoubleField(string name)
            : base(name)
        {
            val = 0;
        }

        public DoubleField(string name, Container container)
            : base(name, container)
        {
            val = 0;
        }

        public double Value
        {
            get
            {
                return val;
            }
            set
            {
                val = value;
                Present = true;
            }
        }

        public override void ReadValue(XmlReader reader)
        {
            val = reader.ReadContentAsDouble();
        }

        public override void WriteValue(XmlWriter writer)
        {
            writer.WriteValue(val);
        }

        private double val;
    }

//  DecimalField is a Field that contains an xml decimal value as defined in http://www.w3.org/TR/xmlschema-2/#decimal
//  The default value for the DecimalField is 0.

    public class DecimalField : Field
    {
        public DecimalField()
            : base("DecimalField")
        {
            val = 0;
        }

        public DecimalField(string name)
            : base(name)
        {
            val = 0;
        }

        public DecimalField(string name, Container container)
            : base(name, container)
        {
            val = 0;
        }

        public decimal Value
        {
            get
            {
                return val;
            }
            set
            {
                val = value;
                Present = true;
            }
        }

        public override void ReadValue(XmlReader reader)
        {
            val = reader.ReadContentAsDecimal();
        }

        public override void WriteValue(XmlWriter writer)
        {
            writer.WriteValue(val);
        }

        private decimal val;
    }

//  BooleanField is a Field that contains an xml boolean value as defined in http://www.w3.org/TR/xmlschema-2/#boolean
//  The default value for the BooleanField is false.

    public class BooleanField : Field
    {
        public BooleanField()
            : base("BooleanField")
        {
            val = false;
        }

        public BooleanField(string name)
            : base(name)
        {
            val = false;
        }

        public BooleanField(string name, Container container)
            : base(name, container)
        {
            val = false;
        }

        public bool Value
        {
            get
            {
                return val;
            }
            set
            {
                val = value;
                Present = true;
            }
        }

        public override void ReadValue(XmlReader reader)
        {
            val = reader.ReadContentAsBoolean();
        }

        public override void WriteValue(XmlWriter writer)
        {
            writer.WriteValue(val);
        }

        private bool val;
    }

//  DateTimeField is a Field that contains an xml dateTime value as defined in http://www.w3.org/TR/xmlschema-2/#dateTime
//  The default value for the DateTimeField is epoc (DateTime whose value is 0).

    public class DateTimeField : Field
    {
        public DateTimeField()
            : base("DateTimeField")
        {
            val = new DateTime(0);
        }

        public DateTimeField(string name)
            : base(name)
        {
            val = new DateTime(0);
        }

        public DateTimeField(string name, Container container)
            : base(name, container)
        {
            val = new DateTime(0);
        }

        public DateTime Value
        {
            get
            {
                return val;
            }
            set
            {
                val = value;
                Present = true;
            }
        }

        public override void ReadValue(XmlReader reader)
        {
            DateTimeOffset dto = reader.ReadContentAsDateTimeOffset();
            val = dto.DateTime;
        }

        public override void WriteValue(XmlWriter writer)
        {
            writer.WriteValue(val);
        }

        private DateTime val;
    }
}
