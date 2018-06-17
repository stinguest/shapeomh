#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

/* 	
	The process parent receives N files from the user input. For each file it creates
	2 processes child: one reading the first half of the file and the other reading 
	the second half. Each process child read its part and counts the number of digits. 
	At the end of the read, the number is send to the parent through the the specific
	pipe. 
*/

void main(int argc, char **argv){

	int N=argc-1; // represents the number of argument (files)
	int i,j; // indexes
	int msg; // the message received by parent
	int fd; // file descriptor
	int pid; // the process id returned by fork()
	char ch; // the buffer for file read
	int counter=0; // used to count the number of characters
	int size; // represents the size of the file
	int chtoread; // represents the number of characters to read from file
	
	// declare & populate the 2N pipes (one for each child)
	int mypipe[2*N][2]; 
	for (j=0;j<2*N;j++)
		pipe(mypipe[j]);

	// Create the N children
	for(i = 0; i < 2*N; i++){

		pid=fork();

		if (pid < 0){
			printf("ERROR");
		}
		else if(pid == 0){

			// loop on pipes, close the ones not in use
			for (j=0;j<2*N;j++){
				if (i!=j) close(mypipe[j][1]);
				close(mypipe[j][0]);
			}

			// Open file
			fd = open(argv[i/2+1], O_RDONLY);
			// Check if it was succefully opened
			if (fd < 0){
				printf("Error opeing the file %s \n", argv[i+1]);
				exit(1);
			}

			// Check the size of the file
			size=lseek(fd, 0, SEEK_END);

			if (i%2==0) lseek(fd, 0, SEEK_SET);
			else lseek(fd, size/2, SEEK_SET);

			if (size%2==1 && i%2==1) chtoread = size/2+1;
			else chtoread = size/2;

			// Loop on file and read char by char
			for(j=0; j<chtoread;j++){
				read(fd, &ch, 1);
				if (ch >='0' && ch <='9')
					counter++;
			}

			// Close file after use
			close(fd);

			// Write on pipe the number of characters
			write(mypipe[i][1],&counter,sizeof(int));

			// Exit
			exit(0);
		}
	}
	// loop on pipes and close the ones not in use
	for (j=0;j<N;j++)
		close(mypipe[j][1]);

	// read from pipes and print msg
	for (j=0;j<2*N;j++){
		read(mypipe[j][0],&msg,sizeof(int));
		printf("MSG = %d \n", msg);
	}

}
