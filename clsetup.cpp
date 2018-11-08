#include "particle.hpp"
using namespace std;

cl_int		ret;
cl_uint		uret;
cl_mem		memobj;
cl_kernel	ker_acc;
cl_kernel	ker_move;
cl_kernel	ker_zoomout;
cl_kernel	ker_zoomin;
cl_program	program;
cl_command_queue command_queue;
cl_context	context;

size_t global_item_size;
size_t local_item_size = 250;
cl_platform_id platform_id;
cl_device_id device_id;

void	getcontext()
{
	CGLContextObj gl_ctx = CGLGetCurrentContext();
	CGLShareGroupObj gl_sharegroup = CGLGetShareGroup(gl_ctx);
	cl_context_properties properties[] = {
		CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
		(cl_context_properties) gl_sharegroup, 0};
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	ret = clGetPlatformIDs(1, &platform_id, &uret);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &uret);
	context = clCreateContext(properties, 1, &device_id, NULL, NULL, &ret);
}

static void	init_particles()
{
	cl_kernel ker_init;
	ker_acc = clCreateKernel(program, "accelerate", &ret);
	ret = clSetKernelArg(ker_acc, 0, sizeof(cl_mem), (void*)&memobj);
	ker_move = clCreateKernel(program, "move", &ret);
	ret = clSetKernelArg(ker_move, 0, sizeof(cl_mem), (void*)&memobj);
	ker_zoomout = clCreateKernel(program, "zoomout", &ret);
	ret = clSetKernelArg(ker_zoomout, 0, sizeof(cl_mem), (void*)&memobj);
	ker_zoomin = clCreateKernel(program, "zoomin", &ret);
	ret = clSetKernelArg(ker_zoomin, 0, sizeof(cl_mem), (void*)&memobj);
	if (circle)
		ker_init = clCreateKernel(program, "init2", &ret);
	else
		ker_init = clCreateKernel(program, "init", &ret);
	ret = clSetKernelArg(ker_init, 0, sizeof(cl_mem), (void*)&memobj);
	ret = clEnqueueNDRangeKernel(command_queue, ker_init, 1, NULL, 
		&global_item_size, &local_item_size, 0, NULL, NULL);
	ret = clReleaseKernel(ker_init);
	clFlush(command_queue);
}

void	clinit()
{
	char *source_str;
	size_t source_size;
	char buf[20];

	FILE *fp;
	if (!(fp = fopen("kernel.cl", "r")))
	{
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	source_str = (char*)realloc(source_str, source_size + 1);
	source_str[source_size] = '\0';
	fclose(fp);
	global_item_size = N;
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	ret = clGetPlatformIDs(1, &platform_id, &uret);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &uret);
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	memobj = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, g_bufs.vao, &ret);
	glFlush();
	ret = clEnqueueAcquireGLObjects(command_queue, N, &memobj, 0, NULL, NULL);
	program = clCreateProgramWithSource(context, 1, 
		(const char**)&source_str, (const size_t*)&source_size, &ret);
	clGetDeviceInfo(device_id, CL_DEVICE_VERSION, 20, buf, NULL);
	cout << YELLO << buf << endl;
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	ret = clEnqueueReleaseGLObjects(command_queue, N, &memobj, 0, NULL, NULL);
	init_particles();
	free((void*)source_str);
}

void	clend()
{
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(ker_acc);
	ret = clReleaseKernel(ker_move);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
}
