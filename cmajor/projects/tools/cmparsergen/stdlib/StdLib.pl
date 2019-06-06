namespace System.Text.Parsing
{
    grammar stdlib
    {
        spaces;
        newline;
        comment;
        line_comment;
        block_comment;
        spaces_and_comments;
        digit_sequence;
        sign;
        sbyte : sbyte;
        byte : byte;
        short : short;
        ushort : ushort;
        int : int;
        uint : uint;
        long : long;
        ulong : ulong;
        hexbyte : byte;
        hexword : ushort;
        hexdword : uint;
        hexqword : ulong;
        hex : ulong;
        hex_literal : ulong;
        octal_digit : int;
        float : float;
        double : double;
        ureal : double;
        fractional_real;
        exponent_part;
        num : double;
        bool : bool;
        identifier : ustring;
        qualified_id : ustring;
        hex_escape : uchar;
        octal_escape : uchar;
        decimal_escape : uchar;
        c_escape : uchar;
        escape : uchar;
        char : uchar;
        string : ustring;
    }
}
