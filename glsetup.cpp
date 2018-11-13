#include "particle.hpp"

float hsv[3] = {0, .6, 1.0};
t_bufs g_bufs;
t_mass mouse;

void	getshader(t_bufs *g_bufs)
{
	t_uint	vertexshader;
	t_uint	fragmentshader;
	const GLchar *const vsrc = filetostr("particle.vs");
	const GLchar *const fsrc = filetostr("particle.fs");

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
	free((void*)vsrc);
	free((void*)fsrc);
}

void	cursor(GLFWwindow* window, double x, double y)
{
	mouse.x = (float)x * 2 / W - 1.0;
	mouse.y = -((float)y * 2 / H - 1.0);
}

void	button(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT
		&& action == GLFW_PRESS && mouse.n < 5)
	{
		mouse.m[2 * mouse.n] = mouse.x;
		mouse.m[2 * mouse.n + 1] = mouse.y;
		mouse.n++;
	}
}

void	scroll(GLFWwindow* window, double x, double y)
{
	if (x > 0)
		mouse.att += 0.001;
	if (x < 0)
		mouse.att -= (mouse.att >= 0.002 ? 0.001 : 0);
	if (y > 0)
	{
		clEnqueueNDRangeKernel(command_queue, ker_zoomout, 1, NULL, 
			&global_item_size, &local_item_size, 0, NULL, NULL);
		for (int i = 0; i < mouse.n; i++)
		{
			mouse.m[2 * i] *= 0.9;
			mouse.m[2 * i + 1] *= 0.9;
		}
	}
	if (y < 0)
	{
		clEnqueueNDRangeKernel(command_queue, ker_zoomin, 1, NULL, 
			&global_item_size, &local_item_size, 0, NULL, NULL);
		for (int i = 0; i < mouse.n; i++)
		{
			mouse.m[2 * i] /= 0.9;
			mouse.m[2 * i + 1] /= 0.9;
		}
	}
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
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		mouse.n = 0;
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		freezehue = !freezehue;
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		newParticles = !newParticles;
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		clReset();
}

void	keyholds(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		g_bufs.bl = (g_bufs.bl + 0.01 > 1 ? 1 : g_bufs.bl + 0.01);
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		g_bufs.bl = (g_bufs.bl - 0.01 < 0 ? 0 : g_bufs.bl - 0.01);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		hsv[1] = (hsv[1] + 0.01 > 1 ? 1 : hsv[1] + 0.01);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		hsv[1] = (hsv[1] - 0.01 < 0 ? 0 : hsv[1] - 0.01);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		hsv[2] = (hsv[2] + 0.01 > 1 ? 1 : hsv[2] + 0.01);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		hsv[2] = (hsv[2] - 0.01 < 0 ? 0 : hsv[2] - 0.01);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		g_bufs.trans[14] += 0.02;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		g_bufs.trans[14] -= 0.02;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		mouse.z += 0.02;
		g_bufs.trans[12] += 0.02;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		mouse.z -= 0.02;
		g_bufs.trans[12] -= 0.02;
	}
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		g_bufs.pt += glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS ? 0.02 : -0.02;
		glPointSize(g_bufs.pt);
	}
}

void	glinit()
{
	mouse.x = 0;
	mouse.y = 0;
	mouse.z = 0;
	mouse.n = 0;
	mouse.att = 0.05;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	window = glfwCreateWindow(W, H, "FPS", NULL, NULL);
	glfwMakeContextCurrent(window);
	getcontext();
	getshader(&g_bufs);
	g_bufs.mat = glGetUniformLocation(g_bufs.shaders, "p");
	g_bufs.mx = glGetUniformLocation(g_bufs.shaders, "mx");
	g_bufs.my = glGetUniformLocation(g_bufs.shaders, "my");
	g_bufs.hsv = glGetUniformLocation(g_bufs.shaders, "hsv");
	glGenBuffers(1, &g_bufs.vbo);
	glGenVertexArrays(1, &g_bufs.vao);
	glBindVertexArray(g_bufs.vao);
	glBindBuffer(GL_ARRAY_BUFFER, g_bufs.vbo);
	glBufferData(GL_ARRAY_BUFFER, N * sizeof(t_p), NULL, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_p), NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glfwSetCursorPosCallback(window, cursor);
	glfwSetMouseButtonCallback(window, button);
	glfwSetScrollCallback(window, scroll);
	glfwSetKeyCallback(window, keys);
	g_bufs.pt = 1;
	g_bufs.camx[0] = 1;
	g_bufs.camx[15] = 1;
	g_bufs.camx[10] = 1;
	g_bufs.camx[15] = 1;
	g_bufs.p[0] = 1.0 / tan(90 / 2 * PI / 180);
	g_bufs.p[5] = g_bufs.p[0];
	glPointSize(g_bufs.pt);
	glUseProgram(g_bufs.shaders);
}

void	glend()
{
	glDeleteVertexArrays(1, &g_bufs.vao);
	glDeleteBuffers(1, &g_bufs.vbo);
	glDeleteProgram(g_bufs.shaders);
	glfwDestroyWindow(window);
	glfwTerminate();
}
