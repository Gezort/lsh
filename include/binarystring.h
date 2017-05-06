//
// Created by Alexey Golomedov on 01.05.17.
//

#ifndef LSH_HAMMING_H
#define LSH_HAMMING_H

#include <vector>
#include <unordered_set>

namespace yasda {
    using BinaryString = std::vector<uint64_t>;
    using SparseBinaryString = std::unordered_set<size_t>;

    template <typename T> bool GetBit(const T& bstr, size_t bitId);
    template <typename T> void SetBit(T& bstr, size_t bitId, bool value);
    template <typename T> bool BinaryStringsAreSame(const T& left, const T& right);
    template <typename T> void CopyBinaryString(const T& copyFrom, T& copyTo);
    template <typename T> size_t GetHammingDistance(const T& left, const T& right);

    template < > bool GetBit(const BinaryString& bstr, size_t bitId);
    template < > void SetBit(BinaryString& binaryString, size_t eltIdx, bool value);
    template < > bool BinaryStringsAreSame(const BinaryString& left, const BinaryString& right);
    template < > void CopyBinaryString(const BinaryString& copyFrom, BinaryString& copyTo);
    template < > size_t GetHammingDistance(const BinaryString& left, const BinaryString& right);
}

#endif //LSH_HAMMING_H
