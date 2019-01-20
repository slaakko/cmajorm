using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using master.util.pattern;
using soul.parsing;

namespace parser
{
    public class ParserRepository : Singleton<ParserRepository>
    {
        public ParserRepository()
        {
            ParsingDomain parsingDomain = new ParsingDomain();
            stdlibGrammar = new stdlib(parsingDomain);
            jsonParser = new JsonGrammar(parsingDomain);
            sourceTokenParser = new SourceToken(parsingDomain);
            projectFileParser = new ProjectFile(parsingDomain);
            solutionFileParser = new SolutionFile(parsingDomain);
            sourceRtfParser = new SourceRtf(parsingDomain);
            sourceLineParser = new SourceLine(parsingDomain);
        }
        public JsonGrammar JsonParser
        {
            get { return jsonParser; }
        }
        public ProjectFile ProjectFileParser
        {
            get { return projectFileParser; }
        }
        public SolutionFile SolutionFileParser
        {
            get { return solutionFileParser; }
        }
        public SourceRtf SourceRtfParser
        {
            get { return sourceRtfParser; }
        }
        public SourceLine SourceLineParser
        {
            get { return sourceLineParser; }
        }
        private stdlib stdlibGrammar;
        private JsonGrammar jsonParser;
        private SourceToken sourceTokenParser;
        private ProjectFile projectFileParser;
        private SolutionFile solutionFileParser;
        private SourceRtf sourceRtfParser;
        private SourceLine sourceLineParser;
    }
}
