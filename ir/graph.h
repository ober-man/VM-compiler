#pragma once

#include "basicblock.h"
#include "inst.h"
#include "pass/passmanager.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace compiler
{

constexpr size_t GRAPH_BB_NUM = 20;
constexpr size_t GRAPH_INST_NUM = 50;

class BasicBlock;
class PassManager;
class Rpo;
class DomTree;
class Loop;
class LoopAnalysis;
class LinearOrder;
class LivenessAnalysis;
class LiveInterval;
class RegisterAllocation;

class Graph
{
  public:
    Graph(std::string name = "") : func_name(name), graph_size(0), cur_inst_id(0)
    {
        BBs.reserve(GRAPH_BB_NUM);
        rpo_BBs.reserve(GRAPH_BB_NUM);
        linear_order_BBs.reserve(GRAPH_BB_NUM);
        pm = std::make_unique<PassManager>(this);
        mm = std::make_unique<MarkerManager>();
    }
    ~Graph();

    using live_intervals_t = std::unordered_map<Inst*, LiveInterval*>;

    size_t size() const noexcept
    {
        return graph_size;
    }

    bool isEmpty() noexcept
    {
        return graph_size == 0;
    }

    DEFINE_GETTER_SETTER(cur_inst_id, CurInstId, size_t)
    DEFINE_ARRAY_GETTER(func_name, Name, std::string)
    DEFINE_ARRAY_GETTER(BBs, BBs, std::vector<BasicBlock*>&)
    DEFINE_ARRAY_GETTER_SETTER(rpo_BBs, RpoBBs, std::vector<BasicBlock*>&)
    DEFINE_ARRAY_GETTER_SETTER(linear_order_BBs, LinearOrderBBs, std::vector<BasicBlock*>&)
    DEFINE_ARRAY_GETTER_SETTER(live_intervals, LiveIntervals, live_intervals_t&)
    DEFINE_GETTER_SETTER(root_loop, RootLoop, Loop*)
    DEFINE_GETTER(first_const, FirstConst, ConstInst*)
    DEFINE_GETTER(last_const, LastConst, ConstInst*)

    BasicBlock* getFirstBB() const noexcept
    {
        if (graph_size == 0)
            return nullptr;
        return BBs[0];
    }

    BasicBlock* getLastBB() const noexcept
    {
        if (graph_size == 0)
            return nullptr;
        return BBs[graph_size - 1];
    }

    BasicBlock* getBB(size_t id) const noexcept
    {
        if (id >= graph_size)
            return nullptr;
        else
            return BBs[id];
    }

    template <typename T>
    ConstInst* findConstant(T value)
    {
        if (first_const == nullptr)
        {
            auto* new_const = new ConstInst{cur_inst_id++, value};
            auto* first_bb = getFirstBB();
            ASSERT(first_bb != nullptr);
            first_bb->pushBackInst(new_const);
            return first_const;
        }

        for (ConstInst* cur_const = first_const; cur_const != nullptr;
             cur_const = static_cast<ConstInst*>(cur_const->getNext()))
            if (cur_const->getType() == getDataType<T>() && cur_const->getValue<T>() == value)
                return cur_const;

        auto* new_const = new ConstInst{cur_inst_id++, value};
        auto* first_bb = getFirstBB();
        first_bb->pushBackInst(new_const);
        last_const = new_const;
        return last_const;
    }

    void pushBackConstInst(ConstInst* inst);

    void removeBB(BasicBlock* bb);
    void removeBB(size_t num);

    void addBB(BasicBlock* bb);
    void insertBB(BasicBlock* bb);
    void insertBBAfter(BasicBlock* prev_bb, BasicBlock* bb, bool is_true_succ = true);
    void addEdge(BasicBlock* prev_bb, BasicBlock* bb);

    void replaceBB(BasicBlock* bb, BasicBlock* new_bb);
    void replaceBB(size_t num, BasicBlock* new_bb);

    marker_t getNewMarker();
    void deleteMarker(marker_t marker);

    void dump(std::ostream& out = std::cout)
    {
        std::cout << "Graph for proc " << func_name << std::endl;
        std::for_each(BBs.begin(), BBs.end(), [&out](auto bb) { bb->dump(out); });
    }

    template <LegalPass PassName, typename... Args>
    bool runPass(Args&&... args)
    {
        ASSERT(pm != nullptr);
        return pm->runPass<PassName>(std::forward<Args>(args)...);
    }

  private:
    std::string func_name = "";
    size_t graph_size = 0;
    size_t cur_inst_id = 0;

    std::vector<BasicBlock*> BBs;
    std::vector<BasicBlock*> rpo_BBs;
    std::vector<BasicBlock*> linear_order_BBs;

    std::unique_ptr<PassManager> pm = nullptr;
    std::unique_ptr<MarkerManager> mm = nullptr;
    Loop* root_loop = nullptr;

    ConstInst* first_const = nullptr;
    ConstInst* last_const = nullptr;

    std::unordered_map<Inst*, LiveInterval*> live_intervals;
    std::unordered_map<Inst*, int32_t> reg_map;
};

} // namespace compiler
