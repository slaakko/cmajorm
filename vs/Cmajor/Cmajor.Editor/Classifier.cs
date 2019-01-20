using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel.Composition;
using Microsoft.VisualStudio.Text.Tagging;
using Microsoft.VisualStudio.Text;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.Utilities;

namespace Cmajor.Editor
{
    [Export(typeof(ITaggerProvider))]
    [ContentType("cmajor")]
    [TagType(typeof(ClassificationTag))]
    internal sealed class CmajorClassifierProvider : ITaggerProvider
    {
        [Import]
        internal IBufferTagAggregatorFactoryService aggregatorFactory = null;

        [Import]
        internal IClassificationTypeRegistryService classificationTypeRegistry = null;

        public ITagger<T> CreateTagger<T>(ITextBuffer buffer) where T : ITag
        {
            ITagAggregator<CmajorTokenTag> cmajorTagAggregator = aggregatorFactory.CreateTagAggregator<CmajorTokenTag>(buffer);
            return new CmajorClassifier(buffer, cmajorTagAggregator, classificationTypeRegistry) as ITagger<T>;
        }
    }

    internal sealed class CmajorClassifier : ITagger<ClassificationTag>
    { 
        internal CmajorClassifier(ITextBuffer buffer, ITagAggregator<CmajorTokenTag> aggregator, IClassificationTypeRegistryService typeService)
        {
            this.buffer = buffer;
            this.aggregator = aggregator;
            this.dictionary = new Dictionary<CmajorTokenType, IClassificationType>();
            IClassificationType keywordClassificationType = typeService.GetClassificationType("keyword");
            if (keywordClassificationType == null)
            {
                throw new Exception("keyword classification type is null");
            }
            dictionary[CmajorTokenType.keyword] = keywordClassificationType;
            IClassificationType identifierClassificationType = typeService.GetClassificationType("identifier");
            if (identifierClassificationType == null)
            {
                throw new Exception("identifier classification type is null");
            }
            dictionary[CmajorTokenType.identifier] = identifierClassificationType;
            IClassificationType numberClassificationType = typeService.GetClassificationType("number");
            if (numberClassificationType == null)
            {
                throw new Exception("number classification type is null"); 
            }
            dictionary[CmajorTokenType.number] = numberClassificationType;
            IClassificationType stringClassificationType = typeService.GetClassificationType("string"); 
            if (stringClassificationType == null)
            {
                throw new Exception("string classification type is null");
            }
            dictionary[CmajorTokenType.charLiteral] = stringClassificationType;
            dictionary[CmajorTokenType.stringLiteral] = stringClassificationType;
            IClassificationType commentClassificationType = typeService.GetClassificationType("comment");
            if (commentClassificationType == null)
            {
                throw new Exception("comment classification type is null");
            }
            dictionary[CmajorTokenType.comment] = commentClassificationType;
            IClassificationType preprocessorKeywordClassificationType = typeService.GetClassificationType("preprocessor keyword");
            if (preprocessorKeywordClassificationType == null)
            {
                throw new Exception("preprocessor keyword classification type is null");
            }
            dictionary[CmajorTokenType.preprocessingDirective] = preprocessorKeywordClassificationType;
            IClassificationType whitespaceClassificationType = typeService.GetClassificationType("whitespace");
            if (whitespaceClassificationType == null)
            {
                throw new Exception("whitespace classification type is null");
            }
            dictionary[CmajorTokenType.whitespace] = whitespaceClassificationType;
        }
        public event EventHandler<SnapshotSpanEventArgs> TagsChanged
        {
            add { }
            remove { }
        }
        public IEnumerable<ITagSpan<ClassificationTag>> GetTags(NormalizedSnapshotSpanCollection spans)
        {
            foreach (var tagSpan in aggregator.GetTags(spans))
            {
                var tagSpans = tagSpan.Span.GetSpans(spans[0].Snapshot);
                yield return new TagSpan<ClassificationTag>(tagSpans[0], new ClassificationTag(dictionary[tagSpan.Tag.tokenType]));
            }
        }
        private ITextBuffer buffer;
        private ITagAggregator<CmajorTokenTag> aggregator;
        private IDictionary<CmajorTokenType, IClassificationType> dictionary;
    }

    public enum CmajorTokenType
    {
        keyword, identifier, number, charLiteral, stringLiteral, comment, preprocessingDirective, whitespace
    }

    public class CmajorTokenTag : ITag
    {
        public CmajorTokenTag(CmajorTokenType tokenType)
        {
            this.tokenType = tokenType;
        }
        public CmajorTokenType tokenType;
    }

    [Export(typeof(ITaggerProvider))]
    [ContentType("cmajor")]
    [TagType(typeof(CmajorTokenTag))]
    internal sealed class CmajorTokenTagProvider : ITaggerProvider
    {
        public ITagger<T> CreateTagger<T>(ITextBuffer buffer) where T : ITag
        {
            return new CmajorTokenTagger() as ITagger<T>;
        }
    }

    internal static class CmajorKeywords
    {
        static CmajorKeywords()
        {
            keywords = new HashSet<string>
            {
                "abstract", "and", "as", "axiom", "base", "bool", "break", "byte",
                "case", "cast", "catch", "cdecl", "char", "class", "concept", "const",
                "constexpr", "construct", "continue", "default", "delegate", "delete", "destroy", "do",
                "double", "else", "enum", "explicit", "extern", "false", "float", "for",
                "goto", "if", "inline", "int", "interface", "internal", "is", "long",
                "namespace", "new", "not", "nothrow", "null", "operator", "or", "override",
                "private", "protected", "public", "return", "sbyte", "short", "sizeof", "static",
                "suppress", "switch", "this", "throw", "true", "try", "typedef", "typename",
                "uchar", "uint", "ulong", "unit_test", "ushort", "using", "virtual", "void",
                "wchar", "where", "while"
            };
        }
        internal static bool Contains(string idText)
        {
            return keywords.Contains(idText);
        }
        private static HashSet<string> keywords;
    }

    internal sealed class CmajorTokenTagger : ITagger<CmajorTokenTag>
    {
        public CmajorTokenTagger()
        {
        }
        public event EventHandler<SnapshotSpanEventArgs> TagsChanged
        {
            add { }
            remove { }
        }
        public IEnumerable<ITagSpan<CmajorTokenTag>> GetTags(NormalizedSnapshotSpanCollection spans)
        {
            foreach (SnapshotSpan span in spans)
            {
                ITextSnapshotLine containingLine = span.Start.GetContainingLine();
                string line = containingLine.GetText();
                int pos = containingLine.Start.Position;
                int length = line.Length;
                int startPos = pos;
                int state = 0;
                Stack<int> stateStack = new Stack<int>();
                for (int index = 0; index < length; ++index)
                {
                    char c = line[index];
                    switch (state)
                    {
                        case 0:
                        {
                            startPos = pos;
                            if (char.IsLetter(c) || c == '_')
                            {
                                state = 1;
                            }
                            else if (char.IsDigit(c) || c == '+' || c == '-' || c == '.')
                            {
                                state = 2;
                            }
                            else if (c == '\'')
                            {
                                state = 3;
                            }
                            else if (c == '@')
                            {
                                state = 5;
                            }
                            else if (c == '"')
                            {
                                state = 7;
                            }
                            else if (c == '/')
                            {
                                state = 8;
                            }
                            else if (char.IsWhiteSpace(c))
                            {
                                state = 12;
                            }
                            else if (c == '#')
                            {
                                state = 13;
                            }
                            break;
                        }
                        case 1:
                        {
                            if (!char.IsLetter(c) && !char.IsNumber(c) && c != '_')
                            {
                                TagSpan<CmajorTokenTag> tagSpan = MakeIdentifierOrKeyword(startPos, pos, span);
                                if (tagSpan != null)
                                {
                                    yield return tagSpan;
                                }
                                GotoStartState(ref state, ref pos, ref index);
                            }
                            break;
                        }
                        case 2:
                        {
                            if (!char.IsDigit(c) && c != '+' && c != '-' && c != '.' && c != 'e' && c != 'E')
                            {
                                TagSpan<CmajorTokenTag> tagSpan = MakeNumber(startPos, pos, span);
                                if (tagSpan != null)
                                {
                                    yield return tagSpan;
                                }
                                GotoStartState(ref state, ref pos, ref index);
                            }
                            break;
                        }
                        case 3:
                        {
                            if (c == '\\')
                            {
                                stateStack.Push(3);
                                state = 4;
                            }
                            else if (c == '\'')
                            {
                                TagSpan<CmajorTokenTag> tagSpan = MakeCharLiteral(startPos, pos, span);
                                if (tagSpan != null)
                                {
                                    yield return tagSpan;
                                }
                                state = 0;
                            }
                            break;
                        }
                        case 4:
                        {
                            state = stateStack.Pop();
                            break;
                        }
                        case 5:
                        {
                            if (c == '"')
                            {
                                state = 6;
                            }
                            else if (!char.IsWhiteSpace(c))
                            {
                                state = 0;
                            }
                            break;
                        }
                        case 6:
                        {
                            if (c == '"')
                            {
                                TagSpan<CmajorTokenTag> tagSpan = MakeStringLiteral(startPos, pos, span);
                                if (tagSpan != null)
                                {
                                    yield return tagSpan;
                                }
                                state = 0;
                            }
                            break;
                        }
                        case 7:
                        {
                            if (c == '\\')
                            {
                                stateStack.Push(7);
                                state = 4;
                            }
                            else if (c == '"')
                            {
                                TagSpan<CmajorTokenTag> tagSpan = MakeStringLiteral(startPos, pos, span);
                                if (tagSpan != null)
                                {
                                    yield return tagSpan;
                                }
                                state = 0;
                            }
                            break;
                        }
                        case 8:
                        {
                            if (c == '/')
                            {
                                state = 9;
                            }
                            else if (c == '*')
                            {
                                state = 10;
                            }
                            else
                            {
                                state = 0;
                            }
                            break;
                        }
                        case 9:
                        {
                            break;
                        }
                        case 10:
                        {
                            if (c == '*')
                            {
                                state = 11;
                            }
                            break;
                        }
                        case 11:
                        {
                            if (c == '/')
                            {
                                TagSpan<CmajorTokenTag> tagSpan = MakeComment(startPos, pos + 1, span);
                                if (tagSpan != null)
                                {
                                    yield return tagSpan;
                                }
                                state = 0;
                            }
                            else if (c != '*')
                            {
                                state = 10;
                            }
                            break;
                        }
                        case 12:
                        {
                            if (!char.IsWhiteSpace(c))
                            {
                                TagSpan<CmajorTokenTag> tagSpan = MakeWhiteSpace(startPos, pos, span);
                                if (tagSpan != null)
                                {
                                    yield return tagSpan;
                                }
                                GotoStartState(ref state, ref pos, ref index);
                            }
                            break;
                        }
                        case 13:
                        {
                            if (!char.IsWhiteSpace(c))
                            {
                                state = 14;
                            }
                            break;
                        }
                        case 14:
                        {
                            if (!char.IsLetter(c) && !char.IsNumber(c) && c != '_')
                            {
                                TagSpan<CmajorTokenTag> tagSpan = MakePreprocessingDirective(startPos, pos, span);
                                if (tagSpan != null)
                                {
                                    yield return tagSpan;
                                }
                                GotoStartState(ref state, ref pos, ref index);
                            }
                            break;
                        }
                    }
                    ++pos;
                }
                switch (state)
                {
                    case 1:
                    {
                        TagSpan<CmajorTokenTag> tagSpan = MakeIdentifierOrKeyword(startPos, pos, span);
                        if (tagSpan != null)
                        {
                            yield return tagSpan;
                        }
                        break;
                    }
                    case 2:
                    {
                        TagSpan<CmajorTokenTag> tagSpan = MakeNumber(startPos, pos, span);
                        if (tagSpan != null)
                        {
                            yield return tagSpan;
                        }
                        break;
                    }
                    case 3:
                    {
                        TagSpan<CmajorTokenTag> tagSpan = MakeCharLiteral(startPos, pos, span);
                        if (tagSpan != null)
                        {
                            yield return tagSpan;
                        }
                        break;
                    }
                    case 6:
                    {
                        TagSpan<CmajorTokenTag> tagSpan = MakeStringLiteral(startPos, pos, span);
                        if (tagSpan != null)
                        {
                            yield return tagSpan;
                        }
                        break;
                    }
                    case 9:
                    case 10:
                    case 11:
                    {
                        TagSpan<CmajorTokenTag> tagSpan = MakeComment(startPos, pos, span);
                        if (tagSpan != null)
                        {
                            yield return tagSpan;
                        }
                        break;
                    }
                    case 12:
                    {
                        TagSpan<CmajorTokenTag> tagSpan = MakeWhiteSpace(startPos, pos, span);
                        if (tagSpan != null)
                        {
                            yield return tagSpan;
                        }
                        break;
                    }
                    case 13:
                    case 14:
                    {
                        TagSpan<CmajorTokenTag> tagSpan = MakePreprocessingDirective(startPos, pos, span);
                        if (tagSpan != null)
                        {
                            yield return tagSpan;
                        }
                        break;
                    }
                }
            }
        }
        private void GotoStartState(ref int state, ref int pos, ref int index)
        {
            state = 0;
            --pos;
            --index;
        }
        private TagSpan<CmajorTokenTag> MakeIdentifierOrKeyword(int startPos, int pos, SnapshotSpan span)
        {
            Span spn = new Span(startPos, pos - startPos);
            SnapshotSpan snapshotSpan = new SnapshotSpan(span.Snapshot, spn);
            if (snapshotSpan.IntersectsWith(span))
            {
                string idText = span.Snapshot.GetText(spn);
                bool isKeyword = CmajorKeywords.Contains(idText);
                if (isKeyword)
                {
                    return new TagSpan<CmajorTokenTag>(snapshotSpan, new CmajorTokenTag(CmajorTokenType.keyword));
                }
                else
                {
                    return new TagSpan<CmajorTokenTag>(snapshotSpan, new CmajorTokenTag(CmajorTokenType.identifier));
                }
            }
            else
            {
                return null;
            }
        }
        private TagSpan<CmajorTokenTag> MakeNumber(int startPos, int pos, SnapshotSpan span)
        {
            Span spn = new Span(startPos, pos - startPos);
            SnapshotSpan snapshotSpan = new SnapshotSpan(span.Snapshot, spn);
            if (snapshotSpan.IntersectsWith(span))
            {
                return new TagSpan<CmajorTokenTag>(snapshotSpan, new CmajorTokenTag(CmajorTokenType.number));
            }
            else
            {
                return null;
            }
        }
        private TagSpan<CmajorTokenTag> MakeCharLiteral(int startPos, int pos, SnapshotSpan span)
        {
            Span spn = new Span(startPos, pos - startPos + 1);
            SnapshotSpan snapshotSpan = new SnapshotSpan(span.Snapshot, spn);
            if (snapshotSpan.IntersectsWith(span))
            {
                return new TagSpan<CmajorTokenTag>(snapshotSpan, new CmajorTokenTag(CmajorTokenType.charLiteral));
            }
            else
            {
                return null;
            }
        }
        private TagSpan<CmajorTokenTag> MakeStringLiteral(int startPos, int pos, SnapshotSpan span)
        {
            Span spn = new Span(startPos, pos - startPos + 1);
            SnapshotSpan snapshotSpan = new SnapshotSpan(span.Snapshot, spn);
            if (snapshotSpan.IntersectsWith(span))
            {
                return new TagSpan<CmajorTokenTag>(snapshotSpan, new CmajorTokenTag(CmajorTokenType.stringLiteral));
            }
            else
            {
                return null;
            }
        }
        private TagSpan<CmajorTokenTag> MakeComment(int startPos, int pos, SnapshotSpan span)
        {
            Span spn = new Span(startPos, pos - startPos);
            SnapshotSpan snapshotSpan = new SnapshotSpan(span.Snapshot, spn);
            if (snapshotSpan.IntersectsWith(span))
            {
                return new TagSpan<CmajorTokenTag>(snapshotSpan, new CmajorTokenTag(CmajorTokenType.comment));
            }
            else
            {
                return null;
            }
        }
        private TagSpan<CmajorTokenTag> MakeWhiteSpace(int startPos, int pos, SnapshotSpan span)
        {
            Span spn = new Span(startPos, pos - startPos);
            SnapshotSpan snapshotSpan = new SnapshotSpan(span.Snapshot, spn);
            if (snapshotSpan.IntersectsWith(span))
            {
                return new TagSpan<CmajorTokenTag>(snapshotSpan, new CmajorTokenTag(CmajorTokenType.whitespace));
            }
            else
            {
                return null;
            }
        }
        private TagSpan<CmajorTokenTag> MakePreprocessingDirective(int startPos, int pos, SnapshotSpan span)
        {
            Span spn = new Span(startPos, pos - startPos);
            SnapshotSpan snapshotSpan = new SnapshotSpan(span.Snapshot, spn);
            if (snapshotSpan.IntersectsWith(span))
            {
                return new TagSpan<CmajorTokenTag>(snapshotSpan, new CmajorTokenTag(CmajorTokenType.preprocessingDirective));
            }
            else
            {
                return null;
            }
        }
    }
}
