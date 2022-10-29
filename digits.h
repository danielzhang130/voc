#ifndef DIGITS_H
#define DIGITS_H

#include "digits/zero.c"
#include "digits/one.c"
#include "digits/two.c"
#include "digits/three.c"
#include "digits/four.c"
#include "digits/five.c"
#include "digits/six.c"
#include "digits/seven.c"
#include "digits/eight.c"
#include "digits/nine.c"

const unsigned char* digits[10] = {
	gImage_zero,
	gImage_one,
	gImage_two,
	gImage_three,
	gImage_four,
	gImage_five,
	gImage_six,
	gImage_seven,
	gImage_eight,
	gImage_nine
};

#endif
