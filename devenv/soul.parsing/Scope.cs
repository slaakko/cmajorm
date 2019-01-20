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

namespace soul.parsing
{
    public interface INamedEntity
    {
        string Name { get; }
        string FullName { get; }
    }

    public class Scope<Entity> where Entity : INamedEntity
    {
        public Scope(string name, string entityTypeName)
        {
            this.name = name;
            this.entityTypeName = entityTypeName;
            shortNames = new Dictionary<string, List<Entity>>();
            fullNames = new Dictionary<string, Entity>();
        }
        public void Add(Entity entity)
        {
            if (fullNames.ContainsKey(entity.FullName))
            {
                throw new Exception(entityTypeName + " '" + entity.FullName + "' already defined in scope " + name);
            }
            shortNames.Add(entity.Name, new List<Entity> { entity });
            fullNames.Add(entity.FullName, entity);
        }
        public bool IsUnique(string shortEntityName)
        {
            List<Entity> references;
            if (shortNames.TryGetValue(shortEntityName, out references))
            {
                return references != null && references.Count == 1;
            }
            return false;
        }
        public bool IsEmpty
        {
            get { return shortNames.Count == 0; }
        }
        public Entity GetFull(string fullEntityName)
        {
            Entity entity;
            if (fullNames.TryGetValue(fullEntityName, out entity))
            {
                return entity;
            }
            return default(Entity);
        }
        public Entity Get(string entityName)
        {
            int lastDotPos = entityName.LastIndexOf('.');
            if (lastDotPos != -1)
            {
                Entity entity;
                if (fullNames.TryGetValue(entityName, out entity))
                {
                    return entity;
                }
            }
            else
            {
                List<Entity> references;
                if (shortNames.TryGetValue(entityName, out references))
                {
                    if (references != null && references.Count > 0)
                    {
                        if (references.Count == 1)
                        {
                            return references[0];
                        }
                        else
                        {
                            StringBuilder list = new StringBuilder();
                            bool first = true;
                            foreach (Entity entity in references)
                            {
                                if (first)
                                {
                                    first = false;
                                }
                                else
                                {
                                    list.Append(", ");
                                }
                                list.Append(entity.FullName);
                            }
                            throw new Exception("Reference to " + entityTypeName.ToLower() + " '" + entityName + "' is ambiguous: " + list + "." + 
                                Environment.NewLine + "Detected in scope '" + name + "'.");
                        }
                    }
                }
            }
            throw new Exception(entityTypeName + " '" + entityName + "' not found in scope " + name);
        }
        public void Merge(Scope<Entity> subScope)
        {
            foreach (KeyValuePair<string, List<Entity>> shortNameEntity in subScope.shortNames)
            {
                List<Entity> list;
                if (shortNames.TryGetValue(shortNameEntity.Key, out list))
                {
                    foreach (Entity entity in shortNameEntity.Value)
                    {
                        if (!list.Contains(entity))
                        {
                            list.Add(entity);
                        }
                    }
                }
                else
                {
                    List<Entity> newList = new List<Entity>();
                    newList.AddRange(shortNameEntity.Value);
                    shortNames.Add(shortNameEntity.Key, newList);
                }
            }
            foreach (Entity entity in subScope.fullNames.Values)
            {
                if (!fullNames.Values.Contains(entity))
                {
                    if (fullNames.ContainsKey(entity.FullName))
                    {
                        throw new Exception(entityTypeName + " '" + entity.FullName + "' already defined in scope " + name);
                    }
                    fullNames.Add(entity.FullName, entity);
                }
            }
        }
        private string name;
        private string entityTypeName;
        private Dictionary<string, List<Entity>> shortNames;
        private Dictionary<string, Entity> fullNames;
    }
}
