#pragma once

#include "inst.h"
#include "marker.h"

namespace compiler
{

constexpr size_t BB_PREDS_NUM = 2;
constexpr size_t BB_DOMINATED_NUM = 5;

class Graph;
class Loop;
class LiveInterval;

class BasicBlock
{
  public:
    BasicBlock(size_t id_, std::shared_ptr<Graph> graph_ = nullptr, std::string name_ = "")
        : id(id_), bb_size(0), name(name_), graph(graph_), markers(new MarkerSet)
    {
        preds.reserve(BB_PREDS_NUM);
        dominators.reserve(BB_DOMINATED_NUM);
    }

    ~BasicBlock();

    size_t size() const noexcept
    {
        return bb_size;
    }

    void incrSize() noexcept
    {
        ++bb_size;
    }

    bool isEmpty() noexcept
    {
        return bb_size == 0;
    }

    DEFINE_GETTER_SETTER(name, Name, std::string)
    DEFINE_GETTER_SETTER(id, Id, size_t)
    DEFINE_GETTER_SETTER(graph, Graph, std::shared_ptr<Graph>)
    DEFINE_ARRAY_GETTER(preds, Preds, std::vector<BasicBlock*>&)
    DEFINE_GETTER_SETTER(true_succ, TrueSucc, BasicBlock*)
    DEFINE_GETTER_SETTER(false_succ, FalseSucc, BasicBlock*)
    DEFINE_GETTER_SETTER(first_inst, FirstInst, Inst*)
    DEFINE_GETTER_SETTER(last_inst, LastInst, Inst*)
    DEFINE_ARRAY_GETTER(dominators, Dominators, std::vector<BasicBlock*>&)
    DEFINE_GETTER(idom, Idom, BasicBlock*)
    DEFINE_GETTER_SETTER(loop, Loop, Loop*)
    DEFINE_GETTER_SETTER(live_int, LiveInterval, LiveInterval*)

    void swapSuccs()
    {
        std::swap(true_succ, false_succ);
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

    bool isHeader() const noexcept;

    void pushBackInst(Inst* inst);
    void pushBackPhiInst(PhiInst* inst);

    void pushFrontInst(Inst* inst);
    void pushFrontPhiInst(PhiInst* inst);

    /**
     * Insert inst after prev_inst
     */
    void insertAfter(Inst* prev_inst, Inst* inst);

    void popFrontInst();
    void popBackInst();
    void removeInst(Inst* inst);

    void addPred(BasicBlock* bb);
    void addSucc(BasicBlock* bb);

    void removePred(BasicBlock* bb);
    void removePred(size_t num);

    void removeSucc(BasicBlock* bb);
    void removeSucc(size_t num);

    void replacePred(BasicBlock* pred, BasicBlock* bb);
    void replacePred(size_t num, BasicBlock* bb);

    void replaceSucc(BasicBlock* succ, BasicBlock* bb);
    void replaceSucc(size_t num, BasicBlock* bb);

    BasicBlock* splitBlockAfterInst(Inst* inst, bool make_true_succ = true);

    void setMarker(marker_t marker);
    void resetMarker(marker_t marker);
    bool isMarked(marker_t marker) const;

    void addDominator(BasicBlock* dom)
    {
        dominators.push_back(dom);
    }

    // return true, if *this is a dominator of bb
    bool dominates(BasicBlock* bb)
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

    MarkerSet* markers;
    Loop* loop = nullptr;
    LiveInterval* live_int = nullptr;
};

} // namespace compiler