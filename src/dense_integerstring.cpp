#include "integerstring.h"
#include <cassert>

#include <iostream>

namespace yasda {

    template < >
    uint64_t GetCoordinate(const yasda::IntegerString& istr, size_t coordId) {
        size_t groups = istr.size();
        if (coordId >= groups) {
            return 0;
        }
        return istr[coordId];
    }

    template < >
    void SetBit(yasda::IntegerString& istr, size_t coordId, uint64_t value) {
        size_t groups = istr.size();

        assert(coordId < groups);

        istr[coordId] = value;
    }

    template < >
    uint64_t GetManhattanDistance(const IntegerString& left, const IntegerString& right) {
        assert(left.size() == right.size());

        uint64_t distance = 0;
        for (size_t coordId = 0; coordId < left.size(); ++coordId) {
            if (left[coordId] > right[coordId]) {
                distance += left[coordId] - right[coordId];
            } else {
                distance += right[coordId] - left[coordId];
            }
        }
        return distance;
    }

    template < >
    bool IntegerStringsAreSame(const IntegerString& left, const IntegerString& right) {
        if (left.size() != right.size()) {
            return false;
        }

        for (size_t coordId = 0; coordId < left.size(); ++coordId) {
            if (left[coordId] != right[coordId]) {
                return false;
            }
        }
        
        return true;
    }

    template < >
    void CopyIntegerString(const IntegerString& copyFrom, IntegerString& copyTo) {
        copyTo.resize(copyFrom.size());

        std::copy(copyFrom.begin(), copyFrom.end(), copyTo.begin());
    }
}
