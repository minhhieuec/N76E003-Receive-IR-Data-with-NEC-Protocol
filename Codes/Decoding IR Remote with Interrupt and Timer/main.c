/*
 *	@author:	MinhHieuEC
 *	@date:		30/11/2019
 */
#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"
#include <N76E003.H>
#include "ir_rx.h"

#define delay_ms(x) Timer2_Delay500us(x * 2)
#define debug printf

uint8_t ir_data[IR_DATA_LEN];
uint8_t led_stt[3] = {0, 0, 0};

char putchar(char c)
{
  while (!TI)
    ;
  TI = 0;
  return (SBUF = c);
}

void main(void)
{
  uint8_t i = 0;

  InitialUART0_Timer1(115200);
  printf("main \r\n");
	
	P12_PushPull_Mode;
	P03_PushPull_Mode;
	P04_PushPull_Mode;
	
	P12 = 0;
	P03 = 0;
	P04 = 0;

  //P05_Input_Mode;
	//Enable_INT_Port0;
  //Enable_BIT5_LowLevel_Trig;
  //Enable_BIT5_FallEdge_Trig;

  ir_rx_setup(IR_USE_EXT1_IT);

  while (1)
  {
    if (is_ir_data_received())
    {
      get_ir_data(&ir_data[0], IR_DATA_LEN);

      if (ir_data[2] == 0x48 && ir_data[3] == 0xb7)
      {
        led_stt[0] = ~led_stt[0];
        printf("P12: %d \n", (uint16_t)led_stt[0]);
        if (led_stt[0])
          P03 = 1;
        else
          P03 = 0;
      }

      if (ir_data[2] == 0x18 && ir_data[3] == 0xE7)
      {
        led_stt[1] = ~led_stt[1];
        printf("P03: %d \n", (uint16_t)led_stt[1]);
        if (led_stt[1])
          P12 = 1;
        else
          P12 = 0;
      }

      if (ir_data[2] == 0x7A && ir_data[3] == 0x85)
      {
        led_stt[2] = ~led_stt[2];
        printf("P04: %d \n", (uint16_t)led_stt[2]);
        if (led_stt[2])
          P04 = 1;
        else
          P04 = 0;
      }

      delay_ms(100);

      printf("ir_rec: ");
      for (i = 0; i < IR_DATA_LEN; i++)
      {
        printf("%x ", (uint16_t)ir_data[i]);
      }
      printf("\n");
    }

    delay_ms(1);
  }
}