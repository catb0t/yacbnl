#include <stdio.h>
#include <stdint.h>
#include <math.h>

typedef long double ld;

/* __ieee754_log(x)
 * Return the logrithm of x
 *
 * Method :
 *   1. Argument Reduction: find k and f such that
 *			x = 2^k * (1+f),
 *	   where  sqrt(2)/2 < 1+f < sqrt(2) .
 *
 *   2. Approximation of log(1+f).
 *	Let s = f/(2+f) ; based on log(1+f) = log(1+s) - log(1-s)
 *		 = 2s + 2/3 s**3 + 2/5 s**5 + .....,
 *	     	 = 2s + s*R
 *      We use a special Reme algorithm on [0,0.1716] to generate
 * 	a polynomial of degree 14 to approximate R The maximum error
 *	of this polynomial approximation is bounded by 2**-58.45. In
 *	other words,
 *		        2      4      6      8      10      12      14
 *	    R(z) ~ Lg1*s +Lg2*s +Lg3*s +Lg4*s +Lg5*s  +Lg6*s  +Lg7*s
 *  	(the values of Lg1 to Lg7 are listed in the program)
 *	and
 *	    |      2          14          |     -58.45
 *	    | Lg1*s +...+Lg7*s    -  R(z) | <= 2
 *	    |                             |
 *	Note that 2s = f - s*f = f - hfsq + s*hfsq, where hfsq = f*f/2.
 *	In order to guarantee error in log below 1ulp, we compute log
 *	by
 *		log(1+f) = f - s*(f - R)	(if f is not too large)
 *		log(1+f) = f - (hfsq - s*(hfsq+R)).	(better accuracy)
 *
 *	3. Finally,  log(x) = k*ln2 + log(1+f).
 *			    = k*ln2_hi+(f-(hfsq-(s*(hfsq+R)+k*ln2_lo)))
 *	   Here ln2 is split into two floating point number:
 *			ln2_hi + ln2_lo,
 *	   where n*ln2_hi is always exact for |n| < 2000.
 *
 * Special cases:
 *	log(x) is NaN with signal if x < 0 (including -INF) ;
 *	log(+INF) is +INF; log(0) is -INF with signal;
 *	log(NaN) is that NaN with no signal.
 *
 * Accuracy:
 *	according to an error analysis, the error is always less than
 *	1 ulp (unit in the last place).
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following
 * constants. The decimal values may be used, provided that the
 * compiler will convert from decimal to binary accurately enough
 * to produce the hexadecimal values shown.
 */

static uint8_t msb (const uint64_t t) {
  uint8_t r = 0;
  uint64_t t_m = t;

  while (t_m >>= 1) {
    r++;
  }
  return r;
}

// https://stackoverflow.com/a/44232045
static ld ln_naive (const ld y) {
  const uint64_t log2 = msb((uint64_t) y); // See: https://stackoverflow.com/a/4970859/6630230
  const ld divisor = (ld) (1 << log2);
  const ld x = y / divisor;    // normalized value between [1.0, 2.0]
  return (-1.7417939 + (2.8212026 + (-1.4699568 + (0.44717955 - 0.056570851 * x) * x) * x) * x)
        + ((float)log2) * 0.69314718; // ln(2) = 0.69314718
}

/*
  https://stackoverflow.com/questions/46879166
  taylor

  uses: pred succ addition subtraction division multiplication reciprocal increment 2* < !=
*/
static ld ln_iter (const ld x, const uint64_t iters) {
  ld total = 0, z = (x + 1) / (x - 1), power = 1, y = 0;

  for (uint64_t j = 0; j < iters; j++) {
    z *= ((x - 1) * (x - 1)) / ((x + 1) * (x + 1));
    y = (1 / power) * z;

    total += y;
    power += 2;
  }

  return 2 * total;
}

int main(void) {
  printf("N     %Lg\n", (ld) ln_naive(43.2f));
  printf("I1    %Lg\n", (ld) ln_iter(43.2f, 1));
  printf("I10   %Lg\n", (ld) ln_iter(43.2f, 10));
  printf("I100  %Lg\n", (ld) ln_iter(43.2f, 100));
  printf("I1000 %Lg\n", (ld) ln_iter(43.2f, 1000));
  printf("A     %Lg\n", (ld) logl(43.2f));
  return 0;
}
