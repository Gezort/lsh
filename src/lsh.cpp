#include "lsh.h"
#include "binarystring.h"
#include <algorithm>
#include <unordered_set>
#include <iostream>

namespace yasda {
    HammingHash::HammingHash(size_t dimensions, size_t projection) :
        dimensions_(dimensions),
        projection_(projection) { }

    HammingHash::HammingHash(const HammingHash& other) :
        dimensions_(other.dimensions_),
        projection_(other.projection_) { }

    HammingHash::HammingHash(size_t dimensions, std::random_device &rd) :
        dimensions_(dimensions)
    {
        std::mt19937 mt(rd());
        std::uniform_int_distribution<size_t> dist(0, dimensions_);
        projection_ = dist(mt);
    }

    bool HammingHash::operator()(const yasda::BinaryString& bstr) const {
        return yasda::GetBit(bstr, projection_);
    }

    HammingHashFamily::HammingHashFamily(size_t dimensions, double allowedDistance, double margin) :
        dimensions_(dimensions),
        allowedDistance_(allowedDistance),
        margin_(margin) { }

    double HammingHashFamily::get_p_1() const {
        return 1.0 - allowedDistance_ / static_cast<double >(dimensions_);
    }

    double HammingHashFamily::get_p_2() const {
        return 1.0 - allowedDistance_ * (1.0 + margin_) / static_cast<double>(dimensions_);
    }

    double HammingHashFamily::get_r_1() const {
        return allowedDistance_;
    }

    double HammingHashFamily::get_r_2() const {
        return allowedDistance_ * (1.0 + margin_);
    }

    HammingHash HammingHashFamily::operator()(size_t projection) const {
        return HammingHash(dimensions_, projection);
    }

    HammingHash HammingHashFamily::operator()(std::random_device &rd) const {
        return HammingHash(dimensions_, rd);
    }

    HashGroup::HashGroup(const HammingHashFamily& hashFamily, size_t size, std::random_device &rd):
        rd_(rd), hashFamily_(hashFamily) {
        std::mt19937 mt(rd_());
        std::uniform_int_distribution<size_t> basesDistribution(0, size);

        for (size_t hashIdx=0; hashIdx < size; ++hashIdx) {
            hashes_.push_back(hashFamily(rd));
            bases_.push_back(basesDistribution(mt));
        }
    }

    HashGroup::HashGroup(const HashGroup & other) :
        rd_(other.rd_), hashFamily_(other.hashFamily_) {
        std::mt19937 mt(rd_());
        std::uniform_int_distribution<size_t> basesDistribution(0, other.hashes_.size());

        for (size_t hashIdx=0; hashIdx < other.hashes_.size(); ++hashIdx) {
            hashes_.push_back(hashFamily_(rd_));
            bases_.push_back(basesDistribution(mt));
        }
    }

    size_t HashGroup::operator()(const yasda::BinaryString& bstr) const {
        size_t result = 0;

        for (size_t hashIdx=0; hashIdx < hashes_.size(); ++hashIdx) {
            result += bases_[hashIdx] * static_cast<size_t>(hashes_[hashIdx](bstr));
        }

        return result;
    }

    LSHHammingStore::LSHHammingStore(double allowedDistance, double margin, size_t size, size_t dimensions,
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
                ceil((log(bucketSize_) / static_cast<double>(size_)) / log(hashFamily_.get_p_2())));

        hashGroups_ = static_cast<size_t>(
                ceil(pow((size / static_cast<double>(bucketSize_)), rho))
        );

        if (atMostHashesInGroup_ < std::numeric_limits<size_t>::max()) {
            if (atMostHashesInGroup_ < hashBits_) {
                throw std::invalid_argument("Need to many bits");
            }
        }

        for (size_t hashIdx=0; hashIdx < hashGroups_; ++hashIdx) {
            hashes_.push_back(HashGroup(hashFamily_, hashBits_, rd));
        }
    }

    LSHHammingStore::LSHHammingStore(const LSHHammingStore &other):
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
                ceil((log(bucketSize_) / static_cast<double>(size_)) / log(hashFamily_.get_p_2())));

        hashGroups_ = static_cast<size_t>(
                ceil(pow((size_ / static_cast<double>(bucketSize_)), rho))
        );

        if (atMostHashesInGroup_ < std::numeric_limits<size_t>::max()) {
            if (atMostHashesInGroup_ < hashBits_) {
                throw std::invalid_argument("Need to many bits");
            }
        }

        for (size_t hashIdx=0; hashIdx < hashGroups_; ++hashIdx) {
            hashes_.push_back(HashGroup(hashFamily_, hashBits_, rd_));
        }
    }

    size_t LSHHammingStore::getHashBitsCount() const {
        return hashBits_;
    }

    size_t LSHHammingStore::getHashGroupsCount() const {
        return hashGroups_;
    }

    bool LSHHammingStore::putInBucket(const yasda::BinaryString *const bstr, size_t bucketId, bool strict) {
        LSHHammingStore::Bucket& bucket = storage_[bucketId];

        if (bucket.size() < bucketSize_) {
            bucket.push_back(const_cast<yasda::BinaryString*>(bstr));
            return true;
        } else {
            if (strict) {
                throw std::invalid_argument("Bucket too large");
            } else {
                return false;
            }
        }
    }

    std::vector<yasda::BinaryString*> LSHHammingStore::getNeighbourCandidates(const BinaryString &query) const {

        size_t candidatesToFind = static_cast<size_t>(ceil(c_ * hashGroups_));

        std::vector<yasda::BinaryString*> candidates;
        size_t hashGroupIdx = 0;

        while (candidates.size() < candidatesToFind and hashGroupIdx < hashGroups_) {
            size_t hashValue = hashes_[hashGroupIdx](query);
            size_t bucketIdx = hashValue % storage_.size();
            for (yasda::BinaryString* bstr: storage_[bucketIdx]) {
                candidates.push_back(bstr);
            }
            ++hashGroupIdx;
        }
        return candidates;
    }

    void LSHHammingStore::put(const yasda::BinaryString *const bstr) {
        for (HashGroup& hash: hashes_) {
            size_t hashValue = hash(*bstr);
            size_t bucketIdx = hashValue % storage_.size();
            putInBucket(bstr, bucketIdx);
        }
    }

    std::vector<yasda::BinaryString*> LSHHammingStore::getKNeighbours(
            const yasda::BinaryString &query, size_t k, std::vector<size_t>* oDistances) const {


        std::vector<BinaryString*> candidates = getNeighbourCandidates(query);
        std::vector<size_t> distances(candidates.size());

        std::transform(candidates.cbegin(), candidates.cend(), distances.begin(),
                       [&query](BinaryString* x) -> size_t {
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

        std::vector<BinaryString*> neighbours;

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

    ApproximateRNN::ApproximateRNN(size_t size, double r, double margin, size_t bucketSize, double memoryUtilization,
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
            dimensions_ = static_cast<size_t>(ceil(r * (1 + margin) / 1 - p_2));
        } else {
            dimensions_ = static_cast<size_t>(ceil(r / (1.0 - similarPointsSameHashesProbability)));
        }

        if (ensureEnoughDimensionsFor < std::numeric_limits<size_t>::max() && ensureEnoughDimensionsFor < dimensions_) {
            throw std::invalid_argument("Not enough dimensions");
        }

        if (lshStoresCount == std::numeric_limits<size_t>::max()) {
            lshStoresCount = repeatsByTolerance(methodTolerance);
        }

        for (size_t storeId=0; storeId < lshStoresCount; ++storeId) {
            stores_.push_back(
                    LSHHammingStore(
                            r,margin, size, dimensions_, rd_, bucketSize, memoryUtilization, atMostHashesInGroup)
            );
        }

    }

    size_t ApproximateRNN::repeatsByTolerance(double tolerance) const {
        return static_cast<size_t>(ceil(1.0 / tolerance));
    }

    void ApproximateRNN::fit(const std::vector<yasda::BinaryString *> data) {
        for (LSHHammingStore& store: stores_) {
            for (auto x: data) {
                store.put(x);
            }
        }
    }

    std::vector<yasda::BinaryString*> ApproximateRNN::getKNearestNeighbours(const yasda::BinaryString &query,
                                                                            size_t k) {
        std::unordered_set<BinaryString *> unique;
        std::vector<BinaryString *> uniqueOrdered;
        std::vector<BinaryString *> neighbours;
        std::vector<size_t> distances;

        for (LSHHammingStore& store: stores_) {
            std::vector<BinaryString *> thisStoreNeighbours = store.getKNeighbours(query, k, &distances);

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
            BinaryString* neighbour = neighbours[idxs[orderedNeighbourId]];
            if (unique.count(neighbour) == 0) {
                unique.insert(neighbour);
                uniqueOrdered.push_back(neighbour);
            }
            ++orderedNeighbourId;
        }

        return uniqueOrdered;
    }

    size_t ApproximateRNN::getHashGroupsCount() const {
        return stores_[0].getHashGroupsCount();
    }

    size_t ApproximateRNN::getHashBitsCount() const {
        return stores_[0].getHashBitsCount();
    }

    size_t ApproximateRNN::getLSHStoresCount() const {
        return stores_.size();
    }

    size_t ApproximateRNN::getMaxAllowedDimensions() const {
        return dimensions_;
    }
}
