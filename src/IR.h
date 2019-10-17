/*
 * IR.h
 *
 *  Created on: 11.08.2017
 *      Author: Wolle
 */

#ifndef IR_H_
#define IR_H_

#include "Arduino.h"
extern __attribute__((weak)) void ir_res(uint32_t res);
extern __attribute__((weak)) void ir_number(const char*);
extern __attribute__((weak)) void ir_key(const char*);

// prototypes
void IRAM_ATTR isr_IR();

class IR {

    private:
        boolean  f_entry=false;  // entryflag
        boolean  f_send=false;   // entryflag
        uint32_t t0;
        uint32_t ir_num=0;
        uint8_t  ir_pin;
        uint8_t  ir_result;
        uint8_t  idx=0;
        char     ir_resultstr[10];
        uint16_t downcount=0;
        int8_t   tmp_resp =(-1);

    protected:
    public:
        IR(uint8_t IR_PIN);
        ~IR();
        void begin();
        void setIRresult(uint8_t result);
        void loop();

};





#endif /* IR_H_ */
