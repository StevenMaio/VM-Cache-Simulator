# Virtual Memory and Cache SImulator

The purpose of this program is simulate a virtual memory system as well as a
simple direct mapped cache.

### To Run this Program
To compile the program, type `make` into the shell. Afterwards, make sure
that both `main` and `mem` are running.

### Using this Program
This program comes with a few simple commands.

1. `create` -- Creates a new 'process'. Each process will have an associted 
and unique thread id (`tid`), and its own page table allocated

2. `list` -- Lists all of the processes that have been created by their value
of `tid`.

3. `allocate X` -- Allocates a 'physical' address and create a corresponding 
virtual memory address for the process with `tid` equal to `X`.

4. `mem X` -- Lists all of the virtual memory addresses currently in use by 
the process with `tid` equal to `X`.

5. `kill X` -- Ends a process with `tid` equal to `X`. Afterwards, the 
'physical' memory addresses associated with the process will be deallocated.

6. `read X Y` -- Retrieves the value at virtual address `Y` of process `X`.

7. `write X Y Z` -- Writes the value `Z` into the virtuall address `Y` of 
process `X`.

8. `exit` -- Exits the program and frees all allocated memory.
