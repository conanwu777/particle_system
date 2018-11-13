#include "particle.hpp"
using namespace std;

GLFWwindow *window;

const unsigned int W = 1400;
const unsigned int H = 1400;
int		N = 1000;

int		db;
bool	freezehue = 0;
bool	go = 1;
bool	explode = 0;
bool	newParticles = 0;
bool	circle = 0;

int		nbFrames = 0;
double	lastTime;

char	*filetostr(char *filename)
{
	char	buf[1000];
	char	*ret;
	FILE	*ptr;
	int		i;

	ret = (char*)calloc(1000, 1);
	ptr = fopen(filename, "r");
	i = 0;
	while (fgets(buf, 999, ptr))
	{
		ret = (char*)realloc(ret, strlen(ret) + strlen(buf) + 1);
		strcpy(ret + i, buf);
		i += strlen(buf);
	}
	fclose(ptr);
	return (ret);
}

void	mult(float *a, float *b, float *p)
{
	int i;
	int j;
	int k;

	i = -1;
	while (++i < 4)
	{
		j = -1;
		while (++j < 4)
		{
			p[i * 4 + j] = 0;
			k = -1;
			while (++k < 4)
				p[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
		}
	}
}

void	getmatrix(float *mat)
{
	float t[16];

	g_bufs.camx[5] = cos(mouse.y * PI);
	g_bufs.camx[6] = sin(mouse.y * PI);
	g_bufs.camx[9] = -sin(mouse.y * PI);
	g_bufs.camx[10] = cos(mouse.y * PI);
	g_bufs.camz[0] = cos(mouse.x * PI);
	g_bufs.camz[2] = sin(mouse.x * PI);
	g_bufs.camz[8] = -sin(mouse.x * PI);
	g_bufs.camz[10] = cos(mouse.x * PI);
	mult(g_bufs.camz, g_bufs.camx, &t[0]);
	memcpy(mat, &t[0], 16 * sizeof(float));
}

void	loop()
{
	static int nPart = 0;
	char buf[20];
	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0)
	{
		sprintf(buf, "%d FPS\n", nbFrames);
		glfwSetWindowTitle(window, buf);
		nbFrames = 0;
		lastTime += 1.0;
	}
	keyholds(window);
	if (!freezehue)
		hsv[0] += 0.001;
	if (hsv[0] > 1)
		hsv[0] -= 1;
	if (go)
	{
		if (newParticles)
		{
			clSetKernelArg(ker_gen, 1, sizeof(t_mass), (void*)&mouse);
			clEnqueueNDRangeKernel(command_queue, ker_gen, 1, NULL, 
				&global_item_size, &local_item_size, 0, NULL, NULL);
			mouse.nPart += 100;
			if (mouse.nPart >= N)
				mouse.nPart -= N;
		}
		if (!explode)
		{
			clSetKernelArg(ker_acc, 1, sizeof(t_mass), (void*)&mouse);
			clEnqueueNDRangeKernel(command_queue, ker_acc, 1, NULL, 
				&global_item_size, &local_item_size, 0, NULL, NULL);
		}
		clEnqueueNDRangeKernel(command_queue, ker_move, 1, NULL, 
			&global_item_size, &local_item_size, 0, NULL, NULL);
		clFlush(command_queue);
	}
	float tmp[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	if (!go)
		getmatrix(&tmp[0]);
	float tmp2[16];
	mult(&tmp[0], &g_bufs.trans[0], tmp2);
	mult(&tmp2[0], &g_bufs.p[0], tmp);
	glUniformMatrix4fv(g_bufs.mat, 1, GL_FALSE, &tmp[0]);
	glUniform1f(g_bufs.mx, mouse.x);
	glUniform1f(g_bufs.my, mouse.y);
	glUniform3f(g_bufs.hsv, hsv[0], hsv[1], hsv[2]);

	glClearColor(g_bufs.bl, g_bufs.bl, g_bufs.bl, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(g_bufs.vao);
	glDrawArrays(GL_TRIANGLES, 0, N);
	glBindVertexArray(g_bufs.vao);
	glfwSwapBuffers(window);
}

int		main(int ac, char **av)
{
	lastTime = glfwGetTime();
	if (ac == 3 && !strcmp(av[2], "-s"))
		circle = 1;
	if (ac >= 2)
		N = atoi(av[1]);
	if (N < 250 || N > 5000000 || ac > 3 || ac == 1)
	{
		printf(ORANGE);
		printf("Usage: ./particle_system number of particles [-s]\n");
		printf("\t\t250 <= number of particles <= 5000000\n");
		exit(1);
	}
	srand(time(NULL));
	glinit();
	clinit();
	while (!glfwWindowShouldClose(window))
	{
		loop();
		glfwPollEvents();
	}
	glend();
	clend();
	return (0);
}
