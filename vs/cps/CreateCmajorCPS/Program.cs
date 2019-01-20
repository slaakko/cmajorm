using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CreateCmajorCPS
{
    class Program
    {
        static void Main(string[] args)
        {
            string zipPath = Path.GetFullPath(@"CmajorCPS.zip");
            string appData = Environment.GetEnvironmentVariable("LOCALAPPDATA");
            string cpsDir = Path.Combine(appData, "CustomProjectSystems");
            try
            {
                Console.WriteLine("Creating Cmajor custom project system to '" + cpsDir + "'");
                System.IO.Compression.ZipFile.ExtractToDirectory(zipPath, cpsDir);
                Console.WriteLine("Cmajor custom project system created.");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error extracting '" + zipPath + "' to '" + cpsDir + "': " + ex.Message);
            }
        }
    }
}
