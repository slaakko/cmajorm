using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace devcore
{
    public abstract class JsonValue
    {
        public virtual bool IsObject()
        {
            return false;
        }
        public virtual bool IsArray()
        {
            return false;
        }
        public virtual bool IsString()
        {
            return false;
        }
        public virtual bool IsNumber()
        {
            return false;
        }
        public virtual bool IsBool()
        {
            return false;
        }
        public virtual bool IsNull()
        {
            return false;
        }
    }

    public class JsonString : JsonValue
    {
        public override bool IsString()
        {
            return true;
        }
        public JsonString()
        {
            this.value = "";
        }
        public JsonString(string value)
        {
            this.value = value;
        }
        public void Append(char c)
        {
            value = value + new string(c, 1);
        }
        public void Append(string s)
        {
            value = value + s;
        }
        public string Value
        {
            get { return this.value; }
            set { this.value = value; }
        }
        public static string JsonCharStr(char c)
        {
            switch (c)
            {
                case '"': return "\\\"";
                case '\\': return @"\\";
                case '/': return @"\/";
                case '\b': return @"\b";
                case '\f': return @"\f";
                case '\n': return @"\n";
                case '\r': return @"\r";
                case '\t': return @"\t";
                default:
                {
                    if (char.IsControl(c)) return @"\u" + string.Format("{0:X4}", Convert.ToInt32(c));
                    return c.ToString();
                }
            }
        }
        public override string ToString() 
        {
            StringBuilder s = new StringBuilder();
            foreach (char c in value)
            {
                s.Append(JsonCharStr(c));
            }
            return "\"" + s.ToString() + "\"";
        }
        private string value;
    }

    public class JsonNumber : JsonValue
    {
        public JsonNumber()
        {
            this.value = 0;
        }
        public JsonNumber(double value)
        {
            this.value = value;
        }
        public override bool IsNumber()
        {
            return true;
        }
        public override string ToString()
        {
            return value.ToString();
        }
        public double Value
        {
            get { return this.value; }
            set { this.value = value; }
        }
        private double value;
    }

    public class JsonBool : JsonValue
    {
        public JsonBool()
        {
            this.value = false;
        }
        public JsonBool(bool value)
        {
            this.value = value;
        }
        public override bool IsBool() 
        { 
            return true;
        }
        public override string ToString()
        {
            return value ? "true" : "false";
        }
        public bool Value
        {
            get { return this.value; }
            set { this.value = value; }
        }
        private bool value;
    }

    public class JsonNull : JsonValue
    {
        public override bool IsNull()
        {
            return true;
        }
        public override string ToString()
        {
            return "null";
        }
    }

    public class JsonObject : JsonValue
    {
        public JsonObject()
        {
            fields = new Dictionary<string, JsonValue>();
        }
        public override bool IsObject() 
        { 
            return true;
        }
        public void AddField(string name, JsonValue value)
        {
            fields[name] = value;
        }
        public JsonValue GetField(string name)
        {
            if (fields.ContainsKey(name))
            {
                return fields[name];
            }
            return null;
        }
        public override string ToString()
        {
            StringBuilder s = new StringBuilder();
            s.Append("{");
            bool first = true;
            foreach (KeyValuePair<string, JsonValue> p in fields)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    s.Append(", ");
                }
                s.Append(new JsonString(p.Key).ToString()).Append(":").Append(p.Value.ToString());
            }
            s.Append("}");
            return s.ToString();
        }
        private Dictionary<string, JsonValue> fields;
    }
    public class JsonArray : JsonValue
    {
        public JsonArray()
        {
            items = new List<JsonValue>();
        }
        public override bool IsArray()  
        { 
            return true;
        }
        public void AddItem(JsonValue item)
        {
            items.Add(item);
        }
        public JsonValue GetItem(int index)
        {
            return items[index];
        }
        public JsonValue this[int index] 
        { 
            get { return items[index]; }
            set { items[index] = value; }
        }
        public int Count
        { 
            get { return items.Count; }
        }
        public override string ToString()
        {
            StringBuilder s = new StringBuilder();
            s.Append("[");
            bool first = true;
            foreach (JsonValue item in items)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    s.Append(", ");
                }
                s.Append(item.ToString());
            }
            s.Append("]");
            return s.ToString();
        }
        private List<JsonValue> items;
    }
}
