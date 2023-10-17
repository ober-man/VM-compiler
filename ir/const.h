#pragma once

namespace compiler
{

enum class InstType
{
    None = 0,
    Binary,
    Unary,
    Const,
    Param,
    Jump,
    Call,
    Cast,
    Mov,
    Phi,
    End = Phi
};

enum class BinOpType
{
    None = 0,

    // Arithmetical
    Add,
    Sub,
    Mul,
    Div,
    Mod,

    // Bitwise
    Shl,
    Shr,

    // Logical
    And,
    Or,
    Xor,

    // Compare
    Cmp,
    End = Cmp
};

enum class UnOpType
{
    None = 0,
    Not,
    Return,
    End = Return
};

enum class JumpOpType
{
    None = 0,
    Jmp,
    Je,
    Jne,
    Jb,
    Jbe,
    Ja,
    Jae,
    End = Jae
};

enum class DataType
{
    None = 0,
    Int32,
    Int64,
    Float32,
    Float64,
    End = Float64
};

} // namespace compiler
