//
// Created by Alexey Golomedov on 01.05.17.
//

#include "hamming.h"
#include <cassert>

#include <iostream>

namespace yasda {

    static const size_t groupSize = 64;

    bool GetBit(const yasda::BinaryString& binaryString, size_t eltIdx) {
        size_t groups = binaryString.size();

        size_t group = eltIdx / groupSize;
        size_t elt = eltIdx % groupSize;

        if (group >= groups) {
            return false;
        }

        return (binaryString[group] & (1 << elt)) > 0;
    }
    void SetBit(yasda::BinaryString& binaryString, size_t eltIdx, bool value) {
        size_t groups = binaryString.size();

        size_t group = eltIdx / groupSize;
        size_t elt = eltIdx % groupSize;

        assert(group < groups);
        assert(elt < groupSize);

        if (value) {
            binaryString[group] = (binaryString[group] | (1 << elt)) ;
        } else {
            binaryString[group] = (binaryString[group] & (~(1 << elt))) ;
        }

    }
}
