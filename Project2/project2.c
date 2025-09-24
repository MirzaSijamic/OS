#include <stdio.h>      // For printf, perror, snprintf
#include <stdlib.h>     // For exit
#include <unistd.h>     // For getpid, sysconf, fork, execvp
#include <sys/mman.h>   // For mmap, munmap
#include <sys/wait.h>   // For waitpid
#include <string.h>     // For memset


void memory_usage(){
	pid_t current_pid = getpid();

	//printf("%d\n", current_pid );

	pid_t child_pid = fork();
	if(child_pid == -1){
		perror("forking failed");
		exit(EXIT_FAILURE);
	}

	//printf("%d\n", current_pid );

	if(child_pid == 0){
		char pid_str[16];
		//char command[256];

		//printf("%d\n", current_pid );

        snprintf(pid_str, sizeof(pid_str), "%d", current_pid);
        
        char *argv[] = {"ps", "-p", pid_str, "-o", "pid,vsz,rss,comm", NULL};

        execvp("ps", argv);

        //ako execvpe faila
        perror("execvpe fail");
        exit(EXIT_FAILURE);

	}
	else{
		int status;
		waitpid(child_pid, &status, 0);
	}
}

int main(){
	printf("Initial stage\n");
	memory_usage();
	getchar(); //pause till ps finishes

	printf("Creating map stage\n");
	void *addres = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if(addres == MAP_FAILED){
		perror("mapping failed");
		exit(EXIT_FAILURE);
	}

	memory_usage();
	getchar();

	//strcpy((char *)addres, "Zdravo asistentu");
	//memset(addres, 42, 4096); //Writing a character to the first byte of mapped memory
	memset(addres, 'A', 1);
	printf("Read back from memory: %c\n", *((char*)addres));
	memory_usage();
	getchar();
    //printf("System page size is: %ld bytes\n", page_size);

    if (munmap(addres, 4096) == -1) {
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }
    printf("Successfully unmapped memory.\n");
    memory_usage();
	

	return 0;
}
