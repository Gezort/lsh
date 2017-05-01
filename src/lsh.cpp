#include "lsh.h"

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

    bool HammingHash::operator()(const yasda::BinaryString &bstr) const {
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

    size_t HashGroup::operator()(const yasda::BinaryString& bstr) const {
        size_t result = 0;

        for (size_t hashIdx=0; hashIdx < hashes_.size(); ++hashIdx) {
            result += bases_[hashIdx] * static_cast<size_t>(hashes_[hashIdx](bstr));
        }

        return result;
    }
}