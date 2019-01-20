using System;
using System.Xml;

namespace master.xml
{
//  Reader class. 

    public static class Reader
    {
//      Creates, reads and returns registered serializable class from the specified XmlReader using following steps:
//      (1) Possible xml declaration and comments are skipped until XmlReader's node type is element.
//      (2) If XmlReader is not at the end of the xml file Reader creates a registered serializable class 
//          whose name matches the XmlReader's current element name using the Registry.
//      (3) Reader reader data to the created serializable class from the xml elements content.
//      (4) Reader returns the serializable class.
//
//      Exception is thrown if
//      (a) xml file does not contain any valid xml element or 
//      (b) the name of the xml element does not match the name of any registered serializable class.

        public static Class Read(XmlReader reader)
        {
            reader.Read();
            while (!reader.EOF && reader.NodeType != XmlNodeType.Element)
            {
                reader.Read();
            }
            if (!reader.EOF)
            {
                Class serializableClass = Registry.Instance.Create(reader.Name);
                serializableClass.Read(reader);
                return serializableClass;
            }
            throw new Exception("File does not contain valid class.");
        }
    }
}
