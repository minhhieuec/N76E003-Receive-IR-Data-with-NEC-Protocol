/*
 *	@author:	MinhHieuEC
 *	@date:		30/11/2019
 */
#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"

#define delay_ms(x) Timer2_Delay500us(x * 2)
#define debug printf

// {0xAA, 0x0A, 0x48, 0xD0, 0x30, 0x22, 0x59, 0x71}
#define sync_high 12000  // 9ms
#define sync_low 6000    // 4.5ms
#define one_high 3600    // 3600 × 0.75ms = 2.7ms
#define one_low 2400     // 2400 × 0.75ms = 1.8ms
#define zero_high 1800   // 1800 × 0.75ms = 1.35ms
#define zero_low 1200    // 1200 × 0.75ms = 0.9ms

//	1 is start ir byte
#define IR_DATA_NUM_BYTES 8
#define IR_RECEIVE_BYTE ((IR_DATA_NUM_BYTES * 8) + 1)

bit received;
unsigned char bits = 0;
unsigned int frames[IR_RECEIVE_BYTE];

bit is_ir_start = 0;
bit is_first_ir_time = 0;

void setup(void);
void set_Timer_0(unsigned int value);
unsigned int get_Timer_0(void);
void erase_frames(void);
unsigned char decode(unsigned char start_pos, unsigned char end_pos);
void decode_NEC(unsigned char *addr, unsigned char *cmd);
void decode_NEC_IR(void);

char putchar(char c) {
  while (!TI)
    ;
  TI = 0;
  return (SBUF = c);
}

void EXTI1_ISR(void) interrupt 2 {
  if (is_ir_start) {
    frames[bits] = get_Timer_0();
    bits++;

    set_Timer_0(0x0000);
    set_TR0;

    if (bits >= IR_RECEIVE_BYTE) {
      received = 1;
      clr_EA;
      clr_TR0;
    }
  } else {
    //	detect first start sync
    if (is_first_ir_time == 0) {
      is_ir_start = 1;
      set_TR0;
      is_first_ir_time = 1;
    } else {
      //	remove end ir send
      is_first_ir_time = 0;
    }
  }

  P03 = ~P03;
}

void main(void) {
  unsigned char address = 0x00;
  unsigned char command = 0x00;
  uint8_t i = 0;

  setup();

  while (1) {
    if (received) {
      for (i = 0; i < IR_RECEIVE_BYTE; i++) {
        debug("frame[%d]: %lld \r\n", (uint16_t)i, (uint32_t)frames[i]);
      }

      // decode_NEC(&address, &command);
      // debug("address: %x \r\n", address);
      // debug("command: %x \r\n", command);
      decode_NEC_IR();

      delay_ms(100);
      erase_frames();
      set_EA;
    }
  };
}

void setup(void) {
  InitialUART0_Timer1(115200);
  printf("main \r\n");

  erase_frames();
  P03_PushPull_Mode;
  TIMER0_MODE1_ENABLE;
  set_Timer_0(0x0000);
  set_IT1;
  set_EX1;
  set_EA;
}

void set_Timer_0(unsigned int value) {
  TH0 = ((value && 0xFF00) >> 8);
  TL0 = (value & 0x00FF);
}

unsigned int get_Timer_0(void) {
  unsigned int value = 0x0000;

  value = TH0;
  value <<= 8;
  value |= TL0;

  return value;
}

void erase_frames(void) {
  for (bits = 0; bits < IR_RECEIVE_BYTE; bits++) {
    frames[bits] = 0;
  }

  set_Timer_0(0x0000);
  received = 0;
  bits = 0;
  is_ir_start = 0;
}

unsigned char decode(unsigned char start_pos, unsigned char end_pos) {
  unsigned char value = 0;

  for (bits = start_pos; bits <= end_pos; bits++) {
    value <<= 1;

    if ((frames[bits] >= one_low) && (frames[bits] <= one_high)) {
      value |= 1;
    }

    else if ((frames[bits] >= zero_low) && (frames[bits] <= zero_high)) {
      value |= 0;
    }

    else if ((frames[bits] >= sync_low) && (frames[bits] <= sync_high)) {
      return 0xFF;
    }
  }

  return value;
}

void decode_NEC(unsigned char *addr, unsigned char *cmd) {
  *addr = decode(1, 8);
  *cmd = decode(17, 24);
}

void decode_NEC_IR(void) {
  uint8_t ir_data[8];
  uint8_t i;

  ir_data[0] = decode(1, 8);
  ir_data[1] = decode(9, 16);
  ir_data[2] = decode(17, 24);
  ir_data[3] = decode(15, 32);

  ir_data[4] = decode(33, 40);
  ir_data[5] = decode(41, 48);
  ir_data[6] = decode(49, 56);
  ir_data[7] = decode(57, 64);

  debug("%x %x %x %x | %x %x %x %x\r\n", (uint16_t)ir_data[0],
        (uint16_t)ir_data[1], (uint16_t)ir_data[2], (uint16_t)ir_data[3],
        (uint16_t)ir_data[4], (uint16_t)ir_data[5], (uint16_t)ir_data[6],
        (uint16_t)ir_data[7]);
}