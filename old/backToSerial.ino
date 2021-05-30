#include <MeMCore.h>
#include <Arduino.h>
#include <Wire.h>

MeRGBLed rgbled_7(7, 2);
MeBuzzer buzzer;
MeLEDMatrix ledMtx_1(1);

String command;
bool hasConnected = false;

String x;

void stringCallback(String string) {
    // if (string == ".") {      
    //     processCommand();
    // }
    // if (command == "."){
    //     command = string;
    // }else{
    //     command=command+string;
    // }
    if (string == "b"){
        buzzer.tone(262, 0.25 * 1000);
    }
    
    
}

String mySubstring(String toProcess, char start, char end){
    String toReturn = toProcess.substring(toProcess.indexOf(start), toProcess.indexOf(start));
    toReturn.remove(0);
    
    return toReturn;
}


void processCommand() {
    if (command.charAt(0) == '.') {
        command.remove(0);
    }
    if (command == "b") {
        buzzer.tone(262, 0.25 * 1000);
    }else if (command.charAt(0) == 'l'){
        //buzzer.tone(274, 0.25 * 1000);
        String r = command.substring(command.indexOf('l'),command.indexOf('r'));
        String g = command.substring(command.indexOf('r'),command.indexOf('g'));
        String b = command.substring(command.indexOf('g'),command.indexOf('b'));
        r.remove(0,1);
        g.remove(0,1);
        b.remove(0,1);
        //ledMtx_1.drawStr(0,0+7,output.c_str());
        rgbled_7.setColor(0,r.toInt(),g.toInt(),b.toInt());
        rgbled_7.show();
        
    }
    command = ".";
}
void setup() {
    Serial.begin(115200);
    ledMtx_1.setColorIndex(1);
    ledMtx_1.setBrightness(6);

}

void loop () {
    while (!Serial.available());
    x = Serial.readString();
    if (x == "b") {
        buzzer.tone(262, 0.25 * 1000);
        x ="";
    }


}


