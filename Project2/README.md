# Project 2: Memory Management
- **Mirza Sijamić** 23004409

## Introduction
Purpose of this project was to conduct a comprehensive analysis of memory management in Linux. Main focus was on the behavior of memory mapping operations using the `mmap()` and `munmap()` system calls. Examination of program's memory usage was examined across four different phases. Those four phases are :

1. Initial state,
2. post-memory mapping,
3. post-writing to mapped memory,
4. post-unmapping
   
The analysis uses system metrics such as Virtual Memory Size (VSZ) and Resident Set Size (RSS), along with detailed examination of `/proc/<PID>/maps` to provide insights into Linux's memory management mechanisms.

## Task 2.1. Memory management tools

 **mmap()** creates a new mapping in the virtual address space of the calling process.
``` 
void *mmap(size_t length; void addr[length], size_t length, int prot, int flags, int fd, off_t offset);
```
Parameters used in the project are as following:

```
void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
```

| Parameter | Value | Description |
| :--- | :--- | :--- |
| `addr` | `NULL` | Tells the OS to choose the starting memory address. |
| `length` | `4096` | Requests 4096 bytes (4KB), a common page size. |
| `prot` | `PROT_READ \| PROT_WRITE` | Sets memory protection to be **readable and writable**. |
| `flags` | `MAP_PRIVATE \| MAP_ANONYMOUS` | Configures the mapping as **private** and not backed by a file (**anonymous**). |
| `fd` | `-1` | The file descriptor, set to -1 because the mapping is anonymous. |
| `offset` | `0` | The file offset, set to 0 because the mapping is anonymous. |

This function return a pointer to an empty block of memory that can be used like an array or a regular buffer.

---

The **munmap()** system call deletes the mappings for the specified address range, and causes further references to addresses within the range to generate invalid memory references. It frees up (unmaps) memory that is allocated by **mmap()**.

```
int munmap(size_t length; void addr[length], size_t length);
```

Parameters used in the the project are as following:
```
munmap(ptr, 4096);
```

| Parameter | Value | Description |
| :--- | :--- | :--- |
| `addr` | `ptr` | The starting address of the memory to be freed (must be the same as the result of `mmap`). |
| `length` | `4096`| The size of the memory to be freed (must be the same as when allocated). |

**mmap()** is useful when you want to manage memory manually, for example creating a replacement for malloc.
After finishing using memory, always use **munmap()** to prevent memory leaks.

# Task 2.2 Implement a C Program (project2.c)

## How it works
The program uses a helper function, **memory_usage()**, which forks a child process. This child process then executes the command `ps -p <parent_pid> -o pid,vsz,rss,comm`. This command reports on the parent's:

- VSZ: Virtual Memory Size (the total memory the process has reserved).
- RSS: Resident Set Size (the portion of that memory currently loaded into physical RAM).


***Initial Stage***
It first calls memory_usage() to show the program's baseline memory footprint right after starting.

***Creating Map Stage (mmap)***
It uses mmap to reserve a 4096-byte (4KB, a standard memory page size) block of virtual memory. This is a request to the operating system to set aside a range of addresses for future use, a concept known as lazy allocation.

***Writing to Memory (memset)***
The program then writes a single character ('A') into the mapped region. This action, often called "touching" the memory, forces the operating system to find a free page of physical RAM and map it to the virtual address the program is accessing.

***Unmapping Memory (munmap)***
Finally, munmap is called to release the memory block. This informs the operating system that the program no longer needs this range of virtual addresses, and any associated physical RAM can be reclaimed.

#### Compiling the program

``` 
gccc project2.c -o projekat
```

#### Running the program
```
./projekat
```

#### Result of the program
```
Initial stage
  PID    VSZ   RSS COMMAND
  41161   2680  1384 projekat

Creating map stage
    PID    VSZ   RSS COMMAND
  41161   2684  1384 projekat

Read back from memory: A
    PID    VSZ   RSS COMMAND
  41161   2684  1512 projekat

Successfully unmapped memory.
    PID    VSZ   RSS COMMAND
  41161   2680  1512 projekat
```

# Task 2.3 and 2.4: Analyse Memory Usage and explain observations

### 1. Initial state
#### Memory statistics:
***VSZ:*** 2680 KB
***RSS:*** 1384 KB

#### Memory mappings:
```
5dae698a6000-5dae698a7000 r--p 00000000 08:02 528397                     /home/Mirza/Desktop/Project2/projekat
5dae698a7000-5dae698a8000 r-xp 00001000 08:02 528397                     /home/Mirza/Desktop/Project2/projekat
5dae698aa000-5dae698ab000 rw-p 00003000 08:02 528397                     /home/Mirza/Desktop/Project2/projekat
5daea9660000-5daea9681000 rw-p 00000000 00:00 0                          [heap]
```
Executable (projekat): We can see multiple segments for my own program. The one with `r-xp` permissions is the actual machine code. The others are for read-only data `r--p` and writable global variables `rw-p`.

`heap`  is the dynamic memory area. This is where our program uses malloc to request memory, it's allocated from this region.

#### Analysis
We can see that the process starts with minimal memory footprint. Initial memory usage consists of the program binary and basic runtime structures. We can see the heap segment but it's size is minimal.

### 2. Creating the map
#### Memory statistics:
***VSZ:*** 2684 KB <--- New 4KB added
***RSS:*** 1384 KB <--- No change

#### Memory mappings:
```
7577cd805000-7577cd812000 rw-p 00000000 00:00 0 
```

#### Analysis:
We can see that VSZ increased by exactly 4KB. This means that virtual memory was succesfully allocated. RSS remains unchanged, demonstrating lazy allocation that Linux uses. That means that the kernel has reserved address space but not yet allocated physical memory.
There is a new private (p), read-write (rw) anonymous mapping that appeared.

### 3. Writing to the mapped memory
#### Memory statistics:
***VSZ:*** 2684 KB <--- No chagne
***RSS:*** 1512 KB <--- New 128KB added

#### Memory mappings:
```
7577cd805000-7577cd812000 rw-p 00000000 00:00 0 
```
After writing to memory (touching it), we can see that VSZ has remained the same but that RSS increased significantly. It has grown by 128KB which are the combination of the 4KB page we activated with `memset` and a much larger chunk of memory (124KB) that the C library allocated for its own internal use, likely triggered by the `printf` call. 
Because Linux uses a lazy allocation method, which typically results in a page fault upon first access, this conduct is consistent with that method.

### 4. Unmaping the memory
#### Memory statistics:
***VSZ:*** 2680 KB <--- Decreased by 4KB
***RSS:*** 1512 KB <--- No change
#### Memory mappings:
Previously allocated anonymous mapping is removed

We notice that VSZ is decreased by 4KB, confirming virtual memory release
RSS remains unchanged due to: memory not immediately returned to the system, Kernel's memory caching mechanisms, potential memory fragmentation.

## Conclusion

| Stage | VSZ (KB) | RSS (KB) | Description |
| :--- | :--- | :--- | :--- |
| Initial | 2680 | 1384 | Before any memory mapping
| After `mmap()` | 2684 | 1384 | Anonymous page mapped, but not yet used
| After writing to memory | 2684 | 1512 | Page accessed (written to), physical memory used
| After `munmap()` | 2680 | 1512 | Unmapping the previously mapped memory

These observations confirm key concepts of Linux memory management:

- ***Lazy Allocation***: When a process uses mmap() to map memory, Linux does not immediately allocate physical pages. Instead, it reserves the address space in the virtual memory layout.
- ***Page Faults***: Physical memory is only allocated when the process accesses the page by writing to it. This triggers a page fault, which causes the kernel to allocate a page frame and map it to the corresponding virtual address.
- ***Impact on RSS***: Since RSS measures the portion of memory that is physically resident, it only increases after the memory is accessed. This explains why VSZ increased slightly after mapping, but RSS did not rise until the write occurred.

Through this experiment, we clearly demonstrated how memory mappings behave differently before and after access, and how tools like `ps` and `/proc/<PID>/maps` help visualize these changes. 

It also effectively demonstrates several key concepts in operating system memory management like:

- **Efficient Resource Utilization**: Lazy allocation ensures that physical memory is only used when it is necessary. We saw this in action in step 2 and 3 of this project.
- **Virtual Memory Abstraction**: The separation of virtual and physical memory provides flexibility and security.
- **System Call Overhead**: Grouping operations into batches minimizes the performance cost associated with frequent system calls.
- **Kernel Optimizations**: The Linux kernel employs sophisticated strategies for memory management so it ensures efficiency and scalability.

# Task 2.5: Create a script to automate the analysis

`analyse.sh` script is made to automate, help, and analyse the memory usage of a running process. To run it we first need to go to the folder it's located. After that we need to make it executable by running this command in the terminal:
```
chmod +x analyse.sh
```
To make use of it we first need to start our previous C code `project2.c` in seperate terminal. After the program pauses we need to run the following command:
```
./analyse.sh $(pgrep mmap_demo)
```
This will display the PID, VSZ, RSS, and command of the running process, as well as also show the memory mappings from `/proc/<PID>/maps`.
It is possible to run this script several moments in mmap_demo to see memory changes, such as after mmap or after writing to memory. This will give us better view into how the memory acctually changes.
