//
// Created by Alexey Golomedov on 08.05.17.
//

#include <boost/program_options.hpp>
#include <binarystring.h>
#include <lsh.h>
#include <iostream>

namespace po = boost::program_options;

struct ProgramOptions {
    size_t bits;
    size_t dimensions;
    size_t k;
    double r;
    double margin;
    size_t bucketSize;
    double memoryUtilization;
    double methodTolerance;
    size_t hashBits;
    bool verbose;
};

void printDebugInfo(
        const yasda::HashedBinaryStringReader& trainReader, const yasda::HashedBinaryStringReader& testReaader,
        const yasda::ApproximateRNN<yasda::HashedSparseBinaryString>& rnn) {
    std::cout << "Train dataset consist of " << trainReader.get().size() << " samples" << std::endl;
    std::cout << "Test dataset consist of " << testReaader.get().size() << " samples" << std::endl;

    std::cout << "LSH parameters are:              " << std::endl;
    std::cout << "\t Count of LSH stores:          " << rnn.getLSHStoresCount() << std::endl;
    std::cout << "\t Maximal allowed dimensions:   " << rnn.getMaxAllowedDimensions() << std::endl;
    std::cout << "\t Hash bits:                    " << rnn.getHashBitsCount() << std::endl;
    std::cout << "\t Hash groups in a store:       " << rnn.getHashGroupsCount() << std::endl;
    std::cout << "\t Buckets in  in a store:       " << rnn.getBucketsCount() << std::endl;
}

int main(int argc, const char* const * argv) {

    ProgramOptions options;
    po::options_description desc("LSH approximate K-RNN search.\n krnn [options] train test.\n Options are");

    desc.add_options()
            ("help,h", "produces help message")
            ("bits,b", po::value<size_t>(&options.bits)->default_value(18), "count of bits used in feature hashing")
            ("dimensions,d", po::value<size_t>(&options.dimensions)->default_value(128), "ensure dimensions for this count of bits")
            ("k_neighbours,k", po::value<size_t>(&options.k)->default_value(1), "count of neighbours to search")
            ("radius,r", po::value<double>(&options.r)->default_value(2), "Close points radius")
            ("margin,e", po::value<double>(&options.margin)->default_value(0.5), "Relative distance between close and far points")
            ("bucket-size,s",po::value<size_t>(&options.bucketSize)->default_value(128), "Size of the bucket")
            ("memory-utilization,a", po::value<double>(&options.memoryUtilization)->default_value(2), "Inverse memory utilization level")
            ("method-tolerance,t", po::value<double>(&options.methodTolerance)->default_value(0.1), "Probability to miss nearest element")
            ("hash-bits,H", po::value<size_t>(&options.hashBits), "Count of bits in a hash group (minihashes)")
            ("verbose,v", po::value<bool>(&options.verbose)->default_value(false), "Show debug output")
            ("train-file", po::value<std::string>())
            ("test-file", po::value<std::string>());

    po::positional_options_description p;
    p.add("train-file", 1);
    p.add("test-file", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            positional(p).options(desc).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        exit(0);
    }

    if (options.verbose) {
        std::cout << "Reading files" << std::endl;
    }

    yasda::HashedBinaryStringReader trainReader(vm["train-file"].as<std::string>(), options.bits);
    yasda::HashedBinaryStringReader testReader(vm["test-file"].as<std::string>(), options.bits);

    if (options.verbose) {
        std::cout << "Done reading files" << std::endl;
    }

    yasda::ApproximateRNN<yasda::HashedSparseBinaryString> rnn(
            trainReader.get().size(), options.r, options.margin, options.bucketSize, options.memoryUtilization,
            options.methodTolerance, std::numeric_limits<size_t>::max(), -1, options.hashBits,
            options.dimensions
    );

    if (options.verbose) {
        printDebugInfo(trainReader, testReader, rnn);
    }

    rnn.fit(trainReader.get(), options.verbose);

    if (options.verbose) {
        std::cout << "Running predictions" << std::endl;
    }

    size_t test_id=0;
    for (auto q: testReader.get()) {
        if (options.verbose) {
            std::cout << "Prediction for "  << test_id << std::endl;
        }
        std::cout << testReader.getId(q) << " ";
        auto neighbours = rnn.getKNearestNeighbours(*q, options.k);

        for (auto ans: neighbours) {
            std::cout << trainReader.getId(ans) << " ";
        }

        std::cout << std::endl;

        if (options.verbose) {
            std::cout << "Ready prediction for "  << test_id << std::endl;
        }
        test_id++;
    }

    exit(0);
}
