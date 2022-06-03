#!/usr/bin/zsh

printf "Run Benchmarks:\n\n"

echo "100 * 10 @ 10 * 100: "
./ublas ../resources/a100x10.txt ../resources/b10x100.txt
./sequential ../resources/a100x10.txt ../resources/b10x100.txt
./openmp ../resources/a100x10.txt ../resources/b10x100.txt
mpiexec -n 8 ./openmpi ../resources/a100x10.txt ../resources/b10x100.txt


echo "100 * 100 @ 100 * 100: "
./ublas ../resources/a100x100.txt ../resources/b100x100.txt
./sequential ../resources/a100x100.txt ../resources/b100x100.txt
./openmp ../resources/a100x100.txt ../resources/b100x100.txt
mpiexec -n 8 ./openmpi ../resources/a100x100.txt ../resources/b100x100.txt

echo "1000 * 100 @ 100 * 1000: "
./ublas ../resources/a1000x100.txt ../resources/b100x1000.txt
./sequential ../resources/a1000x100.txt ../resources/b100x1000.txt
./openmp ../resources/a1000x100.txt ../resources/b100x1000.txt
mpiexec -n 8 ./openmpi ../resources/a1000x100.txt ../resources/b100x1000.txt

echo "1000 * 1000 @ 1000 * 1000: "
./ublas ../resources/a1000x1000.txt ../resources/b1000x1000.txt
./sequential ../resources/a1000x1000.txt ../resources/b1000x1000.txt
./openmp ../resources/a1000x1000.txt ../resources/b1000x1000.txt
mpiexec -n 8 ./openmpi ../resources/a1000x1000.txt ../resources/b1000x1000.txt

#echo "1000 * 10000 @ 10000 * 1000: "
#./ublas ../resources/a1000x10000.txt ../resources/b10000x1000.txt
#./sequential ../resources/a1000x10000.txt ../resources/b10000x1000.txt
#./openmp ../resources/a1000x10000.txt ../resources/b10000x1000.txt
#mpiexec -n 8 ./openmpi ../resources/a1000x10000.txt ../resources/b10000x1000.txt
