#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_BNO08x_RVC.h>
#include <SoftwareSerial.h>

SoftwareSerial BT(10, 11); // RX, TX


Adafruit_PWMServoDriver pca0 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pca1 = Adafruit_PWMServoDriver(0x41);


Adafruit_BNO08x_RVC bno;
SoftwareSerial imuSerial(8, 9);  // RX, TX


struct Joint {
  int pca;
  int channel;
};


struct Point {
  float px;
  float py;
  float pz;
};

struct Leg {
  Joint coxa;
  Joint hip;
  Joint knee;
  float phii;
  Point cb;
};

float leftMagnitude;
float leftAngleDeg;
float rightX;
float rightY;
float lp;
float rp;
bool leftPressed;
bool rightPressed;

Leg legs[6] = {
  { {0x40, 2}, {0x40, 1}, {0x40, 0}, 0.524, {121,0,70} }, // leg 0 left front
  { {0x40, 5}, {0x40, 4}, {0x40, 3}, 1.57 , {0,0,140} }, // leg 1 left middle
  { {0x41, 6}, {0x41, 7}, {0x41, 8}, 2.62, {-121,0,70} }, // leg 2 left back

  { {0x41, 0}, {0x41, 1}, {0x41, 2}, 5.76, {121,0,-70} }, // leg 3 right front
  { {0x40, 6}, {0x40, 7}, {0x40, 8}, 4.71, {0,0,-140} }, // leg 4 right middle
  { {0x41, 3}, {0x41, 4}, {0x41, 5}, 3.67, {-121,0,-70} }  // leg 5 right back
};



struct path {
  float s;
  float h;
};

float legSinPhi[6];
float legCosPhi[6];

const int footpins[6] = {2,3,4,5,6,7};
bool foot_touch[6];

float pitch = 0.0;
float roll = 0.0;
float yaw = 0.0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  BT.begin(9600);
  BT.listen();
  

  pca0.begin();
  pca1.begin();

  Wire.setClock(400000);

  pca0.setPWMFreq(50);

  pca1.setPWMFreq(50);

  /*imuSerial.begin(115200);

  if (!bno.begin(&imuSerial)) {
    Serial.println("BNO085 not found!");
    while (1);
  }*/

  Serial.println("BNO085 ready");


  for (int i =0; i <6; i++) {
    legSinPhi[i] = sin(legs[i].phii);
    legCosPhi[i] = cos(legs[i].phii);
    pinMode(footpins[i], INPUT_PULLUP);
  }

  delay(100);
  
  Serial.println("Setup done");
}

unsigned long lastStep = 0;

const unsigned long stepInterval = 20;  // milliseconds

float stridelength;
float strideheight = 100;
float direction;
float speed;
float turn = 0;


void loop() {
  //movement();
  // ALWAYS check Bluetooth as frequently as possible
  receiveBT();

  direction = leftAngleDeg*PI/180;
  speed = lp*0.06;
  stridelength = rp*180;
  turn = rightX;
  strideheight += rightY;
  strideheight = constrain(strideheight, 0.0f, 150.0f);



  unsigned long now = millis();


  // Robot command update at 50 Hz
  if (now - lastStep >= stepInterval) {

    lastStep = now;


    //tilt(rightMagnitude * 10.0f,rightAngleDeg);
    
    if (leftMagnitude > 0.08 || abs(turn) > 0.08) {

      walk(direction,speed,stridelength, 100, turn, leftMagnitude);

    }
  }
}
