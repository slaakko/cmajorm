using System;
using System.Collections;
using System.Collections.Generic;
using System.Xml;

namespace master.xml
{
//  Array is a serializable container for some serializable item type. 
//  XML-serialization for an array is of the form:
//  <name of the array>
//      <item name>item value</item name>
//      <item name>item value</item name>
//      ...
//  </name of the array>
//  The default value for Array is an empty array.

    public class Array<T> : Serializable, IList<T>
        where T : Serializable, new()
    {
        public Array(string name, string itemName)
            : base(name)
        {
            this.itemName = itemName;
            items = new List<T>();
            itemsContained = true;
        }
        public Array(string name, string itemName, Container container): this(name, itemName)
        {
            container.Add(this);
        }
        public Array(string name, string itemName, Container container, bool itemsContained)
            : this(name, itemName, container)
        {
            this.itemsContained = itemsContained;
        }
        public IEnumerator<T> GetEnumerator()
        {
            return items.GetEnumerator();
        }
        IEnumerator IEnumerable.GetEnumerator()
        {
            return items.GetEnumerator();
        }
        public void Add(T item)
        {
            item.Name = itemName;
            item.Contained = true;
            items.Add(item);
        }
        public void Clear()
        {
            items.Clear();
        }
        public bool Contains(T item)
        {
            return items.Contains(item);
        }
        public void CopyTo(T[] array, int arrayIndex)
        {
            items.CopyTo(array, arrayIndex);
        }
        public bool Remove(T item)
        {
            return items.Remove(item);
        }
        public void RemoveAt(int index)
        {
            items.RemoveAt(index);
        }
        public int IndexOf(T item)
        {
            return items.IndexOf(item);
        }
        public void Insert(int index, T item)
        {
            items.Insert(index, item);
        }
        public int Count
        {
            get { return items.Count; }
        }
        public bool IsReadOnly
        {
            get { return false; }
        }
        public T this[int i]
        {
            get { return items[i]; }
            set { items[i] = value; }
        }
        public List<T> Items
        {
            get { return items; }
        }
        public override void Read(XmlReader reader)
        {
            if (itemsContained)
            {
                bool emptyElement = false;
                if (reader.IsStartElement(Name))
                {
                    emptyElement = reader.IsEmptyElement;
                    reader.ReadStartElement();
                    while (reader.IsStartElement(itemName))
                    {
                        T item = new T();
                        item.Name = itemName;
                        item.Contained = true;
                        item.Read(reader);
                        items.Add(item);
                    }
                    if (!emptyElement)
                    {
                        reader.ReadEndElement();
                    }
                }
            }
            else
            {
                while (reader.IsStartElement(itemName))
                {
                    T item = new T();
                    item.Name = itemName;
                    item.Contained = true;
                    item.Read(reader);
                    items.Add(item);
                }
            }
        }
        public override void Write(XmlWriter writer)
        {
            WriteComments(writer);
            if (itemsContained)
            {
                writer.WriteStartElement(Name);
            }
            foreach (T item in items)
            {
                item.Write(writer);
            }
            if (itemsContained)
            {
                writer.WriteEndElement();
            }
        }
        private string itemName;
        private List<T> items;
        private bool itemsContained;
    }
}
