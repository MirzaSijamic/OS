# [OS2025] Shell Project 4

**IT 204 Operating Systems**  
**Shell Project**  
**March, 2025**

---

## Project: Shell

**Purpose:**  
The purpose of this project is to familiarize you with the mechanics of process control through the implementation of a shell user interface. This includes the relationship between child and parent processes, the steps needed to create a new process, shell variables, and an introduction to user-input parsing and verification.

---

### Task 1.1: Shell interface

The shell (command line) is just a program that continually asks for user input, perhaps does something on the user’s behalf, resets itself, and again asks for user input. Design and implement a basic shell interface that supports the execution of other programs and a series of built-in functions. The shell should be robust (e.g., it should not crash under any circumstance beyond machine failure).

- **Basic:** The prompt should look like this:  
prompt$

- **Advanced:** The prompt should look like this:  
machinename@username:~$

where machinename and username should change depending on the machine and user.

---

### Task 1.2: Shell programs/commands

- **Basic:** Implement the basic functionality of the following programs: `mv`, `du`, `date`, `rev`.  

- **Intermediate:** Provide a few options and/or arguments for at least two programs. Additional points for creativity (e.g. implementing something that does not exist in bash, or differently than it is done in bash).  

- **Advanced:** Allow piping or at least redirecting output to a text file.  

---

### Task 1.3: System calls

- **Basic:** Within the C-programming example of your choice, implement the following system calls:  
`fork()`, `wait()`, and `exec()`.  

- **Intermediate:** Within the C-programming example of your choice, implement `waitpid()`, `execvpe()`.  

- **Additionally:** Carefully explore and then implement the forkbomb.  

---

### Task 1.4: Add some colors to your shell and name it

---

### Task 1.5: Provide a concise and descriptive answer to the following questions

**Question 1.5.1:** What is the purpose of the `fork()` system call?  

**Question 1.5.2:** When a process makes a system call and runs kernel code:  
A. How does the process obtain the address of the kernel instruction to jump to?  
B. Where is the userspace context of the process (program counter and other registers) stored during the transition from user mode to kernel mode?  

**Question 1.5.3:** Explain the following code snippet and write down the list of process state transitions that occur during the following program. You may assume that this is the only process that the CPU is executing.  

```c
int i = 1;  

while (i < 100) { i++; }  

printf("%d ", i);  

while (i > 0) { i--; }  

printf("%d ", i);  
