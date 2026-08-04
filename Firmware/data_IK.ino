

float L1 = 65;
float L2 = 110;
float L3 = 165;

float phi;
float theta1;
float theta2;

float coxaShaft;
float femurShaft;
float tibiaShaft;

bool returnIK(float x,float y,float z) {

  float h = sqrt(x*x+z*z);
  float u = h - L1;
  float d = sqrt(u*u+y*y);

  if (d > L2+L3) {

    return false;
  }

  if (d < abs(L2-L3)) {

    return false;
  }

  

  else {

    phi = atan2(x,z) * 180/PI;
    theta1 = (atan2(y,u) + acos((L2*L2+d*d-L3*L3)/(2*L2*d))) * 180/PI;
    theta2 = 180 - acos((L3*L3+L2*L2-d*d)/(2*L2*L3)) * 180/PI; // 180 minus because knee servo is backwards

    coxaShaft  = 90.0 + phi;
    femurShaft = 90 + theta1;
    tibiaShaft = theta2;

 
    if (tibiaShaft > 180 || tibiaShaft < 0) {
      
      tibiaShaft = 90;

    }

    if (coxaShaft < 30 || coxaShaft > 150) {
      return false;
    }


    return true;
  }

}


void writeIK(int leg, float x, float y, float z) {

  if (!returnIK(x,y,z)) {
    //Serial.println("IK failed");
    return;
  } 


  writeServo(legs[leg].coxa.pca, legs[leg].coxa.channel, coxaShaft); //coxa
  writeServo(legs[leg].hip.pca, legs[leg].hip.channel, femurShaft); //hip
  writeServo(legs[leg].knee.pca, legs[leg].knee.channel, tibiaShaft); //knee

}
