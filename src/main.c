//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

#include "codec27.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 led blink sample (trace via ITM).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the ITM output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 3 / 4)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main (int argc, char* argv[])
{
  // Send a greeting to the trace device (skipped on Release).
  trace_puts ("Hello ARM World!");

  // At this stage the system clock should have already been configured
  // at high speed.
  trace_printf ("System clock: %u Hz\n", SystemCoreClock);

  timer_start ();

  blink_led_init ();

  uint32_t seconds = 0;

  /*
   * VITERBI CODEC TESTING SECTION START
   * */

  /* test data */
#define  NBYTES 64
#define NBITS NBYTES*8
#define ENCBYTES (NBYTES+1)*2

  uint8_t data[NBYTES] =
    { 0 };

  uint8_t decoded[NBYTES] =
    { 0 };

  uint8_t encoded[ENCBYTES] =
    { 0 };

  data[0] = 0x01;
  data[1] = 0x23;
  data[2] = 0x45;
  data[3] = 0x67;
  data[4] = 0x55;

  data[10] = 'T';
  data[11] = 'h';
  data[12] = 'i';
  data[13] = 's';
  data[14] = ' ';
  data[15] = 't';
  data[16] = 'e';
  data[17] = 'x';
  data[18] = 't';
  data[19] = ' ';
  data[20] = 's';
  data[21] = 'h';
  data[22] = 'o';
  data[23] = 'u';
  data[24] = 'l';
  data[25] = 'd';
  data[26] = ' ';
  data[27] = 'b';
  data[28] = 'e';
  data[29] = ' ';
  data[30] = 'e';
  data[31] = 'n';
  data[32] = 'c';
  data[33] = 'o';
  data[34] = 'd';
  data[35] = 'e';
  data[36] = 'd';
  data[37] = '.';

  data[NBYTES - 4] = 0x55;
  data[NBYTES - 3] = 0xaa;
  data[NBYTES - 2] = 0xff;
  data[NBYTES - 1] = 0xff;

  /* encode */
  int res = encode27(encoded, data, NBYTES);

  /* decode */

  /* Test results */
  int result = 0;

  for (int i = 0; i < NBYTES; i++)
    {
      if (data[i] != decoded[i])
	{
	  result = 1;
	}
    }

  /* view result if not same */
  trace_puts("=== Test failed! Data not same ===\n");
  /* view original data */
  trace_printf ("original data:\n");
  for (int i = 0; i < NBYTES; i++)
    trace_printf ("%02x", data[i]);
  trace_printf ("\n\n");
  for (int i = 0; i < NBYTES; i++)
    trace_printf ("%c", data[i]);
  trace_printf ("\n\n");

  /* view encoded data */
  trace_printf ("Encoded data:\n");
  for (int i = 0; i < ENCBYTES; i++)
    trace_printf ("%02x", encoded[i]);
  trace_printf ("\n\n");
  /*  for (int i = 0; i < ENCBYTES; i++)
   trace_printf ("%c", encoded[i]);
   trace_printf ("\n\n");*/

  /* view decoded result*/
  if (result)
    {
      trace_printf ("Decoded data:\n");
      for (int i = 0; i < NBYTES; i++)
	trace_printf ("%02x", decoded[i]);
      trace_printf ("\n\n");
      for (int i = 0; i < NBYTES; i++)
	trace_printf ("%c", decoded[i]);
      trace_printf ("\n\n");
    }
  else
    {
      trace_puts ("Test OK!");
    }
  /*
   * VITERBI CODEC TESTING SECTION END
   * */
  return 0;

  // Infinite loop
  while (1)
    {
      blink_led_on ();
      timer_sleep (seconds == 0 ? TIMER_FREQUENCY_HZ : BLINK_ON_TICKS);

      blink_led_off ();
      timer_sleep (BLINK_OFF_TICKS);

      ++seconds;
      // Count seconds on the trace device.
      trace_printf ("Second %u\n", seconds);
    }
  // Infinite loop, never return.
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
