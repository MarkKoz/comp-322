# COMP 322 - Assignment 4: Memory Allocation

**Course Title:** Intro to Operating Systems & Architecture<br/>
**Semester:** Fall 2021<br/>

## Objective

To simulate memory allocation with hole-fitting algorithms (First-fit, Best-fit) and implement deallocation and defragmentation.

## Specification

The program simulates memory allocation with a chosen hole-fitting algorithm (First-fit, Best-fit) and implements de-allocation and defragmentation. A menu controls the operations, and each choice calls the appropriate procedure, where the choices are:

1. Enter parameters
2. Allocate memory for a block
3. Deallocate memory for a block
4. Defragment memory
5. Quit program and free memory

## Assignment

* The size of physical memory is represented by an integer `physical_size`.
* The algorithm choice is represented by an integer `algorithm`.
    * `-1` Invalid selection
    * `0` First fit algorithm selection
    * `1` Best fit algorithm selection
* The allocated blocks are contained within an array, where each allocated block is a data struct containing the starting address within memory, and the size of the block.
    * The ending address for each block can be calculated as its `start + size`.
    * Because each block can have a `size` of 1 at the least, the maximum allocation for the block array is equal to the physical memory size.
    * An external integer variable `free_index` can be used to determine which blocks are in use. This starts at 0 and can be no larger beyond the array's length.
    * Blocks will be kept in an organized order throughout the runtime, where one block's ending address cannot be greater than the following block's starting address (_this would cause holes of negative values to occur between blocks_).
* Each allocation request specifies the size of the new block. If the remaining physical memory is not enough to fit the request, the request is rejected.
    * Likewise, if all blocks in the array are allocated, the request is also rejected.
* Each deallocation request specifies the index. If the index is invalid, the request is rejected.
    * Likewise, if no blocks in the array are allocated, the request is also rejected.
* Defragmentation compacts the blocks to be contiguous, and coalesces the holes into one hole at the far-right end (highest memory addresses) of physical memory.
    * If no blocks in the array are allocated, the defragmentation is rejected.

## What Not to Do

Any violation will result in an automatic score of 0 on the assignment.

* Do NOT modify the choice values (1,2,3,4) or input characters and then try to convert them to integers - the test script used for grading your assignment will not work correctly.
* Do NOT turn in an alternate version of the assignment downloaded from the Internet (coursehero, chegg, reddit, github, etc.) or submitted from you or another student from a previous semester.
* Do NOT turn in your assignment coded in another programming language (C++, C#, Java).

## What to Turn In

* The source code as a C file uploaded to Canvas by the deadline of 11:59pm PST (50% off one day late, grade of 0% two or more days late)
* Make sure your code compiles with the [online C compiler](https://www.onlinegdb.com/online_c_compiler) before submitting.
* Code that is unable to compile will receive an automatic grade of 0%. (Crashes during runtime are acceptable for partial credit.)

## Sample Output

<details>
<summary>First-fit</summary>

```
Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 1

Enter size of physical memory: 1024
Enter hole-fitting algorithm (0=first fit, 1=best fit): 0

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 128
New block inserted, starting at 0 and ending before 128

Index   Start   Opening
-----------------------
0       0       128
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 320
New block inserted, starting at 128 and ending before 448

Index   Start   Opening
-----------------------
0       0       128
1       128     448
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 224
New block inserted, starting at 448 and ending before 672

Index   Start   Opening
-----------------------
0       0       128
1       128     448
2       448     672
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 288
New block inserted, starting at 672 and ending before 960

Index   Start   Opening
-----------------------
0       0       128
1       128     448
2       448     672
3       672     960
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 3

Enter block index: 2
Block successfully deallocated.

Index   Start   Opening
-----------------------
0       0       128
1       128     448
2       672     960
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 128
New block inserted, starting at 448 and ending before 576

Index   Start   Opening
-----------------------
0       0       128
1       128     448
2       448     576
3       672     960
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 3

Enter block index: 1
Block successfully deallocated.

Index   Start   Opening
-----------------------
0       0       128
1       448     576
2       672     960
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 224
New block inserted, starting at 128 and ending before 352

Index   Start   Opening
-----------------------
0       0       128
1       128     352
2       448     576
3       672     960
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 64
New block inserted, starting at 352 and ending before 416

Index   Start   Opening
-----------------------
0       0       128
1       128     352
2       352     416
3       448     576
4       672     960
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 4

Memory successfully defragmented.

Index   Start   Opening
-----------------------
0       0       128
1       128     352
2       352     416
3       416     544
4       544     832
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 5

Goodbye.
```
</details>

<details>
<summary>Best-fit</summary>

```
Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 1

Enter size of physical memory: 1024
Enter hole-fitting algorithm (0=first fit, 1=best fit): 1

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 128
New block inserted, starting at 0 and ending before 128

Index   Start   Opening
-----------------------
0       0       128
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 320
New block inserted, starting at 128 and ending before 448

Index   Start   Opening
-----------------------
0       0       128
1       128     448
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 224
New block inserted, starting at 448 and ending before 672

Index   Start   Opening
-----------------------
0       0       128
1       128     448
2       448     672
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 288
New block inserted, starting at 672 and ending before 960

Index   Start   Opening
-----------------------
0       0       128
1       128     448
2       448     672
3       672     960
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 3

Enter block index: 2
Block successfully deallocated.

Index   Start   Opening
-----------------------
0       0       128
1       128     448
2       672     960
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 128
New block inserted, starting at 448 and ending before 576

Index   Start   Opening
-----------------------
0       0       128
1       128     448
2       448     576
3       672     960
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 3

Enter block index: 1
Block successfully deallocated.

Index   Start   Opening
-----------------------
0       0       128
1       448     576
2       672     960
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 224
New block inserted, starting at 128 and ending before 352

Index   Start   Opening
-----------------------
0       0       128
1       128     352
2       448     576
3       672     960
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 2

Enter block size: 64
New block inserted, starting at 960 and ending before 1024

Index   Start   Opening
-----------------------
0       0       128
1       128     352
2       448     576
3       672     960
4       960     1024
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 4

Memory successfully defragmented.

Index   Start   Opening
-----------------------
0       0       128
1       128     352
2       352     480
3       480     768
4       768     832
Size: 1024

Memory Allocation
-----------------
1) Enter parameters
2) Allocate memory for block
3) Deallocate memory for block
4) Defragment memory
5) Quit program and free memory

Enter selection: 5

Goodbye.
```
</details>
