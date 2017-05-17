#include "integerstring.h"
#include <cassert>

#include <iostream>

namespace yasda {

    template < >
    uint64_t GetCoordinate(const yasda::HashedSparseIntegerString& istr, size_t coordId) {
        auto found = istr.triggeredCoordinates_.find(istr.hash(coordId));
        if (found != istr.triggeredCoordinates_.end()) {
            return found->second;
        } else {
            return 0;
        }
    }

    template < >
    void SetBit(yasda::HashedSparseIntegerString& istr, size_t coordId, uint64_t value) {
        if (value > 0) {
            istr.triggeredBits_[istr.hash(coordId)] = value;
        } else {
            istr.triggeredBits_.erase(istr.hash(coordId));
        }
    }

    template < >
    uint64_t GetManhattanDistance(const HashedSparseIntegerString& left, const HashedSparseIntegerString& right) {
        SparseIntegerString const * shorter = &left;
        SparseIntegerString const * longer = &right;

        if (shorter->size() > longer->size()) {
            std::swap(shorter, longer);
        }

        uint64_t distance = 0;
        for (auto item: *shorter) {
            distance += item->second;
        }
        for (auto item: *longer) {
            distance += item->second;
        }

        for (auto item: shorter->triggeredCoordinates_) {
            auto found = longer->triggeredCoordinates_.find(item->first);
            if (found != longer->triggeredCoordinates_.end()) {
                if (item->second > found->second) {
                    distance -= 2 * found->second;
                } else {
                    distance -= 2 * item->second;
                }
            }
        }

        return distance;
    }

    template < >
    bool IntegerStringsAreSame(const HashedSparseIntegerString & left, const HashedSparseIntegerString & right) {
        return GetManhattanDistance(left, right) == 0;
    }

    template < >
    void CopyIntegerString(const HashedSparseIntegerString & copyFrom, HashedSparseIntegerString & copyTo) {
        assert(copyFrom.hashBits_ == copyTo.hashBits_);
        copyTo.triggeredCoordinates_.clear();
        std::for_each(
            copyFrom.triggeredCoordinates_.begin(),
            copyFrom.triggeredCoordinates_.end(), 
            [&copyTo] (const auto item) {
                copyTo.triggeredCoordinates_.insert(item);
            }
        );
    }
}
