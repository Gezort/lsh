#include "integerstring.h"
#include <cassert>

#include <iostream>

namespace yasda {

    template < >
    uint64_t GetCoordinate(const yasda::SparseIntegerString& istr, size_t coordId) {
        auto found = istr.find(coordId);
        if (found != istr.end()) {
            return found->second;
        } else {
            return 0;
        }
    }

    template < >
    void SetCoordinate(yasda::SparseIntegerString& istr, size_t coordId, uint64_t value) {
        if (value > 0) {
            istr[coordId] = value;
        } else {
            istr.erase(coordId);
        }
    }

    template < >
    uint64_t GetManhattanDistance(const SparseIntegerString& left, const SparseIntegerString& right) {
        SparseIntegerString const * shorter = &left;
        SparseIntegerString const * longer = &right;

        if (shorter->size() > longer->size()) {
            std::swap(shorter, longer);
        }

        uint64_t distance = 0;
        for (auto item: *shorter) {
            distance += item.second;
        }
        for (auto item: *longer) {
            distance += item.second;
        }

        for (auto item: *shorter) {
            auto found = longer->find(item.first);
            if (found != longer->end()) {
                if (item.second > found->second) {
                    distance -= 2 * found->second;
                } else {
                    distance -= 2 * item.second;
                }
            }
        }

        return distance;
    }

    template < >
    bool IntegerStringsAreSame(const SparseIntegerString & left, const SparseIntegerString & right) {
        return GetManhattanDistance(left, right) == 0;
    }

    template < >
    void CopyIntegerString(const SparseIntegerString & copyFrom, SparseIntegerString & copyTo) {
        copyTo.clear();
        std::for_each(
            copyFrom.begin(), 
            copyFrom.end(), 
            [&copyTo] (const std::pair<size_t, uint64_t> item) {
                copyTo.insert(item);
            }
        );
    }
}
