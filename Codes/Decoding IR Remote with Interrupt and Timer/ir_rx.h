#ifndef _IR_RX_H_
#define _IR_RX_H_
#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"

#define IR_DATA_LEN 4 //  8 bytes

typedef enum
{
    IR_USE_PIN_IT = 0, // use pin interrupt
    IR_USE_EXT0_IT,    // use external interrupt 0
    IR_USE_EXT1_IT,    // use external interrupt 1
} IR_Type_t;

void ir_rx_setup(IR_Type_t ir_type);
uint8_t is_ir_data_received(void);
void get_ir_data(uint8_t *buf, uint8_t len);

#endif