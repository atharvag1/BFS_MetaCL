
extern const char * __meta_gen_opencl_bfs_custom_args;
struct __meta_gen_opencl_metacl_module_frame;
struct __meta_gen_opencl_metacl_module_frame {
  struct __meta_gen_opencl_metacl_module_frame * next_frame;
  cl_device_id device;
  cl_context context;
  cl_command_queue queue;
  const char * bfs_progSrc;
  size_t bfs_progLen;
  cl_program bfs_prog;
  cl_int bfs_init;
  cl_kernel kernel1_kernel;
  cl_kernel kernel2_kernel;
};
#ifdef __cplusplus
extern "C" {
#endif
a_module_record * meta_gen_opencl_metacl_module_registry(a_module_record * record);
void meta_gen_opencl_metacl_module_init();
void meta_gen_opencl_metacl_module_deinit();
cl_int meta_gen_opencl_bfs_kernel1(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_mem * h_graph_nodes, cl_mem * h_graph_edges, cl_mem * h_graph_mask, cl_mem * h_updating_graph_mask, cl_mem * h_graph_visited, cl_mem * h_cost_ref, cl_mem * no_of_nodes, int async, cl_event * event);
cl_int meta_gen_opencl_bfs_kernel2(cl_command_queue queue, size_t (*grid_size)[3], size_t (*block_size)[3], cl_mem * h_graph_mask, cl_mem * h_updating_graph_mask, cl_mem * h_graph_visited, cl_mem * h_over, cl_mem * no_of_nodes, int async, cl_event * event);
#ifdef __cplusplus
}
#endif
