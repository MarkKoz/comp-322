# COMP 322 - Assignment 3: Banker's Algorithm

**Course Title:** Intro to Operating Systems & Architecture<br/>
**Semester:** Fall 2021<br/>

## Objective

To implement resource allocation and demonstrate deadlock avoidance with the Banker's algorithm.

## Specification

The program simulates resource allocation to requesting processes and demonstrates deadlock avoidance with the Banker's algorithm. A menu controls the operations, and each choice calls the appropriate procedure, where the choices are:

1. Enter parameters
2. Print resource and process graphs
3. Determine a safe sequence (using the Banker's algorithm)
4. Quit program and free memory

## Assignment

The program uses a claim graph consisting of processes, multi-unit resources, request edges, allocation edges, and claim edges to represent the state of allocated resources to processes. The graph can be represented by the following set of arrays.

#### Max Requestable Array

An `n * m`-element array, where `n` is the number of processes and `m` is the number of resources, and each entry `max_requestable[i][j]` contains an integer that records the maximum number of units from resource `j` that process `i` may ever request.

#### Allocated Array

An `n * m`-element array, where `n` is the number of processes and `m` is the number of resources, and each entry `allocated[i][j]` contains an integer that records the number of units from resource `j` that process `i` has actually been allocated.

#### Needed Array

An `n * m`-element array, where `n` is the number of processes and `m` is the number of resources, and each entry `needed[i][j]` contains an integer that records the number of units from resource `j` that process `i` may need in the future.

#### Total Units Array

An `m`-element array, where `m` is the number of resources and each entry `total_units[j]` records the total number of units from resource `j`.

#### Available Units Array

An `m`-element array, where `m` is the number of resources and each entry `available_units[j]` records the number of units from resource `j` that are available.

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
<summary>Sample Output</summary>

```
Banker's Algorithm
------------------
1) Enter parameters
2) Print resource and process graphs
3) Determine a safe sequence
4) Quit program and free memory

Enter selection: 1

Enter total number of processes: 5
Enter total number of resources: 3
***
Enter number of total units for resource 0: 10
Enter number of total units for resource 1: 5
Enter number of total units for resource 2: 7
***
Enter max number of units that process 0 can request from resource 0: 7
Enter max number of units that process 0 can request from resource 1: 5
Enter max number of units that process 0 can request from resource 2: 3
Enter max number of units that process 1 can request from resource 0: 3
Enter max number of units that process 1 can request from resource 1: 2
Enter max number of units that process 1 can request from resource 2: 2
Enter max number of units that process 2 can request from resource 0: 9
Enter max number of units that process 2 can request from resource 1: 0
Enter max number of units that process 2 can request from resource 2: 2
Enter max number of units that process 3 can request from resource 0: 2
Enter max number of units that process 3 can request from resource 1: 2
Enter max number of units that process 3 can request from resource 2: 2
Enter max number of units that process 4 can request from resource 0: 4
Enter max number of units that process 4 can request from resource 1: 3
Enter max number of units that process 4 can request from resource 2: 3
***
Enter number of units that process 0 is allocated from resource 0: 0
Enter number of units that process 0 is allocated from resource 1: 1
Enter number of units that process 0 is allocated from resource 2: 0
Enter number of units that process 1 is allocated from resource 0: 2
Enter number of units that process 1 is allocated from resource 1: 0
Enter number of units that process 1 is allocated from resource 2: 0
Enter number of units that process 2 is allocated from resource 0: 3
Enter number of units that process 2 is allocated from resource 1: 0
Enter number of units that process 2 is allocated from resource 2: 2
Enter number of units that process 3 is allocated from resource 0: 2
Enter number of units that process 3 is allocated from resource 1: 1
Enter number of units that process 3 is allocated from resource 2: 1
Enter number of units that process 4 is allocated from resource 0: 0
Enter number of units that process 4 is allocated from resource 1: 0
Enter number of units that process 4 is allocated from resource 2: 2

Banker's Algorithm
------------------
1) Enter parameters
2) Print resource and process graphs
3) Determine a safe sequence
4) Quit program and free memory

Enter selection: 2

Total units:
        r0      r1      r2
        10      5       7

Available units:
        r0      r1      r2
        3       3       2

Max requestable units:
        r0      r1      r2
p0      7       5       3
p1      3       2       2
p2      9       0       2
p3      2       2       2
p4      4       3       3

Allocated units:
        r0      r1      r2
p0      0       1       0
p1      2       0       0
p2      3       0       2
p3      2       1       1
p4      0       0       2

Needed units:
        r0      r1      r2
p0      7       4       3
p1      1       2       2
p2      6       0       0
p3      0       1       1
p4      4       3       1

Banker's Algorithm
------------------
1) Enter parameters
2) Print resource and process graphs
3) Determine a safe sequence
4) Quit program and free memory

Enter selection: 3

Checking if p0's needs [ 7 4 3] are <= to available units [ 3 3 2] - skipping
Checking if p1's needs [ 1 2 2] are <= to available units [ 3 3 2] - p1 safely sequenced
Checking if p2's needs [ 6 0 0] are <= to available units [ 5 3 2] - skipping
Checking if p3's needs [ 0 1 1] are <= to available units [ 5 3 2] - p3 safely sequenced
Checking if p4's needs [ 4 3 1] are <= to available units [ 7 4 3] - p4 safely sequenced
Checking if p0's needs [ 7 4 3] are <= to available units [ 7 4 5] - p0 safely sequenced
Checking if p2's needs [ 6 0 0] are <= to available units [ 7 5 5] - p2 safely sequenced

Banker's Algorithm
------------------
1) Enter parameters
2) Print resource and process graphs
3) Determine a safe sequence
4) Quit program and free memory

Enter selection: 4

Goodbye.
```
</details>
