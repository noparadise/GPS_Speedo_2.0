
// matrix
static const byte digits[10][6] = {
  {5,B01111110,B10000001,B10000001,B10000001,B01111110},{3,B10000010,B11111111,B10000000}, 
  {5,B11000010,B10100001,B10010001,B10001001,B10000110},{5,B01000010,B10000001,B10000001,B10001001,B01110110}, 
  {5,B00011000,B00010100,B00010010,B11111111,B00010000},{5,B01001111,B10001001,B10001001,B10001001,B01110001}, 
  {5,B01111110,B10001001,B10001001,B10001001,B01110010},{5,B00000001,B11100001,B00010001,B00001001,B00000111}, 
  {5,B01110110,B10001001,B10001001,B10001001,B01110110},{5,B01001110,B10010001,B10010001,B10010001,B01111110}
  };

static const byte chvU[4] = {3,B01000100,B00101000,B00010000};
static const byte chvD[4] = {3,B00010000,B00101000,B01000100};
static const byte oParen[3] = {2,B01111110,B10000001};
static const byte cParen[3] = {2,B10000001,B01111110};
static const byte dash[4] = {3,B00010000,B00010000,B00010000};
static const byte colon[2] = {1,B01001000};
static const byte apos[2] = {1,B00000011};
static const byte comma[3] = {2,B10000000,B01000000};
static const byte sqOpen[3] = {2,B11111111,B10000001};
static const byte sqClose[3] = {2,B10000001,B11111111};
static const byte que[6] = {5,B00000010,B00000001,B10110001,B00001001,B00000110};
static const byte pling[2] = {1,B10111111};
static const byte space[2] = {1,0};
static const byte dot[2] = {1,128};


std::unordered_map<int, const byte*> specials = {    
    {-2, dot}, {-3, dash}, {-4, comma}, {-7, cParen}, {-8, oParen}, {-9, apos}, {-15, pling},{-16,space},
    {10, colon}, {12, chvD}, {14, chvU}, {15, que}, {43, sqOpen}, {45, sqClose}
};
 const byte* getNonAlphaNum(int value) {
    auto it = specials.find(value);
    return (it != specials.end()) ? it->second : nullptr;
}
/**
 * end of potentially redundant text based stuff
 */

void initPanels(){
  Serial.println("initialising panels");
  for(int matrixId = 0; matrixId < panels; matrixId++){
    lc.shutdown(matrixId,false);
    lc.setIntensity(matrixId,brightness);
  }
}

void adjustBrightness(int val){
  brightness = constrain(val, 0, 15);
  for(int matrixId = 0; matrixId < panels; matrixId++){
    lc.setIntensity(matrixId,brightness);
  }
}

void clearPanels(){
  for(int matrixId = 0; matrixId < panels; matrixId++){
    lc.clearDisplay(matrixId);
  }
}

void showSpeed(int m_speed){
  int hundreds = (m_speed/100);
  if (hundreds >0) showFixedDigit(-1, hundreds);
  int tens = (m_speed/10)%10;
  if (tens >0 || hundreds >0) showFixedDigit(5, tens);
  showFixedDigit(11, m_speed%10);
}

void showFixedDigit(int offset, int digit){
  int panel;
  int col;
  int width = digits[digit][0];
  if(orientation){
    if (digit == 1) {
      if(offset > -1){
        lc.setRow(offset/8,offset%8,0);
        lc.setRow((offset+1)/8,(offset+1)%8,0);
      }
      offset += 2;
    }
    lc.setRow((offset-1)/8,(offset-1)%8,0);
    for (int i=0; i<width; i++){
      panel = (offset+i)/8;
      col = (offset+i)%8;
      if(headup){
        lc.setRow(panel,col,flipByte(digits[digit][i+1]));
      } else {
        lc.setRow(panel,col,digits[digit][i+1]);
      }
    }
  } else {
    int rev_offset = (panels*8) - offset - 1 ;
    if (digit == 1) {
      if(rev_offset < 64){
        lc.setRow(rev_offset/8,rev_offset%8,0);
        lc.setRow((rev_offset-1)/8,(rev_offset-1)%8,0);
      }
      rev_offset -= 2;
    }
    lc.setRow((rev_offset+1)/8,(rev_offset+1)%8,0);
    for (int i=0; i<width; i++){
      panel = (rev_offset-i)/8;
      col = (rev_offset-i)%8;
      if(headup){
        lc.setRow(panel,col,digits[digit][i+1]);
      } else {
        lc.setRow(panel,col,flipByte(digits[digit][i+1]));
      }
    }
  }
}

byte flipByte(byte c){
  c = ((c>>1)&0x55)|((c<<1)&0xAA);
  c = ((c>>2)&0x33)|((c<<2)&0xCC);
  c = (c>>4) | (c<<4) ;
  return c;
}

int ledFromBearing(double bearing, int numleds){
  double sector = 360/numleds;
  return int((bearing + (sector/2))/sector)%numleds;
  // return int((bearing + 11.25)/22.5)%16;
}
