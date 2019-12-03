/*
 *	@author:	MinhHieuEC
 *	@date:		30/11/2019
 */
#include "ir_rx.h"

#define delay_ms(x) Timer2_Delay500us(x * 2)
#define debug printf

#define sync_high          17000 // 22000 Ã 0.75ms = 16.5ms
#define sync_low           12000 // 14000 Ã 0.75ms = 10.5ms
#define one_high            3600 // 3600 Ã 0.75ms = 2.7ms
#define one_low             2000 // 2400 Ã 0.75ms = 1.8ms
#define zero_high           1800 // 1800 Ã 0.75ms = 1.35ms
#define zero_low            800 // 1200 Ã 0.75ms = 0.9ms

//	1: start ir byte + IR_DATA_NUM_BYTES*8 bytes data
#define IR_DATA_NUM_BYTES 8
#define IR_RECEIVE_BYTE ((IR_DATA_NUM_BYTES * 8) + 1)

bit received = 0;
unsigned char bits = 0;
unsigned int frames[IR_RECEIVE_BYTE] = {0};

bit is_ir_start = 0;
bit is_first_ir_time = 0;

void set_Timer_0(unsigned int value);
unsigned int get_Timer_0(void);
unsigned char decode(unsigned char start_pos, unsigned char end_pos);

void EXTI1_ISR(void) interrupt 2 {
	frames[bits] = get_Timer_0();

  // detect start
  if (frames[bits] > 17000 && frames[bits] < 20000) is_ir_start = 1;

  if (is_ir_start) bits++;

  set_Timer_0(0x0000);
  set_TR0;

  //  frame error detect
  if (frames[bits] < 0) {
    is_ir_start = 0;
    bits = 0;
  }

  if (bits >= IR_RECEIVE_BYTE) {
    received = 1;
    clr_EA;
    clr_TR0;
  }
}

/*
*	@brief: setup neccessary things for read IR data
* - setup timer0 mode 1
*   timer_tick = 12/16MHz = 0.75us
* - setup INT1 and enable all interrupt
*/
void ir_rx_setup(void) {
  erase_frames();
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

void decode_NEC_IR(void) {
  uint8_t ir_data[8];

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
	
	delay_ms(200);
	
	erase_frames();
  set_EA;	//	enable all interrupt
}

uint8_t is_ir_data_received(void) { return received; }