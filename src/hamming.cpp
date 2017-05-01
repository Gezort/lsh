//
// Created by Alexey Golomedov on 01.05.17.
//

#include "hamming.h"
#include <cassert>

namespace yasda {

    static const size_t groupSize = 64;

    bool GetBit(const BinaryString& binaryString, size_t eltIdx) {
        size_t groups = binaryString.size();
        size_t numElts = groups * groupSize;

        size_t group = eltIdx / groupSize;
        size_t elt = eltIdx & groupSize;

        std::assert
    }
    void SetBit(BinaryString& binaryString, bool value) {

    }
}
