#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	if(argc< 2){
		printf(1,"Need type and input filename\n");
		exit();
	}
	char *filename;
	filename=argv[2];
	int type = atoi(argv[1]);
	printf(1,"Type is %d and filename is %s\n",type, filename);

	int tot_sum = 0;	

	int size=1000;
	short arr[size];
	char c;
	int fd = open(filename, 0);
	for(int i=0; i<size; i++){
		read(fd, &c, 1);
		arr[i]=c-'0';
		read(fd, &c, 1);
	}	
  	close(fd);
  	// this is to supress warning
  	printf(1,"first elem %d\n", arr[0]);
  
  	//----FILL THE CODE HERE for unicast sum
	int parPID = getpid();
	int max_process = 8;
	int section_size = (size+max_process-1)/max_process; //ceil(size/max_process)
	int childPID;
	char *msg = (char*)malloc(8);
	//debug -----------------------------------------------------------------------------
  	for(int i=0;i<max_process;++i){
  		int s=i*section_size, e=(i+1)*section_size;
  		if(e>size) e=size;
  		int sum=0;
  		for(int i=s;i<e;++i){
  			sum += arr[i];
  		}
  		printf(1,"i=%d, sum=%d\n", i, sum);
  	}
  	//debug end --------------------------------------------------------------------------
	for(int i=1; i<max_process; ++i){
		childPID = fork();
		if(childPID == 0){
			break;
		}else{
			*msg = '0'+i;
			*(msg+1) = 0;
			send(parPID, childPID, msg);
		}
	}
	if(childPID == 0){
		recv(msg);
		int section = atoi(msg);
		int start = section*section_size, end = (section+1)*section_size;
		if(end > size) end = size;
		int sum=0;
		for(int i=start;i<end;++i){
			sum += arr[i];
		}
		char *t = msg;
		int sz=0;
		while(sum > 0){
			++sz;
			*t++ = '0'+sum%10;
			sum /= 10;
		}
		*t=0;
		//Now we need to reverse the string as we have stored LSB in the beginning
		for(int i=0;i<sz/2;++i){
			char temp = msg[i];
			msg[i] = msg[sz-1-i];
			msg[sz-1-i] = temp;
		}
		send(getpid(), parPID, msg);
		free(msg);
		exit();
	}else{
		for(int i=0;i<section_size;++i){
			tot_sum += arr[i];
		}
		for(int i=1;i<max_process;++i){
			wait(); //Wait till all other processes complete the summation
		}
		for(int i=1;i<max_process;++i){
			recv(msg); //Recieve the sum from one of the child
			int sum = atoi(msg);
			tot_sum += sum;
		}
		free(msg);
	}
  	//------------------

  	if(type==0){ //unicast sum
		printf(1,"Sum of array for file %s is %d\n", filename,tot_sum);
	}
	exit();
}
