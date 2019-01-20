using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace devcore
{
    public class SourcePos
    {
        public SourcePos(string file, int line, LineCol start, LineCol end)
        {
            this.file = file;
            this.line = line;
            this.start = start;
            this.end = end;
        }
        public string File
        {
            get { return file; }
        }
        public int Line
        {
            get { return line; }
        }
        public LineCol Start
        {
            get { return start; }
        }
        public LineCol End
        {
            get { return end; }
        }
        private string file;
        private int line;
        private LineCol start;
        private LineCol end;
    }

    public struct LineCol
    {
        public LineCol(int line, int col)
        {
            this.line = line;
            this.col = col;
        }
        public int Line
        {
            get { return line; }
        }
        public int Col
        {
            get { return col; }
        }
        private int line;
        private int col;
    }
}
