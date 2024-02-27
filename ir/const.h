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
    ACTION(Const, ConstInst)                                                                       \
    ACTION(Param, ParamInst)                                                                       \
    ACTION(Call,  CallInst)                                                                        \
    ACTION(Cast,  CastInst)                                                                        \
    ACTION(Mov,   MovInst)                                                                         \
    ACTION(Phi,   PhiInst)

#define BINARY_OP_LIST(ACTION)                                                                     \
    ACTION(Add,  BinaryInst)                                                                       \
    ACTION(Sub,  BinaryInst)                                                                       \
    ACTION(Mul,  BinaryInst)                                                                       \
    ACTION(Div,  BinaryInst)                                                                       \
    ACTION(Mod,  BinaryInst)                                                                       \
    ACTION(Shl,  BinaryInst)                                                                       \
    ACTION(Shr,  BinaryInst)                                                                       \
    ACTION(AShr, BinaryInst)                                                                       \
    ACTION(And,  BinaryInst)                                                                       \
    ACTION(Or,   BinaryInst)                                                                       \
    ACTION(Xor,  BinaryInst)                                                                       \
    ACTION(Cmp,  BinaryInst)

#define UNARY_OP_LIST(ACTION)                                                                      \
    ACTION(Not,    UnaryInst)                                                                      \
    ACTION(Neg,    UnaryInst)                                                                      \
    ACTION(Return, UnaryInst)

#define JUMP_OP_LIST(ACTION)                                                                       \
    ACTION(Jmp, JumpInst)                                                                          \
    ACTION(Je,  JumpInst)                                                                          \
    ACTION(Jne, JumpInst)                                                                          \
    ACTION(Jb,  JumpInst)                                                                          \
    ACTION(Jbe, JumpInst)                                                                          \
    ACTION(Ja,  JumpInst)                                                                          \
    ACTION(Jae, JumpInst)

enum class InstType
{
    NoneInst = 0,

#define CREATE_INST_TYPE(NAME, BASE) NAME,

    BINARY_OP_LIST(CREATE_INST_TYPE)
    UNARY_OP_LIST(CREATE_INST_TYPE)
    JUMP_OP_LIST(CREATE_INST_TYPE)
    INST_TYPE_LIST(CREATE_INST_TYPE)

#undef CREATE_INST_TYPE

    End
};

//////////////////////////////////////__Name_arrays__///////////////////////////////////////////

constexpr std::array<const char *const, static_cast<uint8_t>(InstType::End)> OPER_NAME{

#define CREATE_INST_OPER_NAME(NAME, BASE) #NAME,

    "NoneInst",
    BINARY_OP_LIST(CREATE_INST_OPER_NAME)
    UNARY_OP_LIST(CREATE_INST_OPER_NAME)
    JUMP_OP_LIST(CREATE_INST_OPER_NAME)
    INST_TYPE_LIST(CREATE_INST_OPER_NAME)

#undef CREATE_INST_OPER_NAME
};

constexpr std::array<const char *const, static_cast<uint8_t>(InstType::End)> BASE_NAME{

#define CREATE_BASE_TYPE_NAME(NAME, BASE) #BASE,

    "Inst",
    BINARY_OP_LIST(CREATE_BASE_TYPE_NAME)
    UNARY_OP_LIST(CREATE_BASE_TYPE_NAME)
    JUMP_OP_LIST(CREATE_BASE_TYPE_NAME)
    INST_TYPE_LIST(CREATE_BASE_TYPE_NAME)

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

#define CREATE_DATA_TYPE(TYPE) TYPE,

    DATA_TYPE_LIST(CREATE_DATA_TYPE)
#undef CREATE_DATA_TYPE

    End
};

constexpr std::array<const char *, static_cast<uint8_t>(DataType::End)> TYPE_NAME{

#define CREATE_DATA_TYPE_NAME(TYPE) #TYPE,

    "NoType", 
    DATA_TYPE_LIST(CREATE_DATA_TYPE_NAME)

#undef CREATE_DATA_TYPE_NAME
};

// clang-format on
} // namespace compiler
