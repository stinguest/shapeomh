#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>


void main(int argc, char **argv){

	int N=argc-1; 
	int i,j;
	int msg; 
	int fd; 
	int pid; 
	char ch; 
	int counter=0;
	
	int mypipe[2*N][2]; 
	for (j=0;j<2*N;j++)
		pipe(mypipe[j]);

	for(i = 0; i < 2*N; i++){

		pid=fork();

		if (pid < 0){
			printf("ERROR");
		}
		else if(pid == 0){


			if(i<N){
				for (j=0;j<2*N;j++){
				if (i!=j) close(mypipe[j][1]);
				close(mypipe[j][0]);
			}
			// Open file
			fd = open(argv[i+1], O_RDONLY);
			if (fd < 0){
				printf("Error opeing the file %s \n", argv[i+1]);
				exit(1);
			}

			while(read(fd, &ch, 1)){
					if(ch >= 'a' && ch <= 'z')
						counter++;

			}

			// Close file after use
			close(fd);
			write(mypipe[i][1],&counter,sizeof(int));
			exit(0);

			}else{
				for (j=0;j<2*N;j++){
					if (i!=j) close(mypipe[j][1]);
					//Leave open pipes of previous process
					if(j!=i-N)close(mypipe[j][0]);
			}
			// Open file from the start
			fd = open(argv[i-N+1], O_RDONLY);
			if (fd < 0){
				printf("Error opeing the file %s \n", argv[i+1]);
				exit(1);
			}

			while(read(fd, &ch, 1)){
					if(ch >= 'A' && ch <= 'Z')
						counter++;

			}
      
			close(fd);
			//Read previous process pipes
			read(mypipe[i-N][0], &msg, 1);
			counter+=msg;
			// Write on pipe the number of characters
			write(mypipe[i][1],&counter,sizeof(int));
			exit(0);

			}

			
			
		}
	}
	// loop on pipes and close the ones not in use
	for (j=0;j<2*N;j++){
		close(mypipe[j][1]);
		if(j<N) close(mypipe[j][0]);
	}

	// read from pipes and print msg
	for (j=N;j<2*N;j++){
		read(mypipe[j][0],&msg,sizeof(int));
		printf("MSG from %d = %d \n",j, msg);
	}

}
