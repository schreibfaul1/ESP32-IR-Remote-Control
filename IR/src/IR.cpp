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

    int8_t          ir_resp=(-1);
    uint16_t        address=0;                 // The first 4 bytes of IR code
    uint16_t        command=0;                 // The last 4 bytes of IR code
    uint32_t        t1, intval;                // Current time and interval since last change

    static uint8_t  levelcounter=0;             // Counts the level changes
    static uint8_t  pulsecounter=0;             // Counts the pulse
    static uint32_t t0=0;                       // To get the interval
    static uint32_t ir_value=0;                 // IR code
    static boolean  ir_begin=false;             // set if HIGH/LOW change

    t1=micros();                                // Get current time
    intval=t1 - t0;                             // Compute interval
    t0=t1;                                      // Save for next compare

    if((intval>4000)&&(intval<5000)) {          // begin sequence of code?
        pulsecounter=0;                         // Reset counter
        ir_value=0;
        levelcounter=0;
        ir_begin=true;
        return;
    }

    if(ir_begin==false) return;
    levelcounter++;
    if(levelcounter%2==1)return;                // only falling edge can pass

    if(pulsecounter==32){
        ir_begin=false;
        ir_resp=(-1);
        address=(ir_value&0xFFFF0000)>>16;
        command=(ir_value&0x0000FFFF);
        if(address==0x00FF){
            switch(command){
                case 19125: ir_resp=0;  break; //ZERO   0x4AB5
                case 26775: ir_resp=1;  break; //ONE    0x6897
                case 39015: ir_resp=2;  break; //TWO    0x9867
                case 45135: ir_resp=3;  break; //THREE  0xB04F
                case 12495: ir_resp=4;  break; //FOUR   0x30CF
                case 6375 : ir_resp=5;  break; //FIVE   0x18E7
                case 31365: ir_resp=6;  break; //SIX    0x7A85
                case 4335 : ir_resp=7;  break; //SEVEN  0x10EF
                case 14535: ir_resp=8;  break; //EIGHT  0x38C7
                case 23205: ir_resp=9;  break; //NINE   0x5AA5
                case 765:   ir_resp=10; break; //OK     0x02FD
                case 25245: ir_resp=11; break; //UP     0x629D
                case 43095: ir_resp=12; break; //DOWN   0xA857
                case 49725: ir_resp=13; break; //RIGHT  0xC23D
                case 8925:  ir_resp=14; break; //LEFT   0x22DD
                case 21165: ir_resp=15; break; //HASH   0x52AD
                case 17085: ir_resp=16; break; //STAR   0x42BD
                default: ir_resp=(-1);
            }
            if(ir_resp>(-1))ir.setIRresult(ir_resp);
        }
    }

    if((intval > 500) && (intval < 700)){       // Short pulse?
        ir_value=(ir_value << 1) + 0;           // Shift in a "zero" bit
        pulsecounter++;                         // Count number of received bits
    }
    if((intval > 1500) && (intval < 1700)){     // Long pulse?

        ir_value=(ir_value << 1) + 1;           // Shift in a "one" bit
        pulsecounter++;                         // Count number of received bits
    }
}
