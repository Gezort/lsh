#ifndef LSH_L1_H
#define LSH_L1_H

#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace yasda {
    using IntegerString = std::vector<uint64_t>;
    using SparseIntegerString = std::unordered_map<size_t, uint64_t>;

    struct HashedSparseIntegerString {
        const size_t hashBits_;
        const size_t hashValue_;
        std::unordered_map<size_t, uint64_t> triggeredCoordinates_;

        explicit HashedSparseIntegerString(size_t hashBits=18) :
            hashBits_(hashBits),
            hashValue_(1 << hashBits) { }

        inline size_t hash(size_t value) const {
            return value % hashValue_;
        }

        HashedSparseIntegerString(const HashedSparseIntegerString&) = delete;
        HashedSparseIntegerString& operator =(const HashedSparseIntegerString&) = delete;
    };

    class HashedIntegerStringReader {
    public:
        HashedIntegerStringReader(std::string fname, size_t hashBits);
        ~HashedIntegerStringReader();
        const std::vector<HashedSparseIntegerString*>& get() const;
        size_t getId(HashedSparseIntegerString * const istr) const;

        HashedIntegerStringReader(const HashedIntegerStringReader&) = delete;
        HashedIntegerStringReader& operator=(const HashedIntegerStringReader&) = delete;
    private:
        void read(std::string fname);

        std::vector<HashedSparseIntegerString*> X_;
        std::unordered_map<HashedSparseIntegerString*, size_t> ptrToId_;
        size_t hashBits_;
    };

    template <typename T> uint64_t GetCoordinate(const T& istr, size_t coordId);
    template <typename T> void SetCoordinate(T& istr, size_t coordId, uint64_t value);
    template <typename T> bool IntegerStringsAreSame(const T& left, const T& right);
    template <typename T> void CopyIntegerString(const T& copyFrom, T& copyTo);
    template <typename T> uint64_t GetManhattanDistance(const T& left, const T& right);

    template < > uint64_t GetCoordinate(const IntegerString& istr, size_t coordId);
    template < > void SetCoordinate(IntegerString& istr, size_t coorId, uint64_t value);
    template < > bool IntegerStringsAreSame(const IntegerString& left, const IntegerString& right);
    template < > void CopyIntegerString(const IntegerString& copyFrom, IntegerString& copyTo);
    template < > size_t GetManhattanDistance(const IntegerString& left, const IntegerString& right);
}

#endif //LSH_L1_H
