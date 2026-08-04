bool isCoxa[2][16] = {
  // PCA 0x40
  {
    false, false, true,  false, false, true,  true,  false,
    false, false, false, false, false, false, false, false
  },

  // PCA 0x41
  {
    true,  false, false, true,  false, false, true,  false,
    false, false, false, false, false, false, false, false
  }
};

bool footsearch[6];

int footswitch[6] = {
  2, //0 left front
  3, //1 left middle
  4, //2 left back
  5, //3 right front
  6, //4 right middle
  7  //5 right back
};

int centerTicks_0x40[16] = {
  338, // 0  left front knee   (1650 us)
  328, // 1  left front hip    (1600 us)
  328, // 2  left front coxa   (1600 us) 20
  317, // 3  left middle knee  (1550 us)
  348, // 4  left middle hip   (1700 us)
  338, // 5  left middle coxa  (1650 us) 20
  328, // 6  right middle coxa (1600 us) 20
  328, // 7  right middle hip  (1600 us)
  328, // 8  right middle knee (1600 us)
  328, // 9
  328, // 10
  328, // 11
  328, // 12
  328, // 13
  328, // 14
  328  // 15
};

int centerTicks_0x41[16] = {
  333, // 0  right front coxa (1625 us) 20
  328, // 1  right front hip  (1600 us)
  328, // 2  right front knee (1600 us)
  338, // 3  right back coxa  (1650 us) 20
  328, // 4  right back hip   (1600 us)
  307, // 5  right back knee  (1500 us)
  338, // 6  left back coxa   (1650 us) 20
  332, // 7  left back hip    (1620 us)
  328, // 8  left back knee   (1600 us)
  328, // 9
  328, // 10
  328, // 11
  328, // 12
  328, // 13
  328, // 14
  328  // 15
};




void writeServo( int pca, int channel, float  angle) {

  const float ticksperdeg20 = 1.52;
  const float ticksperdeg35 = 2.27;

  float ticksperdeg;
  int centerTick;
  int board = (pca == 0x40) ? 0 : 1;

  if (pca == 0x40) {
    centerTick = centerTicks_0x40[channel];
  }
  else {
    centerTick = centerTicks_0x41[channel];
  }
  
  if  (isCoxa[board][channel]) {
    //coxa servo gotta use different ticks dawg
    ticksperdeg = ticksperdeg20;
  }
       
  else {
    ticksperdeg = ticksperdeg35;
  }

  int tick = round(centerTick + (angle - 90) * ticksperdeg);

  if (pca == 0x40) {
    pca0.setPWM(channel, 0, tick);
  } else {
    pca1.setPWM(channel, 0, tick);
  }
}


