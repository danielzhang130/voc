#ifndef BARS_H
#define BARS_H

#include "bars/bar0.c"
#include "bars/bar1.c"
#include "bars/bar2.c"
#include "bars/bar3.c"

const unsigned char* bars[4] = {
	gImage_bar0,
	gImage_bar1,
	gImage_bar2,
	gImage_bar3
};

#endif
