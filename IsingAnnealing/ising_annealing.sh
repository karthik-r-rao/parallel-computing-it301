#!/bin/bash

rm -f results.txt   # remove file if it exists

echo "-----Ising Annealing-----"
echo "This program generates a random graph, performs Ising Annealing to get max cut, and stores the results in a text file."
echo "A comparison is made between the parallel and sequential implementations, and results are checked to ensure both the programs yield the same outputs."

vertices=(50 100 500 1000 5000 10000 20000 30000 50000)

for i in "${vertices[@]}"
    do
        g++ create_graph.cpp -o create_graph && ./create_graph $i
        gcc ising_annealing_sequential.c -o ising_annealing_sequential && ./ising_annealing_sequential
        nvcc ising_annealing_parallel.cu -o ising_annealing_parallel && ./ising_annealing_parallel
done