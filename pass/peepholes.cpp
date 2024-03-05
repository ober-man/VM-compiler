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

    for (auto input : {left, right})
    {
        if (!input->isConstInst())
            continue;

        auto* const_inst = static_cast<ConstInst*>(input);
        auto* other_inst = (input == left) ? right : left;
        if (const_inst->getType() != DataType::i32 && const_inst->getType() != DataType::i64)
        {
            continue;
        }

        auto value = const_inst->getIntValue();
        if (value == 0)
        {
            // 1. Mul v1, 0  -->  Const 0
            inst->replaceUsers(const_inst);
        }
        else if (value == 1)
        {
            // 2. Mul v1, 1  -->  v1
            inst->replaceUsers(other_inst);
        }
        else if (value == static_cast<uint64_t>(-1))
        {
            // 3. Mul v1, -1 --> Neg v1
            auto* neg_inst = new UnaryInst{graph->getCurInstId(), InstType::Neg, other_inst};
            graph->setCurInstId(graph->getCurInstId() + 1);
            auto* bb = inst->getBB();
            bb->insertAfter(inst, neg_inst);
            inst->replaceUsers(neg_inst);
        }
        else if (isPowerOfTwo(value))
        {
            // 3. Mul v1, 2^k  -->  Shl v1, k
            uint32_t power = static_cast<uint32_t>(std::log2(value));
            auto* new_const = graph->findConstant(power);
            auto* shl_inst =
                new BinaryInst{graph->getCurInstId(), InstType::Shl, other_inst, new_const};
            graph->setCurInstId(graph->getCurInstId() + 1);
            auto* bb = inst->getBB();
            bb->insertAfter(inst, shl_inst);
            inst->replaceUsers(shl_inst);
        }
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
    }

    for (auto input : {left, right})
    {
        if (!input->isConstInst())
            continue;

        auto* const_inst = static_cast<ConstInst*>(input);
        auto* other_inst = (input == left) ? right : left;
        if (const_inst->getType() != DataType::i32 && const_inst->getType() != DataType::i64)
        {
            continue;
        }

        auto value = const_inst->getIntValue();
        if (value == 0)
        {
            // 2. Or v1, 0  -->  v1
            inst->replaceUsers(other_inst);
        }
        else if (value == std::numeric_limits<uint64_t>::max())
        {
            // 3. Or v1, 2^k-1  -->  1
            auto* new_const = graph->findConstant(1);
            inst->replaceUsers(new_const);
        }
    }

    if (left->getInstType() == InstType::Not && right->getInstType() == InstType::Not)
    {
        // (De Morgan rule)
        // 4. v3. Not v1           NOP
        //    v4. Not v2           NOP
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
    }
}

void Peepholes::visitAShr([[maybe_unused]] Visitor* v, Inst* inst)
{
    ASSERT(inst->getInstType() == InstType::AShr);
    auto* ashr_inst = static_cast<BinaryInst*>(inst);
    auto* left = ashr_inst->getInput(0);
    auto* right = ashr_inst->getInput(1);
    auto* prev_inst = inst->getPrev();

    for (auto input : {left, right})
    {
        if (!input->isConstInst())
            continue;

        auto* const_inst = static_cast<ConstInst*>(input);
        auto* other_inst = (input == left) ? right : left;
        if (const_inst->getType() != DataType::i32 && const_inst->getType() != DataType::i64)
        {
            continue;
        }

        auto value = const_inst->getIntValue();
        if (value == 0)
        {
            // 1. AShr v1, 0 --> v1
            inst->replaceUsers(other_inst);
        }
    }

    // 2. v3. Shl v1, v2
    //	  v4. AShr v3, v2  -->  v1
    if (prev_inst && prev_inst->getInstType() == InstType::Shl &&
        (ashr_inst->getInput(0) == prev_inst || ashr_inst->getInput(1) == prev_inst))
    {
        auto* shl_inst = static_cast<BinaryInst*>(prev_inst);
        auto* other_inst =
            (ashr_inst->getInput(0) == prev_inst) ? ashr_inst->getInput(1) : ashr_inst->getInput(0);
        auto* res_inst =
            (shl_inst->getInput(0) == other_inst)
                ? shl_inst->getInput(1)
                : (shl_inst->getInput(1) == other_inst) ? shl_inst->getInput(0) : nullptr;
        if (res_inst != nullptr)
        {
            shl_inst->removeUser(inst);
            inst->replaceUsers(res_inst);
        }
    }
}

} // namespace compiler