#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

int available_frames,limit,page_replacement,optimal_replacement; 
int input[30];

struct number{
	int data;
	int flag;
	int memory;
}frame[20];

void help()
{
	printf("virtualmem [−h] [-f available-frames] [−r replacement-policy] [−i input_file]\n");
	printf("\t-h: help\n");
	printf("\t-f: number of available frames\n");
	printf("\t-r: replacement policy\n");
	printf("\t-i: input_file\n");
}

void FIFO()
{
    int i,j,k=0,least_index,page_fault=0,found,index,s;
       
	memset((char *)frame, 0, sizeof(frame));
        
	for(i=0;i<limit;i++){ 
                found = 0;
                for(j=0;j<available_frames;j++){
                        if(input[i]==frame[j].data){
                                found = 1;
                                break;
                        }
                }
                if (found) {
                        continue;
                }
                for(j=0;j<available_frames;j++){
                        if(frame[j].data == 0) {
                                frame[j].data=input[i];
                                found = 1;
                                break;
                        }
                }
                if (found) {
                        continue;
                }
		frame[k].data=input[i];
		k++;
		if(k==available_frames){
			k=0;
		}
		page_fault ++;
        }
        printf("page faults with FIFO = %d\n",page_fault);
	page_replacement=page_fault;
}

void LFU()
{
        int i,j,least_index,page_fault=0,found,index;

	memset((char *)frame, 0, sizeof(frame));

	for(i=0;i<limit;i++){
		found = 0;
		for(j=0;j<available_frames;j++){
			if(input[i]==frame[j].data){
				frame[j].flag ++;
				found = 1;
				break;
			}
		}
		if (found) {
			continue;
		}
		for(j=0;j<available_frames;j++){
			if(frame[j].data == 0) {
				frame[j].data=input[i];
				frame[j].flag = 1;
				found = 1;
				break;
			}
		}
		if (found) {
			continue;
		}
		else{
			least_index=frame[0].flag;
                	for(j=available_frames-1;j >= 0;j--){
                		if(frame[j].flag <= least_index){
                			least_index=frame[j].flag;
					index=j;
	                	}
			}
               		frame[index].data=input[i];
			frame[index].flag ++;
			page_fault ++;
		}
	}
	printf("page faults with LFU = %d\n",page_fault);
	page_replacement=page_fault;
}

void SC()
{
        int i,j,k=0,least_index,page_fault=0,found,index,done;

        memset((char *)frame, 0, sizeof(frame));

        for(i=0;i<limit;i++){
                found = 0;
		done = 0;
                for(j=0;j<available_frames;j++){
                        if(input[i]==frame[j].data){
                                found = 1;
                                break;
                        }
                }
                if (found) {
                        continue;
                }
                for(j=0;j<available_frames;j++){
                        if(frame[j].data == 0) {
                                frame[j].data=input[i];
                                frame[j].flag = 0;
                                found = 1;
                                break;
                        }
                }
                if (found) {
                        continue;
                }
		while(done == 0){
			if(frame[k].flag==0){
				frame[k].flag=1;
				k++;
				if(k==available_frames){
					k=0;
				}
			}
			else{
				frame[k].data=input[i];
				frame[k].flag=0;
				done=1;
				k++;
				if(k==available_frames){
					k=0;
				}
			}
		}
		page_fault ++;
        }
        printf("page faults with SC = %d\n",page_fault);
	page_replacement=page_fault;
}

void optimal()
{
        int i,j,k,o,least_index,page_fault=0,found,index,o_index;

        memset((char *)frame, 0, sizeof(frame));

        for(i=0;i<limit;i++){
                found = 0;
                for(j=0;j<available_frames;j++){
                        if(input[i]==frame[j].data){
                                found = 1;
                                break;
                        }
                }
                if (found) {
                        continue;
                }
                for(j=0;j<available_frames;j++){
                        if(frame[j].data == 0) {
                                frame[j].data=input[i];
                                found = 1;
                                break;
                        }
                }
                if (found) {
                        continue;
                }
		for(j=0;j<available_frames;j++){
                	frame[j].memory=100;
		}
		for(j=0;j<available_frames;j++){
			for(k=i;k<limit;k++){
				if(frame[j].data==input[k]){
					frame[j].memory=k;
					break;
				}
			}
		}
		o_index=0;
		o=frame[0].memory;
		for(j=0;j<available_frames;j++){
			if(frame[j].memory>o){
				o=frame[j].memory;
				o_index = j;
			}
		}
		frame[o_index].data=input[i];
		page_fault ++;
        }
        printf("page faults with optimal = %d\n",page_fault);
	optimal_replacement=page_fault;
}

int main(int argc, char *argv[])
{
	int s,k,sched_policy;
	char ch,file_name[10],number_string[10];
	float fifo,lfu,sc;
	bool file_flag=0;
	FILE *fp;

	//default initial data
	available_frames=5;
	sched_policy=0;

	while ((ch = getopt(argc, argv, "hf:r:i:")) != -1) {
	    switch(ch) {
        	case 'f':{
        		available_frames=atoi(optarg);    
		        printf("Total number of available frames = %d\n",available_frames);
			if ((available_frames < 2) || (available_frames > 20)){
                		fprintf(stderr, "Invalid -f option of %d\n", available_frames);
				help();
           			exit(1);
			}
			break;
		}
		case 'r':{
			if (strcasecmp(optarg, "fifo") == 0) {
                		sched_policy = 0;
               		}
			else if (strcasecmp(optarg, "lfu") == 0) {
                		sched_policy = 1;
			}
			else if (strcasecmp(optarg, "sc") == 0) {
        	        	sched_policy = 2;
               		}
			else {
	                	fprintf(stderr, "Invalid -d option of %s\n", optarg);
                    		help();
                    		exit(1);
                	}
			break;
		}
		case 'i':{
			k=0;
			strcpy(file_name,optarg);
			file_flag=1;
			fp = fopen(file_name,"r");
			if(!fp){
				printf("Invalid -i option of %s\n", optarg);
				exit(1);
			}
			while(fp && (fgets(number_string,30,fp) != NULL)){
				number_string[strlen(number_string)-1]='\0';
				input[k]=atoi(number_string);
				k++;
			}
			fclose(fp);
			limit=k;
			break;
		}
		case 'h':{
       		//default:
        		help();
			exit(1);
		}
	    }
	}
	if(!file_flag){
                printf("Enter the I/O request sequence\n");  
		s=0;
		while(1){
                        scanf("%d",&(input[s]));
                        if(input[s]==-1){
				break;
			}
			s++;
		}
		limit=s;
        }

	optimal();

	if(sched_policy==0){
		FIFO();
		fifo=100*(page_replacement-optimal_replacement)/optimal_replacement;
		printf("perc penalty using FIFO = %f\n",fifo);
	}
	if(sched_policy==1){
       		LFU();
		lfu=100*(page_replacement-optimal_replacement)/optimal_replacement;
		printf("perc penalty using LFU = %f\n",lfu);
	}
	if(sched_policy==2){
       		SC();
		sc=100*(page_replacement-optimal_replacement)/optimal_replacement;
		printf("perc penalty using SC = %f\n",sc);
	}
	return 0;
}
