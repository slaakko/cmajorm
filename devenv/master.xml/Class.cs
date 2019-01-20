using System;
using System.Xml;

namespace master.xml
{
//  Class is a serializable container for fields. 
//  Each field can be an Array, a class derived from a Struct or a Class, or some field type derived from Field.
//  The default value for Class is a class that has default value for each field it contains.

    public class Class : Struct
    {
        public Class()
            : base("Class")
        {
        }

        public Class(string name)
            : base(name)
        {
        }

        public override void Read(XmlReader reader)
        {
            if (Contained)
            {
                base.Read(reader);
            }
            else
            {
                string className = Name == "Class" ? GetType().FullName : Name;
                if (reader.IsStartElement(className))
                {
                    if (reader.IsEmptyElement)
                    {
                        reader.ReadStartElement();
                    }
                    else
                    {
                        reader.ReadStartElement();
                        Fields.Read(reader);
                        reader.ReadEndElement();
                    }
                }
                else
                {
                    throw new Exception(className + " expected.");
                }
            }
        }

        public override void Write(XmlWriter writer)
        {
            if (Contained)
            {
                base.Write(writer);
            }
            else
            {
                writer.WriteStartDocument();
                string className = Name == "Class" ? GetType().FullName : Name;
                WriteComments(writer);
                writer.WriteStartElement(className);
                Fields.Write(writer);
                writer.WriteEndElement();
                writer.WriteEndDocument();
            }
        }
    }
}
