#include "const_folding.h"

namespace compiler
{

bool ConstFolding::runPassImpl()
{
    ASSERT(graph != nullptr, "nullptr graph in ConstFolding pass");
    visitGraph(graph);
    return true;
}

void ConstFolding::visitAdd([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Add);
    auto* binary_inst = static_cast<BinaryInst*>(inst);
    auto* left = binary_inst->getInput(0);
    auto* right = binary_inst->getInput(1);
    if (left->isConstInst() && right->isConstInst())
    {
        auto* left_const = static_cast<ConstInst*>(left);
        auto* right_const = static_cast<ConstInst*>(right);
        auto type = left_const->getType();
        ASSERT(type == right->getType());
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const =
                    graph->findConstant(left_const->getInt32Value() + right_const->getInt32Value());
                break;
            case DataType::i64:
                new_const =
                    graph->findConstant(left_const->getInt64Value() + right_const->getInt64Value());
                break;
            case DataType::f32:
                new_const =
                    graph->findConstant(left_const->getFloatValue() + right_const->getFloatValue());
                break;
            case DataType::f64:
                new_const = graph->findConstant(left_const->getDoubleValue() +
                                                right_const->getDoubleValue());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitSub([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Sub);
    auto* binary_inst = static_cast<BinaryInst*>(inst);
    auto* left = binary_inst->getInput(0);
    auto* right = binary_inst->getInput(1);
    if (left->isConstInst() && right->isConstInst())
    {
        auto* left_const = static_cast<ConstInst*>(left);
        auto* right_const = static_cast<ConstInst*>(right);
        auto type = left_const->getType();
        ASSERT(type == right->getType());
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const =
                    graph->findConstant(left_const->getInt32Value() - right_const->getInt32Value());
                break;
            case DataType::i64:
                new_const =
                    graph->findConstant(left_const->getInt64Value() - right_const->getInt64Value());
                break;
            case DataType::f32:
                new_const =
                    graph->findConstant(left_const->getFloatValue() - right_const->getFloatValue());
                break;
            case DataType::f64:
                new_const = graph->findConstant(left_const->getDoubleValue() -
                                                right_const->getDoubleValue());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitMul([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Mul);
    auto* binary_inst = static_cast<BinaryInst*>(inst);
    auto* left = binary_inst->getInput(0);
    auto* right = binary_inst->getInput(1);
    if (left->isConstInst() && right->isConstInst())
    {
        auto* left_const = static_cast<ConstInst*>(left);
        auto* right_const = static_cast<ConstInst*>(right);
        auto type = left_const->getType();
        ASSERT(type == right->getType());
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const =
                    graph->findConstant(left_const->getInt32Value() * right_const->getInt32Value());
                break;
            case DataType::i64:
                new_const =
                    graph->findConstant(left_const->getInt64Value() * right_const->getInt64Value());
                break;
            case DataType::f32:
                new_const =
                    graph->findConstant(left_const->getFloatValue() * right_const->getFloatValue());
                break;
            case DataType::f64:
                new_const = graph->findConstant(left_const->getDoubleValue() *
                                                right_const->getDoubleValue());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitDiv([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Div);
    auto* binary_inst = static_cast<BinaryInst*>(inst);
    auto* left = binary_inst->getInput(0);
    auto* right = binary_inst->getInput(1);
    if (left->isConstInst() && right->isConstInst())
    {
        auto* left_const = static_cast<ConstInst*>(left);
        auto* right_const = static_cast<ConstInst*>(right);
        auto type = left_const->getType();
        ASSERT(type == right->getType());
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const =
                    graph->findConstant(left_const->getInt32Value() / right_const->getInt32Value());
                break;
            case DataType::i64:
                new_const =
                    graph->findConstant(left_const->getInt64Value() / right_const->getInt64Value());
                break;
            case DataType::f32:
                new_const =
                    graph->findConstant(left_const->getFloatValue() / right_const->getFloatValue());
                break;
            case DataType::f64:
                new_const = graph->findConstant(left_const->getDoubleValue() /
                                                right_const->getDoubleValue());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitMod([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Mod);
    auto* binary_inst = static_cast<BinaryInst*>(inst);
    auto* left = binary_inst->getInput(0);
    auto* right = binary_inst->getInput(1);
    if (left->isConstInst() && right->isConstInst())
    {
        auto* left_const = static_cast<ConstInst*>(left);
        auto* right_const = static_cast<ConstInst*>(right);
        auto type = left_const->getType();
        ASSERT(type == right->getType());
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const =
                    graph->findConstant(left_const->getInt32Value() % right_const->getInt32Value());
                break;
            case DataType::i64:
                new_const =
                    graph->findConstant(left_const->getInt64Value() % right_const->getInt64Value());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitShl([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Shl);
    auto* binary_inst = static_cast<BinaryInst*>(inst);
    auto* left = binary_inst->getInput(0);
    auto* right = binary_inst->getInput(1);
    if (left->isConstInst() && right->isConstInst())
    {
        auto* left_const = static_cast<ConstInst*>(left);
        auto* right_const = static_cast<ConstInst*>(right);
        auto type = left_const->getType();
        ASSERT(type == right->getType());
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const = graph->findConstant(left_const->getInt32Value()
                                                << right_const->getInt32Value());
                break;
            case DataType::i64:
                new_const = graph->findConstant(left_const->getInt64Value()
                                                << right_const->getInt64Value());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitShr([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Shr);
    auto* binary_inst = static_cast<BinaryInst*>(inst);
    auto* left = binary_inst->getInput(0);
    auto* right = binary_inst->getInput(1);
    if (left->isConstInst() && right->isConstInst())
    {
        auto* left_const = static_cast<ConstInst*>(left);
        auto* right_const = static_cast<ConstInst*>(right);
        auto type = left_const->getType();
        ASSERT(type == right->getType());
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const = graph->findConstant(left_const->getInt32Value() >>
                                                right_const->getInt32Value());
                break;
            case DataType::i64:
                new_const = graph->findConstant(left_const->getInt64Value() >>
                                                right_const->getInt64Value());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitAShr([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::AShr);
    auto* binary_inst = static_cast<BinaryInst*>(inst);
    auto* left = binary_inst->getInput(0);
    auto* right = binary_inst->getInput(1);
    if (left->isConstInst() && right->isConstInst())
    {
        auto* left_const = static_cast<ConstInst*>(left);
        auto* right_const = static_cast<ConstInst*>(right);
        auto type = left_const->getType();
        ASSERT(type == right->getType());
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const = graph->findConstant(static_cast<int32_t>(left_const->getInt32Value()) >>
                                                right_const->getInt32Value());
                break;
            case DataType::i64:
                new_const = graph->findConstant(left_const->getInt64Value() >>
                                                right_const->getInt64Value());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitAnd([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::And);
    auto* binary_inst = static_cast<BinaryInst*>(inst);
    auto* left = binary_inst->getInput(0);
    auto* right = binary_inst->getInput(1);
    if (left->isConstInst() && right->isConstInst())
    {
        auto* left_const = static_cast<ConstInst*>(left);
        auto* right_const = static_cast<ConstInst*>(right);
        auto type = left_const->getType();
        ASSERT(type == right->getType());
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const =
                    graph->findConstant(left_const->getInt32Value() & right_const->getInt32Value());
                break;
            case DataType::i64:
                new_const =
                    graph->findConstant(left_const->getInt64Value() & right_const->getInt64Value());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitOr([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Or);
    auto* binary_inst = static_cast<BinaryInst*>(inst);
    auto* left = binary_inst->getInput(0);
    auto* right = binary_inst->getInput(1);
    if (left->isConstInst() && right->isConstInst())
    {
        auto* left_const = static_cast<ConstInst*>(left);
        auto* right_const = static_cast<ConstInst*>(right);
        auto type = left_const->getType();
        ASSERT(type == right->getType());
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const =
                    graph->findConstant(left_const->getInt32Value() | right_const->getInt32Value());
                break;
            case DataType::i64:
                new_const =
                    graph->findConstant(left_const->getInt64Value() | right_const->getInt64Value());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitXor([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Xor);
    auto* binary_inst = static_cast<BinaryInst*>(inst);
    auto* left = binary_inst->getInput(0);
    auto* right = binary_inst->getInput(1);
    if (left->isConstInst() && right->isConstInst())
    {
        auto* left_const = static_cast<ConstInst*>(left);
        auto* right_const = static_cast<ConstInst*>(right);
        auto type = left_const->getType();
        ASSERT(type == right->getType());
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const =
                    graph->findConstant(left_const->getInt32Value() ^ right_const->getInt32Value());
                break;
            case DataType::i64:
                new_const =
                    graph->findConstant(left_const->getInt64Value() ^ right_const->getInt64Value());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitNot([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Not);
    auto* unary_inst = static_cast<UnaryInst*>(inst);
    auto* input = unary_inst->getInput(0);
    if (input->isConstInst())
    {
        auto* input_const = static_cast<ConstInst*>(input);
        auto type = input_const->getType();
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const = graph->findConstant(~input_const->getInt32Value());
                break;
            case DataType::i64:
                new_const = graph->findConstant(~input_const->getInt64Value());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

void ConstFolding::visitNeg([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Neg);
    auto* unary_inst = static_cast<UnaryInst*>(inst);
    auto* input = unary_inst->getInput(0);
    if (input->isConstInst())
    {
        auto* input_const = static_cast<ConstInst*>(input);
        auto type = input_const->getType();
        ConstInst* new_const = nullptr;
        switch (type)
        {
            case DataType::i32:
                new_const = graph->findConstant(-input_const->getInt32Value());
                break;
            case DataType::i64:
                new_const = graph->findConstant(-input_const->getInt64Value());
                break;
            case DataType::f32:
                new_const = graph->findConstant(-input_const->getFloatValue());
                break;
            case DataType::f64:
                new_const = graph->findConstant(-input_const->getDoubleValue());
                break;
            default:
                UNREACHABLE();
        }
        inst->replaceUsers(new_const);
    }
}

} // namespace compiler