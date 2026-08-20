#include <SoftwareSerial.h>

SoftwareSerial BT(10, 11); // RX, TX


const int leftJoyX  = A0;
const int leftJoyY  = A1;

const int rightJoyX = A2;
const int rightJoyY = A3;

const int leftPot  = A4;
const int rightPot = A5;

const int leftButton  = 2;
const int rightButton = 4;

// Left joystick
const int leftXMin    = 0;
const int leftXCenter = 512;
const int leftXMax    = 1023;

const int leftYMin    = 0;
const int leftYCenter = 512;
const int leftYMax    = 1023;


// Right joystick
const int rightXMin    = 0;
const int rightXCenter = 524;
const int rightXMax    = 1023;

const int rightYMin    = 0;
const int rightYCenter = 512;
const int rightYMax    = 1023;

const float smoothingAlpha = 0.30f;

// Left stick radial deadzone
const float joystickDeadzone = 0.08f;

// Right stick individual-axis deadzone
const float rightDeadzone = 0.08f;

// Left joystick reaches full output here
const float joystickOuterLimit = 1.0f;

float smoothLX = 0.0f;
float smoothLY = 0.0f;

float smoothRX = 0.0f;
float smoothRY = 0.0f;

unsigned long lastSend = 0;

const unsigned long sendInterval = 20; // 50 Hz

================

float normalizeAxis(int raw,int minVal,int centerVal,int maxVal) {

  float value;

  if (raw < centerVal) {

    value =(float)(raw - centerVal) /(float)(centerVal - minVal);

  } else {

    value =(float)(raw - centerVal) /(float)(maxVal - centerVal);

  }

  return constrain(value, -1.0f, 1.0f);
}



float processAxis(float value) {

  float absValue = abs(value);

  if (absValue <= rightDeadzone) {
    return 0.0f;
  }

  float output =(absValue - rightDeadzone) /(1.0f - rightDeadzone);

  output = constrain(output, 0.0f, 1.0f);

  if (value < 0.0f) {
    output = -output;
  }

  return output;
}


float readPot(int pin) {

  float value =analogRead(pin) / 1023.0f;

  // Reverse direction
  value = 1.0f - value;

  return constrain(value, 0.0f, 1.0f);
}

void processLeftJoystick(float x,float y,float &magnitude,float &angleDeg) {

  magnitude = sqrt(x * x + y * y);

  if (magnitude <= joystickDeadzone) {

    magnitude = 0.0f;
    angleDeg = 0.0f;

    return;
  }

  magnitude =(magnitude - joystickDeadzone) /(joystickOuterLimit - joystickDeadzone);

  magnitude =constrain(magnitude, 0.0f, 1.0f);

  angleDeg =
    atan2(y, x) * 180.0f / PI;

  if (angleDeg < 0.0f) {
    angleDeg += 360.0f;
  }
}


uint8_t makeChecksum(uint8_t* data, int length) {

  uint8_t checksum = 0;

  for (int i = 0; i < length; i++) {
    checksum ^= data[i];
  }

  return checksum;
}


void sendPacket(float leftMagnitude,float leftAngleDeg,float rightX,float rightY,float lp,float rp,bool leftPressed,bool rightPressed) {

  uint8_t packet[11];

  uint8_t lMag =(uint8_t)constrain(leftMagnitude * 255.0f,0.0f,255.0f);

  uint16_t lAngle = (uint16_t)constrain(leftAngleDeg,0.0f,359.0f);


  int8_t rX =(int8_t)constrain(rightX * 127.0f,-127.0f,127.0f);


  int8_t rY =(int8_t)constrain(rightY * 127.0f,-127.0f,127.0f);

  uint8_t lPot =(uint8_t)constrain(lp * 255.0f,0.0f,255.0f);


  uint8_t rPot =(uint8_t)constrain(rp * 255.0f,0.0f,255.0f);

  uint8_t buttons = 0;

  if (leftPressed) {
    buttons |= 0b00000001;
  }

  if (rightPressed) {
    buttons |= 0b00000010;
  }

  packet[0] = 0xAA;

  // Left joystick
  packet[1] = lMag;

  packet[2] = lowByte(lAngle);
  packet[3] = highByte(lAngle);


  // Right joystick
  //
  // int8_t gets transmitted as its raw byte.
  //
  // Receiver casts back to int8_t.
  //

  packet[4] = (uint8_t)rX;
  packet[5] = (uint8_t)rY;


  // Pots
  packet[6] = lPot;
  packet[7] = rPot;


  // Buttons
  packet[8] = buttons;


  // Reserved for future use
  packet[9] = 0;


  // Checksum packet[1] through packet[9]
  packet[10] =makeChecksum(&packet[1], 9);


  BT.write(packet, sizeof(packet));
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  BT.begin(9600);

  pinMode(leftButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);
}

void loop() {

  unsigned long now = millis();

  if (now - lastSend < sendInterval) {
    return;
  }

  lastSend = now;


 
  float lx =
    -normalizeAxis(
      analogRead(leftJoyX),
      leftXMin,
      leftXCenter,
      leftXMax
    );


  float ly =
    normalizeAxis(
      analogRead(leftJoyY),
      leftYMin,
      leftYCenter,
      leftYMax
    );


  float rx =
    normalizeAxis(
      analogRead(rightJoyX),
      rightXMin,
      rightXCenter,
      rightXMax
    );


  float ry =
    -normalizeAxis(
      analogRead(rightJoyY),
      rightYMin,
      rightYCenter,
      rightYMax
    );



  smoothLX +=
    smoothingAlpha * (lx - smoothLX);

  smoothLY +=
    smoothingAlpha * (ly - smoothLY);


  smoothRX +=
    smoothingAlpha * (rx - smoothRX);

  smoothRY +=
    smoothingAlpha * (ry - smoothRY);


  float leftMagnitude = 0.0f;
  float leftAngleDeg = 0.0f;

  processLeftJoystick(
    smoothLX,
    smoothLY,
    leftMagnitude,
    leftAngleDeg
  );


  float rightX =
    processAxis(smoothRX);

  float rightY =
    processAxis(smoothRY);



  float lp = readPot(leftPot);
  float rp = readPot(rightPot);


  bool leftPressed =
    digitalRead(leftButton) == LOW;

  bool rightPressed =
    digitalRead(rightButton) == LOW;



  sendPacket(
    leftMagnitude,
    leftAngleDeg,
    rightX,
    rightY,
    lp,
    rp,
    leftPressed,
    rightPressed
  );
}
