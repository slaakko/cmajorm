using System;
using System.Xml;
using System.Collections.Generic;

namespace master.xml
{
    public abstract class Serializable
    {
        public Serializable()
        {
            name = "Serializable";
            contained = false;
            comments = new List<string>();
        }

        public Serializable(string name)
        {
            this.name = name;
            contained = false;
            comments = new List<string>();
        }

        public Serializable(string name, Container container)
        {
            this.name = name;
            container.Add(this);
            comments = new List<string>();
        }

        public string Name
        {
            get
            {
                return name;
            }
            set
            {
                name = value;
            }
        }

        public bool Contained
        {
            get
            {
                return contained;
            }
            set
            {
                contained = value;
            }
        }

        public List<string> Comments
        {
            get { return comments; }
        }

        public abstract void Read(XmlReader reader);

        public abstract void Write(XmlWriter writer);

        public void WriteComments(XmlWriter writer)
        {
            foreach (string comment in comments)
            {
                writer.WriteComment(" " + comment + " ");
            }
        }

        private string name;
        private bool contained;
        private List<string> comments;
    }
}
