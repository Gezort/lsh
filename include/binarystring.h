//
// Created by Alexey Golomedov on 01.05.17.
//

#ifndef LSH_HAMMING_H
#define LSH_HAMMING_H

#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace yasda {
    using BinaryString = std::vector<uint64_t>;
    using SparseBinaryString = std::unordered_set<size_t>;

    struct HashedSparseBinaryString {
        const size_t hashBits_;
        const size_t hashValue_;
        std::unordered_set<size_t> triggeredBits_;

        explicit HashedSparseBinaryString(size_t hashBits=18) :
            hashBits_(hashBits),
            hashValue_(1 << hashBits) { }

        inline size_t hash(size_t value) const {
            return value % hashValue_;
        }

        HashedSparseBinaryString(const HashedSparseBinaryString&) = delete;
        HashedSparseBinaryString& operator =(const HashedSparseBinaryString&) = delete;
    };

    class HashedBinaryStringReader {
    public:
        HashedBinaryStringReader(std::string fname, size_t hashBits);
        ~HashedBinaryStringReader();
        const std::vector<HashedSparseBinaryString*>& get() const;
        size_t getId(HashedSparseBinaryString * const bstr) const;

        HashedBinaryStringReader(const HashedBinaryStringReader&) = delete;
        HashedBinaryStringReader& operator=(const HashedBinaryStringReader&) = delete;
    private:
        void read(std::string fname);

        std::vector<HashedSparseBinaryString*> X_;
        std::unordered_map<HashedSparseBinaryString*, size_t> ptrToId_;
        size_t hashBits_;
    };

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
