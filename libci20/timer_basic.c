#include "inttypes.h"
#include "libci20/timer.h"
#include "libci20/peekpoke.h"
#include "ci20board.h"
#include "timer_internal.h"

/*
 * Spin timer functions
*/
void spintimer_init(void)
{
	/* Timer keeps counting, ignoring comparison value */
	poke32(TCU_OSTCSR, OSTCSR_CNT_MD | OSTCSR_PRESCALE_16);

	/* Counter initial value. */
	poke32(TCU_OSTCNTH, 0);
	poke32(TCU_OSTCNTL, 0);

	/* Use EXTCLK as the clock source */
	poke32(TCU_OSTCSR, peek32(TCU_OSTCSR) | OSTCSR_EXT_EN);

	/* Enable the timer*/
	poke32(TESR, TER_OSTEN);
}

void spintimer_usleep(uint64_t usec)
{
	/* Doesn't handle the wrap case, which, with 64-bit values and 3 million
	 * ticks per second, may crop up once every 194980 years. */
	uint64_t target = spintimer_getval() + (usec * OS_TIMER_USEC_DIV);

	while(spintimer_getval() < target)
		;
}

uint64_t spintimer_getval(void)
{
	return peek32(TCU_OSTCNTL) | ((uint64_t)(peek32(TCU_OSTCNTHBUF)) << 32);
}

