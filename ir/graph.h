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

#define GRAPH_BB_NUM 50

class BasicBlock;
class PassManager;
class Rpo;
class DomTree;
class Loop;
class LoopAnalysis;
class LinearOrder;
class LivenessAnalysis;
class LiveInterval;

class Graph
{
  public:
    Graph(std::string name = "") : func_name(name), graph_size(0)
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

    DEFINE_ARRAY_GETTER(func_name, Name, std::string)
    DEFINE_ARRAY_GETTER(BBs, BBs, std::vector<BasicBlock*>&)
    DEFINE_ARRAY_GETTER_SETTER(rpo_BBs, RpoBBs, std::vector<BasicBlock*>&)
    DEFINE_ARRAY_GETTER_SETTER(linear_order_BBs, LinearOrderBBs, std::vector<BasicBlock*>&)
    DEFINE_ARRAY_GETTER_SETTER(live_intervals, LiveIntervals, live_intervals_t&)
    DEFINE_GETTER_SETTER(root_loop, RootLoop, Loop*)

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

    void removeBB(BasicBlock* bb)
    {
        auto it = BBs.erase(std::find(BBs.begin(), BBs.end(), bb));
        ASSERT(it != BBs.end(), "remove not existing bb");
    }

    void removeBB(size_t num)
    {
        auto it = BBs.erase(
            std::find_if(BBs.begin(), BBs.end(), [num](auto bb) { return bb->getId() == num; }));
        ASSERT(it != BBs.end(), "remove not existing bb");
    }

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

    std::vector<BasicBlock*> BBs;
    std::vector<BasicBlock*> rpo_BBs;
    std::vector<BasicBlock*> linear_order_BBs;

    std::unique_ptr<PassManager> pm = nullptr;
    std::unique_ptr<MarkerManager> mm = nullptr;
    Loop* root_loop = nullptr;

    std::unordered_map<Inst*, LiveInterval*> live_intervals;
};

} // namespace compiler
