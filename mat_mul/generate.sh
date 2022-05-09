

/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-g++ -DALGO=-1 -DSIZE=48 -O3 -march=rv64gc -nodefaultlibs mat_mul_aligned.cpp -c -o mat_mul.o
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-gcc -march=rv64gv -lm init.S sdot_vec08.S mat_mul.o -T link.ld -nostartfiles -lm -o 48x48_matnone_o3.out
for VARIABLE in 0 1 2 3 4
do
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-g++ -DALGO=$VARIABLE -DSIZE=48 -O3 -march=rv64gc -nodefaultlibs mat_mul_aligned.cpp -c -o mat_mul.o
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-gcc -march=rv64gv -lm init.S sdot_vec08.S mat_mul.o -T link.ld -nostartfiles -lm -o 48x48_mat${VARIABLE}_o3.out
done

/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-g++ -DALGO=4 -DSIZE=48 -O3 -march=rv64gc -nodefaultlibs mat_mul_aligned.cpp -c -o mat_mul.o
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-gcc -march=rv64gv -lm init.S sdot_vec08_128.S mat_mul.o -T link.ld -nostartfiles -lm -o 48x48_mat4_128_o3.out

for SIZE in 8 16 32 64 96
do
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-g++ -DALGO=-1 -DSIZE=$SIZE -O3 -march=rv64gc -nodefaultlibs mat_mul_aligned.cpp -c -o mat_mul.o
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-gcc -march=rv64gv -lm init.S sdot_vec08.S mat_mul.o -T link.ld -nostartfiles -lm -o ${SIZE}x${SIZE}_none.out
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-g++ -DALGO=3 -DSIZE=$SIZE -O3 -march=rv64gc -nodefaultlibs mat_mul_aligned.cpp -c -o mat_mul.o
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-gcc -march=rv64gv -lm init.S sdot_vec08.S mat_mul.o -T link.ld -nostartfiles -lm -o ${SIZE}x${SIZE}_seq.out
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-g++ -DALGO=4 -DSIZE=$SIZE -O3 -march=rv64gc -nodefaultlibs mat_mul_aligned.cpp -c -o mat_mul.o
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-gcc -march=rv64gv -lm init.S sdot_vec08.S mat_mul.o -T link.ld -nostartfiles -lm -o ${SIZE}x${SIZE}_vec_o3.out
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-g++ -DALGO=4 -DSIZE=$SIZE -O3 -march=rv64gc -nodefaultlibs mat_mul_aligned.cpp -c -o mat_mul.o
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-gcc -march=rv64gv -lm init.S sdot_vec08_128.S mat_mul.o -T link.ld -nostartfiles -lm -o ${SIZE}x${SIZE}_vec_128_o3.out
done