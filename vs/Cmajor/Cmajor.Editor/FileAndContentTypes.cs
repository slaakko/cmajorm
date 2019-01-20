using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel.Composition;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.Utilities;

namespace Cmajor.Editor
{
    internal static class FileAndContentTypeDefinitions
    {
        [Export]
        [Name("cmajor")]
        [BaseDefinition("code")]
        internal static ContentTypeDefinition cmajorContentType = null;

        [Export]
        [FileExtension(".cm")]
        [ContentType("cmajor")]
        internal static FileExtensionToContentTypeDefinition cmajorFileExtensionDefinition = null;
    }
}
