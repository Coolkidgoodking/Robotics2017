#ifndef TSOP_H
#define TSOP_H

#define TSOP_POWER 50
#define TSOP_DELAY 2
#define TSOP_0 46
#define TSOP_1 48
#define TSOP_2 28
#define TSOP_3 26
#define TSOP_4 30
#define TSOP_5 32
#define TSOP_6 34
#define TSOP_7 36
#define TSOP_8 38
#define TSOP_9 40
#define TSOP_10 42
#define TSOP_11 44

class TSOP{
public:
  int TSOPPINS[12] = {TSOP_0,TSOP_1,TSOP_2,TSOP_3,TSOP_4,TSOP_5,TSOP_6,TSOP_7,TSOP_8,TSOP_9,TSOP_10,TSOP_11};
  int TSOPVAL[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
  int count = 0;
  void Setup();
  void ReadOnce();
  void Read();
  void FinishRead();
  void Refresh();
};

#endif