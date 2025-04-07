/*---------------------------------------------------------------------------
 * Copyright (c) 2021 Arm Limited (or its affiliates). All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *---------------------------------------------------------------------------*/

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include "network.h"         // Added
#include "common.h"

// Definitions from demo.cpp (adjust array size if needed)
#define TA_ID 0
#define TA_LEN 15
#define TB_ID 1
#define TB_LEN 3

// Buffer from demo.cpp
__ALIGNED(16) // Make sure __ALIGNED is defined (often in CMSIS headers)
float32_t internal_ta[TA_LEN];

/*---------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/

 static void app_main (void *argument) {
  (void)argument;

  printf("Starting MLHelium demo logic...\r\n");
  osDelay(100); // Small delay

  /* --- Start of logic adapted from demo.cpp --- */

  // You MUST have the definitions for these helper functions available
  // Otherwise the code below will not link or run.

  /* Read first tensor from network description */
  float32_t *ta = get_f32_tensor(network, TA_ID); // Need helper definition
  if (ta != NULL) { // Add NULL checks for safety
   printf("TA:\r\n");
   for(int i=0; i<TA_LEN; i++) {
     // Note: Printing floats requires correct C library configuration
    //  printf("%f\r\n", ta[i]);
     float32_t val = ta[i]; // Or internal_ta[i] or (float32_t)tb[i]
     int32_t int_part = (int32_t)val;
     // Get fractional part scaled up (e.g., 3 decimal places)
     int32_t frac_part = (int32_t)((val - int_part) * 1000.0f); 
     // Handle negative fractional part if value was negative
     if (frac_part < 0) {
         frac_part = -frac_part;
     }
     // Ensure leading zeros for fractional part
     printf("%ld.%03ld\r\n", int_part, frac_part); 
   }
  } else {
    printf("Error getting TA tensor\r\n");
  }
  printf("test \r\n"); // Original printf from demo.cpp

  /* Read second tensor from network description */
  float8_t *tb = get_f8_tensor(network, TB_ID); // Need helper definition
  if (tb != NULL) { // Add NULL checks for safety
   printf("TB:\r\n");
   for(int i=0; i<TB_LEN; i++) {
     // Printing float16_t might require conversion or specific formatting
     // For simplicity, let's assume printf handles it or convert to float32
    //  printf("%f\r\n", (float32_t)tb[i]); // Example: Cast to float32_t for printf
     float32_t val = ta[i]; // Or internal_ta[i] or (float32_t)tb[i]
    int32_t int_part = (int32_t)val;
    // Get fractional part scaled up (e.g., 3 decimal places)
    int32_t frac_part = (int32_t)((val - int_part) * 1000.0f); 
    // Handle negative fractional part if value was negative
    if (frac_part < 0) {
        frac_part = -frac_part;
    }
    // Ensure leading zeros for fractional part
    printf("%ld.%03ld\r\n", int_part, frac_part); 
   }
  } else {
   printf("Error getting TB tensor\r\n");
  }
  printf("\r\n");

  /* Copy first tensor to internal memory */
  copy_tensor((uint8_t*)internal_ta, network, TA_ID); // Need helper definition

  printf("INTERNAL TA:\r\n");
  for(int i=0; i<TA_LEN; i++) {
   printf("%f\r\n", internal_ta[i]);
  }
  printf("\r\n");

  /* --- End of logic adapted from demo.cpp --- */

  printf("MLHelium demo logic finished.\r\n");

  // Decide what to do now - exit, loop, etc.
  // The original Hello World exited after the loop.
  exit(0); // Or loop forever: while(1) { osDelay(osWaitForever); }
}

// ... (app_initialize and main functions remain mostly the same as kiel-hello-main.c) ...

/*---------------------------------------------------------------------------
 * Application initialization
 *---------------------------------------------------------------------------*/
static void app_initialize (void) {
  osThreadNew(app_main, NULL, NULL);
}



/*---------------------------------------------------------------------------
 * C main
 *---------------------------------------------------------------------------*/
int main (void) {

#if defined(RTE_Compiler_EventRecorder) && \
    (defined(__MICROLIB) || \
    !(defined(RTE_CMSIS_RTOS2_RTX5) || defined(RTE_CMSIS_RTOS2_FreeRTOS)))
  EventRecorderInitialize(EventRecordAll, 1U);
#endif

  osKernelInitialize();                 // Initialize CMSIS-RTOS2
  app_initialize();                     // Initialize application
  osKernelStart();                      // Start thread execution

  for (;;) {}
}

