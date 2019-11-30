#ifndef _IR_RX_H_
#define _IR_RX_H_
#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"

void ir_rx_setup(void);
uint8_t is_ir_data_received(void);
void erase_frames(void);
void decode_NEC_IR(void);

#endif