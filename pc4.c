#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>

void mergeSort(int [],int,int);
void merge(int [],int,int,int);

int main(int argc,char **argv){
	
	MPI_Init(&argc,&argv);
	
	int rank,size,length;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	MPI_Status status;
	
	//parent process sending the array to child process.
	if(rank==0){
		int n,i;
		printf("enter no. of elemnts:");
		scanf("%d",&n);
		
		//sending array size to child process
		MPI_Send(&n,1,MPI_INT,1,3,MPI_COMM_WORLD);
		
		int a[n];
		for(i=0;i<n;i++)
			scanf("%d",&a[i]);
		
		//sending array to child process
		MPI_Send(a,n,MPI_INT,1,1,MPI_COMM_WORLD);
	}
	else{
		int i,sum=0,n1;
		
		//receiving array size from parent process
		MPI_Recv(&n1,1,MPI_INT,0,3,MPI_COMM_WORLD,&status);
		
		//receiving array from parent process
		int b[n1];
		MPI_Recv(b,n1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
		
		printf("original array:\n");
		for(i=0;i<n1;i++)
			printf("%d ",b[i]);
		printf("\n");
		
		//this function is executed parallely by openmp
		mergeSort(b,0,n1-1);
		
		//using openmp parallelism to print the sorted array
		#pragma omp for
		printf("sorted array:\n");
		for(i=0;i<n1;i++)
			printf("%d ",b[i]);
		printf("\n");
	}
	
	
	MPI_Finalize();

	return 0;
}


void mergeSort(int a[],int low,int high){

	if(low<high){
		int mid=(low+high)/2;
		
		//using 2 threads for two sub arrays
		#pragma omp parallel sections num_threads(2)
		{
			//parallel section1
			#pragma omp section
			mergeSort(a,low,mid);
			
			//parallel section2
			#pragma omp section
			mergeSort(a,mid+1,high);
		}
		
		merge(a,low,mid,high);
	}
}

void merge(int a[],int low,int mid,int high){
	int l=mid-low+1,r=high-mid,i,j,k;
	int left[l],right[r];
	
	#pragma omp for
	for(i=0,k=0;i<l;i++,k++)
		left[i]=a[k];
		
	#pragma omp for	
	for(i=0;i<r;i++,k++)
		right[i]=a[k];
		
	
	/* array is placed in a critical section because it is shared by all the processes.
	so that at a time,only one process can access it. */
	#pragma omp for //parallelizing the for loop
	for(i=0,j=0,k=0;i<l && j<r;){
		if(left[i]<=right[j]){
			#pragma omp critical
			a[k++]=left[i++];
		}
		else{
			#pragma omp critical
			a[k++]=right[j++];
		}
	}
	
	
	#pragma omp for
	for(;i<l;){
		#pragma omp critical
		a[k++]=left[i++];
	}
	
	#pragma omp for
	for(;j<r;){
		#pragma omp critical
		a[k++]=right[j++];
	}
}













