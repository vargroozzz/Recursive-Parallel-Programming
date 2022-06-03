//
// Created by nsolonko on 29.05.22.
//

#ifndef RPP_COMMON_HPP
#define RPP_COMMON_HPP

#include <chrono>

#include <string>
#include <fstream>
#include <exception>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace utils {

    using namespace boost::numeric::ublas;

    using MatrixDataType = int;

    using Matrix = boost::numeric::ublas::matrix<MatrixDataType>;


    struct Benchmark {

        explicit Benchmark(std::string msg, std::ostream &out = std::cout) : out(out), msg(std::move(msg)),
                                                                             start(std::chrono::high_resolution_clock::now()) {}

        ~Benchmark() {
            out << msg << ": " << std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now() - start).count() << " microsecs\n";
        }

    private:
        std::ostream &out;
        std::string msg;
        const std::chrono::time_point<std::chrono::high_resolution_clock> start;
    };

    template<typename T>
    boost::numeric::ublas::matrix<T> ReadMatrixFromFile(const std::string &filename) {
        std::ifstream fin{filename};
        if (!fin) {
            throw std::runtime_error("Can't open file");
        }

        boost::numeric::ublas::matrix<T> m;
        fin >> m;
        if (!fin) {
            throw std::runtime_error("Can't read matrix from file");
        }

        return m;
    }
}


#endif //RPP_COMMON_HPP
