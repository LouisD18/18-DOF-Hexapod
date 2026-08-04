void tilt(float tilt, float alpha) { //uses rodrigues' matrix transform
  float radtilt = tilt * 3.14159/180;
  float radalpha = (90+alpha) * 3.14159/180;

  float ct = cos(radtilt);
  float st = sin(radtilt);
  float ca = cos(radalpha);
  float sa = sin(radalpha);
  float omt = 1.0f - ct;  
  
  Point rest;
  rest.px = 0;
  rest.py = -100;
  rest.pz = 150;

  Point footbody[6];

  for (int i = 0; i < 6; i++) {
    float sinphi = legSinPhi[i];
    float cosphi = legCosPhi[i];

    Point restbody;
    
    restbody.px = -rest.px*sinphi+rest.pz*cosphi;
    restbody.py = rest.py;
    restbody.pz = rest.px*cosphi+rest.pz*sinphi;

    footbody[i].px = legs[i].cb.px + restbody.px;
    footbody[i].py = legs[i].cb.py + restbody.py;
    footbody[i].pz = legs[i].cb.pz + restbody.pz;

    float easyx = footbody[i].px;
    float easyy = footbody[i].py;
    float easyz = footbody[i].pz;

    float x = easyx*(ct+ca*ca*omt)-(easyy*sa*st)+(easyz*ca*sa*omt) - legs[i].cb.px;
    float y = easyx*sa*st+easyy*ct-easyz*ca*st - legs[i].cb.py;
    float z = easyx*ca*sa*omt+easyy*ca*st+easyz*(ct+sa*sa*omt) - legs[i].cb.pz;

    float locx = -x*sinphi+z*cosphi;
    float locy = y;
    float locz = x*cosphi+z*sinphi;

    writeIK(i,locx,locy,locz);
  }

  
}
