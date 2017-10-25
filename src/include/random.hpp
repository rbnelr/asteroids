#include <time.h>

namespace random {
	static void init_same_seed_everytime () {
		srand(0);
	}
	static void init () {
		srand( time(NULL) );
	}
	static f32 f32_01 () {
		return (f32)rand() / (f32)RAND_MAX;
	}
	static fv3 v3_01 () {
		return (fv3)(s32v3(rand(), rand(), rand())) / fv3((f32)RAND_MAX);
	}
	static fv2 v2_n1p1 () {
		return ((fv2)(s32v2(rand(), rand())) / fv2((f32)RAND_MAX)) * 2 -1;
	}
	
}
