﻿/**************************************************************************************************
  Filename:       hal_led.c
  Revised:        $Date: 2013-05-17 11:25:11 -0700 (Fri, 17 May 2013) $
  Revision:       $Revision: 34355 $

  Description:    This file contains the interface to the HAL LED Service.

**************************************************************************************************/

/***************************************************************************************************
 *                                             INCLUDES
 ***************************************************************************************************/

#include "OSAL.h"

#include "OSAL_Timers.h"
#include "hal_drivers.h"
#ifndef _WIN32
#include "bsp_led.h"
#else
#include "BSP.h"
#endif
#include "hal_led.h"

/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                              MACROS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                              TYPEDEFS
 ***************************************************************************************************/
/* LED control structure */
typedef struct {
  uint8_t mode;       /* Operation mode */
  uint8_t todo;       /* Blink cycles left */
  uint8_t onPct;      /* On cycle percentage */
  uint16_t time;      /* On/off cycle time (msec) */
  uint32_t next;      /* Time for next change */
} HalLedControl_t;

typedef struct
{
  HalLedControl_t HalLedControlTable[HAL_LED_DEFAULT_MAX_LEDS];
  uint8_t           sleepActive;
} HalLedStatus_t;

/***************************************************************************************************
 *                                           GLOBAL VARIABLES
 ***************************************************************************************************/


static uint8_t HalLedState;              // LED state at last set/clr/blink update

#if HAL_LED == TRUE

static uint8_t preBlinkState;            // Original State before going to blink mode
                                       // bit 0, 1, 2, 3 represent led 0, 1, 2, 3
#endif

#ifdef BLINK_LEDS
  static HalLedStatus_t HalLedStatusControl;
#endif

/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/
#if (HAL_LED == TRUE)
void HalLedUpdate (void);
void HalLedOnOff (uint8_t leds, uint8_t mode);
#endif /* HAL_LED */

/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      HalLedInit
 *
 * @brief   Initialize LED Service
 *
 * @param   init - pointer to void that contains the initialized value
 *
 * @return  None
 ***************************************************************************************************/
void HalLedInit (void)
{
  // Init LED
#ifndef _WIN32
  BSP_LED_Init();
#endif

#if (HAL_LED == TRUE)
  /* Initialize all LEDs to OFF */
  HalLedSet (HAL_LED_ALL, HAL_LED_MODE_OFF);
#endif /* HAL_LED */
#ifdef BLINK_LEDS
  /* Initialize sleepActive to FALSE */
  HalLedStatusControl.sleepActive = FALSE;
#endif
}

/***************************************************************************************************
 * @fn      HalLedSet
 *
 * @brief   Tun ON/OFF/TOGGLE given LEDs
 *
 * @param   led - bit mask value of leds to be turned ON/OFF/TOGGLE
 *          mode - BLINK, FLASH, TOGGLE, ON, OFF
 * @return  None
 ***************************************************************************************************/
uint8_t HalLedSet (uint8_t leds, uint8_t mode)
{

#if (defined (BLINK_LEDS)) && (HAL_LED == TRUE)
  uint8_t led;
  HalLedControl_t *sts;

  switch (mode)
  {
    case HAL_LED_MODE_BLINK:
      /* Default blink, 1 time, D% duty cycle */
      HalLedBlink (leds, 1, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
      break;

    case HAL_LED_MODE_FLASH:
      /* Default flash, N times, D% duty cycle */
      HalLedBlink (leds, HAL_LED_DEFAULT_FLASH_COUNT, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
      break;

    case HAL_LED_MODE_ON:
    case HAL_LED_MODE_OFF:
    case HAL_LED_MODE_TOGGLE:

      led = HAL_LED_1;
      leds &= HAL_LED_ALL;
      sts = HalLedStatusControl.HalLedControlTable;

      while (leds)
      {
        if (leds & led)
        {
          if (mode != HAL_LED_MODE_TOGGLE)
          {
            sts->mode = mode;  /* ON or OFF */
          }
          else
          {
            sts->mode ^= HAL_LED_MODE_ON;  /* Toggle */
          }
          HalLedOnOff (led, sts->mode);
          leds ^= led;
        }
        led <<= 1;
        sts++;
      }
      break;

    default:
      break;
  }

#elif (HAL_LED == TRUE)
  LedOnOff(leds, mode);
#else
  // HAL LED is disabled, suppress unused argument warnings
  (void) leds;
  (void) mode;
#endif /* BLINK_LEDS && HAL_LED   */

  return ( HalLedState );

}

/***************************************************************************************************
 * @fn      HalLedBlink
 *
 * @brief   Blink the leds
 *
 * @param   leds       - bit mask value of leds to be blinked
 *          numBlinks  - number of blinks
 *          percent    - the percentage in each period where the led
 *                       will be on
 *          period     - length of each cycle in milliseconds
 *
 * @return  None
 ***************************************************************************************************/
void HalLedBlink (uint8_t leds, uint8_t numBlinks, uint8_t percent, uint16_t period)
{
#if (defined (BLINK_LEDS)) && (HAL_LED == TRUE)
  uint8_t led;
  HalLedControl_t *sts;

  if (leds && percent && period)
  {
    if (percent < 100)
    {
      led = HAL_LED_1;
      leds &= HAL_LED_ALL;
      sts = HalLedStatusControl.HalLedControlTable;

      while (leds)
      {
        if (leds & led)
        {
          /* Store the current state of the led before going to blinking if not already blinking */
          if(sts->mode < HAL_LED_MODE_BLINK )
          	preBlinkState |= (led & HalLedState);

          sts->mode  = HAL_LED_MODE_OFF;                    /* Stop previous blink */
          sts->time  = period;                              /* Time for one on/off cycle */
          sts->onPct = percent;                             /* % of cycle LED is on */
          sts->todo  = numBlinks;                           /* Number of blink cycles */
          if (!numBlinks) sts->mode |= HAL_LED_MODE_FLASH;  /* Continuous */
          sts->next = osal_GetSystemClock();                /* Start now */
          sts->mode |= HAL_LED_MODE_BLINK;                  /* Enable blinking */
          leds ^= led;
        }
        led <<= 1;
        sts++;
      }
      // Cancel any overlapping timer for blink events
      osal_stop_timerEx(Hal_TaskID, HAL_LED_BLINK_EVENT);
      osal_set_event (Hal_TaskID, HAL_LED_BLINK_EVENT);
    }
    else
    {
      HalLedSet (leds, HAL_LED_MODE_ON);                    /* >= 100%, turn on */
    }
  }
  else
  {
    HalLedSet (leds, HAL_LED_MODE_OFF);                     /* No on time, turn off */
  }
#elif (HAL_LED == TRUE)
  percent = (leds & HalLedState) ? HAL_LED_MODE_OFF : HAL_LED_MODE_ON;
  HalLedOnOff (leds, percent);                              /* Toggle */
#else
  // HAL LED is disabled, suppress unused argument warnings
  (void) leds;
  (void) numBlinks;
  (void) percent;
  (void) period;
#endif /* BLINK_LEDS && HAL_LED */
}

#if (HAL_LED == TRUE)
/***************************************************************************************************
 * @fn      HalLedUpdate
 *
 * @brief   Update leds to work with blink
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void HalLedUpdate (void)
{
  uint8_t led;
  uint8_t pct;
  uint8_t leds;
  HalLedControl_t *sts;
  uint32_t time;
  uint16_t next;
  uint16_t wait;

  next = 0;
  led  = HAL_LED_1;
  leds = HAL_LED_ALL;
  sts = HalLedStatusControl.HalLedControlTable;

  /* Check if sleep is active or not */
  if (!HalLedStatusControl.sleepActive)
  {
    while (leds)
    {
      if (leds & led)
      {
        if (sts->mode & HAL_LED_MODE_BLINK)
        {
          time = osal_GetSystemClock();
          if (time >= sts->next)
          {
            if (sts->mode & HAL_LED_MODE_ON)
            {
              pct = 100 - sts->onPct;               /* Percentage of cycle for off */
              sts->mode &= ~HAL_LED_MODE_ON;        /* Say it's not on */
              HalLedOnOff (led, HAL_LED_MODE_OFF);  /* Turn it off */

              if (!(sts->mode & HAL_LED_MODE_FLASH))
              {
                sts->todo--;                        /* Not continuous, reduce count */
              }
            }            
            else if ( (!sts->todo) && !(sts->mode & HAL_LED_MODE_FLASH) )
            {
                  sts->mode ^= HAL_LED_MODE_BLINK;  /* No more blinks */
            }            
            else
            {
              pct = sts->onPct;                     /* Percentage of cycle for on */
              sts->mode |= HAL_LED_MODE_ON;         /* Say it's on */
              HalLedOnOff (led, HAL_LED_MODE_ON);   /* Turn it on */
            }
            if (sts->mode & HAL_LED_MODE_BLINK)
            {
              wait = (((uint32_t)pct * (uint32_t)sts->time) / 100);
              sts->next = time + wait;
            }
            else
            {
              /* no more blink, no more wait */
              wait = 0;
              /* After blinking, set the LED back to the state before it blinks */
              HalLedSet (led, ((preBlinkState & led)!=0)?HAL_LED_MODE_ON:HAL_LED_MODE_OFF);
              /* Clear the saved bit */
              preBlinkState &= (led ^ 0xFF);
            }
          }
          else
          {
            wait = (uint16_t)(sts->next - time);  /* Time left */
          }

          if (!next || ( wait && (wait < next) ))
          {
            next = wait;
          }
        }
        leds ^= led;
      }
      led <<= 1;
      sts++;
    }

    if (next)
    {
      osal_start_timerEx(Hal_TaskID, HAL_LED_BLINK_EVENT, next);   /* Schedule event */
    }
  }
}

/***************************************************************************************************
 * @fn      HalLedOnOff
 *
 * @brief   Turns specified LED ON or OFF
 *
 * @param   leds - LED bit mask
 *          mode - LED_ON,LED_OFF,
 *
 * @return  none
 ***************************************************************************************************/
void HalLedOnOff (uint8_t leds, uint8_t mode)
{
  if (leds & HAL_LED_1)
  {
    if (mode == HAL_LED_MODE_ON)
    {
#ifndef _WIN32
      BSP_LED_Off(USER_LD1);
#else
      BSP_SetLED(0);
#endif
    }
    else
    {
#ifndef _WIN32
      BSP_LED_On(USER_LD1);
#else
      BSP_ClrLED(0);
#endif
    }
  }

  if (leds & HAL_LED_2)
  {
    if (mode == HAL_LED_MODE_ON)
    {
#ifndef _WIN32
      BSP_LED_Off(USER_LD2);
#else
      BSP_SetLED(1);
#endif
    }
    else
    {
#ifndef _WIN32
      BSP_LED_On(USER_LD2);
#else
      BSP_ClrLED(1);
#endif
    }
  }

  if (leds & HAL_LED_3)
  {
    if (mode == HAL_LED_MODE_ON)
    {
#ifndef _WIN32
      BSP_LED_Off(USER_LD3);
#else
      BSP_SetLED(2);
#endif
    }
    else
    {
#ifndef _WIN32
      BSP_LED_On(USER_LD3);
#else
      BSP_ClrLED(2);
#endif
    }
  }

  /* Remember current state */
  if (mode)
  {
    HalLedState |= leds;
  }
  else
  {
    HalLedState &= (leds ^ 0xFF);
  }
}
#endif /* HAL_LED */

/***************************************************************************************************
 * @fn      HalGetLedState
 *
 * @brief   Dim LED2 - Dim (set level) of LED2
 *
 * @param   none
 *
 * @return  led state
 ***************************************************************************************************/
uint8_t HalLedGetState ()
{
#if (HAL_LED == TRUE)
  return HalLedState;
#else
  return 0;
#endif
}

/***************************************************************************************************
***************************************************************************************************/




