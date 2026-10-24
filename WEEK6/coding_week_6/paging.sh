#!/bin/bash
# paging-experiments.sh: demonstrate ch18 q4/q5 edge cases
P=./paging-linear-translate.py

echo "===== Q4: weird address space params ====="
$P -P 8 -a 32 -p 1024 -v -s 1
echo
$P -P 8k -a 32k -p 1m -v -s 2
echo
$P -P 1m -a 256m -p 512m -v -s 3
echo

echo "===== Q5: limits / edge cases ====="
echo "--- address space bigger than physical memory ---"
$P -P 1k -a 32m -p 16m -v -n 0
echo "--- address space equal to physical memory ---"
$P -P 1k -a 16m -p 16m -v -n 0
echo "--- page size bigger than address space ---"
$P -P 1000m -a 1m -p 512m -v -n 0
echo "--- address space not a power of 2 ---"
$P -P 1k -a 12k -p 512m -v -n 0
echo "--- physical memory not a multiple of page size ---"
$P -P 2k -a 4k -p 5k -v -n 0
echo "--- size of 1 GB or more ---"
$P -P 1k -a 1g -p 2g -v -n 0
