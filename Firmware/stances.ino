void neutral_stance() {
  for (int i = 0; i<6; i++)
    writeIK(i,0,-100,150);
}

void sit() {
  for (int i = 0; i<6; i++)
    writeIK(i,0,30,200);
}
