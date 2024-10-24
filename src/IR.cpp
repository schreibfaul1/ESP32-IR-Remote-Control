
/*
 * IR.cpp
 *
 *  Created on: 11.08.2017
 *      Author: Wolle
 *  Updated on: 24.10.2024
 */
#include "IR.h"


// global var
DRAM_ATTR int16_t ir_cmd_a = -1; // set from isr
DRAM_ATTR int16_t ir_cmd_b = -1; // set from isr
DRAM_ATTR int16_t ir_adr_a = 0;  // set from isr
DRAM_ATTR int16_t ir_adr_b = 0;  // set from isr
DRAM_ATTR uint8_t ir_rc = 0;
DRAM_ATTR uint8_t ir_addressCode;
DRAM_ATTR uint8_t g_ir_pin;

DRAM_ATTR uint32_t ir_intval_l = 0;
DRAM_ATTR uint32_t ir_intval_h = 0;
DRAM_ATTR int16_t ir_pulsecounter = 0;

IR::IR(int8_t IR_Pin){
    m_ir_pin = IR_Pin;
    ir_adr_b = -1;
    m_t0 = 0;
    ir_cmd_b = -1;
    m_f_error = false;

    m_ir_buttons[ 0] = 0xFFFF; // 0xFFFF -> -1 unused
    m_ir_buttons[ 1] = 0xFFFF; //
    m_ir_buttons[ 2] = 0xFFFF; //
    m_ir_buttons[ 3] = 0xFFFF; //
    m_ir_buttons[ 4] = 0xFFFF; //
    m_ir_buttons[ 5] = 0xFFFF; //
    m_ir_buttons[ 6] = 0xFFFF; //
    m_ir_buttons[ 7] = 0xFFFF; //
    m_ir_buttons[ 8] = 0xFFFF; //
    m_ir_buttons[ 9] = 0xFFFF; //
    m_ir_buttons[10] = 0xFFFF; //  short pressed command
    m_ir_buttons[11] = 0xFFFF; //  short pressed command
    m_ir_buttons[12] = 0xFFFF; //  short pressed command
    m_ir_buttons[13] = 0xFFFF; //  short pressed command
    m_ir_buttons[14] = 0xFFFF; //  short pressed command
    m_ir_buttons[15] = 0xFFFF; //  short pressed command
    m_ir_buttons[16] = 0xFFFF; //  short pressed command
    m_ir_buttons[17] = 0xFFFF; //  short pressed command
    m_ir_buttons[18] = 0xFFFF; //  short pressed command
    m_ir_buttons[19] = 0xFFFF; //  short pressed command
    m_ir_buttons[20] = 0xFFFF; //   long pressed command
    m_ir_buttons[21] = 0xFFFF; //   long pressed command
    m_ir_buttons[22] = 0xFFFF; //   long pressed command
    m_ir_buttons[23] = 0xFFFF; //   long pressed command
    m_ir_buttons[24] = 0xFFFF; //   long pressed command
    m_ir_buttons[25] = 0xFFFF; //   long pressed command
    m_ir_buttons[26] = 0xFFFF; //   long pressed command
    m_ir_buttons[27] = 0xFFFF; //   long pressed command
    m_ir_buttons[28] = 0xFFFF; //   long pressed command
    m_ir_buttons[29] = 0xFFFF; //   long pressed command
}

void IR::begin(){
    if(m_ir_pin >= 0){
        g_ir_pin = m_ir_pin;
        pinMode(m_ir_pin, INPUT_PULLUP);
        attachInterrupt(m_ir_pin, isr_IR, CHANGE); // Interrupts will be handle by isr_IR
    }
}

IR::~IR(){
    ;
}

void IR::set_irButtons(uint8_t btnNr,  uint8_t cmd){
        m_ir_buttons[btnNr] = cmd;
        // log_w("ButtonNumber: %i, Command: 0x%02x", btnNr, cmd);
}

int16_t* IR::get_irButtons(){
    return m_ir_buttons;
}

void IR::set_irAddress(uint8_t addr){
    ir_addressCode = addr;
}

int16_t IR::get_irAddress(){
    return ir_addressCode;
}


void IRAM_ATTR IR::setIRresult(uint8_t userCode_a, uint8_t userCode_b, uint8_t dataCode_a, uint8_t dataCode_b){
    ir_cmd_a = dataCode_a;
    ir_cmd_b = dataCode_b;
    ir_adr_a = userCode_a;
    ir_adr_b = userCode_b;
}

void IRAM_ATTR IR::rcCounter (uint8_t rc){
        ir_rc = rc;
}

void IRAM_ATTR IR::error(uint32_t intval_l, uint32_t intval_h, uint8_t pulsecounter){
        ir_intval_l = intval_l;
        ir_intval_h = intval_h;
        ir_pulsecounter = pulsecounter;
        m_f_error = true;
}


void IR::loop(){ // transform raw data from IR to ir_result
    static uint16_t number = 0;
    static uint32_t t_loop = millis();
    static bool found_number = false;
    static bool found_short = false;
    static bool found_long = false;
    static bool wait = false;


    if(wait){ // waiting for repeat code
        if(t_loop + 150 < millis()){
            wait = false;
        }
        else{
            return;
        }
    }

    if(ir_cmd_a != -01){  // -001 is idle
        t_loop = millis();
        wait = true;
        if(ir_cmd_a == -100){ ir_cmd_a = - 01; goto long_pressed;}  // -100 is repeat code
        if(ir_cmd_a + ir_cmd_b != 0xFF){ ir_cmd_a = - 01; return;}

        if(ir_code) ir_code(ir_adr_a, ir_cmd_a);
        // log_i("ir_adr_a %i  ir_adr_b %i ir_cmd_a %i  ir_cmd_b %i", ir_adr_a, ir_adr_b, ir_cmd_a, ir_cmd_b);
        if(ir_adr_a != ir_addressCode){ir_cmd_a = -01; return;}
        m_t0 = millis();
        for(uint8_t i = 0; i < 30; i++){
            if(ir_cmd_a == m_ir_buttons[i]){
                if(i <= 9){
                    found_number = true;
                    uint8_t digit = i;
                    number *= 10;
                    number += digit;
                }
                if(i >= 10 && i <= 19){ // is not a number but short cmd
                    found_short = true;
                    m_short_key = i;
                    m_t1 = millis();
                }
                if(i >= 20){  // is not a number but long cmd
                    found_long = true;
                    m_long_key = i;
                    m_t1 = millis();
                }
            }
        }
        if(found_number){if(ir_number) ir_number(number);}
        if(!found_number && !found_short && !found_long){
            log_w("No function has been assigned to the code 0x%02x", ir_cmd_a);
            return;
        }
        ir_cmd_a = -01;
        goto exit;
    }

    if(found_short && m_t1 + 120 < millis()){
        if(ir_rc < 14) if(ir_short_key) ir_short_key(m_short_key); // short pressed
        m_short_key = -1;
        found_short = false;
    }
    if(found_number && (m_t0 + 2000 < millis())){
        if(ir_res) ir_res(number);
        number = 0;
        found_number = false;
    }

long_pressed:
    if(found_long && (m_t0 + 2000 < millis())){
        if(ir_rc > 14){
            if(ir_long_key) ir_long_key(m_long_key); // long pressed
        }
        m_long_key = -1;
        found_long = false;
    }
exit:
    if(m_f_error){
        log_d("something went wrong, intval_l %d, intval_h %d, pulsecounter %d", ir_intval_l, ir_intval_h, ir_pulsecounter);
        m_f_error = false;
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
void IRAM_ATTR isr_IR(){

    extern IR ir;

    uint16_t        userCode = 0;                       // The first 4 bytes of IR code
    uint16_t        dataCode = 0;                       // The last 4 bytes of IR code
    int32_t         t1=0, intval_h=0, intval_l = 0;     // Current time and interval since last change

    static uint8_t  pulsecounter=0;                     // Counts the pulse
    static uint32_t t0 = 0;                             // To get the interval
    static uint32_t ir_value=0;                         // IR code
    static uint64_t bit = 0x00000001;

    static boolean f_AGC = 0;                          // AGC pulse 9000µs (negative)
    static boolean f_LP = 0;                           // LP pulse 4500µs (positive)
    static boolean f_BURST  = 0;                       // BURST pulse 562.5µs (negative)
    static boolean f_P = 0;                            // SPACE pulse 2250µs (positive), in repeat code
    static boolean f_RC = false;                       // repeat code sequence received
    static uint8_t RC_cnt = 0;                         // repeat code counter

    t1 = micros();                                      // Get current time
    if(!digitalRead(g_ir_pin)) intval_h = t1 - t0;      // Compute interval, only high
    else                       intval_l = t1 - t0;      // Compute interval, only low
    t0 = t1;                                            // Save for next compare

    if(intval_l >= 8000 && intval_l < 10000){           // 9000µs AGC
        f_AGC = true;
        f_LP = false;
        return;
    }

    if(f_AGC && !f_LP){
        if((intval_h >= 3500)&&(intval_h <= 5500)) {    // begin sequence of code?
            f_LP = true;
            f_AGC = false;
            pulsecounter = 0;                           // Reset counter
            ir_value = 0;
            bit = 0x00000001;
            RC_cnt = 0;
            return;
        }
        // else fall through
    }

    if(f_LP){
        if((intval_h > 400) && (intval_h < 750)){  // Logical '0' – a 562.5µs pulse burst followed by a 562.5µs space
            if(f_BURST){
                f_BURST = false;
                bit <<= 1;
                pulsecounter++;
                return;
            }
        }

        if((intval_h > 1500) && (intval_h < 1700)){ // Logical '1' – a 562.5µs pulse burst followed by a 1.6875ms space
            if(f_BURST){
                f_BURST = false;
                ir_value += bit;
                bit <<= 1;
                pulsecounter++;
            }
            return;
        }

        if((intval_l > 400) && (intval_l < 750)){
            f_BURST = true;
            if(pulsecounter < 32) return;
            // last p_BURST
            userCode =  ir_value & 0x0000FFFF;          // aka address
            dataCode = (ir_value & 0xFFFF0000) >> 16;   // aka command
            uint8_t a, b, c, d;
            a = (userCode & 0x00FF);      // Extended NEC protocol: Address high
            b = (userCode & 0xFF00) >> 8; // Extended NEC protocol: Address low
            d = (dataCode & 0xFF00) >> 8;
            c = (dataCode & 0x00FF);
            ir.setIRresult(a, b, c, d);

            pulsecounter++;
            f_LP = false;
            return;
        }
        // else fall through
    }


    if(f_AGC){  // repeat code
        if((intval_h > 1700) && (intval_h < 2800)){ // repeat code 2250µs
            f_P = true;
            return;
        }
       if(f_P){
            if((intval_l > 400) && (intval_l < 750)){
                f_AGC = false;
                f_RC = true;
            }
       }
    }
    if(f_RC){
        f_RC = false;
        RC_cnt++;
        ir.rcCounter(RC_cnt);
        ir_cmd_a = -100;
        return;
    }

    if(intval_l) ir.error( intval_l, intval_h, pulsecounter);
}
//------------------------------------------------------------------------------------------------------------------
