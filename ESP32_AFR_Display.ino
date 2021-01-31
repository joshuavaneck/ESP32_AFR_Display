/*
   AFR monitor using TTGO ESP 32 module with 240 x 135 TFT full colour display
   Use ESP 32 Dev Module board definition.
*/

#include <TFT_eSPI.h>
#include<SPI.h>
//#include "Free_Fonts.h" // Include the header file attached to this sketch
#include <User_Setup.h>

float targetAFR;
float actualAFR;
float afrVar;
uint16_t* tft_buffer = (uint16_t*) malloc( 30000 );
bool      buffer_loaded = false;
float  o2Actual;
float  o2Target;
#define sTX 21    // Serial2 transmit (out), pin J4 on Speeduino connector
#define sRX 22    // Serial2 recieve (in), pin K4 on Speeduino connector

// define display
TFT_eSPI tft = TFT_eSPI(135, 240);
// sprite objects
TFT_eSprite afrNum = TFT_eSprite(&tft);       // the ARF number displayed
TFT_eSprite afrVarInd = TFT_eSprite(&tft);     // the AFR variance indicator

void showAFR() {
  // displays the current AFR and a triangle on the bottom for AFR variance
  // display AFR
  static byte oldPos;
  float dispActO2 = o2Actual / 10;   // convert actual o2 to a floating point number for display
  Serial.print("Actual AFR: "); Serial.println(dispActO2);
  afrNum.drawFloat(dispActO2, 1, 0, 40);
  afrNum.pushSprite(20, 0);
  // calculate variance
  float afrVar = o2Actual - o2Target;
  float dispAfrVar = afrVar / 10;
  //Serial.print("AFR Variance :"); Serial.println(dispAfrVar);
  // calulate position of variance indicator
  // can be 0 - 240 px;  0= variance of -1, 240 = variance of +1
  int indPosition = (dispAfrVar * 50) + 100;
  // only draw indicator if it's moved
  if (indPosition != oldPos) {
    drawBar();
    afrVarInd.pushSprite(indPosition, 85, TFT_TRANSPARENT);
    oldPos = indPosition;
  }
}

void drawBar() {
  //draw bar at bottom
  tft.fillRect(0, 90, 120, 45, TFT_RED);
  tft.fillRect(120, 90, 120, 45, TFT_BLUE);
}

//void simulateValues() {
//  //generate simulated AFR values
//  const float MIN_AFR = 12.0;
//  const float MAX_AFR = 17.5;
//  float afrRange = (MAX_AFR - MIN_AFR);
//  long updateFreq = 100;
//  static long lastUpdate = millis();
//  static float sinAFR = 0;
//  static float sinVar = 0;
//  float varInc = 1;
//  float targetInc = .5;
//  const float VAR_RANGE = 1;
//  float AFRMedian = MIN_AFR + (afrRange / 2);
//
//  if (millis() - lastUpdate > updateFreq) {
//    // Target AFR is a floating point number that ranges between 11.5 and 19
//    // use a sin curve to continuously vary this value
//    // Actual will vary around the target +- afrRange
//    // First generate new simulated TargetAFR
//    targetAFR = AFRMedian + (sin(sinAFR / 10) * (afrRange / 2));
//    Serial.print("Target AFR: "); Serial.println(targetAFR);
//    //strTargetAFR = String(targetAFR, 1);
//    sinAFR = sinAFR + targetInc;
//    if (sinAFR > 65) {
//      sinAFR = 0;
//    }
//    //generate a simulated variance from target.  This will be used to calculate simulated actual afr
//    //simulated variance will be a sin curve but different period from targetafr
//    afrVar = sin(sinVar / 10) * VAR_RANGE;
//    Serial.print("AFR variance: "); Serial.println(afrVar);
//    //actualAFR = targetAFR + afrVar;
//    Serial.print("actual AFR: "); Serial.println(actualAFR);
//    sinVar = sinVar + varInc;
//    if (sinVar > 65) {
//      sinVar = 0;
//    }
//    lastUpdate = millis();
//  }
//}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, sRX, sTX); //Serial port for connecting to Speeduino
  Serial.println("Start");

  // initialize display
  tft.init();
  tft.setRotation(3);         // landscape
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);

  // create sprite for AFR numeric display
  afrNum.setTextFont(8);
  afrNum.createSprite(200, 85);
  afrNum.fillSprite(TFT_BLACK);
  afrNum.setTextColor(TFT_WHITE, TFT_BLACK);
  afrNum.setTextDatum(MC_DATUM);

  // create sprite for AFR variance indicator
  afrVarInd.createSprite(40, 50);
  afrVarInd.fillSprite(TFT_TRANSPARENT);
  afrVarInd.fillTriangle(20, 0, 0, 50, 40, 50, TFT_BLACK);
  afrVarInd.fillTriangle(20, 5, 3, 47, 37, 47, TFT_GREENYELLOW);

  drawBar();
}


void loop() {
  // put your main code here, to run repeatedly:
  //simulateValues();
  getAFR();
  showAFR();

}