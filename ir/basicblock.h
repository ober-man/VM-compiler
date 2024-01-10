#pragma once

#include "inst.h"
#include "marker.h"

namespace compiler
{

#define BB_PREDS_NUM 5
#define BB_DOMINATED_NUM 5

class Graph;
class Loop;
class LiveInterval;

class BasicBlock
{
  public:
    BasicBlock(size_t id_, std::shared_ptr<Graph> graph_ = nullptr, std::string name_ = "")
        : id(id_), bb_size(0), name(name_), graph(graph_)
    {
        preds.reserve(BB_PREDS_NUM);
        dominators.reserve(BB_DOMINATED_NUM);
        markers = std::make_unique<MarkerSet>();
    }

    ~BasicBlock();

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

    std::vector<BasicBlock*>& getPreds() noexcept
    {
        return preds;
    }

    BasicBlock* getTrueSucc() const noexcept
    {
        return true_succ;
    }

    BasicBlock* getFalseSucc() const noexcept
    {
        return false_succ;
    }

    void setTrueSucc(BasicBlock* bb) noexcept
    {
        true_succ = bb;
    }

    void setFalseSucc(BasicBlock* bb) noexcept
    {
        false_succ = bb;
    }

    void swapSuccs()
    {
        std::swap(true_succ, false_succ);
    }

    Inst* getFirstInst() const noexcept
    {
        return first_inst;
    }

    Inst* getLastInst() const noexcept
    {
        return last_inst;
    }

    Inst* getFirstPhi() const noexcept
    {
        return static_cast<Inst*>(first_phi);
    }

    Inst* getLastPhi() const noexcept
    {
        return static_cast<Inst*>(last_phi);
    }

    Inst* getInst(size_t id)
    {
        for (auto inst = first_inst; inst != nullptr; inst = inst->getNext())
            if (inst->getId() == id)
                return inst;
        return nullptr;
    }

    std::vector<BasicBlock*>& getDominators()
    {
        return dominators;
    }

    BasicBlock* getIdom() const noexcept
    {
        return idom;
    }

    Loop* getLoop() const noexcept
    {
        return loop;
    }

    void setLoop(Loop* loop_) noexcept
    {
        loop = loop_;
    }

    bool isHeader() const noexcept;

    LiveInterval* getLiveInterval() const noexcept
    {
        return live_int;
    }

    void setLiveInterval(LiveInterval* live) noexcept
    {
        live_int = live;
    }

    void pushBackInst(Inst* inst)
    {
        ASSERT(inst->getInstType() != InstType::Phi);
        ASSERT(!inst->getPrev(), "inserted inst has predecessor");

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

    void pushBackPhiInst(PhiInst* inst)
    {
        ASSERT(inst->getInstType() == InstType::Phi);
        ASSERT(!inst->getPrev(), "inserted inst has predecessor");

        if (first_phi == nullptr)
            first_phi = inst;

        inst->setPrev(last_phi);
        inst->setNext(nullptr);
        inst->setBB(this);

        if (last_phi != nullptr)
            last_phi->setNext(inst);
        last_phi = inst;
        ++bb_size;
    }

    void pushFrontInst(Inst* inst)
    {
        ASSERT(inst->getInstType() != InstType::Phi);
        ASSERT(!inst->getNext(), "inserted inst has successor");

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

    void pushFrontPhiInst(PhiInst* inst)
    {
        ASSERT(inst->getInstType() == InstType::Phi);
        ASSERT(!inst->getNext(), "inserted inst has successor");

        if (!last_phi)
            last_phi = inst;

        inst->setPrev(nullptr);
        inst->setNext(first_phi);
        inst->setBB(this);

        if (first_phi)
            first_phi->setPrev(inst);
        first_phi = inst;
        ++bb_size;
    }

    /**
     * Insert inst after prev_inst
     */
    void insertAfter(Inst* prev_inst, Inst* inst)
    {
        ASSERT(!inst->getPrev(), "inserted inst has predecessor");
        ASSERT(!inst->getNext(), "inserted inst has successor");

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
        ASSERT(first_inst, "first inst not existed");
        auto second_inst = first_inst->getNext();
        second_inst->setPrev(nullptr);
        first_inst->setNext(nullptr);
        first_inst = second_inst;
        --bb_size;
    }

    void popBackInst()
    {
        ASSERT(last_inst, "last inst not existed");
        auto prev_inst = last_inst->getPrev();
        prev_inst->setNext(nullptr);
        last_inst->setPrev(nullptr);
        last_inst = prev_inst;
        --bb_size;
    }

    void removeInst(Inst* inst)
    {
        auto next_inst = inst->getNext();
        auto prev_inst = inst->getPrev();
        if (next_inst)
            next_inst->setPrev(prev_inst);
        if (prev_inst)
            prev_inst->setNext(next_inst);
        --bb_size;
    }

    void addPred(BasicBlock* bb)
    {
        ASSERT(std::find(preds.begin(), preds.end(), bb) == preds.end(), "pred already existed");
        preds.push_back(bb);
    }

    void addSucc(BasicBlock* bb)
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

    void removePred(BasicBlock* bb)
    {
        preds.erase(std::find(preds.begin(), preds.end(), bb));
    }

    void removePred(size_t num)
    {
        preds.erase(std::find_if(preds.begin(), preds.end(),
                                 [num](auto pred) { return pred->getId() == num; }));
    }

    void removeSucc(BasicBlock* bb)
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

    void replacePred(BasicBlock* pred, BasicBlock* bb);
    void replacePred(size_t num, BasicBlock* bb);

    void replaceSucc(BasicBlock* succ, BasicBlock* bb)
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

    void replaceSucc(size_t num, BasicBlock* bb)
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

    void setMarker(marker_t marker);
    void resetMarker(marker_t marker);
    bool isMarked(marker_t marker) const;

    void addDominator(BasicBlock* dom)
    {
        dominators.push_back(dom);
    }

    // return true, if *this is a dominator of bb
    bool isDominator(BasicBlock* bb)
    {
        auto& doms = bb->getDominators();
        return std::find(doms.begin(), doms.end(), this) != doms.end();
    }

    void countIdom()
    {
        size_t dom_num = dominators.size();
        if (dom_num == 1)
            idom = dominators[dominators.size() - 1];
        else
            idom = dominators[dominators.size() - 2];
    }

    void dump(std::ostream& out = std::cout) const;

  private:
    size_t id = 0;
    size_t bb_size = 0;
    std::string name = "";
    std::shared_ptr<Graph> graph = nullptr;

    std::vector<BasicBlock*> preds;
    BasicBlock* true_succ;
    BasicBlock* false_succ;

    Inst* first_inst = nullptr;
    Inst* last_inst = nullptr;
    PhiInst* first_phi = nullptr;
    PhiInst* last_phi = nullptr;

    std::vector<BasicBlock*> dominators;
    BasicBlock* idom = nullptr;

    std::unique_ptr<MarkerSet> markers;
    Loop* loop = nullptr;

    LiveInterval* live_int = nullptr;
};

} // namespace compiler