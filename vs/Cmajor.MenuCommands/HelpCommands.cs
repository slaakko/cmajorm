using System;
using System.ComponentModel.Design;
using System.Globalization;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using Task = System.Threading.Tasks.Task;

namespace Cmajor.MenuCommands
{
    /// <summary>
    /// Command handler
    /// </summary>
    internal sealed class HelpCommands
    {
        /// <summary>
        /// Command ID.
        /// </summary>
        //public const int CommandId = 0x0100;
        public const int localCmajorDocumentationCommandId = 0x0100;
        public const int cmajorHomepageCommandId = 0x0101;

        /// <summary>
        /// Command menu group (command set GUID).
        /// </summary>
        public static readonly Guid CommandSet = new Guid("3bc1af2f-e359-446b-841e-efa07a115651");

        /// <summary>
        /// VS Package that provides this command, not null.
        /// </summary>
        private readonly AsyncPackage package;

        OleMenuCommand localCmajorDocumentationCommand;
        OleMenuCommand cmajorHomepageCommand;
        /// <summary>
        /// Initializes a new instance of the <see cref="HelpCommands"/> class.
        /// Adds our command handlers for menu (commands must exist in the command table file)
        /// </summary>
        /// <param name="package">Owner package, not null.</param>
        /// <param name="commandService">Command service to add command to, not null.</param>
        private HelpCommands(AsyncPackage package, OleMenuCommandService commandService)
        {
            this.package = package ?? throw new ArgumentNullException(nameof(package));
            commandService = commandService ?? throw new ArgumentNullException(nameof(commandService));
            localCmajorDocumentationCommand = new OleMenuCommand(ExecuteLocalCmajorDocumentationCommand, new CommandID(HelpCommands.CommandSet, localCmajorDocumentationCommandId));
            commandService.AddCommand(localCmajorDocumentationCommand);
            cmajorHomepageCommand = new OleMenuCommand(ExecuteCmajorHomepageCommand, new CommandID(HelpCommands.CommandSet, cmajorHomepageCommandId));
            commandService.AddCommand(cmajorHomepageCommand);
        }

        /// <summary>
        /// Gets the instance of the command.
        /// </summary>
        public static HelpCommands Instance
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the service provider from the owner package.
        /// </summary>
        private Microsoft.VisualStudio.Shell.IAsyncServiceProvider ServiceProvider
        {
            get
            {
                return this.package;
            }
        }

        /// <summary>
        /// Initializes the singleton instance of the command.
        /// </summary>
        /// <param name="package">Owner package, not null.</param>
        public static async Task InitializeAsync(AsyncPackage package)
        {
            // Switch to the main thread - the call to AddCommand in HelpCommands's constructor requires
            // the UI thread.
            await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync(package.DisposalToken);

            OleMenuCommandService commandService = await package.GetServiceAsync((typeof(IMenuCommandService))) as OleMenuCommandService;
            Instance = new HelpCommands(package, commandService);
        }
        private void ExecuteLocalCmajorDocumentationCommand(object sender, EventArgs e)
        {
            try
            {
                string cmajorRoot = Environment.GetEnvironmentVariable("CMAJOR_ROOT");
                if (string.IsNullOrEmpty(cmajorRoot))
                {
                    throw new Exception("CMAJOR_ROOT environment variable not set, please set it to contain /path/to/cmajor directory");
                }
                System.Diagnostics.Process.Start(System.IO.Path.Combine(cmajorRoot, System.IO.Path.Combine("doc", "index.html")));
            }
            catch (Exception ex)
            {
                string message = ex.Message;
                string title = "Cmajor Help Commands";
                VsShellUtilities.ShowMessageBox(
                    this.package,
                    message,
                    title,
                    OLEMSGICON.OLEMSGICON_INFO,
                    OLEMSGBUTTON.OLEMSGBUTTON_OK,
                    OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
            }
        }
        private void ExecuteCmajorHomepageCommand(object sender, EventArgs e)
        {
            try
            {
                System.Diagnostics.Process.Start("http://slaakko.github.io/cmajor/");
            }
            catch (Exception ex)
            {
                string message = ex.Message;
                string title = "Cmajor Help Commands";
                VsShellUtilities.ShowMessageBox(
                    this.package,
                    message,
                    title,
                    OLEMSGICON.OLEMSGICON_INFO,
                    OLEMSGBUTTON.OLEMSGBUTTON_OK,
                    OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
            }
        }
    }
}
