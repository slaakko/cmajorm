using System;
using System.Collections.Generic;
using master.util.pattern;

namespace master.xml
{
//  Registry singleton class is used to register and create serializable classes.

    public class Registry: Singleton<Registry>
    {
        public Registry()
        {
            table = new Dictionary<string, Type>();
        }

//      Registers a type using the types full name. Type is typically a class that is derived from Class class.

        public void Register(Type type)
        {
            table.Add(type.FullName, type);
        }

//      Registers a type using the given name. Type is typically a class that is derived from Class class.

        public void Register(Type type, string name)
        {
            table.Add(name, type);
        }

//      Looks up the type with a given name. If it is found creates and returns a new instance of it. Otherwise throws Exception.
        
        public Class Create(string name)
        {
            Type type;
            if (table.TryGetValue(name, out type))
            {
                Class c = (Class)Activator.CreateInstance(type);
                if (name != c.GetType().FullName)
                {
                    c.Name = name;
                }
                return c;
            }
            throw new Exception("Class " + name + " not registered.");
        }

        Dictionary<string, Type> table;
    }
}
