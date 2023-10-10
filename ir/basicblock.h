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
    BasicBlock(size_t id_, std::shared_ptr<Graph> graph_ = nullptr,
               std::string name_ = "")
        : id(id_), graph(graph_), name(name_)
    {
        preds.reserve(BB_PREDS_NUM);
        dominators.reserve(BB_DOMINATED_NUM);
        bb_size = 0;
    }

    ~BasicBlock() = default;

    size_t getId() const
    {
        return id;
    }
    void incrId()
    {
        ++id;
    }
    void setId(size_t id_);

    std::string getName() const
    {
        return name;
    }
    void setName(std::string name_)
    {
        name = name_;
    }

    size_t size() const
    {
        return bb_size;
    }
    bool isEmpty()
    {
        return bb_size == 0;
    }

    std::shared_ptr<Graph> getGraph() const
    {
        return graph;
    }
    void setGraph(std::shared_ptr<Graph> graph_)
    {
        graph = graph_;
    }

    std::vector<std::shared_ptr<BasicBlock>> &getPreds()
    {
        return preds;
    }
    std::shared_ptr<BasicBlock> getTrueSucc()
    {
        return true_succ;
    }
    std::shared_ptr<BasicBlock> getFalseSucc()
    {
        return false_succ;
    }

    std::shared_ptr<Inst> getFirstInst() const
    {
        return first_inst;
    }
    std::shared_ptr<Inst> getLastInst() const
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

    void pushBackInst(std::shared_ptr<Inst> inst)
    {
        assert(!inst->getPrev() && "inserted inst has predecessor");

        if (!first_inst)
            first_inst = inst;

        inst->setPrev(last_inst);
        inst->setNext(nullptr);
        inst->setBB(std::make_shared<BasicBlock>(*this));

        if (last_inst)
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
        inst->setBB(std::make_shared<BasicBlock>(*this));

        if (first_inst)
            first_inst->setPrev(inst);
        first_inst = inst;
        ++bb_size;
    }

    /**
     * Insert inst after prev_inst
     */
    void insertAfter(std::shared_ptr<Inst> prev_inst,
                     std::shared_ptr<Inst> inst)
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
        inst->setBB(std::make_shared<BasicBlock>(*this));
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

    void addPred(std::shared_ptr<BasicBlock> bb)
    {
        assert(std::find(preds.begin(), preds.end(), bb) == preds.end() &&
               "pred already existed");
        preds.push_back(bb);
    }

    void addSucc(std::shared_ptr<BasicBlock> bb)
    {
        if (true_succ != nullptr)
            true_succ = bb;
        else if (false_succ != nullptr)
            false_succ = bb;
        else
        {
            std::cerr << "cannot add succ" << std::endl;
            abort();
        }
    }

    void removePred(std::shared_ptr<BasicBlock> bb)
    {
        preds.erase(std::find(preds.begin(), preds.end(), bb));
    }

    void removePred(size_t num)
    {
        preds.erase(std::find_if(preds.begin(), preds.end(), [num](auto pred) {
            return pred->getId() == num;
        }));
    }

    void removeSucc(std::shared_ptr<BasicBlock> bb)
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

    void replacePred(std::shared_ptr<BasicBlock> pred,
                     std::shared_ptr<BasicBlock> bb);
    void replacePred(size_t num, std::shared_ptr<BasicBlock> bb);

    void replaceSucc(std::shared_ptr<BasicBlock> succ,
                     std::shared_ptr<BasicBlock> bb)
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

    void replaceSucc(size_t num, std::shared_ptr<BasicBlock> bb)
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

    void addDominator(std::shared_ptr<BasicBlock> dom)
    {
        dominators.push_back(dom);
    }

    void dump(std::ostream &out = std::cout);
    void dumpDomTree(std::ostream &out = std::cout);

    bool isVisited() const
    {
        return visited;
    }
    void setVisited(bool v = true)
    {
        visited = v;
    }

  private:
    size_t id = 0;
    size_t bb_size = 0;
    std::string name = "";
    std::shared_ptr<Graph> graph = nullptr;

    std::vector<std::shared_ptr<BasicBlock>> preds;
    std::shared_ptr<BasicBlock> true_succ;
    std::shared_ptr<BasicBlock> false_succ;

    std::shared_ptr<Inst> first_inst = nullptr;
    std::shared_ptr<Inst> last_inst = nullptr;

    // TODO: make marker class
    bool visited = false;

    std::vector<std::shared_ptr<BasicBlock>> dominators;
};

} // namespace compiler