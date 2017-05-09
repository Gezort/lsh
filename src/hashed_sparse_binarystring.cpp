//
// Created by Alexey Golomedov on 01.05.17.
//

#include "binarystring.h"
#include <cassert>

#include <iostream>

namespace yasda {

    template < >
    bool GetBit(const yasda::HashedSparseBinaryString& binaryString, size_t eltIdx) {
        if (binaryString.triggeredBits_.find(binaryString.hash(eltIdx)) != binaryString.triggeredBits_.end()) {
            return  true;
        } else {
            return false;
        }
    }

    template < >
    void SetBit(yasda::HashedSparseBinaryString& binaryString, size_t eltIdx, bool value) {
        if (value) {
            binaryString.triggeredBits_.insert(binaryString.hash(eltIdx));
        } else {
            binaryString.triggeredBits_.erase(binaryString.hash(eltIdx));
        }
    }

    template < >
    size_t GetHammingDistance(const HashedSparseBinaryString& left, const HashedSparseBinaryString& right) {
        HashedSparseBinaryString const * shorter = &left;
        HashedSparseBinaryString const * longer = &right;

        if (shorter->triggeredBits_.size() > longer->triggeredBits_.size()) {
            std::swap(shorter, longer);
        }

        size_t notMatchedBits = longer->triggeredBits_.size() + shorter->triggeredBits_.size();

        for (size_t triggeredBit: shorter->triggeredBits_) {
            if (longer->triggeredBits_.find(triggeredBit) != longer->triggeredBits_.end()) {
                notMatchedBits-= 2;
            }
        }
        return notMatchedBits;
    }

    template < >
    bool BinaryStringsAreSame(const HashedSparseBinaryString & left, const HashedSparseBinaryString & right) {
        return GetHammingDistance(left, right) == 0;
    }

    template < >
    void CopyBinaryString(const HashedSparseBinaryString & copyFrom, HashedSparseBinaryString & copyTo) {
        assert(copyFrom.hashBits_ == copyTo.hashBits_);
        copyTo.triggeredBits_.clear();
        std::for_each(copyFrom.triggeredBits_.begin(), copyFrom.triggeredBits_.end(), [&copyTo] (const size_t bit) {
            copyTo.triggeredBits_.insert(bit);
        });
    }
}
