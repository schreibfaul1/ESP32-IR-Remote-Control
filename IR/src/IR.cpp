/*
 * IR.cpp
 *
 *  Created on: 11.08.2017
 *      Author: Wolle
 */
#include "IR.h"

// global var
DRAM_ATTR int8_t ir_resp; // set from isr

IR::IR(uint8_t IR_PIN){
    ir_pin=IR_PIN;
    ir_result=0;
    ir_resp=(-1);
    t0=0;
    //log_i("init");
}

IR::~IR(){

}

void IR::begin(){
    pinMode(ir_pin, INPUT);
    attachInterrupt(ir_pin, isr_IR, CHANGE); // Interrupts will be handle by isr_IR
}
void IRAM_ATTR IR::setIRresult(uint8_t result){
    ir_resp=result;
}
void IR::loop(){
// transform raw data from IR to ir_result

    if(f_entry == false){t0=millis(); f_entry=true;}
    if((t0 + 49 > millis())&&(t0<=millis())) return;   // wait 50ms (not if millis overflow)
    f_entry=false;
    // entry every 50ms in this routine
    if(downcount) downcount--;                             // 1.5 sec countdown
    else{
        if(f_send){
            if(ir_res) ir_res(ir_num);
            //log_i("ir_res %i", ir_num);
            idx=0;
            ir_num=0;
            f_send=false;
        }
    }

    if(tmp_resp == ir_resp) return;   // new value from IR?
    tmp_resp=ir_resp;
    ir_resp=(-1);                     // yes, set ir_resp to default

    if(tmp_resp >= 10){               // it is a function key in ir_result
        switch(tmp_resp){
        case 10: ir_resultstr[0]='k'; break;
        case 11: ir_resultstr[0]='u'; break;
        case 12: ir_resultstr[0]='d'; break;
        case 13: ir_resultstr[0]='r'; break;
        case 14: ir_resultstr[0]='l'; break;
        case 15: ir_resultstr[0]='#'; break;
        case 16: ir_resultstr[0]='*'; break;
        }
        ir_resultstr[1]=0;
        if(ir_key) ir_key(ir_resultstr);
        //log_i("ir_key %s", ir_resultstr);
        downcount=0;
        ir_num=0;
        f_send=false;
        return;
    }
    if(tmp_resp > ( -1)){   // it is a number key in ir_result, can be 0...9
        if(idx < 3){
            ir_resultstr[idx]=tmp_resp + 48;   // convert in ASCII
            idx++;
            ir_resultstr[idx]=0;             // terminate
            if(ir_number) ir_number(ir_resultstr);
            //log_i("ir_number %s", ir_resultstr);
            ir_num=ir_num * 10 + tmp_resp;
            f_send=true;
            downcount=30;          // await nex tmp_resp
        }
    }
}

//**************************************************************************************************
//                                          I S R _ I R                                            *
//**************************************************************************************************
// Interrupts received from VS1838B on every change of the signal.                                 *
// Intervals are 640 or 1640 microseconds for data.  syncpulses are 3400 micros or longer.         *
// Input is complete after 65 level changes.                                                       *
// Only the last 32 level changes are significant.                                                 *
//**************************************************************************************************
void IRAM_ATTR isr_IR()
{
    extern IR ir;
    int8_t ir_resp=(-1);
    static uint32_t t0=0;                        // To get the interval
    uint16_t ir_value = 0;                       // IR code
    uint32_t t1, intval;                         // Current time and interval since last change
    static uint32_t ir_locvalue=0;               // IR code
    static int ir_loccount;                      // Length of code
    uint32_t mask_in=2;                          // Mask input for conversion
    uint16_t mask_out=1;                         // Mask output for conversion
    t1=micros();                                 // Get current time
    intval=t1 - t0;                              // Compute interval
    t0=t1;                                       // Save for next compare
    if((intval > 400) && (intval < 700)){        // Short pulse?
        ir_locvalue=ir_locvalue << 1;            // Shift in a "zero" bit
        ir_loccount++;                           // Count number of received bits
    }
    else if((intval > 1500) && (intval < 1700)){ // Long pulse?

        ir_locvalue=(ir_locvalue << 1) + 1;      // Shift in a "one" bit
        ir_loccount++;                           // Count number of received bits
    }
    else if(ir_loccount == 65){                  // Value is correct after 65 level changes
        t0=0;
        while(mask_in){                          // Convert 32 bits to 16 bits
            if(ir_locvalue & mask_in) {          // Bit set in pattern?
                ir_value|=mask_out;              // Set set bit in result
            }
            mask_in<<=2;                         // Shift input mask 2 positions
            mask_out<<=1;                        // Shift output mask 1 position
        }
        ir_loccount=0;                           // Ready for next input
        switch(ir_value){
        case 19125: ir_resp=0;  break; //ZERO
        case 26775: ir_resp=1;  break; //ONE
        case 39015: ir_resp=2;  break; //TWO
        case 45135: ir_resp=3;  break; //THREE
        case 12495: ir_resp=4;  break; //FOUR
        case 6375 : ir_resp=5;  break; //FIVE
        case 31365: ir_resp=6;  break; //SIX
        case 4335 : ir_resp=7;  break; //SEVEN
        case 14535: ir_resp=8;  break; //EIGHT
        case 23205: ir_resp=9;  break; //NINE
        case 765:   ir_resp=10; break; //OK
        case 25245: ir_resp=11; break; //UP
        case 43095: ir_resp=12; break; //DOWN
        case 49725: ir_resp=13; break; //RIGHT
        case 8925:  ir_resp=14; break; //LEFT
        case 21165: ir_resp=15; break; //HASH
        case 17085: ir_resp=16; break; //STAR
        default: ir_resp=(-1);
        }
        if(ir_resp>(-1))ir.setIRresult(ir_resp);
    }
    else if((intval>4000)&&(intval<5000)) {
        ir_loccount=0;          // Reset decoding
        ir_locvalue=0;
    }
    else
    {
        // do nothing
    }
}
