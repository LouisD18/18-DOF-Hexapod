
float x,y,z;
float offset_z = 150;
float ground_y = -150;
float u;
float g_t1;
float g_t2 = 0.5;

float contactT[6] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

bool justTouched[6];
bool lastFootTouch[6];

bool landed[6] = {false, false, false, false, false, false};


float phase[6] = {0, 0.5, 0, 0.5, 0, 0.5};

/*float bezier(float p0, float p1, float p2, float p3, float t) {
  return pow((1-t),3)*p0+3*pow(1-t,2)*t*p1+3*(1-t)*t*t*p2+t*t*t*p3;
}*/

float bezier(float p0, float p1, float p2, float p3, float t) {
  float omt = 1.0f - t;
  float omt2 = omt * omt;
  float t2 = t * t;

  return omt2 * omt * p0 + 3.0f * omt2 * t * p1 + 3.0f * omt * t2 * p2 + t2 * t * p3;
}

/*path normalpath(float t) {
  path p;
  float u;

  if (t < 0.5) { //stride
    u =t/0.5;

    p.s = stride / 2.0f - 2.0f * t * stride;
    p.h = 0.0f;
  }

  else if (t < 0.75) { //ascent
    u = (t - 0.5f) / 0.25f;

    p.s = bezier(
      -stride / 2.0,
      -5.0*stride/6.0,
      -stride / 2.0,
      0.0f,
      u
    );

    p.h = bezier(0.0f, 0.0f, height, height, u);
  }

  else { //descent
    u = (t - 0.75f) / 0.25f;

    p.s = bezier(
      0.0f,
      stride / 2.0,
      5.0*stride/6.0,
      stride / 2.0,
      u
    );

    p.h = bezier(height, height, 0.0f, 0.0f, u);
  }
  return p;
}*/

path adaptablepath(float t, float contactT, float stride, float height) {
  path p;

  float contactU = (contactT - 0.75f) / 0.25f;

  float u;
  float contactS = bezier(0.0f, stride/2.0, 5.0 * stride/6.0, stride / 2.0, contactU);
  float contactY = bezier(height, height, 0.0f, 0.0f, contactU);
  float recoveryS = contactS - stride;
  float recoveryY = contactY ;

  if (t < 0.5) { //stride
    u =t/0.5;

    p.s = contactS - stride * u;
    p.h = contactY;
  }

  else if (t < 0.75) { //recovery ascent
    u = (t - 0.5f) / 0.25f;
    p.s = bezier(recoveryS, recoveryS, recoveryS, 0.0f, u);
    p.h = bezier(recoveryY, recoveryY, height, height, u);
  }

  else { //descend
    u = (t - 0.75f) / 0.25f;

    p.s = bezier(0.0f, stride / 2.0, stride/2.0, stride / 2.0, u);
    p.h = bezier(height, height, 0.0f, 0.0f, u);
  }

  return p;
}

void walk(float rad, float speed, float stridelength, float strideheight, float turn, float magnitude) {
  rad -= PI/2;
  for (int i = 0; i <6; i++) {
    foot_touch[i] = digitalRead(footpins[i]) == LOW;

    justTouched[i] = foot_touch[i] && !lastFootTouch[i];
    lastFootTouch[i] = foot_touch[i];

    if (phase[i] == 0.0f && g_t1 == 0) {
      landed[i] = false;
    }

    if (phase[i] == 0.5f && g_t2 == 0) {
      landed[i] = false;
    }

    if (justTouched[i] && phase[i] == 0.0f && g_t1 >= 0.75f) {
      landed[i] = true;
      contactT[i] = g_t1;
    }

    if (justTouched[i] && phase[i] == 0.5f && g_t2 >= 0.75f) {
      landed[i] = true;
      contactT[i] = g_t2;
    }

    path p;

    if (phase[i] == 0.0f) {

      if (!landed[i]) {
        p = adaptablepath(g_t1,contactT[i],stridelength,strideheight);
      }

      if (landed[i]) {
        p = adaptablepath(contactT[i],contactT[i],stridelength,strideheight);
      }
      
    }

    if (phase[i] == 0.5f) {

      if (!landed[i]) {
        p = adaptablepath(g_t2,contactT[i],stridelength,strideheight);
      }

      if (landed[i]) {
        p = adaptablepath(contactT[i],contactT[i],stridelength,strideheight);
      }

    }

    float phii = legs[i].phii;

    float alpha = rad - phii;

    float move = 1;

    if (magnitude <= 0.08f) {
      move  = 0;
    }

    float turnFactor = 1.5f;
    
    float dirX = move * sin(alpha) + turn * turnFactor;
    float dirZ = move * cos(alpha);
      
    writeIK(i, p.s*dirX, ground_y+p.h, offset_z+move*p.s*dirZ);
    
  }

  g_t1 += speed;
  g_t2 += speed;

  if (g_t1 >= 1.0f) {
    // Legs 0, 2, 4 use g_t1.
    for (int i = 0; i < 6; i += 2) {
      if (!landed[i]) {
        contactT[i] = 1.0f;
      }
    }

    g_t1 = 0.0f;
  }

  if (g_t2 >= 1.0f) {
    // Legs 1, 3, 5 use g_t2.
    for (int i = 1; i < 6; i += 2) {
      if (!landed[i]) {
        contactT[i] = 1.0f;
      }
    }

    g_t2 = 0.0f;
  }
}

void sweepJoint(int pca, int channel, int minAngle, int maxAngle, int stepDelay) {
  for (int a = minAngle; a <= maxAngle; a++) {
    writeServo(pca, channel, a);
    delay(stepDelay);
  }

  for (int a = maxAngle; a >= minAngle; a--) {
    writeServo(pca, channel, a);
    delay(stepDelay);
  }
}

void neutral_stance() {
  for (int i = 0; i<6; i++)
    writeIK(i,0,ground_y,offset_z);
}
