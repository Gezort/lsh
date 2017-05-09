//
// Created by Alexey Golomedov on 08.05.17.
//
#include "binarystring.h"
#include <boost/lexical_cast.hpp>
#include <fstream>

namespace yasda {

    HashedBinaryStringReader::HashedBinaryStringReader(std::string fname, size_t hashBits) :
        hashBits_(hashBits) {
        read(fname);
    }

    HashedBinaryStringReader::~HashedBinaryStringReader() {
        for (auto x: X_) {
            delete x;
        }
    }

    const std::vector<HashedSparseBinaryString*>& HashedBinaryStringReader::get() const {
        return X_;
    }

    void HashedBinaryStringReader::read(std::string fname) {
        std::ifstream ifile(fname);

        std::string line;
        while (std::getline(ifile, line)) {
            std::istringstream iss(line);
            size_t rowId;
            iss >> rowId;

            HashedSparseBinaryString* bstr = new HashedSparseBinaryString(hashBits_);
            X_.push_back(bstr);
            ptrToId_[bstr] = rowId;

            std::for_each(
                    std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>(),
                    [bstr](std::string s) {
                        size_t bitId = boost::lexical_cast<size_t>(s);
                        yasda::SetBit(*bstr, bitId, true);
                    }
            );
        }
    }

    size_t HashedBinaryStringReader::getId(HashedSparseBinaryString * const bstr) const {
        auto found = ptrToId_.find(bstr);
        if (found == ptrToId_.end()) {
            return std::numeric_limits<size_t>::max();
        } else {
            return found->second;
        }
    }

} // namespace yasda
