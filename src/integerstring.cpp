#include "integerstring.h"
#include <boost/lexical_cast.hpp>
#include <fstream>

namespace yasda {

    HashedIntegerStringReader::HashedIntegerStringReader(std::string fname, size_t hashBits) :
        hashBits_(hashBits) {
        read(fname);
    }

    HashedIntegerStringReader::~HashedIntegerStringReader() {
        for (auto x: X_) {
            delete x;
        }
    }

    const std::vector<HashedSparseIntegerString*>& HashedIntegerStringReader::get() const {
        return X_;
    }

    void HashedIntegerStringReader::read(std::string fname) {
        std::ifstream ifile(fname);

        std::string line;
        while (std::getline(ifile, line)) {
            std::istringstream iss(line);
            size_t rowId;
            iss >> rowId;

            HashedSparseIntegerString* istr = new HashedSparseIntegerString(hashBits_);
            X_.push_back(istr);
            ptrToId_[istr] = rowId;

            std::for_each(
                    std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>(),
                    [istr](std::string s) {
                        std::istringstream in(s);
                        std::string key;
                        std::string value;
                        std::getline(in, key, ':');
                        std::getline(in, value);
                        size_t coordId = boost::lexical_cast<size_t>(key);
                        size_t coordValue = boost::lexical_cast<uint64_t>(value);
                        yasda::SetCoordinate(*istr, coordId, coordValue);
                    }
            );
        }
    }

    size_t HashedIntegerStringReader::getId(HashedSparseIntegerString * const istr) const {
        auto found = ptrToId_.find(istr);
        if (found == ptrToId_.end()) {
            return std::numeric_limits<size_t>::max();
        } else {
            return found->second;
        }
    }

} // namespace yasda
