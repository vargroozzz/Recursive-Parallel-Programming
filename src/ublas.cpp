#include <iostream>

#include "common.hpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Wrong usage, should be 2 parameters\n";
        return 1;
    }

    auto a = utils::ReadMatrixFromFile<utils::MatrixDataType>(argv[1]);
    auto b = utils::ReadMatrixFromFile<utils::MatrixDataType>(argv[2]);

    {
        utils::Benchmark benchmark("Ublas");
        utils::Matrix res = utils::prod(a, b);
    }

    return 0;
}