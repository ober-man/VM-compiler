#pragma once

namespace compiler
{

enum class InstType
{
    NONE = 0,
    BINARY,
    UNARY,
    CONST,
    PARAM,
    JUMP,
    CALL,
    MOV,
    CAST,
    CMP,
    PHI,
    END = PHI
};

enum class BinOpType
{
    NONE = 0,

    // Arithmetical
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,

    // Bitwise
    SHL,
    SHR,

    // Logical
    AND,
    OR,
    XOR,

    // Compare
    CMP,
    END = CMP
};

enum class UnOpType
{
    NONE = 0,
    NOT,
    MOV,
    RETURN,
    END = RETURN
};

enum class JumpOpType
{
    NONE = 0,
    JMP,
    JE,
    JNE,
    JLT,
    JLE,
    JGT,
    JGE,
    END = JGE
};

enum class DataType
{
    NONE = 0,
    INT32,
    INT64,
    FLOAT32,
    FLOAT64,
    END = FLOAT64
};

} // namespace compiler
