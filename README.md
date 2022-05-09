# RISCV-Vector-Tests
A set of simple tests that solve some given problem with a sequential as well as a vectorised approach.

All programs are fully self contained (no external libraries used) and only use the stack for memory allocation, making them very portable and easy to implement on barebones RVV implementations.

For mat_mul, use ./generate.sh to generate a set of tests that cover both different algorithms and different matrix sizes. Using -DALGO=5 and -DDEBUG allows for testing the validity of the algorithms.

For sound_mix, I -DSEQ generates a sequential saturating-add test, -DVECTOR generates a vectorised saturating add, and using neither allows for testing the time it takes to generate just the initial arrays. Using -DCOMPARE and -DDEBUG can be used to check the validity of the algorithms.

The algorithms can be built with any optimisation and still yield useful results given that the datatypes are marked as volatile. 