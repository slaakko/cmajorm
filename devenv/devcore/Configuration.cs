using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace devcore
{
    public class Configuration
    {
        public Configuration(string name)
        {
            this.name = name;
            defines = new HashSet<string>();
        }
        public string Name
        {
            get { return name; }
        }
        public void AddDefine(string define)
        {
            defines.Add(define);
        }
        public bool IsSymbolDefined(string symbol)
        {
            return defines.Contains(symbol);
        }
        public static Configuration GetDebugConfig()
        {
            Configuration config = new Configuration("debug");
            config.AddDefine("DEBUG");
            config.AddDefine("ASSERT");
            return config;
        }
        public static Configuration GetReleaseConfig()
        {
            Configuration config = new Configuration("release");
            config.AddDefine("RELEASE");
            return config;
        }
        public static Configuration GetProfileConfig()
        {
            Configuration config = new Configuration("profile");
            config.AddDefine("PROFILE");
            return config;
        }
        private string name;
        private HashSet<string> defines;
    }
}
