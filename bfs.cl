typedef struct
{
	int starting;
	int no_of_edges;
}Node;

__kernel void kernel1(__global Node* h_graph_nodes,
		__global int* h_graph_edges,
		__global int* h_graph_mask,
		__global int* h_updating_graph_mask,
		__global int* h_graph_visited,
		__global int* h_cost_ref,
		__global int* no_of_nodes) 
{
	unsigned int tid = get_global_id(0);
	if(tid<*no_of_nodes && h_graph_mask[tid] != 0)
		{
			h_graph_mask[tid] = 0;
			int loc= h_graph_nodes[tid].starting;
			int max = (h_graph_nodes[tid].no_of_edges + loc);
			
			for(int i = loc; i < max; i++)
			{
				int id = h_graph_edges[i];
				if(!h_graph_visited[id])
				{
					h_cost_ref[id] = h_cost_ref[tid] + 1;
					h_updating_graph_mask[id] = 1;
				}
			}
		}
	
}

__kernel void kernel2(__global int* h_graph_mask,
		__global int* h_updating_graph_mask,
		__global int* h_graph_visited,
		__global int* h_over,
		__global int* no_of_nodes)
{
	unsigned int tid = get_global_id(0);
	    
	
		if(tid <*no_of_nodes && h_updating_graph_mask[tid] == 1)
		{
			h_graph_mask[tid] = 1;
			h_graph_visited[tid] = 1;
			*h_over = 1;
			h_updating_graph_mask[tid] = 0;
		}
		
}
