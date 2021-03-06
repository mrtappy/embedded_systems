/**
  @page PWR_STOP PWR_STOP
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    PWR/STOP/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Description of the PWR STOP example.
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
   @endverbatim

@par Example Description 

This example shows how to enters the system to STOP mode and wake-up from this
mode using RTC Wakeup Timer Event connected to EXTI Line 22.

In the associated software
  - the system clock is set to 120 MHz
  - the EXTI_Line15 is configured to generate interrupt on falling edge
  - the EXTI_Line22 connected internally to the RTC Wakeup event is configured
    to generate an interrupt on rising edge each 4s
  - the SysTick is programmed to generate an interrupt each 250 ms 
In the SysTick interrupt handler, LED2 is toggled, this is used to indicate whether
the MCU is in STOP or RUN mode.

The system enters STOP mode and will wait for the RTC Wakeup event to be 
generated each 4s, or Key push button is pressed. 
 - If the RTC WakeUp event (EXTI_Line22) is the source of wakeup from STOP, LED1 is toggled.
 - If the Key button (EXTI_Line15) is the source of wakeup from STOP, LED4 is toggled.

This behavior is repeated in an infinite loop.

LEDs are used to monitor the system state as following:
 - LED2 toggling: system in RUN mode
 - LED1 toggled: system woken up from STOP using RTC WakeUp Interrupt
 - LED4 toggled: system woken up from STOP using EXTI_Line15 (KEY push button)
 
 @note To measure the current consumption in STOP mode, please refer to 
      @subpage PWR_CurrentConsumption example.

 @note This example can not be used in DEBUG mode, this is due to the fact 
       that the Cortex?-M3 core is no longer clocked during low power mode 
       so debugging features are disbaled



@par Directory contents 

  - PWR/STOP/system_stm32f2xx.c   STM32F2xx system clock configuration file
  - PWR/STOP/stm32f2xx_conf.h     Library Configuration file
  - PWR/STOP/stm32f2xx_it.c       Interrupt handlers
  - PWR/STOP/stm32f2xx_it.h       Header for stm32f2xx_it.c
  - PWR/STOP/main.c               Main program
  
@note The "system_stm32f2xx.c" is generated by an automatic clock configuration 
      tool and can be easily customized to your own configuration. 
      To select different clock setup, use the "STM32F2xx_Clock_Configuration_V1.0.0.xls" 
      provided with the AN3362 package available on <a href="http://www.st.com/internet/mcu/family/141.jsp">  ST Microcontrollers </a>

      
@par Hardware and Software environment 

  - This example runs on STM32F2xx Devices.
  
  - This example has been tested with STM322xG-EVAL RevB and can be easily tailored
    to any other development board. 

  - STM322xG-EVAL Set-up
    - Use LED1, LED2 and LED4 connected respectively to PG.06, PG.08 and PC.07 pins
    - Use the Key push-button connected to pin PG15 (EXTI_Line15)


@par How to use it ? 

In order to make the program work, you must do the following :
 - Copy all source files from this example folder to the template folder under
   Project\STM32F2xx_StdPeriph_Template
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */


