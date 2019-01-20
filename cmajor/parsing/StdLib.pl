namespace cmajor.parsing
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
        int: int32_t;
        uint: uint32_t;
        long: int64_t;
        ulong: uint64_t;
        hexuint: uint32_t;
        hex: uint64_t;
        hex_literal: uint64_t;
        real: double;
        ureal: double;
        fractional_real;
        exponent_real;
        exponent_part;
        num: double;
        bool: bool;
        identifier: std::u32string;
        qualified_id: std::u32string;
        escape: char32_t;
        char: char32_t;
        string: std::u32string;
    }
}
