#include <W78E054_052D.H>
#define  sol_out  P15  //輸出
#define  start_pb  P17 //啟動
#define  sol_in  P11   //電磁閥信號
#define  lack_snr  P10   //送料檢知
#define  error_lack_snr  P12
unsigned int;
int buffer;
char DATA_7SEG[10];
int (Autoflow);
int (set_Value);
int (isScrewInTube);
int (isScrewInClip);
int (lackCounter);
//void delay10ms(int t);
void delay_1s();
void dataChange(void);
//void delay(unsigned int value);
/*****************************/
void main(void)
{
	P0UPR=1;
	buffer=0;
	Autoflow=0;
  sol_out=0;
	isScrewInClip=1;
	error_lack_snr=0;
	isScrewInTube=1;
	lackCounter=0;
	while(1)
	{	
  if(start_pb==0)
	{
	isScrewInClip=1;
	error_lack_snr=0;
  }	
	if(P2==0xff) set_Value=0;	
	if(P2==0xfe) set_Value=1;
	if(P2==0xfd) set_Value=2;
	if(P2==0xfc) set_Value=3;
	if(P2==0xfb) set_Value=4;
	if(P2==0xfa) set_Value=5;
  if(P2==0xf9) set_Value=6;
  if(P2==0xf8) set_Value=7;
  if(P2==0xf7) set_Value=8;
  if(P2==0xf6) set_Value=9;
	if(buffer==0)P0=0x3f;	
	if(buffer==1)P0=0x06;	
	if(buffer==2)P0=0x5b;	
	if(buffer==3)P0=0x4f;	
	if(buffer==4)P0=0x66;		
	if(buffer==5)P0=0x6d;	
	if(buffer==6)P0=0x7d;	
	if(buffer==7)P0=0x07;
	if(buffer==8)P0=0x7f;	
	if(buffer==9)P0=0x6f;	
	if(Autoflow==0) 	//POWER_ON
	{ 
		sol_out=0;	
  }	
	if(Autoflow==0 && sol_out==0) 	//idle
	{ 
		Autoflow=1;
  }
	/*送料啟動=2*/
	if(Autoflow==1 && start_pb==0 && sol_out==0) 
	  {
				Autoflow=2;
				buffer=0;
    }
/*螺絲檢查=3*/
	if(Autoflow==2) 	
	{
		Autoflow=3;
  }
	/*缺螺絲異常=13*/
if(isScrewInClip==0 && Autoflow==3 )		
  {
		error_lack_snr=1;
		Autoflow=1;
  }
	else if( Autoflow==3 && isScrewInClip==1 )		/*缺螺絲無異常=4*/
   {
		Autoflow=4;
   }
  /*檢查是否完成=5*/
		if(Autoflow==4) 
		{
			Autoflow=5; 
    }
	/*未完成=6*/
  if(Autoflow==5 && buffer<set_Value)
		{
			   Autoflow=6;	
     }
	 /*已完成回idle*/
		else if(Autoflow==5 && buffer>=set_Value)
			{
		   Autoflow=1;
			}
	/*未完成輸出送料信號ON*/
	if(Autoflow==6)
	{
		sol_out=1;
	}
	/*輸出送料信號OFF=7*/
	if(Autoflow==6 && sol_out==1)
	{
		dataChange();
		delay_1s();
		Autoflow=7;
  }
	if(Autoflow==7)		 
	{
	  sol_out=0;
  }
  /*送料料中=8*/
	if(Autoflow==7 && sol_out==0)		 
	{
	  Autoflow=8;
  }
	/*送料料完成=9*/
	if(Autoflow==8 && sol_in==0 )
		{
			Autoflow=9;
    }
	/*缺螺絲檢查=1*/
 	if(Autoflow==9 && isScrewInTube==1 )//當檢知動作時
	{
		lackCounter=0;
		Autoflow=10;
	}
	if(Autoflow==9 && isScrewInTube==0 && sol_in==1 )
	 {
		 lackCounter++;
	 }
	if(lackCounter>3000)
		{	 
		  Autoflow=10;
		 }
  /*送料料完成+1=11*/
   if(Autoflow==10 && sol_in==1 && sol_out==0)
	 {		
			Autoflow=11;	
		 lackCounter=0;
		 buffer++;
    }
	if(Autoflow==11)
	{
		Autoflow=3;
  }
	/*入料檢查*/
	if (lack_snr==0)
	{
		isScrewInTube=1;
  }
 }
}
/********************************/
/*顯示器字形資料*/
char DATA_7SEG[10]=
{ 
0x3f,   //0	
0x06,   //1
0x5b,   //2
0x4f,   //3
0x66,   //4
0x6d,   //5
0x7d,   //6
0x07,   //7
0x7f,   //8
0x6f    //9
};	
/*----------------------------------------*/

//void delay10ms(int t)
//{
//  int i;
//   for(i=0;i<t;i++);
//}
/*----------------------------------------*/
// 10us delay time
//void delay(unsigned int value)
// {
//	 while(value > 0)
//		 value --;
 //}
  
/*******************************/
void delay_1s()
{ 
	int i;
 TMOD = 0x01; 
 for(i=0;i<20;i++) 
 {
 TH0 = (65536-50000) / 256; 
 TL0 = (65536-50000) % 256;
 TR0 = 1; 
 while(TF0 == 0); 
 TF0 = 0; 
 TR0 = 0;
 }
} 
/************************************************************
**Feed Screw Check
*************************************************************/
void dataChange(void)
{
	isScrewInClip=isScrewInTube;
	isScrewInTube=0;
}

/************************************************************
**
*************************************************************/
