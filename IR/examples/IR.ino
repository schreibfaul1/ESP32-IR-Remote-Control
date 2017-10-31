#include "Arduino.h"
#include "IR.h"

#define IR_PIN        34
IR ir(IR_PIN);                  // do not change the objectname, it must be "ir"



void setup()
{
    ir.begin();  // Init InfraredDecoder
}


void loop()
{
    ir.loop();
}


// events called from IR Library
void ir_res(uint32_t res){
    log_e("ir_res %i",res);
}

void ir_number(const char* num){
    log_e("ir_number %s", num);
}

void ir_key(const char* key){
    log_e("ir_key %s", key);
}
