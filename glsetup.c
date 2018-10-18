#include "particle.h"

size_t global_item_size;
size_t local_item_size;
int ret;

char        *filetostr(char *filename)
{
	char    buf[1000];
	char    *ret;
	FILE    *ptr;
	int     i;

	ret = calloc(1000, 1);
	ptr = fopen(filename, "r");
	i = 0;
	while (fgets(buf, 999, ptr))
	{
		ret = realloc(ret, strlen(ret) + strlen(buf) + 1);
		strcpy(ret + i, buf);
		i += strlen(buf);
	}
	fclose(ptr);
	return (ret);
}

void        getshader(t_bufs *g_bufs)
{
	t_uint              vertexshader;
	t_uint              fragmentshader;
	const GLchar *const vsrc = filetostr("test.vs");
	const GLchar *const fsrc = filetostr("test.fs");

	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, &vsrc, NULL);
	glCompileShader(vertexshader);
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentshader, 1, &fsrc, NULL);
	glCompileShader(fragmentshader);
	g_bufs->shaders = glCreateProgram();
	glAttachShader(g_bufs->shaders, vertexshader);
	glAttachShader(g_bufs->shaders, fragmentshader);
	glLinkProgram(g_bufs->shaders);
	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);
	free(vsrc);
	free(fsrc);
}

void	cursor(GLFWwindow* window, double x, double y)
{
	mousex = (float)x * 2 / W - 1.0;
	mousey = -((float)y * 2 / H - 1.0);
}

void	scroll(GLFWwindow* window, double x, double y)
{
	(void)x;
	if (y > 0)
		ret = clEnqueueNDRangeKernel(command_queue, ker_zoomout, 1, NULL, 
			&global_item_size, &local_item_size, 0, NULL, NULL);
	if (y < 0)
		ret = clEnqueueNDRangeKernel(command_queue, ker_zoomin, 1, NULL, 
			&global_item_size, &local_item_size, 0, NULL, NULL);
	if (y != 0)
		loop();
}

void	keys(GLFWwindow* window, int key, int scan, int action, int mods)
{
	(void)scan;
	(void)mods;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        go = !go;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        explode = !explode;
}

void	keyholds(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        g_bufs.br = (g_bufs.br + 0.01 > 1 ? 1 : g_bufs.br + 0.01);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        g_bufs.bg = (g_bufs.bg + 0.01 > 1 ? 1 : g_bufs.bg + 0.01);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        g_bufs.bb = (g_bufs.bb + 0.01 > 1 ? 1 : g_bufs.bb + 0.01);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        g_bufs.br = (g_bufs.br - 0.01 < 0 ? 0 : g_bufs.br - 0.01);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        g_bufs.bg = (g_bufs.bg - 0.01 < 0 ? 0 : g_bufs.bg - 0.01);
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        g_bufs.bb = (g_bufs.bb - 0.01 < 0 ? 0 : g_bufs.bb - 0.01);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        sat = (sat + 0.01 > 1 ? 1 : sat + 0.01);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        sat = (sat - 0.01 < 0 ? 0 : sat - 0.01);
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
        g_bufs.pt += glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS ? 0.02 : -0.02;
    	glPointSize(g_bufs.pt);
    }
}

void	glinit()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	window = glfwCreateWindow(W, H, "test", NULL, NULL);
	glfwMakeContextCurrent(window);
	getcontext();
	getshader(&g_bufs);
	g_bufs.mx = glGetUniformLocation(g_bufs.shaders, "mx");
	g_bufs.my = glGetUniformLocation(g_bufs.shaders, "my");
	g_bufs.hue = glGetUniformLocation(g_bufs.shaders, "hue");
	g_bufs.sat = glGetUniformLocation(g_bufs.shaders, "sat");
	glGenVertexArrays(1, &g_bufs.vao);
	glGenBuffers(1, &g_bufs.vbo);
	glBindVertexArray(g_bufs.vao);
	glBindBuffer(GL_ARRAY_BUFFER, g_bufs.vbo);
	glBufferData(GL_ARRAY_BUFFER, N * sizeof(t_p), NULL, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_p), NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glfwSetCursorPosCallback(window, cursor);
	glfwSetScrollCallback(window, scroll);
	glfwSetKeyCallback(window, keys);
	g_bufs.pt = 1.5;
	glPointSize(g_bufs.pt);
}

void    end()
{
	glDeleteVertexArrays(1, &g_bufs.vao);
	glDeleteBuffers(1, &g_bufs.vbo);
	glDeleteProgram(g_bufs.shaders);
	glfwDestroyWindow(window);
	glfwTerminate();
}
