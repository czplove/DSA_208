#ifndef _reg_h
#define _reg_h

extern volatile register WORD                          REG_Surplus_Time;
extern volatile register WORD                          REG_TimeTemp;

extern volatile register WORD                          REG_Year;
extern volatile register WORD                          REG_1Msecond;
extern volatile register BYTE                          REG_Month;
extern volatile register BYTE                          REG_Date;
extern volatile register BYTE                          REG_Hour;
extern volatile register BYTE                          REG_Minute;
extern volatile register BYTE                          REG_Second;


extern volatile register WORD                          Time_1ms_Counter;
extern volatile register WORD                          Time_2048ms_Counter;

extern volatile register TYPE_CAN_PORT                 REG_CAN0_Port;
extern volatile register TYPE_CAN_PROC                 REG_CAN0_Proc;



extern register BYTE  *Core_Src_Pt_B;
extern register BYTE  *Core_Det_Pt_B;  
extern register WORD  *Core_Src_Pt;
extern register WORD  *Core_Det_Pt;
extern register WORD   Core_Src_Unit;
extern register WORD   Core_Src_Len;
extern register WORD   Core_Temp_Loop;
extern register WORD   Core_Temp_Loop1;
extern register WORD   Core_Temp_ShortInt;

/************** PORT *************/
extern register unsigned char   port_no;
extern register unsigned char   port_check;
extern register unsigned short  temp_int;
extern register unsigned short  temp_loop;
extern register unsigned short  temp_loop1;
extern register unsigned short  temp_lp_int;
extern register unsigned short *temp_ptS,*temp_ptD;
extern register unsigned long  *temp_ptL;
extern register BYTE           *temp_ptS_B;
extern register BYTE           *temp_ptD_B;
extern register BYTE           *temp_fpt_B;


#endif /* _reg_h */
