#ifndef PARTICLE_H
#define PARTICLE_H

#include <GLFW/glfw3.h>
#include <OpenCL/opencl.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/gl3.h>

#include <iostream>
#include <time.h>
#include <exception>

#define MAX_SOURCE_SIZE 10000
# define RED "\e[1;38;2;225;20;20m"
# define WHITE "\e[1;38;2;255;251;214m"
# define YELLO "\e[1;38;2;255;200;0m"
# define ORANGE "\e[1;38;2;255;120;10m"
# define GREEN "\e[1;38;2;0;175;117m"

extern const unsigned int W;
extern const unsigned int H;
extern int N;
typedef unsigned int	t_uint;

extern GLFWwindow *window;
extern float hsv[3];

typedef struct			s_buffers
{
	t_uint  vao;
	t_uint	vm;
	t_uint  vbo;
	t_uint  shaders;
	t_uint  mx;
	t_uint  my;
	t_uint  hsv;
	float   br;
	float   bg;
	float   bb;
	float   pt;
}						t_bufs;

typedef struct			s_p
{
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
}						t_p;

typedef struct			s_mass
{
	float x;
	float y;
	int	n;
	float m[100];
	float att;
}						t_mass;

extern t_bufs			g_bufs;
extern t_mass mouse;
extern bool freezehue;
extern bool go;
extern bool explode;

extern size_t global_item_size;
extern size_t local_item_size;

extern cl_program program;
extern cl_kernel ker_acc;
extern cl_kernel ker_move;
extern cl_kernel ker_zoomin;
extern cl_kernel ker_zoomout;
extern cl_command_queue command_queue;
extern cl_device_id device_id;

extern bool circle;

void	getcontext();
void	glinit();
void	glend();
void	loop();
void	keyholds(GLFWwindow *window);
char	*filetostr(char *filename);
void	getcontext();
void	clinit();
void	clend();

#endif
