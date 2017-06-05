//
// Created by Alexey Golomedov on 01.05.17.
//

#ifndef LSH_LSH_H
#define LSH_LSH_H

#include <cstddef>
#include <random>
#include <binarystring.h>
#include <integerstring.h>
#include <iostream>

namespace yasda {

    template <typename T>
    class HammingHash {
    public:
        HammingHash(size_t dimensions, size_t projection);
        HammingHash(size_t dimensions, std::random_device &rd);
        bool operator()(const T& bstr) const;

        HammingHash(const HammingHash<T>&);

        HammingHash& operator=(const HammingHash<T>&) = delete;

    private:
        size_t dimensions_;
        size_t projection_;
    };

    template <typename T>
    class ManhattanHash {
    public:
        ManhattanHash(size_t dimensions, uint64_t maxCoordinate, size_t projection);
        ManhattanHash(size_t dimensions, uint64_t maxCoordinate, std::random_device &rd);
        bool operator()(const T& istr) const;

        ManhattanHash(const ManhattanHash<T>&);

        ManhattanHash& operator=(const ManhattanHash<T>&) = delete;

    private:
        size_t dimensions_;
        uint64_t maxCoordinate_;
        size_t projection_;
    };

    template <typename T>
    class HammingHashFamily {
    public:
        HammingHashFamily(size_t dimensions, double allowedDistance, double margin);

        double get_p_1() const;
        double get_p_2() const;
        double get_r_1() const;
        double get_r_2() const;

        HammingHash<T> operator ()(size_t projection) const;
        HammingHash<T> operator ()(std::random_device &rd) const;

        HammingHashFamily(const HammingHashFamily&) = delete;
        HammingHashFamily& operator=(const HammingHashFamily&) = delete;

    private:
        size_t dimensions_;
        double allowedDistance_;
        double margin_;
    };

    template <typename T>
    class ManhattanHashFamily {
    public:
        ManhattanHashFamily(size_t dimensions, uint64_t maxCoordinate, 
                                double allowedDistance, double margin);

        double get_p_1() const;
        double get_p_2() const;
        double get_r_1() const;
        double get_r_2() const;

        ManhattanHash<T> operator()(size_t projection) const;
        ManhattanHash<T> operator()(std::random_device &rd) const;

        ManhattanHashFamily(const ManhattanHashFamily&) = delete;
        ManhattanHashFamily& operator=(const ManhattanHashFamily&) = delete;

    private:
        size_t dimensions_;
        uint64_t maxCoordinate_;
        double allowedDistance_;
        double margin_;
    };


    template <typename T, template <typename> class F, template <typename> class H>
    class HashGroup {
    public:
        HashGroup(const F<T>& hashFamily, size_t size, std::random_device& rd);
        size_t operator()(const T& bstr) const;

        HashGroup(const HashGroup<T, F, H>&);
        HashGroup& operator=(const HashGroup<T, F, H>&) = delete;
    private:
        std::vector<H<T>> hashes_;
        std::vector<size_t> bases_;
        std::random_device& rd_;
        const F<T>& hashFamily_;
    };

    template <typename T>
    class LSHHammingStore {
    public:
        LSHHammingStore(double allowedDistance, double margin, size_t size, size_t dimensions,
                        std::random_device& rd,
                        size_t bucketSize=128,
                        double memoryUtilization=2,
                        size_t at_most_hashes_in_group=std::numeric_limits<size_t>::max());

        size_t getHashBitsCount() const;
        size_t getHashGroupsCount() const;
        size_t getBucketsCount() const;
        void put(const T* const bstr);
        std::vector<T*> getKNeighbours(
                const T& query, size_t k, std::vector<size_t>* distances= nullptr) const;

        LSHHammingStore(const LSHHammingStore<T>&);
        LSHHammingStore& operator=(const LSHHammingStore<T>&) = delete;

    private:
        using Bucket = std::vector<T*>;
        // see P2 near Th 1 in article referenced above for what is rho and c
        const double c_ = 4;

        bool putInBucket(const T* const bstr, size_t bucketId, bool strict= false);
        std::vector<T*> getNeighbourCandidates(const T& query) const;

        size_t size_;
        size_t bucketSize_;

        std::vector<Bucket> storage_;
        HammingHashFamily<T> hashFamily_;
        size_t hashBits_;
        size_t hashGroups_;
        std::vector<HashGroup<T, HammingHashFamily, HammingHash>> hashes_;
        std::random_device& rd_;

        double allowedDistance_;
        double margin_;
        size_t dimensions_;
        double memoryUtilization_;
        size_t atMostHashesInGroup_;
    };

    template <typename T>
    class ApproximateRNN {
    public:
        ApproximateRNN(
                size_t size, double r, double margin, size_t bucketSize=128, double memoryUtilization=2,
                double methodTolerance=-1, size_t lshStoresCount=std::numeric_limits<size_t>::max(),
                double similarPointsSameHashesProbability=-1, size_t hashBits=std::numeric_limits<size_t>::max(),
                size_t ensureEnoughDimensionsFor=std::numeric_limits<size_t>::max(),
                size_t atMostHashesInGroup=std::numeric_limits<size_t>::max()
        );
        std::vector<T*> getKNearestNeighbours(const T& query, size_t k);
        void fit(const std::vector<T*>& data, bool verbose=false);
        size_t getMaxAllowedDimensions() const;
        size_t getLSHStoresCount() const;
        size_t getBucketsCount() const;
        size_t getHashBitsCount() const;
        size_t getHashGroupsCount() const;

    private:
        size_t repeatsByTolerance(double tolerance) const;

        size_t dimensions_;
        std::random_device rd_;
        std::vector<LSHHammingStore<T>> stores_;
    };

    template <typename T>
    HammingHash<T>::HammingHash(size_t dimensions, size_t projection) :
            dimensions_(dimensions),
            projection_(projection) { }

    template <typename T>
    HammingHash<T>::HammingHash(const HammingHash<T>& other) :
            dimensions_(other.dimensions_),
            projection_(other.projection_) { }

    template <typename T>
    HammingHash<T>::HammingHash(size_t dimensions, std::random_device &rd) :
            dimensions_(dimensions)
    {
        std::mt19937 mt(rd());
        std::uniform_int_distribution<size_t> dist(0, dimensions_);
        projection_ = dist(mt);
    }

    template <typename T>
    bool HammingHash<T>::operator()(const T& bstr) const {
        return yasda::GetBit(bstr, projection_);
    }

    template <typename T>
    HammingHashFamily<T>::HammingHashFamily(size_t dimensions, double allowedDistance, double margin) :
            dimensions_(dimensions),
            allowedDistance_(allowedDistance),
            margin_(margin) { }

    template <typename T>
    double HammingHashFamily<T>::get_p_1() const {
        return 1.0 - allowedDistance_ / static_cast<double >(dimensions_);
    }

    template <typename T>
    double HammingHashFamily<T>::get_p_2() const {
        return 1.0 - allowedDistance_ * (1.0 + margin_) / static_cast<double>(dimensions_);
    }

    template <typename T>
    double HammingHashFamily<T>::get_r_1() const {
        return allowedDistance_;
    }

    template <typename T>
    double HammingHashFamily<T>::get_r_2() const {
        return allowedDistance_ * (1.0 + margin_);
    }

    template <typename T>
    HammingHash<T> HammingHashFamily<T>::operator()(size_t projection) const {
        return HammingHash<T>(dimensions_, projection);
    }

    template <typename T>
    HammingHash<T> HammingHashFamily<T>::operator()(std::random_device &rd) const {
        return HammingHash<T>(dimensions_, rd);
    }


    template <typename T, template <typename> class F, template <typename> class H>
    HashGroup<T, F, H>::HashGroup(const F<T>& hashFamily, size_t size, std::random_device &rd):
            rd_(rd), hashFamily_(hashFamily) {
        std::mt19937 mt(rd_());
        std::uniform_int_distribution<size_t> basesDistribution(0, size);

        for (size_t hashIdx=0; hashIdx < size; ++hashIdx) {
            hashes_.push_back(hashFamily(rd));
            bases_.push_back(basesDistribution(mt));
        }
    }

    template <typename T, template <typename> class F, template <typename> class H>
    HashGroup<T, F, H>::HashGroup(const HashGroup<T, F, H>& other) :
            rd_(other.rd_), hashFamily_(other.hashFamily_) {
        std::mt19937 mt(rd_());
        std::uniform_int_distribution<size_t> basesDistribution(0, other.hashes_.size());

        for (size_t hashIdx=0; hashIdx < other.hashes_.size(); ++hashIdx) {
            hashes_.push_back(hashFamily_(rd_));
            bases_.push_back(basesDistribution(mt));
        }
    }

    template <typename T, template <typename> class F, template <typename> class H>
    size_t HashGroup<T, F, H>::operator()(const T& bstr) const {
        size_t result = 0;

        for (size_t hashIdx=0; hashIdx < hashes_.size(); ++hashIdx) {
            result += bases_[hashIdx] * static_cast<size_t>(hashes_[hashIdx](bstr));
        }

        return result;
    }

    template <typename T>
    LSHHammingStore<T>::LSHHammingStore(double allowedDistance, double margin, size_t size, size_t dimensions,
                                     std::random_device& rd,
                                     size_t bucketSize, double memoryUtilization, size_t atMostHashesInGroup) :
            size_(size), bucketSize_(bucketSize),
            dimensions_(dimensions), allowedDistance_(allowedDistance),
            margin_(margin), memoryUtilization_(memoryUtilization),
            atMostHashesInGroup_(atMostHashesInGroup),
            hashFamily_(dimensions, allowedDistance, margin),
            rd_(rd)
    {
        size_t storageSize = static_cast<size_t>(memoryUtilization * size_ / bucketSize_);
        storage_.resize(storageSize);

        double rho = log(1.0 / hashFamily_.get_p_1()) / log(1.0 / hashFamily_.get_p_2());

        hashBits_ = static_cast<size_t>(
                ceil(
                        log(static_cast<double>(bucketSize_) / static_cast<double>(size_)) / log(hashFamily_.get_p_2())
                )
        );

        hashGroups_ = static_cast<size_t>(
                ceil(pow((size / static_cast<double>(bucketSize_)), rho))
        );

        if (atMostHashesInGroup_ < std::numeric_limits<size_t>::max()) {
            if (atMostHashesInGroup_ < hashBits_) {
                throw std::invalid_argument("Need to many bits");
            }
        }

        for (size_t hashIdx=0; hashIdx < hashGroups_; ++hashIdx) {
            hashes_.push_back(HashGroup<T, HammingHashFamily, HammingHash>(hashFamily_, hashBits_, rd));
        }
    }

    template <typename T>
    LSHHammingStore<T>::LSHHammingStore(const LSHHammingStore<T> &other):
            size_(other.size_), bucketSize_(other.bucketSize_),
            dimensions_(other.dimensions_), allowedDistance_(other.allowedDistance_),
            margin_(other.margin_), memoryUtilization_(other.memoryUtilization_),
            atMostHashesInGroup_(other.atMostHashesInGroup_),
            hashFamily_(other.dimensions_, other.allowedDistance_, other.margin_),
            rd_(other.rd_) {

        size_t storageSize = static_cast<size_t>(memoryUtilization_ * size_ / bucketSize_);
        storage_.resize(storageSize);

        double rho = log(1.0 / hashFamily_.get_p_1()) / log(1.0 / hashFamily_.get_p_2());
        hashBits_ = static_cast<size_t>(
                ceil(log(bucketSize_ / static_cast<double>(size_)) / log(hashFamily_.get_p_2())));

        hashGroups_ = static_cast<size_t>(
                ceil(pow((size_ / static_cast<double>(bucketSize_)), rho))
        );

        if (atMostHashesInGroup_ < std::numeric_limits<size_t>::max()) {
            if (atMostHashesInGroup_ < hashBits_) {
                throw std::invalid_argument("Need to many bits");
            }
        }

        for (size_t hashIdx=0; hashIdx < hashGroups_; ++hashIdx) {
            hashes_.push_back(HashGroup<T, HammingHashFamily, HammingHash>(hashFamily_, hashBits_, rd_));
        }
    }

    template <typename T>
    size_t LSHHammingStore<T>::getHashBitsCount() const {
        return hashBits_;
    }

    template <typename T>
    size_t LSHHammingStore<T>::getHashGroupsCount() const {
        return hashGroups_;
    }

    template <typename T>
    bool LSHHammingStore<T>::putInBucket(const T *const bstr, size_t bucketId, bool strict) {
        Bucket& bucket = storage_[bucketId];

        if (bucket.size() < bucketSize_) {
            bucket.push_back(const_cast<T*>(bstr));
            return true;
        } else {
            if (strict) {
                throw std::invalid_argument("Bucket too large");
            } else {
                return false;
            }
        }
    }

    template <typename T>
    std::vector<T*> LSHHammingStore<T>::getNeighbourCandidates(const T &query) const {

        size_t candidatesToFind = static_cast<size_t>(ceil(c_ * hashGroups_));

        std::vector<T*> candidates;
        size_t hashGroupIdx = 0;

        while (candidates.size() < candidatesToFind and hashGroupIdx < hashGroups_) {
            size_t hashValue = hashes_[hashGroupIdx](query);
            size_t bucketIdx = hashValue % storage_.size();
            for (T* bstr: storage_[bucketIdx]) {
                candidates.push_back(bstr);
            }
            ++hashGroupIdx;
        }
        return candidates;
    }

    template <typename T>
    void LSHHammingStore<T>::put(const T *const bstr) {
        for (HashGroup<T, HammingHashFamily, HammingHash>& hash: hashes_) {
            size_t hashValue = hash(*bstr);
            size_t bucketIdx = hashValue % storage_.size();
            putInBucket(bstr, bucketIdx);
        }
    }

    template <typename T>
    std::vector<T*> LSHHammingStore<T>::getKNeighbours(
            const T &query, size_t k, std::vector<size_t>* oDistances) const {


        std::vector<T*> candidates = getNeighbourCandidates(query);
        std::vector<size_t> distances(candidates.size());

        std::transform(candidates.cbegin(), candidates.cend(), distances.begin(),
                       [&query](T* x) -> size_t {
                           return yasda::GetHammingDistance(*x, query);
                       }
        );

        std::vector<size_t> idxs;
        for (size_t idx=0; idx < candidates.size(); ++idx) {
            idxs.push_back(idx);
        }

        std::sort(idxs.begin(), idxs.end(), [&distances](size_t l, size_t r) -> bool {
            return distances[l] < distances[r];
        });

        std::vector<T*> neighbours;

        size_t neighbourIdx = 0;

        while (neighbourIdx < k && neighbourIdx < candidates.size()) {
            neighbours.push_back(candidates[idxs[neighbourIdx]]);
            if (oDistances != nullptr) {
                oDistances->push_back(distances[idxs[neighbourIdx]]);
            }
            ++neighbourIdx;
        }

        return neighbours;
    }

    template <typename T>
    size_t LSHHammingStore<T>::getBucketsCount() const {
        return storage_.size();
    }


    template <typename T>
    ApproximateRNN<T>::ApproximateRNN(size_t size, double r, double margin, size_t bucketSize, double memoryUtilization,
                                   double methodTolerance, size_t lshStoresCount,
                                   double similarPointsSameHashesProbability, size_t hashBits,
                                   size_t ensureEnoughDimensionsFor, size_t atMostHashesInGroup) {

        if ((similarPointsSameHashesProbability < 0 && hashBits == std::numeric_limits<size_t>::max()) ||
            (similarPointsSameHashesProbability > 0 && hashBits < std::numeric_limits<size_t>::max())) {
            throw std::invalid_argument("Specify either similarPointsSameHashesProbability or hashBits");
        }
        if ((methodTolerance < 0 && lshStoresCount == std::numeric_limits<size_t>::max()) ||
            (methodTolerance > 0 && lshStoresCount < std::numeric_limits<size_t>::max())) {
            throw std::invalid_argument("Specify either method tolerance or lshStores");
        }

        if (hashBits < std::numeric_limits<size_t>::max()) {
            double p_2 = pow((bucketSize / static_cast<double>(size)), 1.0 / hashBits);
            dimensions_ = static_cast<size_t>(ceil(r * (1 + margin) / (1 - p_2)));
        } else {
            dimensions_ = static_cast<size_t>(ceil(r / (1.0 - similarPointsSameHashesProbability)));
        }

        if (ensureEnoughDimensionsFor < std::numeric_limits<size_t>::max() && ensureEnoughDimensionsFor > dimensions_) {
            std::cerr << "Not enough dimensions asked: " << ensureEnoughDimensionsFor << " has only " << dimensions_ << std::endl;
            throw std::invalid_argument("Not enough dimensions");
        }

        if (lshStoresCount == std::numeric_limits<size_t>::max()) {
            lshStoresCount = repeatsByTolerance(methodTolerance);
        }

        for (size_t storeId=0; storeId < lshStoresCount; ++storeId) {
            stores_.push_back(
                    LSHHammingStore<T>(
                            r,margin, size, dimensions_, rd_, bucketSize, memoryUtilization, atMostHashesInGroup)
            );
        }

    }

    template <typename T>
    size_t ApproximateRNN<T>::repeatsByTolerance(double tolerance) const {
        return static_cast<size_t>(ceil(1.0 / tolerance));
    }

    template <typename T>
    void ApproximateRNN<T>::fit(const std::vector<T*>& data, bool verbose) {
        size_t storeId=0;
        for (LSHHammingStore<T>& store: stores_) {
            if (verbose) {
                std::cout << "Saving samples to store " << storeId << std::endl;
            }
            size_t sampleId=1;
            for (auto x: data) {
                store.put(x);
                if (verbose && sampleId % 10000 == 0) {
                    std::cout << "\t..." << sampleId << " done" << std::endl;
                }
                ++sampleId;
            }
            if (verbose) {
                std::cout << "Saved samples to store " << storeId << std::endl;
            }
            ++storeId;
        }
    }

    template <typename T>
    std::vector<T*> ApproximateRNN<T>::getKNearestNeighbours(const T &query,
                                                                            size_t k) {
        std::unordered_set<T*> unique;
        std::vector<T*> uniqueOrdered;
        std::vector<T*> neighbours;
        std::vector<size_t> distances;

        for (LSHHammingStore<T>& store: stores_) {
            std::vector<T*> thisStoreNeighbours = store.getKNeighbours(query, k, &distances);

            for (auto neighbour: thisStoreNeighbours) {
                neighbours.push_back(neighbour);
            }
        }

        std::vector<size_t> idxs;

        for (size_t idx = 0; idx < neighbours.size(); ++idx){
            idxs.push_back(idx);
        }

        std::sort(idxs.begin(), idxs.end(), [&distances](size_t l, size_t r) -> bool {
            return distances[l] < distances[r];
        });

        size_t orderedNeighbourId = 0;

        while (unique.size() < k && orderedNeighbourId < neighbours.size()) {
            T* neighbour = neighbours[idxs[orderedNeighbourId]];
            if (unique.count(neighbour) == 0) {
                unique.insert(neighbour);
                uniqueOrdered.push_back(neighbour);
            }
            ++orderedNeighbourId;
        }

        return uniqueOrdered;
    }

    template <typename T>
    size_t ApproximateRNN<T>::getHashGroupsCount() const {
        return stores_[0].getHashGroupsCount();
    }

    template <typename T>
    size_t ApproximateRNN<T>::getHashBitsCount() const {
        return stores_[0].getHashBitsCount();
    }

    template <typename T>
    size_t ApproximateRNN<T>::getLSHStoresCount() const {
        return stores_.size();
    }

    template <typename T>
    size_t ApproximateRNN<T>::getMaxAllowedDimensions() const {
        return dimensions_;
    }

    template <typename T>
    size_t ApproximateRNN<T>::getBucketsCount() const {
        return stores_[0].getBucketsCount();
    }
} // namespace yasda

#endif //LSH_LSH_H
