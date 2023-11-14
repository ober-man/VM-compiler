#pragma once

#include <bitset>

namespace compiler
{

#define MARKERS_SHIFT 2
#define MARKERS_NUM (1 << MARKERS_SHIFT)
#define POS_MASK (MARKERS_NUM - 1)

#define UNREACHABLE() __builtin_unreachable()

using marker_t = uint32_t;

class MarkerManager
{
public:
	MarkerManager() = default;
	~MarkerManager() = default;

	marker_t getNewMarker()
	{
		for (uint32_t i = 0; i < MARKERS_NUM; ++i)
			if (!available_mrks[i])
			{
				available_mrks[i] = true;
				return i;
			}
		UNREACHABLE();
	}

	void deleteMarker(marker_t marker)
	{
		uint32_t pos = marker & POS_MASK;
		available_mrks[pos] = false;
	}

private:
	std::bitset<MARKERS_NUM> available_mrks;
};

class MarkerSet
{
public:
	MarkerSet() = default;
	~MarkerSet() = default;

	void setMarker(marker_t marker)
	{
		uint32_t pos = marker & POS_MASK;
		markers[pos] = true;
	}

	void resetMarker(marker_t marker)
	{
		uint32_t pos = marker & POS_MASK;
		markers[pos] = false;
	}

	bool isMarked(marker_t marker)
	{
		return markers[marker & POS_MASK];
	}	

private:
	std::bitset<MARKERS_NUM> markers;
};

} // namespace compiler