# Usage

## Compile with

$ pgc++ -acc -Minfo=accel -o pagerank_acc pagerank.cpp

## Run with

./pagerank_acc \<vertex-count\> \<iteration-count\> [log]

e.g.

./pagerank_acc 1000 100 log

# Remarks

- Runs iterative version of PageRank algorithm

- The output has been tested for correctness with given output using an [online tool](https://tools.withcode.uk/pagerank/).

- Can run on a single GPU on a single node, but it is able to fully utilize said GPU.

- GPU-accelerated version of the code is about 46 times faster than single-threaded serial code on CPU. (Nvidia Tesla V100 GPU vs Intel Xeon Gold 6126 CPU@2.60GHz)

# Author

Jakub Výmola (VYM0038), VŠB-FEI, 2021-2022
Course: Parallel aglorithms