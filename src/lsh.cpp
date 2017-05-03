#include "lsh.h"
#include "hamming.h"
#include <algorithm>

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

    HashGroup::HashGroup(const HammingHashFamily& hashFamily, size_t size, std::random_device &rd) {
        std::mt19937 mt(rd());
        std::uniform_int_distribution<size_t> basesDistribution(0, size);

        for (size_t hashIdx=0; hashIdx < size; ++hashIdx) {
            hashes_.push_back(hashFamily(rd));
            bases_.push_back(basesDistribution(mt));
        }
    }

    size_t HashGroup::operator()(const yasda::BinaryString& const bstr) const {
        size_t result = 0;

        for (size_t hashIdx=0; hashIdx < hashes_.size(); ++hashIdx) {
            result += bases_[hashIdx] * static_cast<size_t>(hashes_[hashIdx](bstr));
        }

        return result;
    }

    LSHHammingStore::LSHHammingStore(double allowedDistance, double margin, size_t size, size_t dimensions,
                                     size_t bucketSize, double memoryUtilization, size_t at_most_hashes_in_group) :
        size_(size), bucketSize_(bucketSize),
        hashFamily_(dimensions, allowedDistance, margin)
    {
        size_t storageSize = static_cast<size_t>(memoryUtilization * size_ / bucketSize_);
        storage_.resize(storageSize);

        double rho = log(1.0 / hashFamily_.get_p_1()) / log(1.0 / hashFamily_.get_p_2());
        hashBits_ = static_cast<size_t>(
                ceil((log(bucketSize_) / static_cast<double>(size_)) / log(hashFamily_.get_p_2())));

        hashGroups_ = static_cast<size_t>(
                ceil(pow((size / static_cast<double>(bucketSize_)), rho))
        );

        if (at_most_hashes_in_group < std::numeric_limits<size_t>::max()) {
            if (at_most_hashes_in_group < hashBits_) {
                throw std::invalid_argument("Need to many bits");
            }
        }

        for (size_t hashIdx=0; hashIdx < hashGroups_; ++hashIdx) {
            hashes_.push_back(HashGroup(hashFamily_, hashBits_, rd));
        }
    }

    size_t LSHHammingStore::getHashBitsCount() const {
        return hashBits_;
    }

    size_t LSHHammingStore::getHashGroupsCount() const {
        return hashGroups_;
    }

    bool LSHHammingStore::putInBucket(const yasda::BinaryString *const bstr, size_t bucketId, bool strict=false) {
        Bucket& bucket = storage_[bucketId];

        if (bucket.size() < bucketSize_) {
            bucket.push_back(bstr);
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
            size_t hashValue = hash(&bstr);
            size_t bucketIdx = hashValue % storage_.size();
            putInBucket(bstr, bucketIdx);
        }
    }

    std::vector<yasda::BinaryString*> LSHHammingStore::getKNeighbours(
            const yasda::BinaryString &query, size_t k) const {


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

        while (neighbourIdx < k && neighbourIdx < neighbours.size()) {
            neighbours.push_back(candidates[idxs[neighbourIdx]]);
            ++neighbourIdx;
        }

        return neighbours;
    }
}