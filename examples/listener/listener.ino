#include <Keypad.h>

class Listener : public KeypadListener {
 void keyStateChanged(Keypad* source, 
           int keyindex, KeyState olds, KeyState news){
    Serial.write('Key State Changed');
    Serial.print(keyindex);
    Serial.write(',');
    Serial.print(olds);
    Serial.write(',');
    Serial.print(news);
    Serial.write('\n');
 } 
};

char keys[16] = {'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
int rowpins[4] = {22,24,26,28};
int colpins[4] = {23,25,27,29};

Keypad keypad = Keypad(keys,rowpins,colpins,4,4);
Listener listener;


void setup() {
   Serial.begin(9600);
   keypad.init();
   keypad.setListener(&listener);
}

void loop() {
    keypad.getPressed();
}
