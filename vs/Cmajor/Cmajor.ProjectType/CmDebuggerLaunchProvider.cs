using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Threading.Tasks;
using Microsoft.VisualStudio.ProjectSystem;
using Microsoft.VisualStudio.ProjectSystem.Debug;
using Microsoft.VisualStudio.ProjectSystem.Properties;
using Microsoft.VisualStudio.ProjectSystem.VS.Debug;

namespace cmajor
{
    [ExportDebugger(CmDebugger.SchemaName)]
    [AppliesTo(MyUnconfiguredProject.UniqueCapability)]
    public class CmDebuggerLaunchProvider : DebugLaunchProviderBase
    {
        [ImportingConstructor]
        public CmDebuggerLaunchProvider(ConfiguredProject configuredProject)
            : base(configuredProject)
        {
        }

        // TODO: Specify the assembly full name here
        [ExportPropertyXamlRuleDefinition("cmajor, Version=1.0.0.0, Culture=neutral, PublicKeyToken=9be6e469bc4921f1", "XamlRuleToCode:CmDebugger.xaml", "Project")]
        [AppliesTo(MyUnconfiguredProject.UniqueCapability)]
        private object DebuggerXaml { get { throw new NotImplementedException(); } }

        /// <summary>
        /// Gets project properties that the debugger needs to launch.
        /// </summary>
        [Import]
        private ProjectProperties DebuggerProperties { get; set; }

        public override async Task<bool> CanLaunchAsync(DebugLaunchOptions launchOptions)
        {
            var properties = await this.DebuggerProperties.GetCmDebuggerPropertiesAsync();
            string projectFilePath = base.ConfiguredProject.UnconfiguredProject.FullPath;
            bool debug = base.ConfiguredProject.ProjectConfiguration.Name.StartsWith("Debug");
            string config = debug ? "debug" : "release";
            string exePath = Path.Combine(
                Path.Combine(Path.Combine(Path.GetDirectoryName(projectFilePath), "bin"), config), Path.GetFileNameWithoutExtension(projectFilePath) + ".exe");
            return File.Exists(exePath);
            //string commandValue = await properties.CmDebuggerCommand.GetEvaluatedValueAtEndAsync();
            //return !string.IsNullOrEmpty(commandValue);
        }

        public override async Task<IReadOnlyList<IDebugLaunchSettings>> QueryDebugTargetsAsync(DebugLaunchOptions launchOptions)
        {
            var settings = new DebugLaunchSettings(launchOptions);

            // The properties that are available via DebuggerProperties are determined by the property XAML files in your project.
            var debuggerProperties = await this.DebuggerProperties.GetCmDebuggerPropertiesAsync();
            settings.CurrentDirectory = await debuggerProperties.CmDebuggerWorkingDirectory.GetEvaluatedValueAtEndAsync();
            string projectFilePath = base.ConfiguredProject.UnconfiguredProject.FullPath;
            bool debug = base.ConfiguredProject.ProjectConfiguration.Name.StartsWith("Debug");
            string config = debug ? "debug" : "release";
            string exePath = Path.Combine(
                Path.Combine(Path.Combine(Path.GetDirectoryName(projectFilePath), "bin"), config), Path.GetFileNameWithoutExtension(projectFilePath) + ".exe");
            //settings.Executable = await debuggerProperties.CmDebuggerCommand.GetEvaluatedValueAtEndAsync();
            settings.Executable = exePath;
            settings.Arguments = await debuggerProperties.CmDebuggerCommandArguments.GetEvaluatedValueAtEndAsync();
            settings.LaunchOperation = DebugLaunchOperation.CreateProcess;

            // TODO: Specify the right debugger engine
            settings.LaunchDebugEngineGuid = DebuggerEngines.NativeOnlyEngine;

            return new IDebugLaunchSettings[] { settings };
        }
    }
}
