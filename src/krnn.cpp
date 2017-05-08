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
    size_t k;
    double r;
    double margin;
    size_t bucketSize;
    double memoryUtilization;
    double methodTolerance;
    size_t hashBits;
};

int main(int argc, char** argv) {

    ProgramOptions options;
    po::options_description desc("LSH approximate K-RNN search.\n krnn [options] train test.\n Options are");

    desc.add_options()
            ("help", "produces help message")
            ("bits,b", po::value<size_t>(&options.bits)->default_value(8), "count of bits used in feature hashing")
            ("k_neighbours,k", po::value<size_t>(&options.k), "count of neighbours to search")
            ("radius,r", po::value<double>(&options.r)->default_value(2), "Close points radius")
            ("margin,e", po::value<double>(&options.margin)->default_value(0.5), "Relative distance between close and far points")
            ("bucket-size,s",po::value<size_t>(&options.bucketSize)->default_value(128), "Size of the bucket")
            ("memory-utilization,a", po::value<double>(&options.memoryUtilization)->default_value(2), "Inverse memory utilization level")
            ("method-tolerance,t", po::value<double>(&options.methodTolerance)->default_value(0.1), "Probability to miss nearest element")
            ("hash-bits,h", po::value<size_t>(&options.hashBits), "Count of bits in a hash group (minihashes)");

    po::positional_options_description p;
    p.add("train-file", 1);
    p.add("test-file", 1);


    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        exit(0);
    }

    yasda::HashedBinaryStringReader trainReader(vm["train-file"].as<std::string>(), options.bits);
    yasda::HashedBinaryStringReader testReader(vm["test-file"].as<std::string>(), options.bits);

    yasda::ApproximateRNN<yasda::HashedSparseBinaryString> rnn(
            trainReader.get().size(), options.r, options.margin, options.bucketSize, options.memoryUtilization,
            options.methodTolerance, std::numeric_limits<size_t>::max(), -1, options.hashBits,
            static_cast<size_t >(1 << options.bits)
    );

    rnn.fit(trainReader.get());

    for (auto q: testReader.get()) {
        std::cout << testReader.getId(q) << " ";
        for (auto ans: rnn.getKNearestNeighbours(*q, options.k)) {
            std::cout << trainReader.getId(ans);
        }

        std::cout << std::endl;
    }

    exit(0);
}