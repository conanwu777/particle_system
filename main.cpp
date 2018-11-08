#include "particle.hpp"

GLFWwindow *window;

const unsigned int W = 1400;
const unsigned int H = 1400;
int		N = 1000;

int		db;
bool	freezehue = 0;
bool	go = 1;
bool	explode = 0;
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

void	loop()
{
	char buf[20];
	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0 ){
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
	clSetKernelArg(ker_acc, 1, sizeof(t_mass), (void*)&mouse);

	if (!explode)
		clEnqueueNDRangeKernel(command_queue, ker_acc, 1, NULL, 
			&global_item_size, &local_item_size, 0, NULL, NULL);
	clEnqueueNDRangeKernel(command_queue, ker_move, 1, NULL, 
		&global_item_size, &local_item_size, 0, NULL, NULL);
	clFlush(command_queue);
	glUniform2f(g_bufs.mx, mouse.x, mouse.y);
	glUniform3f(g_bufs.hsv, hsv[0], hsv[1], hsv[2]);

	glClearColor(g_bufs.br, g_bufs.bg, g_bufs.bb, 1.0f);
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
	if (N <= 0 || N >= 10000000 || ac > 3 || ac == 1)
	{
		printf(ORANGE);
		printf("Usage: ./particle_system number of particles [-s]\n");
		printf("\t\t0 < number of particles < 10000000\n");
		exit(1);
	}
	srand(time(NULL));
	glinit();
	clinit();
	while (!glfwWindowShouldClose(window))
	{
		if (go)
			loop();
		glfwPollEvents();
	}
	glend();
	clend();
	return (0);
}
