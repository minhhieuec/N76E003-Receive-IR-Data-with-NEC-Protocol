# N76E003_IR_Receiver_NEC
Receive ir data from ir remote with intterupt and timer capture.

To test this code, please using IR_TX_SOURCE at https://github.com/minhhieuec/Send_IR_Using_PWM_N76E003 or any ir remote which use NEC Protocol.

References:
- https://www.sbprojects.net/knowledge/ir/nec.php
- http://embedded-lab.com/blog/getting-started-nuvoton-8-bit-microcontrollers-coding-part-3/

## How to use
This code support receive ir data using pin intterupt or external interrupt on N76E003.
To config number byte receive of data, please config:

`#define IR_DATA_LEN 4 //  8 bytes`

### Pin interrupt
N76E003 support pin intterupt at any port. So you can choose any pin to use.

Note that: before call `ir_rx_setup(IR_USE_PIN_IT);` function, you need to config gpio pin for your ir receive led pin.
Example:
```c
P05_Input_Mode;
Enable_INT_Port0;
Enable_BIT5_FallEdge_Trig;
ir_rx_setup(IR_USE_PIN_IT);
```

At this time, this code only support intterupt on P05 pin. I will support for any pin in future.

### External Interrupt (INT0, INT1)
When you use this mode, You don't need to config gpio for it. You only call `ir_rx_setup(IR_USE_PIN_IT);` function in setup.

At this time, this code only support INT1 intterrupt. I will support for INT0 later.

## Schematic
![IR RX Schematic](https://github.com/minhhieuec/N76E003_IR_Receiver_NEC/blob/master/schematic.png)

## Log
![Debug Information](https://github.com/minhhieuec/N76E003_IR_Receiver_NEC/blob/master/printf.png)
