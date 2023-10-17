#include "inst.h"
#include "basicblock.h"
#include "graph.h"

namespace compiler
{

template <class T, class... Args>
std::shared_ptr<Inst> createInst(Args &&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// TODO rewrite to macroses
std::string BinaryInst::getBinOpTypeString() const noexcept
{
    switch (op)
    {
    case BinOpType::Add:
        return "add";

    case BinOpType::Sub:
        return "sub";

    case BinOpType::Mul:
        return "mul";

    case BinOpType::Div:
        return "div";

    case BinOpType::Mod:
        return "mod";

    case BinOpType::Shl:
        return "shl";

    case BinOpType::Shr:
        return "shr";

    case BinOpType::And:
        return "and";

    case BinOpType::Or:
        return "or";

    case BinOpType::Xor:
        return "xor";

    case BinOpType::Cmp:
        return "cmp";

    default:
        return "";
    }
}

std::string UnaryInst::getUnOpTypeString() const noexcept
{
    switch (op)
    {
    case UnOpType::Not:
        return "not";

    case UnOpType::Return:
        return "return";

    default:
        return "";
    }
}

std::string JumpInst::getJumpOpTypeString() const noexcept
{
    switch (op)
    {
    case JumpOpType::Jmp:
        return "jmp";

    case JumpOpType::Je:
        return "je";

    case JumpOpType::Jne:
        return "jne";

    case JumpOpType::Jb:
        return "jb";

    case JumpOpType::Jbe:
        return "jbe";

    case JumpOpType::Ja:
        return "ja";

    case JumpOpType::Jae:
        return "jae";

    default:
        return "";
    }
}

void JumpInst::dump(std::ostream &out) const
{
    out << "\t"
        << "v" << id << ". " << getJumpOpTypeString() << " bb"
        << target->getId() << std::endl;
}

std::string getDataTypeString(DataType type)
{
    switch (type)
    {
    case DataType::Int32:
        return "i32";

    case DataType::Int64:
        return "i64";

    case DataType::Float32:
        return "f32";

    case DataType::Float64:
        return "f64";

    default:
        return "";
    }
}

CallInst::CallInst(std::initializer_list<size_t> args_)
    : Inst(0, InstType::Call)
{
    for (auto arg : args_)
        args.push_back(bb->getInst(arg));
}

void CallInst::dump(std::ostream &out) const
{
    out << "\t"
        << "v" << id << ". "
        << "call " << func->getName();
    out << "(";
    std::for_each(args.begin(), args.end(),
                  [&out](auto arg) { out << arg->getId() << " "; });
    out << ")" << std::endl;
}

void PhiInst::dump(std::ostream &out) const
{
    out << "\t"
        << "v" << id << ". "
        << "phi ";
    for (auto &&input : inputs)
        out << "("
            << "v" << input.first->getId() << ", bb" << input.second->getId()
            << ") ";
    out << std::endl;
}

} // namespace compiler