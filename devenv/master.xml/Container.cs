using System;
using System.Collections.Generic;
using System.Xml;

namespace master.xml
{
//  Container is a serializable container for serializable classes. 
//  Container keeps a mapping from the names of the serializables to the serializable objects it contains.
//  The default value for Container is an empty container.

    public class Container : Serializable
    {
        public Container()
            : base("Container")
        {
            serializables = new List<Serializable>();
            table = new Dictionary<string, Serializable>();
        }

//      Add a serializable to the container and add a mapping from the name of the serializable to the serializable itself.

        public void Add(Serializable serializable)
        {
            serializable.Contained = true;
            serializables.Add(serializable);
            table.Add(serializable.Name, serializable);
        }

//      Look up a contained object matching the name of the given reader's current xml element. 
//      If found, read the value from the content of the xml element to the contained object, otherwise skip the current xml element.

        public override void Read(XmlReader reader)
        {
            while (reader.IsStartElement())
            {
                Serializable serializable;
                if (table.TryGetValue(reader.Name, out serializable))
                {
                    serializable.Read(reader);
                }
                else
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
                            reader.ReadContentAsString();
                        }
                        reader.ReadEndElement();
                    }
                }
            }
        }

//      Serialize contained objects to xml.

        public override void Write(XmlWriter writer)
        {
            foreach (Serializable serializable in serializables)
            {
                serializable.Write(writer);
            }
        }

        private List<Serializable> serializables;
        private Dictionary<string, Serializable> table;
    }
}
