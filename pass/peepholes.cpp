#include "peepholes.h"
#include <math.h>

namespace compiler
{

bool Peepholes::runPassImpl()
{
    ASSERT(graph != nullptr, "nullptr graph in Peepholes pass");
    visitGraph(graph);
    return true;
}

static bool isPowerOfTwo(int value)
{
    return value && !(value & (value - 1));
}

void Peepholes::visitMul([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Mul);
    auto* mul_inst = static_cast<BinaryInst*>(inst);
    auto* left = mul_inst->getInput(0);
    auto* right = mul_inst->getInput(1);
    if (left->isConstInst())
    {
        mul_inst->swapInputs();
        left = mul_inst->getInput(0);
        right = mul_inst->getInput(1);
    }
    else if (!right->isConstInst())
        return;

    auto* const_inst = static_cast<ConstInst*>(right);
    if (const_inst->getType() != DataType::i32 && const_inst->getType() != DataType::i64)
        return;

    auto value = const_inst->getIntValue();
    if (value == 0)
    {
        // 1. Mul v1, 0  -->  Const 0
        inst->replaceUsers(const_inst);
    }
    else if (value == 1)
    {
        // 2. Mul v1, 1  -->  v1
        inst->replaceUsers(left);
    }
    else if (value == static_cast<uint64_t>(-1))
    {
        // 3. Mul v1, -1 --> Neg v1
        auto* neg_inst = new UnaryInst{graph->getCurInstId(), InstType::Neg, left};
        graph->setCurInstId(graph->getCurInstId() + 1);
        auto* bb = inst->getBB();
        bb->insertAfter(inst, neg_inst);
        inst->replaceUsers(neg_inst);
    }
    else if (isPowerOfTwo(value))
    {
        // 4. Mul v1, 2^k  -->  Shl v1, k
        uint32_t power = static_cast<uint32_t>(std::log2(value));
        auto* new_const = graph->findConstant(power);
        auto* shl_inst = new BinaryInst{graph->getCurInstId(), InstType::Shl, left, new_const};
        graph->setCurInstId(graph->getCurInstId() + 1);
        auto* bb = inst->getBB();
        bb->insertAfter(inst, shl_inst);
        inst->replaceUsers(shl_inst);
    }
}

void Peepholes::visitOr([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::Or);
    auto* or_inst = static_cast<BinaryInst*>(inst);
    auto* left = or_inst->getInput(0);
    auto* right = or_inst->getInput(1);

    if (left == right)
    {
        // 1. Or v1, v1  -->  v1
        inst->replaceUsers(left);
        return;
    }

    if (left->getInstType() == InstType::Not && right->getInstType() == InstType::Not)
    {
        // (De Morgan rule)
        // 2. v3. Not v1      -->  NOP
        //    v4. Not v2      -->  NOP
        //    v5. Or v3, v4   -->  And v1, v2
        auto* left_neg = static_cast<UnaryInst*>(left);
        auto* right_neg = static_cast<UnaryInst*>(right);
        auto* left_input = left_neg->getInput(0);
        auto* right_input = right_neg->getInput(0);
        auto* and_inst =
            new BinaryInst{graph->getCurInstId(), InstType::And, left_input, right_input};
        graph->setCurInstId(graph->getCurInstId() + 1);
        auto* bb = inst->getBB();
        bb->insertAfter(inst, and_inst);
        left_neg->removeUser(inst);
        right_neg->removeUser(inst);
        inst->replaceUsers(and_inst);
        return;
    }

    if (left->isConstInst())
    {
        or_inst->swapInputs();
        left = or_inst->getInput(0);
        right = or_inst->getInput(1);
    }
    else if (!right->isConstInst())
        return;

    auto* const_inst = static_cast<ConstInst*>(right);
    if (const_inst->getType() != DataType::i32 && const_inst->getType() != DataType::i64)
        return;

    auto value = const_inst->getIntValue();
    if (value == 0)
    {
        // 3. Or v1, 0  -->  v1
        inst->replaceUsers(left);
    }
    else if (value == std::numeric_limits<uint64_t>::max())
    {
        // 4. Or v1, 2^k-1  -->  1
        auto* new_const = graph->findConstant(1);
        inst->replaceUsers(new_const);
    }
}

void Peepholes::visitAShr([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::AShr);
    auto* ashr_inst = static_cast<BinaryInst*>(inst);
    auto* left = ashr_inst->getInput(0);
    auto* right = ashr_inst->getInput(1);

    if (left->isConstInst())
    {
        ashr_inst->swapInputs();
        left = ashr_inst->getInput(0);
        right = ashr_inst->getInput(1);
    }
    else if (!right->isConstInst())
        return;

    auto* const_inst = static_cast<ConstInst*>(right);
    if (const_inst->getType() != DataType::i32 && const_inst->getType() != DataType::i64)
        return;

    auto value = const_inst->getIntValue();
    if (value == 0)
    {
        // 1. AShr v1, 0 --> v1
        inst->replaceUsers(left);
    }
}

} // namespace compiler