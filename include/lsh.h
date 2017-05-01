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

        HammingHash(const HammingHash&) = delete;
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
    private:
        std::vector<HammingHash> hashes_;
        std::vector<size_t> bases_;
    };

}

#endif //LSH_LSH_H
