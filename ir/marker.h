#pragma once

#include "utils.h"
#include <array>
#include <bitset>

namespace compiler
{

constexpr uint32_t MARKERS_SHIFT = 2;
constexpr uint32_t MARKERS_NUM = (1 << MARKERS_SHIFT);
constexpr uint32_t POS_MASK = (MARKERS_NUM - 1);
constexpr uint32_t EMPTY_MARKER = 0;

using marker_t = uint32_t;

class MarkerManager final
{
  public:
    MarkerManager() = default;
    ~MarkerManager() = default;

    marker_t getNewMarker()
    {
        ++current;
        for (uint32_t i = 0; i < MARKERS_NUM; ++i)
            if (!available_mrks[i])
            {
                available_mrks[i] = true;
                return (current << MARKERS_SHIFT) | i;
            }
        UNREACHABLE();
    }

    void deleteMarker(marker_t marker)
    {
        uint32_t pos = marker & POS_MASK;
        available_mrks[pos] = false;
    }

  private:
    uint32_t current = 0;
    std::bitset<MARKERS_NUM> available_mrks;
};

class MarkerSet final
{
  public:
    MarkerSet() = default;
    ~MarkerSet() = default;

    void setMarker(marker_t marker)
    {
        uint32_t pos = marker & POS_MASK;
        uint32_t val = marker >> MARKERS_SHIFT;
        markers[pos] = val;
    }

    void resetMarker(marker_t marker)
    {
        uint32_t pos = marker & POS_MASK;
        markers[pos] = EMPTY_MARKER;
    }

    bool isMarked(marker_t marker) const
    {
        uint32_t pos = marker & POS_MASK;
        uint32_t val = marker >> MARKERS_SHIFT;
        return markers[pos] == val;
    }

  private:
    std::array<marker_t, MARKERS_NUM> markers;
};

} // namespace compiler