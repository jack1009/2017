#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

/*input define*/
#define withstand_sol 	PEout(7)
#define upDown_sol 			PEout(8)
#define sendScrew_sol		PEout(9)
#define screwStart_ry 	PEout(10)
#define homeOutput_ry 	PEout(11)
#define finishOutput_ry PEout(12)
#define errorCode2_ry 	PEout(13)
#define errorCode1_ry 	PEout(14)
#define errorCode0_ry 	PEout(15)

/*output define*/
#define autoManual_pb PDin(0)
#define start_pb 			PDin(1)
#define stop_pb 			PDin(2)
#define reset_pb			PDin(3)
#define upDown_pb 		PDin(4)
#define withstand_pb 	PDin(5)
#define sendScrew_pb 	PDin(6)
#define screwStart_pb PDin(7)
#define up_ls 				PGin(9)
#define down_ls 			PGin(10)
#define slip_snr 			PGin(11)
#define float_snr 		PGin(12)
#define lack_snr 			PGin(13)
#define full_snr 			PGin(14)

void LED_Init(void);		 				    
#endif

















