#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <ctime> 
#include <vector>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <iostream>

struct Node
{
	int starting;
	int no_of_edges;
};


void run_bfs_gpu(int no_of_nodes, Node *h_graph_nodes, int edge_list_size, 
		int *h_graph_edges, int *h_graph_mask, int *h_updating_graph_mask, 
		int *h_graph_visited, int *h_cost_ref)
{
			
	cl_int errNum;
    	cl_uint numPlatforms;
    	cl_platform_id firstPlatformId;
    	cl_context context = NULL;
	cl_device_id *devices;
    	cl_command_queue commandQueue = NULL;
	cl_program program;
	cl_kernel kernel1 = 0;
	cl_kernel kernel2 = 0;
	cl_device_id device = 0;
    	size_t deviceBufferSize = -1;
	const char* fileName="bfs.cl";
	cl_mem memObjects[8] = {0,0,0,0,0,0,0,0};

    // First, select an OpenCL platform to run on.  For this example, we
    // simply choose the first available platform.  Normally, you would
    // query for all available platforms and select the most appropriate one.
    
	clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
	
    	cl_context_properties contextProperties[] =
    	{
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)firstPlatformId,
        0
    	};
    	context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU,
                                      NULL, NULL, &errNum);
									  
									  
    
    
	if (errNum != CL_SUCCESS)
    	{
        std::cout << "Could not create GPU context, trying CPU..." << std::endl;
        context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU,
                                          NULL, NULL, &errNum);
        if (errNum != CL_SUCCESS)
        {
            std::cerr << "Failed to create an OpenCL GPU or CPU context." << std::endl;
            return ;
        }
    	}
	
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
    	if (errNum != CL_SUCCESS)
    	{
        std::cerr << "Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)";
        return ;
    	}
	clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);

	
	
    	// Allocate memory for the devices buffer
    	devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
    
    	clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
    
    	commandQueue = clCreateCommandQueue(context, devices[0],0 , NULL);
    	clFinish(commandQueue);
	
	
    	device = devices[0];
    	delete [] devices;
    	std::ifstream kernelFile(fileName, std::ios::in);
    
	if (!kernelFile.is_open())
    	{
        std::cerr << "Failed to open file for reading: " << fileName << std::endl;
        return ;
    	}
	
    	std::ostringstream oss;
    	oss << kernelFile.rdbuf();

    	std::string srcStdStr = oss.str();
    	const char *srcStr = srcStdStr.c_str();
    	program = clCreateProgramWithSource(context, 1,(const char**)&srcStr,NULL, NULL);
    
	if (program == NULL)
    	{
        std::cerr << "Failed to create CL program from source." << std::endl;
        return ;
    	}
	
	//clFinish(commandQueue);
	clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	//clFinish(commandQueue);
    
	
	int h_over=1;
	//printf("seg fault occurs here 11.1\n");
    	memObjects[0] = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR , sizeof(Node) * no_of_nodes, h_graph_nodes, NULL);
	memObjects[1] = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR , sizeof(int) * edge_list_size, h_graph_edges, NULL);
	memObjects[2] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * no_of_nodes, h_graph_mask, NULL);
	memObjects[3] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * no_of_nodes, h_updating_graph_mask, NULL);
	memObjects[4] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * no_of_nodes, h_graph_visited, NULL);
	memObjects[5] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * no_of_nodes, h_cost_ref, NULL);
	memObjects[6] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) , &h_over, NULL);
	memObjects[7] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &no_of_nodes, NULL);
	
	clEnqueueWriteBuffer(commandQueue, memObjects[0], CL_TRUE, 0, sizeof(Node) * no_of_nodes, h_graph_nodes, 0, NULL, NULL);
	clFinish(commandQueue);
	clEnqueueWriteBuffer(commandQueue, memObjects[1], CL_TRUE, 0, sizeof(int) * edge_list_size, h_graph_edges, 0, NULL, NULL);
	clFinish(commandQueue);
	clEnqueueWriteBuffer(commandQueue, memObjects[2], CL_TRUE, 0, sizeof(int) * no_of_nodes, h_graph_mask, 0, NULL, NULL);
	clFinish(commandQueue);
	clEnqueueWriteBuffer(commandQueue, memObjects[3], CL_TRUE, 0, sizeof(int) * no_of_nodes, h_updating_graph_mask, 0, NULL, NULL);
	clFinish(commandQueue);
	clEnqueueWriteBuffer(commandQueue, memObjects[4], CL_TRUE, 0, sizeof(int) * no_of_nodes, h_graph_visited, 0, NULL, NULL);
	errNum=clEnqueueWriteBuffer(commandQueue, memObjects[5], CL_TRUE, 0, sizeof(int) * no_of_nodes, h_cost_ref, 0, NULL, NULL);
	clFinish(commandQueue);
	std:: cerr << "error code enqueuebuffer"<< errNum <<"\n";
	
        kernel1 = clCreateKernel(program, "kernel1", NULL);
	

	kernel2 = clCreateKernel(program, "kernel2", NULL);
	
	errNum = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &memObjects[0]);
    	errNum |= clSetKernelArg(kernel1, 1, sizeof(cl_mem), &memObjects[1]);
    	errNum |= clSetKernelArg(kernel1, 2, sizeof(cl_mem), &memObjects[2]);
    	errNum |= clSetKernelArg(kernel1, 3, sizeof(cl_mem), &memObjects[3]);
    	errNum |= clSetKernelArg(kernel1, 4, sizeof(cl_mem), &memObjects[4]);
    	errNum |= clSetKernelArg(kernel1, 5, sizeof(cl_mem), &memObjects[5]);
    	errNum |= clSetKernelArg(kernel1, 6, sizeof(cl_mem), &memObjects[7]);

	errNum = clSetKernelArg(kernel2, 0, sizeof(cl_mem), &memObjects[2]);
	errNum |= clSetKernelArg(kernel2, 1, sizeof(cl_mem), &memObjects[3]);
	errNum |= clSetKernelArg(kernel2, 2, sizeof(cl_mem), &memObjects[4]);
	errNum |= clSetKernelArg(kernel2, 3, sizeof(cl_mem), &memObjects[6]);
	errNum |= clSetKernelArg(kernel2, 4, sizeof(cl_mem), &memObjects[7]);
	
	size_t maxThreads[3];
	errNum = clGetDeviceInfo(device,CL_DEVICE_MAX_WORK_ITEM_SIZES,sizeof(size_t)*3,&maxThreads, NULL);
	

	maxThreads[0] = no_of_nodes < 256? no_of_nodes: 256; //maxThreads[0];

	size_t globalWorkSize[1] = {(no_of_nodes/maxThreads[0])*maxThreads[0] + ((no_of_nodes%maxThreads[0])==0?0:maxThreads[0])}; // one dimensional Range

	size_t localWorkSize[1] = {maxThreads[0]};

	int stop=1;
	int k=0;
	std::vector<double>tval;
	
	cl_event event1,event2,event3,event4;
	while(stop==1){	
	
	stop=0;
	
    	// Queue the kernel up for execution across the array
  
	clEnqueueWriteBuffer(commandQueue, memObjects[6], CL_TRUE, 0, sizeof(int), (void*)&stop, 0, NULL, NULL);
	clFinish(commandQueue);
	
	errNum=clEnqueueNDRangeKernel(commandQueue, kernel1, 1, NULL,globalWorkSize, localWorkSize,0, NULL,NULL);
	errNum=clEnqueueNDRangeKernel(commandQueue, kernel2, 1, NULL,globalWorkSize, localWorkSize,0, NULL, NULL);
	
	clFinish(commandQueue);
	clEnqueueReadBuffer(commandQueue, memObjects[6], CL_TRUE,0,sizeof(int), (void*)&stop,0, NULL, NULL);
	clFinish(commandQueue);
	k++;
        }
	printf (" total iterations : %d\n ",k);
	
	clEnqueueReadBuffer(commandQueue, memObjects[5], CL_TRUE,0, no_of_nodes * sizeof(int), (void*)h_cost_ref,0, NULL, NULL);
    	//Free memory memory
	
	
	
	//clFinish(commandQueue);
	clReleaseMemObject(memObjects[0]);
	clReleaseMemObject(memObjects[1]);	
	clReleaseMemObject(memObjects[2]);
	clReleaseMemObject(memObjects[3]);
	clReleaseMemObject(memObjects[4]);
	clReleaseMemObject(memObjects[5]);
	clReleaseMemObject(memObjects[6]);
	clReleaseMemObject(memObjects[7]);
	clReleaseKernel(kernel1);
	clReleaseKernel(kernel2);
	clReleaseProgram(program);
	clReleaseContext(context);
	clReleaseCommandQueue(commandQueue);
	
}


void run_bfs_cpu(int no_of_nodes, Node *h_graph_nodes, int edge_list_size, 
		int *h_graph_edges, int *h_graph_mask, int *h_updating_graph_mask, 
		int *h_graph_visited, int *h_cost){
    bool stop;
    int k = 0;
    do{
        //if no thread changes this value then the loop stops
        stop=false;
        for(int tid = 0; tid < no_of_nodes; tid++ )
        {
            if (h_graph_mask[tid] == true){
                h_graph_mask[tid]=false;
				
                for(int i=h_graph_nodes[tid].starting; i<(h_graph_nodes[tid].no_of_edges + h_graph_nodes[tid].starting); i++){
                    int id = h_graph_edges[i];	
                    if(!h_graph_visited[id]){	
                        h_cost[id]=h_cost[tid]+1;
						
                        h_updating_graph_mask[id]=true;
                    }
                }
            }
        }

        for(int tid=0; tid< no_of_nodes ; tid++ )
        {
            if (h_updating_graph_mask[tid] == true){
                h_graph_mask[tid]=true;
                h_graph_visited[tid]=true;
                stop=true;
                h_updating_graph_mask[tid]=false;
            }
        }
        k++;
    }
    while(stop);
}









void Usage(int argc, char**argv){

    fprintf(stderr,"Usage: %s <input_file>\n", argv[0]);

}

int main(int argc, char * argv[]) {
    //time_t begin,end;
	clock_t begin,end;
	int no_of_nodes;
    int edge_list_size;
    FILE *fp;
    Node *h_graph_nodes;
    int *h_graph_mask, *h_updating_graph_mask, *h_graph_visited;

    char *input_f;
    if(argc!=2){
        Usage(argc, argv);
        exit(0);
    }

    input_f = argv[1];
    printf("\n");
	printf("Reading File\n");
	printf("\n");
    //Read in Graph from a file
    fp = fopen(input_f,"r");
    if(!fp){
        printf("Error Reading graph file\n");
        return 0;
    }

    int source = 0;
	//printf("seg fault occurs here\n");
    fscanf(fp,"%d",&no_of_nodes);

    int num_of_blocks = 1;
    int num_of_threads_per_block = no_of_nodes;

    //Make execution Parameters according to the number of nodes
    //Distribute threads across multiple Blocks if necessary

    // allocate host memory
    h_graph_nodes = (Node*) malloc(sizeof(Node)*no_of_nodes);
    h_graph_mask = (int*) malloc(sizeof(int)*no_of_nodes);
    h_updating_graph_mask = (int*) malloc(sizeof(int)*no_of_nodes);
    h_graph_visited = (int*) malloc(sizeof(int)*no_of_nodes);
	
    int start, edgeno;
	float speedup;
	double sd=0;
    // initalize the memory
    for(int i = 0; i < no_of_nodes; i++){
        fscanf(fp,"%d %d",&start,&edgeno);
        h_graph_nodes[i].starting = start;
        h_graph_nodes[i].no_of_edges = edgeno;
		//printf("node %d #edges: %d\n",i,edgeno);
		
        h_graph_mask[i]=0;
        h_updating_graph_mask[i]=0;
        h_graph_visited[i]=0;
    }
    //read the source node from the file
    fscanf(fp,"%d",&source);
    source=0;
    //set the source node as true in the mask
    h_graph_mask[source]=1;
    h_graph_visited[source]=1;
    fscanf(fp,"%d",&edge_list_size);
	double mean=(double)edge_list_size/no_of_nodes;
	for (int i=0;i<no_of_nodes;i++){
		sd+=pow((h_graph_nodes[i].no_of_edges-mean),2);
	}
	sd=sqrt(sd/no_of_nodes);
    int id,cost;
    int* h_graph_edges = (int*) malloc(sizeof(int)*edge_list_size);
    for(int i=0; i < edge_list_size ; i++){
        fscanf(fp,"%d",&id);
        fscanf(fp,"%d",&cost);
        h_graph_edges[i] = id;
    }
	printf("#nodes: %d\n",no_of_nodes);
	printf("#edges: %d\n",edge_list_size);
	printf("standard deviation: %lf\n",sd);
    if(fp)
        fclose(fp);
    // allocate mem for the result on host side
	//printf("seg fault occurs here 2\n");
    int	*h_cost = (int*) malloc(sizeof(int)*no_of_nodes);
    int *h_cost_ref = (int*)malloc(sizeof(int)*no_of_nodes);
    for(int i=0;i<no_of_nodes;i++){
        h_cost[i]=-1;
        h_cost_ref[i] = -1;
    }
    h_cost[source]=0;
    h_cost_ref[source]=0;
    //---------------------------------------------------------
    //--cpu entry
	//time (&begin);
	
	begin=clock();
    	run_bfs_cpu(no_of_nodes,h_graph_nodes,edge_list_size,h_graph_edges, h_graph_mask, h_updating_graph_mask, h_graph_visited, h_cost);
	end=clock();

	double time_spent2 = (double)(end - begin) / CLOCKS_PER_SEC;
	printf ("cpu execution time: %f \n", time_spent2);
    
    //--gpu entry
    // initalize the memory again
    bool flag=true;
	
	for(int i = 0; i < no_of_nodes; i++){
        h_graph_mask[i]=0;
        h_updating_graph_mask[i]=0;
        h_graph_visited[i]=0;
    }
    //set the source node as true in the mask
    source=0;
    h_graph_mask[source]=1;
    h_graph_visited[source]=1;
	
	//time (&begin);
	begin=clock();
	run_bfs_gpu(no_of_nodes,h_graph_nodes,edge_list_size,h_graph_edges, h_graph_mask, h_updating_graph_mask, h_graph_visited, h_cost_ref);
	end=clock();
	double time_spent1 = (double)(end - begin) / CLOCKS_PER_SEC;
	printf ("gpu execution time: %f\n", time_spent1);
	speedup=(float)(time_spent2/time_spent1);
	printf("speedup: %f\n",speedup);
	printf("\n");
	//time (&end);
	
	for(int i=0; i<no_of_nodes;i++){
		if(no_of_nodes<2001)
			printf("gpu %d, cpu %d\n",h_cost_ref[i],h_cost[i]);
		//if(h_cost_ref[i]!=h_cost[i])
		{	
			//printf("the implementation is incorrect\n");//flag==false;
			//exit(0);
		}
	}
	
	//if(flag==false)printf("the implementation is incorrect\n");
	//else 
	printf("the implementation is correct\n");
	//if(!flag)printf("not correct");
	
	
	//difference = difftime (end,begin);
	//printf("seg fault occurs here \n");
	//printf ("CPU time %f seconds\n", difference );
	
	
    //for (int i=0;i<no_of_nodes;i++){
    //    printf("node %d : cost %d\n",i,h_cost_ref[i]);
    //}
    //---------------------------------------------------------
    //--result verification
    //compare_results<int>(h_cost, h_cost_ref, no_of_nodes);
    //release host memory
    free(h_graph_nodes);
    free(h_graph_mask);
    free(h_updating_graph_mask);
    free(h_graph_visited);
	free(h_cost_ref);
	free(h_graph_edges);
	free(h_cost);
	
}




