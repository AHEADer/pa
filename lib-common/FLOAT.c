#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	//nemu_assert(0);
	float tmp_a = (float)a/(1<<16);
	float tmp_b = (float)b/(1<<16);
	float tmp = tmp_a*tmp_b;
	return (FLOAT)(tmp*(1<<16));
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	float tmp_a = (float)a/(1<<16);
	float tmp_b = (float)b/(1<<16);
	float tmp = tmp_a/tmp_b;
	return (FLOAT)(tmp*(1<<16));
}

FLOAT f2F(float a) {
	return (FLOAT)(a*(1<<16));
}

FLOAT Fabs(FLOAT a) {
	FLOAT x= (FLOAT)(a*(1<<16));
	FLOAT tmp = (x >> 31);
	return (x + tmp) ^ tmp; 
}

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

