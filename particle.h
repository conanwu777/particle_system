#ifndef PARTICLE_H
#define PARTICLE_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <OpenGL/openGL.h>
#else
#include <CL/cl.h>
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_SOURCE_SIZE (0x100000)

extern const unsigned int W;
extern const unsigned int H;
extern const int N;
typedef unsigned int    t_uint;

extern float *data;
extern GLFWwindow *window;

typedef struct          s_buffers
{
    t_uint  vao;
    t_uint  vbo;
    t_uint  shaders;
    t_uint  mx;
    t_uint  my;
    t_uint  hue;
    t_uint  sat;
    float   br;
    float   bg;
    float   bb;
    float   pt;
}                       t_bufs;

typedef struct  s_p
{
    float x;
    float y;
    float z;
    float vx;
    float vy;
    float vz;
}               t_p;

extern t_bufs          g_bufs;
extern float mousex;
extern float mousey;
extern bool go;
extern bool explode;

extern size_t global_item_size;
extern size_t local_item_size;

extern cl_kernel ker_zoomin;
extern cl_kernel ker_zoomout;
extern cl_command_queue command_queue;

extern float sat;

void	getcontext();
void    glinit();
void    end();
void	loop();
void    keyholds(GLFWwindow *window);

#endif
