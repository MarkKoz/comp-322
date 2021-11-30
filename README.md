# COMP 322 - Assignment 5: Disk Scheduling

Course Title: Intro to Operating Systems & Architecture<br/>
Semester: Fall 2021<br/>

## Objective

To compare the performance of disk scheduling algorithms FIFO and SSTF.

## Specification

The program compares the disk scheduling algorithms First-in-first-out (FIFO) and Shortest-seek-time-first (STTF) in terms of traversing a set of tracks input by the user, and calculates the number of tracks traversed, the longest delay experienced by a track, and the average delay of all tracks processed later than originally scheduled. A menu controls the operations, and each choice calls the appropriate procedure, where the choices are:

1. Enter parameters
2. Schedule disk tracks to traverse using FIFO
3. Schedule disk tracks to traverse using SSTF
4. Quit program and free memory

## Assignment

* A disk consists of n concentric tracks, numbered 0 through `n - 1`.
    * At minimum, there must be 2 tracks (0 and 1). At maximum, there can be as many tracks as desired.
    * Each scheduling simulation starts at track 0, and must traverse to at least one other track.
* A disk scheduling algorithms accepts a sequence of `m` integers in the range `[1, n - 1]`.
    * Each integer in this sequence (`t`) is unique, cannot be duplicate of another integer, and cannot be 0 (where the simulation begins from).
    * The number of integers in the sequence (`m`) can also be in the range `[1, n - 1]`.
    * As such, this range is used for both the size of the sequence, and the contents of the sequence.
* Each scheduling algorithm generates an ordering according to the `m` requests, and calculates the number of tracks traversed. For SSTF, additionally, the algorithm calculates:
    1. The longest delay experienced by a track
    2. The track upon which the longest delay occurs
    3. The average delay of all tracks processed later than originally scheduled.
        * The average delay can be calculated from the sum of all delays experienced, divided by the number of tracks experiencing those delays. (Tracks with 0 delay don't count.)

### Example

A disk with 7 tracks, using a sequence of all 6 possible requests: `(1, 5, 3, 2, 6, 4)`

#### FIFO

FIFO processes the given sequence of tracks in the same order, resulting in 14 steps total.

* 0 → 1: 1 step
* 1 → 5: 4 steps
* 5 → 3: 2 steps
* 3 → 2: 1 step
* 2 → 6: 4 steps
* 6 → 4: 2 steps

#### SSTF

Because track 0 is always the first location, SSTF processes the requests in ascending order `(1, 2, 3, 4, 5, 6)`. Tracks 2 and 4 are processed earlier than under FIFO. The preferential treatment is at the expense of track 5, which is delayed by 3 steps, and track 6, which is delayed by 1 step. In the above example, track 5 was delayed by 3 steps and track 6 by 1 step. The average delay is therefore `(3 + 1) / 2 = 2.0`.


## What Not to Do

Any violation will result in an automatic score of 0 on the assignment.

* Do NOT modify the choice values (1,2,3,4) or input characters and then try to convert them to integers - the test script used for grading your assignment will not work correctly.
* Do NOT turn in an alternate version of the assignment downloaded from the Internet (coursehero, chegg, reddit, github, etc.) or submitted from you or another student from a previous semester.
* Do NOT turn in your assignment coded in another programming language (C++, C#, Java).

## What to Turn In

* The source code as a C file uploaded to Canvas by the deadline of 11:59pm PST (50% off one day late, grade of 0% two or more days late)
* Remember to add your full name/course/assignment number to the top of your code file as on (or more) comment line(s). Points will be taken off if names are absent.
* Make sure your code compiles with the [online C compiler](https://www.onlinegdb.com/online_c_compiler) before submitting.
* Code that is unable to compile will receive an automatic grade of 0%. (Crashes during runtime are acceptable for partial credit.)

## Sample Output

<details>
<summary>Sample Output</summary>

```
Disk Scheduling
---------------
1) Enter parameters
2) Schedule disk tracks with FIFO
3) Schedule disk tracks with SSTF
4) Quit program and free memory

Enter selection: 1

Enter number of concentric tracks (2 or more): 7
Enter size of sequence (1-6): 6
Enter track index (1-6) for sequence index 0: 1
Enter track index (1-6) for sequence index 1: 5
Enter track index (1-6) for sequence index 2: 3
Enter track index (1-6) for sequence index 3: 2
Enter track index (1-6) for sequence index 4: 6
Enter track index (1-6) for sequence index 5: 4

Disk Scheduling
---------------
1) Enter parameters
2) Schedule disk tracks with FIFO
3) Schedule disk tracks with SSTF
4) Quit program and free memory

Enter selection: 2

Sequence of tracks to seek: 1 5 3 2 6 4
Traversed sequence: 1 5 3 2 6 4
The number of tracks traversed is: 14

Disk Scheduling
---------------
1) Enter parameters
2) Schedule disk tracks with FIFO
3) Schedule disk tracks with SSTF
4) Quit program and free memory

Enter selection: 3

Sequence of tracks to seek: 1 5 3 2 6 4
Traversed sequence: 1 2 3 4 5 6
The number of tracks traversed is: 6
The average delay of all tracks processed later is: 2.00
The longest delay experienced by a track is: 3 by track 5

Disk Scheduling
---------------
1) Enter parameters
2) Schedule disk tracks with FIFO
3) Schedule disk tracks with SSTF
4) Quit program and free memory

Enter selection: 4

Goodbye.
```
</details>
