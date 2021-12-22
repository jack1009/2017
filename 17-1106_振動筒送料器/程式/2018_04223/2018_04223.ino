int SNR_full = 8;
int PB_start = 9;
//int PB_start2 = 10;
int RY_feed = 4;
int RY_full = 5;
int SOL_feed = 6;
int External_output = 7;
//bool buttonOnUp = true;
int RY_feedOn = 0;
int RY_fullOn = 0;
int SOL_feedOn = 0;
int AutoFlow = 0;
int AutoFlow2 = 0;
int FeedScrew_delay = A0; //送螺絲時間(IN)
int FullOn_delay = A1; //震筒啟動時間(IN)
int FullOff_delay = A2; //震筒關閉時間(IN)
int FeedScrew_value = 0; //送螺絲時間
int FullOn_value = 0; //震筒啟動時間
int FullOff_value = 0; //震筒關閉時間
int newdata1 = 0; //送螺絲時間變數
int newdata2 = 0; //震筒啟動時間變數
int newdata3 = 0; //震筒關閉時間變數
int i = 0; //計數變數
bool count1Start = false; //平送起振開始
bool count1Finish = false; //平送起振完成
bool count2Start = false; //送螺絲開始
bool count2Finish = false; //送螺絲完成
bool count3Start = false; //平送關開始
bool count3Finish = false; //平送關完成
bool count4Start = false; //震桶無料起振開始
bool count4Finish = false; //震桶無料起振完成
bool count5Start = false; //震桶有料完成開始
bool count5Finish = false; //震桶有料起振完成
bool countFinishPB = false;//啟動開關1濾波完成
bool countFinishPB2 = false;//啟動開關2濾波完成
int count1 = 0;
int count2 = 0;
int count3 = 0;
int count4 = 0;
int count5 = 0;
int countPB = 0;
int countPB2 = 0;
void setup() {
  Serial.begin(9600);
  pinMode(SNR_full, INPUT);
  pinMode(PB_start, INPUT);
  //pinMode(PB_start2, INPUT);
  pinMode(RY_feed, OUTPUT);
  pinMode(RY_full, OUTPUT);
  pinMode(SOL_feed, OUTPUT);
  pinMode(External_output, OUTPUT);
}

void loop() {
  RY_feedOn = digitalRead(RY_feed);
  SOL_feedOn = digitalRead(SOL_feed);
  RY_fullOn = digitalRead(RY_full);
  //送螺絲時間
  FeedScrew_value = analogRead(FeedScrew_delay);
  newdata1 = map(FeedScrew_value, 0, 1023, 0, 2000);
  //震筒起震時間
  FullOn_value = analogRead(FullOn_delay);
  newdata2 = map(FullOn_value, 0, 1023, 0, 5000);
  //震筒關閉時間
  FullOff_value = analogRead(FullOff_delay);
  newdata3 = map(FullOff_value, 0, 1023, 0, 5000);
  //Serial.println(newdata1);
  //Serial.println(newdata2);
  //Serial.println(newdata3);
  // Serial.println(AutoFlow);
  Serial.println(AutoFlow2);
  //Serial.println(count1);
  //Serial.println(count2);
  //Serial.println(count3);
  Serial.println(count4);
  // Serial.println(count5);
  delay(2);
  //idle=1
  if (AutoFlow == 0) {
    AutoFlow = 1;
  }
  //平送起震=2
  if (AutoFlow == 1 and (countFinishPB == true or countFinishPB2 == true) )
  {
    count1Start = true;
    //buttonOnUp = false;
    countFinishPB = false;
    countFinishPB2 = false;
    AutoFlow = 2;
  }
  //起振計時完成=3
  if (AutoFlow == 2 and RY_feedOn == 1 and count1Finish == true and countFinishPB == false and countFinishPB2 == false )
  {
    count1Start = false;
    count1 = 0;
    AutoFlow = 3;
  }
  //送螺絲出=4
  if (AutoFlow == 3 and count1Start == false and count1 == 0 )
  {
    digitalWrite(SOL_feed, HIGH);
    digitalWrite(External_output, HIGH);
    count2Start = true;
    AutoFlow = 4;
  }
  //送螺絲計時完=5
  if (AutoFlow == 4 and digitalRead(SOL_feed) == HIGH and digitalRead(External_output) == HIGH and count2Finish == true )
  {
    count2Start = false;
    count2 = 0;
    AutoFlow = 5;
  }
  //送螺絲回=6
  if (AutoFlow == 5 and count2Start == false and count2 == 0 )
  {
    digitalWrite(SOL_feed, LOW);
    digitalWrite(External_output, LOW);
    count3Start = true;
    AutoFlow = 6;
  }
  //平送OFF計時完=7
  if (AutoFlow == 6 and digitalRead(SOL_feed) == LOW and digitalRead(External_output) == LOW and count3Finish == true )
  {
    count3Start = false;
    count3 = 0;
    AutoFlow = 7;
  }
  //平送OFF
  if (AutoFlow == 7 and count3Start == false and count3 == 0 and digitalRead(PB_start) == HIGH)
  {
    AutoFlow = 1;
  }
  if (AutoFlow == 2 or AutoFlow == 3  or RY_fullOn == 1 or AutoFlow == 4 or AutoFlow == 5 or AutoFlow == 6 )
  {
    digitalWrite(RY_feed, HIGH);
  }
  else {
    digitalWrite(RY_feed, LOW);
  }
  //震動筒
  //idle2
  if (AutoFlow2 == 0)
  {
    AutoFlow2 = 1;
  }
  //震動筒無料計時=2
  if (AutoFlow2 == 1 and digitalRead(SNR_full) == HIGH and digitalRead(RY_full) == LOW )
  {
    count4Start = true;
    AutoFlow2 = 2;
  }
  //有料
  if (AutoFlow2 == 2 and count4 <= newdata2 and digitalRead(SNR_full) == LOW  )
  {
    count4Start = false;
    count4 = 0;
    AutoFlow2 = 1;
  }
  //無料=3
  if (AutoFlow2 == 2 and count4Finish == true and digitalRead(SNR_full) == HIGH  )
  {
    digitalWrite(RY_full, HIGH);
    count4Start = false;
    count4 = 0;
    AutoFlow2 = 3;
  }
  //起振完成=4
  if (AutoFlow2 == 3 and digitalRead(RY_full) == HIGH and count4Start == false and  count4 == 0)
  {
    AutoFlow2 = 4;
  }
  //有料判斷
  if ( AutoFlow2 == 4 and  digitalRead(SNR_full) == LOW and digitalRead(RY_full) == HIGH)
  {
    count5Start = true;
    AutoFlow2 = 5;
  }

  if ( AutoFlow2 == 5 and  digitalRead(SNR_full) == HIGH and digitalRead(RY_full) == HIGH)
  { count5Start = false;
    count5 = 0;
    AutoFlow2 = 4;
  }
  if ( AutoFlow2 == 5 and count5Finish == true   )
  {
    count5Start = false;
    count5 = 0;
    digitalWrite(RY_full, LOW);
    AutoFlow2 = 6;
  }
  if (AutoFlow2 == 6 and digitalRead(RY_full) == LOW and count5Start == false and count5 == 0 )
  {
    AutoFlow2 = 1;
  }
  //防止開關長壓住
 // if (digitalRead(PB_start) == HIGH and digitalRead(PB_start2) == HIGH and buttonOnUp != true)
 // {
 //   buttonOnUp = true;
 // }
  //取代delay程式(平送起振)
  if (count1Start == true)
  {
    count1 = count1 + 1;
  }
  if (count1Start and count1 >= 100)
  {
    count1Finish = true;
  }
  if (count1 == 0 and count1Start == false )
  {
    count1Finish = false;
  }
  //取代delay程式(送螺絲)
  if (count2Start == true)
  {
    count2 = count2 + 1;
  }
  if (count2Start and count2 >= newdata1)
  {
    count2Finish = true;
  }
  if (count2 == 0 and count2Start == false )
  {
    count2Finish = false;
  }
  //取代delay程式(平送關)
  if (count3Start == true)
  {
    count3 = count3 + 1;
  }
  if (count3Start and count3 >= 100)
  {
    count3Finish = true;
  }
  if (count3 == 0 and count3Start == false )
  {
    count3Finish = false;
  }
  //取代delay程式(震桶無料)
  if (count4Start == true)
  {
    count4 = count4 + 1;
  }
  if (count4Start and count4 >= newdata2)
  {
    count4Finish = true;
  }
  if (count4 == 0 and count4Start == false )
  {
    count4Finish = false;
  }

  //取代delay程式(震桶有料)
  if (count5Start == true)
  {
    count5 = count5 + 1;
  }
  if (count5Start and count5 >= newdata3)
  {
    count5Finish = true;
  }
  if (count5 == 0 and count5Start == false )
  {
    count5Finish = false;
  }
  //啟動開關1濾波程式
  if (digitalRead(PB_start) == LOW and AutoFlow == 1 )
  {
    countPB = countPB + 1;
  }
  if (digitalRead(PB_start) == LOW and countPB >= 100 )
  {
    countFinishPB = true;
  }
  if (digitalRead(PB_start) == HIGH )
  {
    countFinishPB = false;
    countPB = 0;
  }
  //啟動開關2濾波程式
 // if (digitalRead(PB_start2) == LOW and AutoFlow == 1)
 // {
  //  countPB2 = countPB2 + 1;
 // }
 // if (digitalRead(PB_start2) == LOW and countPB2 >= 100)
 // {
//    countFinishPB2 = true;
//  }
 // if (digitalRead(PB_start2) == HIGH)
 // {
 //   countFinishPB2 = false;
//    countPB2 = 0;
//  }
}


