typedef struct	s_p
{
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
}				t_p;

typedef struct			s_mass
{
	float x;
	float y;
	float z;
	int	n;
	float m[10];
	float att;
	int nPart;
}						t_mass;

__kernel void accelerate(__global t_p *ps, const t_mass mouse) {
    int i = get_global_id(0);

	float dx = mouse.x - ps[i].x;
	float dy = mouse.y - ps[i].y;
	float dz = mouse.z - ps[i].z;
	float ir = 1.0/(dx * dx + dy * dy + dz * dz + 0.00001);
	ir = sqrt(ir);
	float ax = mouse.att * ir * dx;
	float ay = mouse.att * ir * dy;
	float az = mouse.att * ir * dz;
	for (int j = 0; j < mouse.n; j++)
	{
		dx = mouse.m[2 * j] - ps[i].x;
		dy = mouse.m[2 * j + 1] - ps[i].y;
		dz = mouse.z - ps[i].z;
		ir = 1.0/(dx * dx + dy * dy + dz * dz + 0.00001);
		ir = sqrt(ir);
		ax += mouse.att * ir * dx;
		ay += mouse.att * ir * dy;
		az += mouse.att * ir * dz;
	}
	ps[i].vx += 0.2 * ax;
	ps[i].vy += 0.2 * ay;
	ps[i].vz += 0.2 * az;
}

__kernel void move(__global t_p *ps) {
    int i = get_global_id(0);

	ps[i].x += 0.2 * ps[i].vx;
	ps[i].y += 0.2 * ps[i].vy;
	ps[i].z += 0.2 * ps[i].vz;
}

__kernel void gen(__global t_p *ps, const t_mass mouse) {
    int i = get_global_id(0);

	if (i >= mouse.nPart && i < mouse.nPart + 100)
	{
	    ps[i].x = mouse.x + float(i - mouse.nPart) / 10000.0;
	    ps[i].y = mouse.y + float(i - mouse.nPart) / 10000.0;
	    ps[i].z = mouse.z + float(i - mouse.nPart) / 10000.0;
	}
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
	ps[i].x = (n % 200000 - 100000) / 300000.0f;
    n = n * n % (91 * 7703);
	ps[i].y = (n % 200000 - 100000) / 300000.0f;
    n = n * n % (91 * 7703);
	ps[i].z = (n % 200000 - 100000) / 300000.0f;
	ps[i].vx = 0;
	ps[i].vy = 0;
	ps[i].vz = 0;
}

__kernel void init2(__global t_p *ps) {
    int i = get_global_id(0);
    int n = i * i % (91 * 7703);
    float r = sqrt((float)(n % 4000000)) / 2000.0;
    n = n * n % (91 * 7703);
    float theta = n % 100000 * 2 * 3.1415926 / 100000.0f;
    n = n * n % (91 * 7703);
	ps[i].x = r * cos(theta);
	ps[i].y = r * sin(theta);
	ps[i].z = (n % 200000 - 100000) / 300000.0f;
	ps[i].vx = 0;
	ps[i].vy = 0;
	ps[i].vz = 0;
}
