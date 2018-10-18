#include "particle.h"

GLFWwindow *window;

const unsigned int W = 1400;
const unsigned int H = 1400;
const int N = 3000000;

int db;
float hue = 0;
float sat = 0.6;
t_bufs g_bufs;
float mousex = 0;
float mousey = 0;
bool go = 1;
bool explode = 0;
bool circle = 0;

cl_int ret;
cl_mem memobj;
cl_kernel ker_acc;
cl_kernel ker_move;
cl_kernel ker_zoomout;
cl_kernel ker_zoomin;
cl_program program;
cl_command_queue command_queue;
cl_context context;
char *source_str;
size_t source_size;

size_t global_item_size = N;
size_t local_item_size = 250;
cl_platform_id platform_id;
cl_device_id device_id;

int nbFrames = 0;
double lastTime;

void	loop()
{
	double currentTime = glfwGetTime();
	nbFrames++;
	if ( currentTime - lastTime >= 1.0 ){
		printf("%d FPS\n", nbFrames);
		nbFrames = 0;
		lastTime += 1.0;
	}
	keyholds(window);
	hue += 0.001;
	if (hue > 1)
		hue -= 1;
	ret = clSetKernelArg(ker_acc, 2, sizeof(int), (void*)&mousex);
	ret = clSetKernelArg(ker_acc, 3, sizeof(int), (void*)&mousey);
	if (!explode)
		ret = clEnqueueNDRangeKernel(command_queue, ker_acc, 1, NULL, 
			&global_item_size, &local_item_size, 0, NULL, NULL);
	ret = clEnqueueNDRangeKernel(command_queue, ker_move, 1, NULL, 
		&global_item_size, &local_item_size, 0, NULL, NULL);
	clFlush(command_queue);

	glUniform1f(g_bufs.mx, mousex);
	glUniform1f(g_bufs.my, mousey);
	glUniform1f(g_bufs.hue, hue);
	glUniform1f(g_bufs.sat, sat);
	glClearColor(g_bufs.br, g_bufs.bg, g_bufs.bb, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(g_bufs.shaders);
	glBindVertexArray(g_bufs.vao);
	glDrawArrays(GL_TRIANGLES, 0, 3 * N);
	glfwSwapBuffers(window);
}

void getcontext()
{
    CGLContextObj    gl_ctx        = CGLGetCurrentContext();
    CGLShareGroupObj gl_sharegroup = CGLGetShareGroup(gl_ctx);
    cl_context_properties properties[] = {CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
        (cl_context_properties) gl_sharegroup, 0};
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    clGetPlatformIDs(1, &platform_id, &ret);
    clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret);
    context = clCreateContext(properties, 1, &device_id, NULL, NULL, &ret);
}

void	init_particles()
{
   	int num = N;

	cl_kernel ker_init;
	ker_acc = clCreateKernel(program, "accelerate", &ret);
	ret = clSetKernelArg(ker_acc, 0, sizeof(cl_mem), (void*)&memobj);
	ret = clSetKernelArg(ker_acc, 1, sizeof(int), (void*)&num);
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
    FILE *fp;

    if (!(fp = fopen("kernel.cl", "r"))) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    source_str = realloc(source_str, source_size + 1);
    fclose(fp);
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    clGetPlatformIDs(1, &platform_id, &ret);
    clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret);
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	memobj = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, g_bufs.vao, &ret);
    glFlush();
	ret = clEnqueueAcquireGLObjects(command_queue, N, &memobj, 0, NULL, NULL);
	program = clCreateProgramWithSource(context, 1, 
		(const char**)&source_str, (const size_t*)&source_size, &ret);
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	init_particles();
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

int		main(int ac, char **av)
{
	lastTime = glfwGetTime();
	if (ac == 2 && !strcmp(av[1], "s"))
		circle = 1;
	srand(time(NULL));
	glinit();
	clinit();
	while (!glfwWindowShouldClose(window))
	{
		if (go)
			loop(window);
		glfwPollEvents();
	}
	end();
	clend();
	return (0);
}
