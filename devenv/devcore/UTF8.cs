using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace devcore
{
    public static class UTF8
    {
        public static byte[] Encode(string s)
        {
            return Encoding.UTF8.GetBytes(s.ToArray());
        }
        public static string Decode(byte[] bytes)
        {
            char[] chars = Encoding.UTF8.GetChars(bytes);
            StringBuilder s = new StringBuilder();
            foreach (char c in chars)
            {
                s.Append(c);
            }
            return s.ToString();
        }
    }
}
