typedef struct	s_p
{
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
}				t_p;

__kernel void accelerate(__global t_p *ps, int n, float mousex, float mousey) {
    int i = get_global_id(0);

	float dx = mousex - ps[i].x;
	float dy = mousey - ps[i].y;
	float dz = -ps[i].z;
	float ir = 1.0/(dx * dx + dy * dy + dz * dz + 0.000001);
	float ax = 0.05 * ir * dx / sqrt(ir);
	float ay = 0.05 * ir * dy / sqrt(ir);
	float az = 0.05 * ir * dz / sqrt(ir);

	//for (int j = 0; j < n; j++)
	//{
	//	dx = ps[j].x - ps[i].x;
	//	dy = ps[j].y - ps[i].y;
	//	dz = ps[j].z - ps[i].z;
	//	ir = 1.0/(dx * dx + dy * dy + dz * dz + 0.00001);
	//	ax += 0.000001 * ir * dx / sqrt(ir);
	//	ay += 0.000001 * ir * dy / sqrt(ir);
	//	az += 0.000001 * ir * dz / sqrt(ir);
	//}
	ps[i].vx += 0.2 * ax;
	ps[i].vy += 0.2 * ay;
	ps[i].vz += 0.2 * az;
}

__kernel void move(__global t_p *ps) {
    int i = get_global_id(0);

	ps[i].x += 0.2 * ps[i].vx;
	ps[i].y += 0.2 * ps[i].vy;
	ps[i].z += 0.2 * ps[i].vz;
	ps[i].vx *= 0.999;
	ps[i].vy *= 0.999;
	ps[i].vz *= 0.999;
}

__kernel void zoomout(__global t_p *ps) {
    int i = get_global_id(0);

	ps[i].x *= 0.9;
	ps[i].y *= 0.9;
	ps[i].z *= 0.9;
	ps[i].vx *= 0.9;
	ps[i].vy *= 0.9;
	ps[i].vz *= 0.9;
}

__kernel void zoomin(__global t_p *ps) {
    int i = get_global_id(0);

	ps[i].x *= 1.1;
	ps[i].y *= 1.1;
	ps[i].z *= 1.1;
	ps[i].vx *= 1.1;
	ps[i].vy *= 1.1;
	ps[i].vz *= 1.1;
}

__kernel void init(__global t_p *ps) {
    int i = get_global_id(0);

    int n = i * i % (91 * 7703);
	ps[i].x = (n % 200000 - 100000) / 500000.0f;
    n = n * n % (91 * 7703);
	ps[i].y = (n % 200000 - 100000) / 500000.0f;
    n = n * n % (91 * 7703);
	ps[i].z = (n % 200000 - 100000) / 500000.0f;
	ps[i].vx = 0;
	ps[i].vy = 0;
	ps[i].vz = 0;
}

__kernel void init2(__global t_p *ps) {
    int i = get_global_id(0);
    int n = i * i % (91 * 7703);
    float r = sqrt((float)(n % 4000000)) / 5000.0;
    n = n * n % (91 * 7703);
    float theta = n % 100000 * 2 * 3.1415926 / 100000.0f;
	ps[i].x = r * cos(theta);
	ps[i].y = r * sin(theta);
	ps[i].z = 0;
	ps[i].vx = 0;
	ps[i].vy = 0;
	ps[i].vz = 0;
}
