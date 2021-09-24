# COMP 322 - Assignment 2: Batch Process Scheduling

**Course Title:** Intro to Operating Systems & Architecture<br/>
**Semester:** Fall 2021<br/>

## Objective

To calculate the timing parameters of batch processes based on different scheduling algorithms.

## Specification

The program mimics the execution of different processes under different scheduling algorithms. The simulation maintains a table that reflects the current state of the system, based on choosing from a menu of choices, where each choice calls the appropriate procedure, where the choices are:

1. Enter parameters
2. Schedule processes with the FIFO algorithm
3. Schedule processes with the SJF algorithm
4. Quit program and free memory

## Assignment

### Data Structures

#### `table`

Create a list of processes as a fixed 2D array of length `n`. This list will be referred to as `table` and will be indexed from `0` to `n - 1`.

#### `process`

Each `process` in the array is a structure consisting of six fields:

    1. `id`: The process index, corresponding to the iteration variable from the for-loop that the process is assigned within.
    2. `arrival`: A user-input value that determines what time the process arrives into the scheduler.
    3. `total_cpu`: A user-input value that determines how long the process will remain in the scheduler, once it has begun.
    4. `start`: The time at which the process will begin running in the scheduler. The core purpose of this assignment's algorithms is to calculate this value for each process.
    5. `end`: The time at which the process terminates in the scheduler. This can be calculated as `start + total_cpu`.
    6. `turnaround`: The total time in which the process existed in the scheduler. This can be calculated as `end` - `arrival`.

### Functions

#### `print_table()`

Prints out every `process` within `table`, displaying each of the six fields in an organized grid (see sample output below).

#### `sort_table()`

Organizes every `process` within `table`, based on `arrival`. The organization goes in ascending order (lowest to highest).

#### `schedule(algorithm)`

Runs one of two algorithms, based on the value of algorithm passed in:

    * `0` - First In First Out (FIFO)
    * `1` - Shortest Job First (SJF)

`schedule()` starts by making a call to `sort_table()` to ensure each `process` is organized by `arrival`, before iterating (`i`) through `table`.

At `i = 0` the first process' `start` time will always be its `arrival`. When `i > 0`, a decision must be made whether a timing conflict currently exists. If the current (`i`) process' `arrival` is greater/equal to the previous (`i-1`) process' `end`, then there is no conflict, and the current process' `start` is once again assigned to its `arrival`. However, if the opposite is true, then there exists a scheduling conflict, and one of two scheduling algorithms must take effect:

* `FIFO` - The current process' `start` is assigned the value of the previous process' `end`. No process replacement occurs.
* `SJF` - We must compare the current conflicting process with other conflicting processes. The process with the lowest `total_cpu` time will be selected as the new current process to schedule.
    * To achieve this, an inner loop occurs (`j`), starting from the next element `i + 1` (if it exists) until either (A) the list's last element is reached or (B) the `arrival` from process `j` is greater/equal to the `end` from the previous (`i - 1`) process - whichever occurs first.
    * Within this loop, compare the `total_cpu` of the current (`i`) process with the `total_cpu` of process `j`. Process `i` is supposed to have the smallest `total_cpu`. So if process `j` has one that is smaller, then swap the two processes with one another in the list (three-step switch).
    * It is possible for zero, one, or multiple replacements to be made within this inner loop, where the final replacement has the lowest `total_cpu` among the selection of conflicting processes.
    * Once the loop concludes, the new current (`i`) process' `start` is assigned the value of the previous (`i - 1`) process' `end`.

`schedule()` ends by making a call to `print_table()` to display the results.

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
Batch scheduling
----------------
1) Enter parameters
2) Schedule processes with FIFO algorithm
3) Schedule processes with SJF algorithm
4) Quit program and free memory

Enter selection: 1
Enter total number of processes: 3
Enter arrival time for process 0: 0
Enter total CPU time for process 0: 6
Enter arrival time for process 1: 1
Enter total CPU time for process 1: 3
Enter arrival time for process 2: 3
Enter total CPU time for process 2: 2

Batch scheduling
----------------
1) Enter parameters
2) Schedule processes with FIFO algorithm
3) Schedule processes with SJF algorithm
4) Quit program and free memory

Enter selection: 2
ID      Arrival Total   Start   End     Turnaround
--------------------------------------------------
0       0       6       0       6       6
1       1       3       6       9       8
2       3       2       9       11      8

Batch scheduling
----------------
1) Enter parameters
2) Schedule processes with FIFO algorithm
3) Schedule processes with SJF algorithm
4) Quit program and free memory

Enter selection: 3
ID      Arrival Total   Start   End     Turnaround
--------------------------------------------------
0       0       6       0       6       6
2       3       2       6       8       5
1       1       3       8       11      10

Batch scheduling
----------------
1) Enter parameters
2) Schedule processes with FIFO algorithm
3) Schedule processes with SJF algorithm
4) Quit program and free memory

Enter selection: 4
Goodbye.
```
</details>
