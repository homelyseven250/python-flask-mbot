#include <MeMCore.h>
#include <Arduino.h>
// #include <Wire.h>

MeRGBLed rgbled_7(7, 2);
MeBuzzer buzzer;
MeLEDMatrix ledMtx_1(1);


MeDCMotor motor_9(9);
MeDCMotor motor_10(10);
MeLightSensor lightsensor_6(6);

const byte numChars = 32;
int ledX;
int ledY;
unsigned char drawBuffer[16];
unsigned char *drawTemp;
char txtToShow[numChars] = {0};
bool isShowingText = false;
int lastCharTime = 0;
int txtLoopCount = 0;
String textToShow = "";

unsigned char toDraw[16][8] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int numCounts[] = { 128, 64, 32, 16, 8, 4, 2, 1 };

void renderToDraw() {
  drawTemp = new unsigned char[16]{};
  for (int x = 0; x < 8; x++) {
    for (int i = 0; i < 16; i++) {
      if (toDraw[i][x] == 1) {
        drawTemp[i] = drawTemp[i] + numCounts[x];
      }
    }
  }
  

  memcpy(drawBuffer, drawTemp, 16);
  free(drawTemp);
  ledMtx_1.drawBitmap(0, 0, 16, drawBuffer);
}



// Example 5 - Receive with start- and end-markers combined with parsing


char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

      // variables to hold the parsed data
char messageFromPC[numChars] = {0};
int direction = 0;
int speed = 0;

boolean newData = false;

void move(int direction, int speed) {
  int leftSpeed = 0;
  int rightSpeed = 0;
  if(direction == 1) {
    leftSpeed = speed;
    rightSpeed = speed;
  } else if(direction == 2) {
    leftSpeed = -speed;
    rightSpeed = -speed;
  } else if(direction == 3) {
    leftSpeed = -speed;
    rightSpeed = speed;
  } else if(direction == 4) {
    leftSpeed = speed;
    rightSpeed = -speed;
  }
  motor_9.run((9) == M1 ? -(leftSpeed) : (leftSpeed));
  motor_10.run((10) == M1 ? -(rightSpeed) : (rightSpeed));
}

//============

void setup() {
    Serial.begin(115200);
    ledMtx_1.setColorIndex(1);
    ledMtx_1.setBrightness(6);
}

//============

void loop() {
    recvWithStartEndMarkers();
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        parseData();
        newData = false;
    }
    showText();
}

//============

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//============

void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
    if (*messageFromPC == 'b') {
        buzzer.tone(262, 0.25 * 1000);
        Serial.println("<rBuzzer should have triggered");
    } else if (*messageFromPC == 'm') {
        strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
        direction = atoi(strtokIndx);     // convert this part to an integer
        strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
        speed = atoi(strtokIndx);     // convert this part to an integer
        move(direction,speed / 100.0 * 255);
        if (speed == 0) {
            Serial.println("<rStopped");
        } else if (direction == 1) {
            Serial.println("<rMoving forward at speed " + String(speed));
        } else if (direction == 2) {
            Serial.println("<rMoving backward at speed " + String(speed));
        } else if (direction == 3) {
            Serial.println("<rMoving right at speed " + String(speed));
        } else {
            Serial.println("<rMoving left at speed " + String(speed));
        }
    }else if(*messageFromPC == 's') {
        Serial.println(lightsensor_6.read());
    } else if(*messageFromPC == 'l') {
        strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
        ledX = atoi(strtokIndx);     // convert this part to an integer
        strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
        ledY = atoi(strtokIndx);     // convert this part to an integer
        if (toDraw[ledX][ledY] == 0) {
            toDraw[ledX][ledY] = 1;
            Serial.println("<rTurned led on");
        } else {
            toDraw[ledX][ledY] = 0;
            Serial.println("<rTurned led off");
        }
        renderToDraw();
    } else if (*messageFromPC == 'c') {
        for (int x = 0; x < 8; x++) {
            for (int i = 0; i < 16; i++) {
                toDraw[i][x] = 0;                
            }
        }
        ledMtx_1.clearScreen();
    } else{
        txtLoopCount = 0;
        textToShow = receivedChars;
        ledMtx_1.drawStr(0,0+7,String(textToShow[txtLoopCount]).c_str());
        isShowingText = true;
        lastCharTime = millis();

    }


    // strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    // integerFromPC = atoi(strtokIndx);     // convert this part to an integer

    // strtokIndx = strtok(NULL, ",");
    // floatFromPC = atof(strtokIndx);     // convert this part to a float

}

//============
void showText(){
    if (txtLoopCount > sizeof(textToShow)) {
        isShowingText = false;
        txtLoopCount = 0;
    }
    if ((millis() - lastCharTime) >= 500 && isShowingText) {
        txtLoopCount++;
        ledMtx_1.drawStr(0,0+7,String(receivedChars[txtLoopCount]).c_str());
        lastCharTime = millis();
    }
}