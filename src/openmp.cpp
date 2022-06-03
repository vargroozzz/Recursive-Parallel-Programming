#include "common.hpp"
#include "omp.h"

utils::Matrix MPProduct(const utils::Matrix &lhs, const utils::Matrix &rhs) {
    utils::Matrix res = utils::zero_matrix<utils::MatrixDataType>(lhs.size1(), rhs.size2());
    assert(lhs.size2() == rhs.size1());
    const auto n = res.size1();
    const auto m = res.size2();
    const auto k = rhs.size1();
    auto &res_data = res.data();
    const auto &lhs_data = lhs.data();
    const auto &rhs_data = rhs.data();

#pragma omp parallel for shared(lhs_data, rhs_data, res_data, k, m, n) default(none)
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            const auto i_prod_m = i * m;
            const auto i_prod_k = i * k;
#pragma omp simd
            for (size_t p = 0; p < k; ++p) {
                res_data[i_prod_m + j] += lhs_data[i_prod_k + p] * rhs_data[(k * p) + j];
            }
        }
    }

    return res;
}


int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Wrong usage, should be 2 parameters\n";
        return 1;
    }

    auto a = utils::ReadMatrixFromFile<utils::MatrixDataType>(argv[1]);
    auto b = utils::ReadMatrixFromFile<utils::MatrixDataType>(argv[2]);

    {
        utils::Benchmark benchmark("MP");
        const auto res = MPProduct(a, b);
    }

    return 0;
}