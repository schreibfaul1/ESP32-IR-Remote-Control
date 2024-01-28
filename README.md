# ESP32-IR-Remote-Control
C++ class for the infrared remote control and reciever module 

![Arduino IR kit](https://github.com/schreibfaul1/ESP32-IR-Remote-Control/blob/master/images/Infrarot%20IR%20Empf%C3%A4nger%20Modul%20Wireless%20Remote%20Control%20Kit%20f%C3%BCr%20Arduino.jpg)

### Examplecode:

```` c++
#include "Arduino.h"
#include "src/IR.h"

#define IR_PIN  4
IR ir(IR_PIN);  // do not change the objectname, it must be "ir"

    uint8_t ir_button[20] = {0x52,  // 0
                             0x16,  // 1
                             0x19,  // 2
                             0x0d,  // 3
                             0x0c,  // 4
                             0x18,  // 5
                             0x5e,  // 6
                             0x08,  // 7
                             0x1c,  // 8
                             0x5a,  // 9
                             0x42,  // #
                             0x4a,  // *
                             0x44,  // left
                             0x43,  // right 
                             0x46,  // up
                             0x15,  // down
                             0x40,  // OK 
                             0x00, 
                             0x00, 
                             0x00
    };

//--------------------------------------------------------------
void setup(){
    Serial.begin(115200);
    ir.begin();
    for(uint8_t i = 0; i < 20; i++) {
        ir.set_irButtons(i, ir_button[i]);
    }
    ir.set_irAddress(0x00);
}

void loop(){
    ir.loop();
    // do something else
    // ...
}
//--------------------------------------------------------------
//   events called from IR Library
//--------------------------------------------------------------
void ir_res(uint32_t res){
    Serial.print("ir_res: ");
    Serial.println(res);
}

void ir_number(uint16_t num){
    Serial.print("ir_number: ");
    Serial.println(num);
}

void ir_key(uint8_t key){
    Serial.print("ir_key: ");
    Serial.println(key);
}

void ir_long_key(int8_t key){
    Serial.print("long pressed key nr: ");
    Serial.println(key);
}

void ir_code(uint8_t addr, uint8_t cmd){
    Serial.printf("ir_code: ..  IR address 0x%02x, IR command 0x%02x\n", addr, cmd);
}
````
### console output (pressed # 123):

![Serial Console Output](https://github.com/schreibfaul1/ESP32-IR-Remote-Control/blob/master/images/IR_ConsoleOutput.jpg)

### recieved codewods for this RC:

![RC Code](https://github.com/schreibfaul1/ESP32-IR-Remote-Control/blob/master/images/RemoteControl.jpg)

### VS1838 pins;

![VS1838B Pins](https://github.com/schreibfaul1/ESP32-IR-Remote-Control/blob/master/images/VS1838B%20Pins.jpg)
