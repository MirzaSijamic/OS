# Operating Systems Projects

This repository contains the solutions and reports for key projects completed during my Operating Systems course.  
The assignments cover fundamental concepts in OS design, including process management, memory management, and concurrency — all implemented from scratch.

---

## Project 1: Shell Implementation

### Purpose
This project was designed to provide hands-on experience with process control by implementing a basic command-line shell interface.  
The goal was to understand the relationship between parent and child processes, handle user input, and manage program execution.

### What I Did
- Implemented a robust shell interface with a dynamic prompt that continually accepts and processes user input.  
- Developed the core functionality for several built-in commands, including `mv`, `du`, `date`, and `rev`.  
- Handled advanced features such as command-line arguments and options for specific programs.  
- Utilized fundamental Linux system calls like `fork()`, `wait()`, and `exec()` to manage process creation and execution.  
- Analyzed and explained theoretical concepts of process management and system calls.  

---

## Project 2: Memory Management

### Purpose
This project focused on the practical application of memory management concepts in Linux.  
I explored how processes interact with virtual and physical memory using system calls and analyzed the behavior of memory allocation.

### What I Did
- Wrote a C program that uses the `mmap()` and `munmap()` system calls to dynamically allocate and deallocate a 4KB page of memory.  
- Used system tools like `ps` to measure and record the virtual memory size (VSZ) and resident set size (RSS) of the process at different stages of memory usage.  
- Examined the process's memory maps in the `/proc` filesystem to observe how memory segments change.  
- Analyzed and explained key memory management concepts, including **lazy allocation** and **page faults**, based on empirical observations.  
- Created an automated shell script to simplify the process of gathering and displaying memory analysis data.  

---

## Project 3: Concurrency

### Purpose
The goal of this project was to understand the mechanics of concurrency and solve classical synchronization problems using primitives like locks, condition variables, and semaphores.  
The core objective was to ensure thread safety, prevent race conditions, and manage resource contention efficiently.

### What I Did
- Implemented solutions to classical concurrency challenges by writing synchronized pseudocode for multi-threaded scenarios.  
- Used **semaphores exclusively** to coordinate thread access to shared resources.  
- Ensured proper synchronization logic to solve the **"Parenting A Child"** problem, where:  
  - A child can only take a toy from a non-empty box.  
  - A mother can only refill a box when it’s empty.  

---
