#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <string>

namespace compiler
{
// clang-format off

constexpr size_t INVALID_REG = 1000;

//////////////////////////////////////__InstType__///////////////////////////////////////////////

#define INST_TYPE_LIST(ACTION)                                                                     \
    ACTION(Binary, BinaryInst)                                                                     \
    ACTION(Unary, UnaryInst)                                                                       \
    ACTION(Const, ConstInst)                                                                       \
    ACTION(Param, ParamInst)                                                                       \
    ACTION(Jump, JumpInst)                                                                         \
    ACTION(Call, CallInst)                                                                         \
    ACTION(Cast, CastInst)                                                                         \
    ACTION(Mov, MovInst)                                                                           \
    ACTION(Phi, PhiInst)

enum class InstType
{
    NoneInst = 0,

#define CREATE_INST_TYPE(NAME, BASE) NAME,

    INST_TYPE_LIST(CREATE_INST_TYPE)
#undef CREATE_INST_TYPE

    End
};

//////////////////////////////////////__BinOpType__//////////////////////////////////////////////

#define BINARY_OP_LIST(ACTION)                                                                     \
    ACTION(Add, BinaryInst)                                                                        \
    ACTION(Sub, BinaryInst)                                                                        \
    ACTION(Mul, BinaryInst)                                                                        \
    ACTION(Div, BinaryInst)                                                                        \
    ACTION(Mod, BinaryInst)                                                                        \
    ACTION(Shl, BinaryInst)                                                                        \
    ACTION(Shr, BinaryInst)                                                                        \
    ACTION(And, BinaryInst)                                                                        \
    ACTION(Or, BinaryInst)                                                                         \
    ACTION(Xor, BinaryInst)                                                                        \
    ACTION(Cmp, BinaryInst)

enum class BinOpType
{
    NoneBinOp = static_cast<uint8_t>(InstType::End),

#define CREATE_BINOP_TYPE(NAME, BASE) NAME,

    BINARY_OP_LIST(CREATE_BINOP_TYPE)
#undef CREATE_BINOP_TYPE

    End
};

//////////////////////////////////////__UnOpType__///////////////////////////////////////////////

#define UNARY_OP_LIST(ACTION)                                                                      \
    ACTION(Not, UnaryInst)                                                                         \
    ACTION(Return, UnaryInst)

enum class UnOpType
{
    NoneUnOp = static_cast<uint8_t>(BinOpType::End),

#define CREATE_UNOP_TYPE(NAME, BASE) NAME,

    UNARY_OP_LIST(CREATE_UNOP_TYPE)
#undef CREATE_UNOP_TYPE

    End
};

//////////////////////////////////////__JumpOpType__////////////////////////////////////////////

#define JUMP_OP_LIST(ACTION)                                                                       \
    ACTION(Jmp, JumpInst)                                                                          \
    ACTION(Je, JumpInst)                                                                           \
    ACTION(Jne, JumpInst)                                                                          \
    ACTION(Jb, JumpInst)                                                                           \
    ACTION(Jbe, JumpInst)                                                                          \
    ACTION(Ja, JumpInst)                                                                           \
    ACTION(Jae, JumpInst)

enum class JumpOpType
{
    NoneJumpOp = static_cast<uint8_t>(UnOpType::End),

#define CREATE_JUMP_OP_TYPE(NAME, BASE) NAME,

    JUMP_OP_LIST(CREATE_JUMP_OP_TYPE)
#undef CREATE_JUMP_OP_TYPE

    End
};

//////////////////////////////////////__Name_arrays__///////////////////////////////////////////

constexpr std::array<const char *const, static_cast<uint8_t>(JumpOpType::End)> OPER_NAME{

#define CREATE_INST_OPER_NAME(NAME, BASE) \
    #NAME,

    "NoneInst", 
    INST_TYPE_LIST(CREATE_INST_OPER_NAME) 
    "NoneBinOp",
    BINARY_OP_LIST(CREATE_INST_OPER_NAME) 
    "NoneUnOp",
    UNARY_OP_LIST(CREATE_INST_OPER_NAME) 
    "NoneJumpOp",
    JUMP_OP_LIST(CREATE_INST_OPER_NAME)

#undef CREATE_INST_OPER_NAME
};

constexpr std::array<const char *const, static_cast<uint8_t>(JumpOpType::End)> BASE_NAME{

#define CREATE_BASE_TYPE_NAME(NAME, BASE) \
    #BASE,

    "Inst", 
    INST_TYPE_LIST(CREATE_BASE_TYPE_NAME) 
    "BinaryInst",
    BINARY_OP_LIST(CREATE_BASE_TYPE_NAME) 
    "UnaryInst",
    UNARY_OP_LIST(CREATE_BASE_TYPE_NAME) 
    "JumpInst",
    JUMP_OP_LIST(CREATE_BASE_TYPE_NAME)

#undef CREATE_BASE_TYPE_NAME
};

//////////////////////////////////////__DataType__//////////////////////////////////////////////

#define DATA_TYPE_LIST(ACTION)                                                                     \
    ACTION(i32)                                                                                    \
    ACTION(i64)                                                                                    \
    ACTION(f32)                                                                                    \
    ACTION(f64)

enum class DataType
{
    NoType = 0,

#define CREATE_DATA_TYPE(TYPE) \
    TYPE,

    DATA_TYPE_LIST(CREATE_DATA_TYPE)
#undef CREATE_DATA_TYPE

    End
};

constexpr std::array<const char *, static_cast<uint8_t>(DataType::End)> TYPE_NAME{

#define CREATE_INST_TYPE_NAME(TYPE) \
    #TYPE,

    "NoType", 
    DATA_TYPE_LIST(CREATE_INST_TYPE_NAME)

#undef CREATE_INST_TYPE_NAME
};

// clang-format on
} // namespace compiler
