#include "cronometer.h"

float diff (tempo end, tempo init) {
    return ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - 
	((double)init.tv_sec + 1.0e-9*init.tv_nsec);
}

