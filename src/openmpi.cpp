#include "common.hpp"

#include "cstring"

#include "mpi.h"

enum class ProcessClass : int {
    Root = 0,
    Worker
};

int *MatrixProduct(const int *lhs, const int *rhs, int rows, int shared_side, int columns) {
    auto res = new int[rows * columns]{};

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < columns; ++j) {
            const auto i_prod_columns = i * columns;
            const auto i_prod_shared = i * shared_side;
            for (size_t p = 0; p < shared_side; ++p) {
                res[i_prod_columns + j] += lhs[i_prod_shared + p] * rhs[(shared_side * p) + j];
            }
        }
    }

    return res;
}

struct FirstMessage {
    int rows;
    int shared_side;
    int columns;
};

int SendDataToWorkers(const utils::Matrix &a, const utils::Matrix &b, int proc_num, int rows_per_worker) {
    int offset{0};

    const auto el_in_row = (int) a.size2();

    FirstMessage first_message{};

    first_message.rows = rows_per_worker;
    first_message.shared_side = (int) b.size1();
    first_message.columns = (int) b.size2();

    for (int i = 1; i < proc_num; ++i) {
        MPI_Send(&first_message, sizeof(first_message) / sizeof(int), MPI_INT, i, 3 * i, MPI_COMM_WORLD);
        MPI_Send(&(a.data()[offset]), rows_per_worker * el_in_row, MPI_INT, i, 3 * i + 1, MPI_COMM_WORLD);
        //MPI_Send(&(b.data()[0]), int(b.size1() * b.size2()), MPI_INT, i, 3 * i + 2, MPI_COMM_WORLD);
        offset += rows_per_worker * el_in_row;
    }

    auto b_size = (int) (b.size1() * b.size2());
    auto b_buff = new int[b_size];
    std::memcpy(b_buff, &(b.data()[0]), b_size);

    MPI_Bcast(b_buff, b_size, MPI_INT, 0, MPI_COMM_WORLD);

    return offset;
}

void Gather(int *res, int num_of_proc, int elements_per_worker) {
    size_t curr_offset = 0;
    MPI_Status status{};
    for (int i = 1; i < num_of_proc; ++i) {
        MPI_Recv(&res[curr_offset], elements_per_worker, MPI_INT, i, 4 * num_of_proc + i, MPI_COMM_WORLD, &status);
        if (status.MPI_ERROR != MPI_SUCCESS) {
            std::cout << "Abort 5, error: " << status.MPI_ERROR << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 5);
        }
        curr_offset += elements_per_worker;
    }
}

int *RecvAndHandleData(int id, int &rows, int &columns) {
    FirstMessage msg{};
    MPI_Status status{};
    MPI_Recv(&msg, sizeof(msg) / sizeof(int), MPI_INT, (int) ProcessClass::Root, 3 * id, MPI_COMM_WORLD, &status);
    if (status.MPI_ERROR != MPI_SUCCESS) {
        std::cout << "Abort 1, error: " << status.MPI_ERROR << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int a_size = msg.rows * msg.shared_side;
    auto a = new int[a_size]{};

    MPI_Recv(a, a_size, MPI_INT, (int) ProcessClass::Root, 3 * id + 1, MPI_COMM_WORLD, &status);


    if (status.MPI_ERROR != MPI_SUCCESS) {
        std::cout << "Abort 2, error: " << status.MPI_ERROR << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 2);
    }

    int b_size = msg.columns * msg.shared_side;
    auto b = new int[b_size]{};

    //MPI_Recv(b, b_size, MPI_INT, (int) ProcessClass::Root, 3 * id + 2, MPI_COMM_WORLD, &status);
    //if (status.MPI_ERROR != MPI_SUCCESS) {
    //    MPI_Abort(MPI_COMM_WORLD, 3);
    //}

    MPI_Bcast(b, b_size, MPI_INT, 0, MPI_COMM_WORLD);

    auto res = MatrixProduct(a, b, msg.rows, msg.shared_side, msg.columns);

    rows = msg.rows;
    columns = msg.columns;

    delete[] a;
    delete[] b;

    return res;
}


int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Wrong usage, should be 2 parameters\n";
        return 1;
    }

    int proc_num{0};
    int rank{0};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);


    if (rank == (int) ProcessClass::Root) {
        auto a = utils::ReadMatrixFromFile<utils::MatrixDataType>(argv[1]);
        auto b = utils::ReadMatrixFromFile<utils::MatrixDataType>(argv[2]);
        if (a.size1() < proc_num * 10) {
            std::cout << "Matrix is too small!\n";
            return -4;
        }
        assert(a.size2() == b.size1());
        {
            utils::Benchmark mpi("MPI");

            const auto rows_per_worker = (int) a.size1() / (proc_num - 1);


            const auto offset = SendDataToWorkers(a, b, proc_num, rows_per_worker);

            int *res = new int[a.size1() * b.size2()]{};
            if(offset != a.size1() * a.size2()) {
                int rows = a.size1() - (offset / a.size2());
                auto handle_offset = MatrixProduct(
                       &a.data()[offset], &b.data()[0], rows,
                       (int) b.size1(), (int) b.size2());

                std::memcpy(&res[offset], handle_offset, rows * b.size2());
                delete[] handle_offset;
            }
            const auto element_per_worker = rows_per_worker * (int) b.size2();
            Gather(res, proc_num, element_per_worker);
        }


    } else {
        int rows{0};
        int columns{0};
        int *res = RecvAndHandleData(rank, rows, columns);
        MPI_Send(res, rows * columns, MPI_INT, (int) ProcessClass::Root, 4 * proc_num + rank, MPI_COMM_WORLD);
        delete[] res;
    }

    MPI_Finalize();

    return 0;
}