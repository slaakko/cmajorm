using System;
using System.Xml;

namespace master.xml
{
//  Struct is serializable container for fields that is itself contained as a field of a Class or a Struct or is an item of an Array.
//  Each contained field of a Struct can be an Array, a class derived from a Struct or a Class, or some field type derived from Field.
//  The default value for Struct is a class that has default value for each field it contains.

    public class Struct : Serializable
    {
        public Struct()
            : base("Struct")
        {
        }

        public Struct(string name)
            : base(name)
        {
            fields = new Container();
        }

        public Struct(string name, Container container)
            : this(name)
        {
            container.Add(this);
        }

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
                    fields.Read(reader);
                    reader.ReadEndElement();
                }
            }
        }

        public override void Write(XmlWriter writer)
        {
            WriteComments(writer);
            writer.WriteStartElement(Name);
            fields.Write(writer);
            writer.WriteEndElement();
        }

        public Container Fields
        {
            get
            {
                return fields;
            }
        }

        private Container fields;
    }
}
