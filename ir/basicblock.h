#pragma once

#include "inst.h"

namespace compiler
{

#define BB_PREDS_NUM 5
#define BB_DOMINATED_NUM 5

class Graph;

class BasicBlock
{
  public:
    BasicBlock(size_t id_, std::shared_ptr<Graph> graph_ = nullptr, std::string name_ = "")
        : id(id_), graph(graph_), name(name_)
    {
        preds.reserve(BB_PREDS_NUM);
        dominators.reserve(BB_DOMINATED_NUM);
        bb_size = 0;
    }

    ~BasicBlock() = default;

    size_t getId() const noexcept
    {
        return id;
    }

    void incrId() noexcept
    {
        ++id;
    }

    void setId(size_t id_) noexcept;

    std::string getName() const noexcept
    {
        return name;
    }

    void setName(std::string name_) noexcept
    {
        name = name_;
    }

    size_t size() const noexcept
    {
        return bb_size;
    }

    bool isEmpty() noexcept
    {
        return bb_size == 0;
    }

    std::shared_ptr<Graph> getGraph() const noexcept
    {
        return graph;
    }

    void setGraph(std::shared_ptr<Graph> graph_) noexcept
    {
        graph = graph_;
    }

    std::vector<BasicBlock *> &getPreds() noexcept
    {
        return preds;
    }

    BasicBlock *getTrueSucc() const noexcept
    {
        return true_succ;
    }

    BasicBlock *getFalseSucc() const noexcept
    {
        return false_succ;
    }

    void setTrueSucc(BasicBlock *bb) noexcept
    {
        true_succ = bb;
    }

    void setFalseSucc(BasicBlock *bb) noexcept
    {
        false_succ = bb;
    }

    std::shared_ptr<Inst> getFirstInst() const noexcept
    {
        return first_inst;
    }

    std::shared_ptr<Inst> getLastInst() const noexcept
    {
        return last_inst;
    }

    std::shared_ptr<Inst> getInst(size_t id)
    {
        for (auto inst = first_inst; inst != nullptr; inst = inst->getNext())
            if (inst->getId() == id)
                return inst;
        return nullptr;
    }

    std::vector<BasicBlock *> &getDominators()
    {
        return dominators;
    }

    void pushBackInst(std::shared_ptr<Inst> inst)
    {
        assert(!inst->getPrev() && "inserted inst has predecessor");

        if (first_inst == nullptr)
            first_inst = inst;

        inst->setPrev(last_inst);
        inst->setNext(nullptr);
        inst->setBB(this);

        if (last_inst != nullptr)
            last_inst->setNext(inst);
        last_inst = inst;
        ++bb_size;
    }

    void pushFrontInst(std::shared_ptr<Inst> inst)
    {
        assert(!inst->getNext() && "inserted inst has successor");

        if (!last_inst)
            last_inst = inst;

        inst->setPrev(nullptr);
        inst->setNext(first_inst);
        inst->setBB(this);

        if (first_inst)
            first_inst->setPrev(inst);
        first_inst = inst;
        ++bb_size;
    }

    /**
     * Insert inst after prev_inst
     */
    void insertAfter(std::shared_ptr<Inst> prev_inst, std::shared_ptr<Inst> inst)
    {
        assert(!inst->getPrev() && "inserted inst has predecessor");
        assert(!inst->getNext() && "inserted inst has successor");

        if (!prev_inst->getNext())
        {
            pushBackInst(inst);
            return;
        }

        auto next_inst = prev_inst->getNext();
        next_inst->setPrev(inst);
        prev_inst->setNext(inst);

        inst->setNext(next_inst);
        inst->setPrev(prev_inst);
        inst->setBB(this);
        ++bb_size;
    }

    void popFrontInst()
    {
        assert(first_inst && "first inst not existed");
        auto second_inst = first_inst->getNext();
        second_inst->setPrev(nullptr);
        first_inst->setNext(nullptr);
        first_inst = second_inst;
        --bb_size;
    }

    void popBackInst()
    {
        assert(last_inst && "last inst not existed");
        auto prev_inst = last_inst->getPrev();
        prev_inst->setNext(nullptr);
        last_inst->setPrev(nullptr);
        last_inst = prev_inst;
        --bb_size;
    }

    void removeInst(std::shared_ptr<Inst> inst)
    {
        auto next_inst = inst->getNext();
        auto prev_inst = inst->getPrev();
        if (next_inst)
            next_inst->setPrev(prev_inst);
        if (prev_inst)
            prev_inst->setNext(next_inst);
        --bb_size;
    }

    void addPred(BasicBlock *bb)
    {
        assert(std::find(preds.begin(), preds.end(), bb) == preds.end() && "pred already existed");
        preds.push_back(bb);
    }

    void addSucc(BasicBlock *bb)
    {
        if (true_succ == nullptr)
            true_succ = bb;
        else if (false_succ == nullptr)
            false_succ = bb;
        else
        {
            std::cerr << "cannot add succ" << std::endl;
            abort();
        }
    }

    void removePred(BasicBlock *bb)
    {
        preds.erase(std::find(preds.begin(), preds.end(), bb));
    }

    void removePred(size_t num)
    {
        preds.erase(std::find_if(preds.begin(), preds.end(),
                                 [num](auto pred) { return pred->getId() == num; }));
    }

    void removeSucc(BasicBlock *bb)
    {
        if (true_succ == bb)
            true_succ = nullptr;
        else if (false_succ == bb)
            false_succ = nullptr;
        else
            return;
    }

    void removeSucc(size_t num)
    {
        if (true_succ->getId() == num)
            true_succ = nullptr;
        else if (false_succ->getId() == num)
            false_succ = nullptr;
        else
            return;
    }

    void replacePred(BasicBlock *pred, BasicBlock *bb);
    void replacePred(size_t num, BasicBlock *bb);

    void replaceSucc(BasicBlock *succ, BasicBlock *bb)
    {
        if (true_succ == succ)
            true_succ = bb;
        else if (false_succ == succ)
            false_succ = bb;
        else
        {
            std::cerr << "replace not existing succ" << std::endl;
            abort();
        }
    }

    void replaceSucc(size_t num, BasicBlock *bb)
    {
        if (true_succ->getId() == num)
            true_succ = bb;
        else if (false_succ->getId() == num)
            false_succ = bb;
        else
        {
            std::cerr << "replace not existing succ" << std::endl;
            abort();
        }
    }

    void addDominator(BasicBlock *dom)
    {
        dominators.push_back(dom);
    }

    void dump(std::ostream &out = std::cout) const;
    void dumpDomTree(std::ostream &out = std::cout) const;

    bool isVisited() const noexcept
    {
        return visited;
    }

    void setVisited(bool v = true) noexcept
    {
        visited = v;
    }

  private:
    size_t id = 0;
    size_t bb_size = 0;
    std::string name = "";
    std::shared_ptr<Graph> graph = nullptr;

    std::vector<BasicBlock *> preds;
    BasicBlock *true_succ;
    BasicBlock *false_succ;

    std::shared_ptr<Inst> first_inst = nullptr;
    std::shared_ptr<Inst> last_inst = nullptr;

    // TODO: make marker class
    bool visited = false;

    std::vector<BasicBlock *> dominators;
};

} // namespace compiler