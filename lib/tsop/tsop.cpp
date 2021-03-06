#include <Arduino.h>
#include "tsop.h"
#include <apexCommon.h>

void TSOP::Setup(){
  pinMode(TSOP_POWER, OUTPUT);

  for(int i = 0; i < TSOP_NUM; i++){
    pinMode(TSOPPINS[i], INPUT);
  }

  digitalWrite(TSOP_POWER, HIGH);
}

void TSOP::ReadOnce(){
  for(int i = 0; i < TSOP_NUM; i++){
    TSOPTEMPVAL[i] += digitalRead(TSOPPINS[i])^1;
  }
  count++;
}

void TSOP::Refresh(){
  digitalWrite(TSOP_POWER, LOW);
  delay(2);
  digitalWrite(TSOP_POWER, HIGH);
  delay(2);
}

void TSOP::Read(){
  while(count<255){
    ReadOnce();
  }
  FinishRead();
}

void TSOP::FinishRead(){
  for (int i = 0; i < TSOP_NUM; i++) {
    if (TSOPTEMPVAL[i] > 50) {
      TSOPVAL[i] = TSOPTEMPVAL[i];
      TSOPTEMPVAL[i] = 0;
    }
    else {
      TSOPVAL[i] = 0;
      TSOPTEMPVAL[i] = 0;
    }
  }
  count = 0;
  Refresh();
}

void TSOP::FilterValues(){
  //Remove Noise
  for (int i = 0; i < TSOP_NUM; i++) {
    #if TSOP_FILTER_NOISE
      if(TSOPVAL[i] < 5 || TSOPVAL[i] >= 200){
        TEMPFILTEREDVAL[i] = 0;
      } else{
        TEMPFILTEREDVAL[i] = TSOPVAL[i];
      }
    #else
      TEMPFILTEREDVAL[i] = TSOPVAL[i];
    #endif
  }

  for (int i = 0; i < TSOP_NUM; i++) {
    FILTEREDVAL[i] = TEMPFILTEREDVAL[i];
  }
  /*
  //Filter By Surrounding
  for (int i = 0; i < TSOP_NUM; i++) {
    #if TSOP_FILTER_SURROUNDING
      int temp = TSOP_K1 * TEMPFILTEREDVAL[i] +
      TSOP_K2 * (TEMPFILTEREDVAL[mod(i - 1, TSOP_NUM)] +
      TEMPFILTEREDVAL[mod(i - 1, TSOP_NUM)]) +
      TSOP_K3 * (TEMPFILTEREDVAL[mod(i - 2, TSOP_NUM)] +
      TEMPFILTEREDVAL[mod(i - 2, TSOP_NUM)]);
    #else
      int temp = TEMPFILTEREDVAL[i] << 4;
    #endif

    FILTEREDVAL[i] = temp >> 4;

  }
  */

  //Sorting TSOP Values
  for (int i = 0; i < TSOP_NUM; i++) {
    int maxVal = 0;
    int maxInd = 0;
    for (int j = 0; j < TSOP_NUM; j++) {
      if (FILTEREDVAL[j]>maxVal) {
        maxVal = FILTEREDVAL[j];
        maxInd = j;
      }
    }
    SORTEDFILTEREDVAL[i] = maxVal;
    SORTEDINDEX[i] = maxInd;
    FILTEREDVAL[maxInd] = -1;
  }


}

void TSOP::GetAngleSimple(){
  if(SORTEDFILTEREDVAL[0] <= 5){
    simpleAngle = -1;
  } else {
    simpleAngle = SORTEDINDEX[0] * 360 / TSOP_NUM;
  }
}


void TSOP::GetAngle(int n){
  double vectori = 0;
  double vectorj = 0;

  for(int i = 0; i < n; i++){
    vectori += SORTEDFILTEREDVAL[i]*sin((SORTEDINDEX[i]*30)*pi/180);
    vectorj += SORTEDFILTEREDVAL[i]*cos((SORTEDINDEX[i]*30)*pi/180);
  }
  if(vectori!=0||vectorj!=0){
    if(vectori==0){
      angle = vectorj > 0 ? 0 : 180;
    }else{
      if(vectori<0){
        angle = (int)(round(270-atan(vectorj/vectori)*180/pi));
      } else{
        angle = (int)(round(90-atan(vectorj/vectori)*180/pi));
      }
    }
  }else{
    angle = 0;
  }
  angle = 360 - angle;
  int mag = pow(vectori * vectori + vectorj * vectorj, 0.5);
  // Serial.println(mag);
  if (mag < 25) angle = -30;
//Serial.println(angle);
}

void TSOP::GetStrengthSimple(){
  simpleStrength = SORTEDFILTEREDVAL[0];
}

void TSOP::GetStrength(int n){
  int total = 0;
  for (int i = 0; i < n; i++){
    total += SORTEDFILTEREDVAL[i];
  }
  strength = (double)total / n;
}
