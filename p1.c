#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

void main(int argc, char **argv){

	int N=argc-1; // represents the number of argument (files)
	int i,j; // indexes
	int msg; // the message received by parent
	int fd; // file descriptor
	int pid; // the process id returned by fork()
	char ch; // the buffer for file read
	int counter=0; // used to count the number of characters
	int size=0;
	int mypipe[2*N][2]; // declare the N pipes (one for each child)
	for (j=0;j<2*N;j++)
		pipe(mypipe[j]); // populate with values



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


		//if(i%2==0){
				// Open file
			fd = open(argv[i/2+1], O_RDONLY);
			// Check if it was succefully opened
	        if (fd < 0){
				printf("Error opeing the file %s \n", argv[i+1]);
				exit(1);
			}

			//find size
			size=lseek(fd,0,SEEK_END);
			
			//find middle
			size=size/2;
			
			if(i%2==0){

				//reset cursor to start
				lseek(fd,0,SEEK_SET);

			}else{

				//reset cursor to middle
				lseek(fd,size,SEEK_SET);

			}

			//READ
			for(j=0;j<size;j++){

				read(fd, &ch, 1);
				
				if(ch >= '1' && ch <= '9'){
				
					counter++;
				
				}
			}

			//send value
			write(mypipe[i][1], &counter,sizeof(int));
			close(fd);
			exit(0);

		}
		
	}

	// loop on pipes and close the ones not in use
	for (j=0;j<2*N;j++)
		close(mypipe[j][1]);

	// read from pipes and print msg
	for (j=0;j<2*N;j++){
		read(mypipe[j][0],&msg,sizeof(int));
		printf("MSG = %d \n", msg);
	}
}