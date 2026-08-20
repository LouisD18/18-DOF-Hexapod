uint8_t makeChecksum(uint8_t* data, int length) {

  uint8_t checksum = 0;

  for (int i = 0; i < length; i++) {
    checksum ^= data[i];
  }

  return checksum;
}


void receiveBT() {

  static uint8_t packet[11];
  static int index = 0;


  while (BT.available()) {

    uint8_t incoming = BT.read();

    if (index == 0) {

      if (incoming == 0xAA) {

        packet[0] = incoming;
        index = 1;
        }

      continue;
    }
    packet[index] = incoming;
    index++;



    if (index >= 11) {

      index = 0;


      uint8_t expectedChecksum =
        makeChecksum(&packet[1], 9);


      if (expectedChecksum != packet[10]) {

        continue;
      }

      uint8_t lMag = packet[1];


      uint16_t lAngle =
        ((uint16_t)packet[3] << 8) |
        packet[2];

        //decoding now

      int8_t rXRaw =
        (int8_t)packet[4];

      int8_t rYRaw =
        (int8_t)packet[5];

      uint8_t lPot = packet[6];

      uint8_t rPot = packet[7];


      uint8_t buttons = packet[8];


      if (lAngle > 359) {
        continue;
      }


      leftMagnitude =lMag / 255.0f;

      leftAngleDeg =(float)lAngle;


      rightX =rXRaw / 127.0f;

      rightY =rYRaw / 127.0f;


      lp =lPot / 255.0f;

      rp =rPot / 255.0f;

      leftPressed =buttons & 0b00000001;

      rightPressed =buttons & 0b00000010;
    }
  }
}
