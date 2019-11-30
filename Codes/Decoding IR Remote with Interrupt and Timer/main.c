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

char putchar(char c) {
  while (!TI)
    ;
  TI = 0;
  return (SBUF = c);
}

void main(void) {
  InitialUART0_Timer1(115200);
  printf("main \r\n");
  
  ir_rx_setup();

  while (1) {
    if (is_ir_data_received()) {
      decode_NEC_IR();
    }
  };
}