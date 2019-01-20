/*========================================================================
    Copyright (c) 2011-2012 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

using System;
using System.Collections.Generic;
using System.Text;

namespace soul.parsing
{
    public abstract class UnaryParser : Parser 
    {
        public UnaryParser(Parser child, string info)
            : base(info)
        {
            this.child = child;
        }
        public Parser Child
        {
            get { return child; }
        }
        private Parser child;
    }

    public class OptionalParser : UnaryParser
    {
        public OptionalParser(Parser child)
            : base(child, child.Info + "?")
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Position save = scanner.Position;
            Match match = Child.Parse(scanner, stack);
            if (match.Hit)
            {
                return match;
            }
            scanner.Position = save;
            return Match.Empty();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            Child.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class PositiveParser : UnaryParser
    {
        public PositiveParser(Parser child)
            : base(child, child.Info)
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Match match = Child.Parse(scanner, stack);
            if (match.Hit)
            {
                for (; ; )
                {
                    Position save = scanner.Position;
                    scanner.Skip();
                    Match next = Child.Parse(scanner, stack);
                    if (next.Hit)
                    {
                        match.Concatenate(next);
                    }
                    else
                    {
                        scanner.Position = save;
                        break;
                    }
                }
            }
            return match;
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            Child.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class KleeneStarParser : UnaryParser
    {
        public KleeneStarParser(Parser child)
            : base(child, child.Info)
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Match match = Match.Empty();
            bool first = true;
            for (; ; )
            {
                Position save = scanner.Position;
                if (first)
                {
                    first = false;
                }
                else
                {
                    scanner.Skip();
                }
                Match next = Child.Parse(scanner, stack);
                if (next.Hit)
                {
                    match.Concatenate(next);
                }
                else
                {
                    scanner.Position = save;
                    return match;
                }
            }
        }
        public override void  Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            Child.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class ExpectationParser : UnaryParser
    {
        public ExpectationParser(Parser child)
            : base(child, child.Info)
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Position position = scanner.Position;
            Match match = Match.Nothing();
            try
            {
                match = Child.Parse(scanner, stack);
            }
            catch (ExpectationFailure ex)
            {
                throw new ExpectationFailure(Child.Info + ex.Info, ex.Content, ex.Position, ex.FileName);
            }
            if (match.Hit)
            {
                return match;
            }
            else
            {
                throw new ExpectationFailure(Child.Info, scanner.Content, position, scanner.FileName);
            }
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            Child.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class TokenParser : UnaryParser
    {
        public TokenParser(Parser child)
            : base(child, child.Info)
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            scanner.BeginToken();
            Match match = Child.Parse(scanner, stack);
            scanner.EndToken();
            return match;
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            Child.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public abstract class BinaryParser : Parser
    {
        public BinaryParser(Parser left, Parser right, string info)
            : base(info)
        {
            this.left = left;
            this.right = right;
        }
        public Parser Left
        {
            get { return left; }
        }
        public Parser Right
        {
            get { return right; }
        }
        private Parser left;
        private Parser right;
    }

    public class SequenceParser : BinaryParser
    {
        public SequenceParser(Parser left, Parser right)
            : base(left, right, left.Info)
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Match left = Left.Parse(scanner, stack);
            if (left.Hit)
            {
                scanner.Skip();
                Match right = Right.Parse(scanner, stack);
                if (right.Hit)
                {
                    left.Concatenate(right);
                    return left;
                }
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            Left.Accept(visitor);
            visitor.Visit(this);
            Right.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class AlternativeParser : BinaryParser
    {
        public AlternativeParser(Parser left, Parser right)
            : base(left, right, left.Info + " | " + right.Info)
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Position save = scanner.Position;
            Match left = Left.Parse(scanner, stack);
            if (left.Hit)
            {
                return left;
            }
            scanner.Position = save;
            return Right.Parse(scanner, stack);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            Left.Accept(visitor);
            visitor.Visit(this);
            Right.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class DifferenceParser : BinaryParser
    {
        public DifferenceParser(Parser left, Parser right)
            : base(left, right, left.Info + " - " + right.Info)
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Position save = scanner.Position;
            Match left = Left.Parse(scanner, stack);
            if (left.Hit)
            {
                Position tmp = scanner.Position;
                scanner.Position = save;
                save = tmp;
                Match right = Right.Parse(scanner, stack);
                if (!right.Hit || right.Length < left.Length)
                {
                    scanner.Position = save;
                    return left;
                }
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            Left.Accept(visitor);
            visitor.Visit(this);
            Right.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class ExclusiveOrParser : BinaryParser
    {
        public ExclusiveOrParser(Parser left, Parser right)
            : base(left, right, left.Info + " ^ " + right.Info)
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Position save = scanner.Position;
            Match left = Left.Parse(scanner, stack);
            Position temp = scanner.Position;
            scanner.Position = save;
            save = temp;
            Match right = Right.Parse(scanner, stack);
            bool match = left.Hit ? !right.Hit : right.Hit;
            if (match)
            {
                if (left.Hit)
                {
                    scanner.Position = save;
                }
                return left.Hit ? left : right;
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            Left.Accept(visitor);
            visitor.Visit(this);
            Right.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class IntersectionParser : BinaryParser
    {
        public IntersectionParser(Parser left, Parser right)
            : base(left, right, left.Info + " & " + right.Info)
        {
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            Position save = scanner.Position;
            Match left = Left.Parse(scanner, stack);
            if (left.Hit)
            {
                scanner.Position = save;
                Match right = Right.Parse(scanner, stack);
                if (left.Length == right.Length)
                {
                    return left;
                }
            }
            return Match.Nothing();
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            Left.Accept(visitor);
            visitor.Visit(this);
            Right.Accept(visitor);
            visitor.EndVisit(this);
        }
    }

    public class ListParser : UnaryParser
    {
        public ListParser(Parser left, Parser right)
            : base(new SequenceParser(left, new KleeneStarParser(new SequenceParser(right, left))), left.Info + " % " + right.Info)
        {
            this.left = left;
            this.right = right;
        }
        public override Match Parse(Scanner scanner, Stack<object> stack)
        {
            return Child.Parse(scanner, stack);
        }
        public override void Accept(Visitor visitor)
        {
            visitor.BeginVisit(this);
            left.Accept(visitor);
            visitor.Visit(this);
            right.Accept(visitor);
            visitor.EndVisit(this);
        }
        public Parser Left
        {
            get { return left; }
        }
        public Parser Right
        {
            get { return right; }
        }
        private Parser left;
        private Parser right;
    }
}
