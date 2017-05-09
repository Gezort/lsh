//
// Created by Alexey Golomedov on 01.05.17.
//

#include "binarystring.h"
#include <cassert>

#include <iostream>

namespace yasda {

    template < >
    bool GetBit(const yasda::SparseBinaryString& binaryString, size_t eltIdx) {
        if (binaryString.find(eltIdx) != binaryString.end()) {
            return  true;
        } else {
            return false;
        }
    }

    template < >
    void SetBit(yasda::SparseBinaryString& binaryString, size_t eltIdx, bool value) {
        if (value) {
            binaryString.insert(eltIdx);
        } else {
            binaryString.erase(eltIdx);
        }
    }

    template < >
    size_t GetHammingDistance(const SparseBinaryString& left, const SparseBinaryString& right) {
        SparseBinaryString const * shorter = &left;
        SparseBinaryString const * longer = &right;

        if (shorter->size() > longer->size()) {
            std::swap(shorter, longer);
        }

        size_t notMatchedBits = longer->size() + shorter->size();

        for (size_t triggeredBit: *shorter) {
            if (longer->find(triggeredBit) != longer->end()) {
                notMatchedBits-= 2;
            }
        }

        return notMatchedBits;
    }

    template < >
    bool BinaryStringsAreSame(const SparseBinaryString & left, const SparseBinaryString & right) {
        return GetHammingDistance(left, right) == 0;
    }

    template < >
    void CopyBinaryString(const SparseBinaryString & copyFrom, SparseBinaryString & copyTo) {
        copyTo.clear();
        std::for_each(copyFrom.begin(), copyFrom.end(), [&copyTo] (const size_t bit) {
            copyTo.insert(bit);
        });
    }
}
