# ESP32-IR-Remote-Control
C++ class for the infrared remote control and reciever module

![Arduino IR kit](https://github.com/schreibfaul1/ESP32-IR-Remote-Control/blob/master/images/Infrarot%20IR%20Empf%C3%A4nger%20Modul%20Wireless%20Remote%20Control%20Kit%20f%C3%BCr%20Arduino.jpg)

### Examplecode:

```` c++
#include "Arduino.h"
#include "src/IR.h"

#define IR_PIN  4
IR ir(IR_PIN);  // do not change the objectname, it must be "ir"

    uint8_t ir_button[20] = {0x52,  // 0     (numbers)                       idx 00
                             0x16,  // 1     (numbers)                       idx 01
                             0x19,  // 2     (numbers)                       idx 02
                             0x0d,  // 3     (numbers)                       idx 03
                             0x0c,  // 4     (numbers)                       idx 04
                             0x18,  // 5     (numbers)                       idx 05
                             0x5e,  // 6     (numbers)                       idx 06
                             0x08,  // 7     (numbers)                       idx 07
                             0x1c,  // 8     (numbers)                       idx 08
                             0x5a,  // 9     (numbers)                       idx 09
                             0x42,  // #     (not a number, short pressed)   idx 10
                             0x4a,  // *     (not a number, short pressed)   idx 11
                             0x44,  // left  (not a number, short pressed)   idx 12
                             0x43,  // right (not a number, short pressed)   idx 13
                             0x46,  // up    (not a number, short pressed)   idx 14
                             0x15,  // down  (not a number, short pressed)   idx 15
                             0x40,  // OK    (not a number, short pressed)   idx 16
                             0x00,  //       (not a number, short pressed)   idx 17
                             0x00,  //       (not a number, short pressed)   idx 18
                             0x00,  //       (not a number, short pressed)   idx 19
                             0x40,  // OK    (not a number,  long pressed)   idx 20
                             0x00,  //       (not a number,  long pressed)   idx 21
                             0x00,  //       (not a number,  long pressed)   idx 22
                             0x00,  //       (not a number,  long pressed)   idx 23
                             0x00,  //       (not a number,  long pressed)   idx 24
                             0x00,  //       (not a number,  long pressed)   idx 25
                             0x00,  //       (not a number,  long pressed)   idx 26
                             0x00,  //       (not a number,  long pressed)   idx 27
                             0x00,  //       (not a number,  long pressed)   idx 28
                             0x00,  //       (not a number,  long pressed)   idx 29
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

void ir_short_key(uint8_t key){
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
