//
// Created by Alexey Golomedov on 01.05.17.
//

#ifndef LSH_LSH_H
#define LSH_LSH_H

#include <cstddef>
#include <random>
#include <hamming.h>

namespace yasda {

    class HammingHash {
    public:
        HammingHash(size_t dimensions, size_t projection);
        HammingHash(size_t dimensions, std::random_device &rd);
        bool operator()(const yasda::BinaryString& bstr) const;

        HammingHash(const HammingHash&);

        HammingHash& operator=(const HammingHash&) = delete;

    private:
        size_t dimensions_;
        size_t projection_;
    };

    class HammingHashFamily {
    public:
        HammingHashFamily(size_t dimensions, double allowedDistance, double margin);

        double get_p_1() const;
        double get_p_2() const;
        double get_r_1() const;
        double get_r_2() const;

        HammingHash operator ()(size_t projection) const;
        HammingHash operator ()(std::random_device &rd) const;

        HammingHashFamily(const HammingHashFamily&) = delete;
        HammingHashFamily& operator=(const HammingHashFamily&) = delete;

    private:
        size_t dimensions_;
        double allowedDistance_;
        double margin_;
    };

    class HashGroup {
    public:
        HashGroup(const HammingHashFamily& hashFamily, size_t size, std::random_device& rd);
        size_t operator()(const yasda::BinaryString& bstr) const;

        HashGroup(const HashGroup&);
        HashGroup& operator=(const HashGroup&) = delete;
    private:
        std::vector<HammingHash> hashes_;
        std::vector<size_t> bases_;
        std::random_device& rd_;
        const HammingHashFamily& hashFamily_;
    };

    class LSHHammingStore {
    public:
        LSHHammingStore(double allowedDistance, double margin, size_t size, size_t dimensions,
                        std::random_device& rd,
                        size_t bucketSize=128,
                        double memoryUtilization=2,
                        size_t at_most_hashes_in_group=std::numeric_limits<size_t>::max());

        size_t getHashBitsCount() const;
        size_t getHashGroupsCount() const;
        void put(const yasda::BinaryString* const bstr);
        std::vector<yasda::BinaryString*> getKNeighbours(
                const yasda::BinaryString& query, size_t k, std::vector<size_t>* distances= nullptr) const;

        LSHHammingStore(const LSHHammingStore&);
        LSHHammingStore& operator=(const LSHHammingStore&) = delete;

    private:
        using Bucket = std::vector<yasda::BinaryString *>;
        // see P2 near Th 1 in article referenced above for what is rho and c
        const double c_ = 4;

        bool putInBucket(const yasda::BinaryString* const bstr, size_t bucketId, bool strict= false);
        std::vector<yasda::BinaryString*> getNeighbourCandidates(const BinaryString& query) const;

        size_t size_;
        size_t bucketSize_;

        std::vector<Bucket> storage_;
        HammingHashFamily hashFamily_;
        size_t hashBits_;
        size_t hashGroups_;
        std::vector<HashGroup> hashes_;
        std::random_device& rd_;

        double allowedDistance_;
        double margin_;
        size_t dimensions_;
        double memoryUtilization_;
        size_t atMostHashesInGroup_;
    };

    class ApproximateRNN {
    public:
        ApproximateRNN(
                size_t size, double r, double margin, size_t bucketSize=128, double memoryUtilization=2,
                double methodTolerance=-1, size_t lshStoresCount=std::numeric_limits<size_t>::max(),
                double similarPointsSameHashesProbability=-1, size_t hashBits=std::numeric_limits<size_t>::max(),
                size_t ensureEnoughDimensionsFor=std::numeric_limits<size_t>::max(),
                size_t atMostHashesInGroup=std::numeric_limits<size_t>::max()
        );
        std::vector<yasda::BinaryString*> getKNearestNeighbours(const yasda::BinaryString& query, size_t k);
        void fit(const std::vector<yasda::BinaryString*> data);
        size_t getMaxAllowedDimensions() const;
        size_t getLSHStoresCount() const;
        size_t getHashBitsCount() const;
        size_t getHashGroupsCount() const;

    private:
        size_t repeatsByTolerance(double tolerance) const;

        size_t dimensions_;
        std::random_device rd_;
        std::vector<LSHHammingStore> stores_;
    };

}

#endif //LSH_LSH_H
