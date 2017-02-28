/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define ARDUINO_MAIN
#include "Arduino.h"

extern "C" {
  #include <app_timer.h>
  #include <BLE.h>
  #include "nrf_soc.h"
}

#define APP_TIMER_PRESCALER     0 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE 4 /**< Size of timer operation queues. */

APP_TIMER_DEF(m_main_loop_timer);

/*
 * @brief Function to sleep until a BLE event is received by the application.
 */
static void power_manage(void)
{
    ret_code_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

static void run_main_loop(void * p_context)
{
  loop();
  if (serialEventRun) serialEventRun();
}

static void create_timers()
{
    uint32_t err_code;
    // Create timers
    err_code = app_timer_create(&m_main_loop_timer, APP_TIMER_MODE_REPEATED, run_main_loop);
    APP_ERROR_CHECK(err_code);
}


// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

/*
 * \brief Main entry point of Arduino application
 */
int main( void )
{
  APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);
  init();
  initVariant();
  delay(1);
  setup();

  create_timers();
  APP_ERROR_CHECK( app_timer_start(m_main_loop_timer, APP_TIMER_TICKS(1, APP_TIMER_PRESCALER), NULL) );

  for (;;)
  {
    power_manage();
  }

  return 0;
}
