/*****************************************************************************/
/*       FileName  :   REG.C                                                 */
/*       Content   :   DSA-208 REG Module                                    */
/*       Date      :   Fri  02-22-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/



#pragma  regconserve


#include _SFR_H_
#include _FUNCS_H_

#include "common.h"


/***********************************************/
/* Definition of global variables              */
/***********************************************/




register WORD                          REG_Surplus_Time;
register WORD                          REG_TimeTemp;

register WORD                          REG_Year;
register WORD                          REG_1Msecond;        ;;;这些是全局变量,可以定义为内部寄存器使用
register BYTE                          REG_Month;
register BYTE                          REG_Date;
register BYTE                          REG_Hour;
register BYTE                          REG_Minute;
register BYTE                          REG_Second;


register WORD                          Time_1ms_Counter;
register WORD                          Time_2048ms_Counter; // MAX count 37 hour


register TYPE_CAN_PORT                 REG_CAN0_Port;
register TYPE_CAN_PROC                 REG_CAN0_Proc;


/*********** CORE ***********/
register BYTE  *Core_Src_Pt_B;
#pragma locate (Core_Src_Pt_B       = 0x50)          ;;;定义了绝对地址
register BYTE  *Core_Det_Pt_B;
#pragma locate (Core_Det_Pt_B       = 0x54)
register WORD  *Core_Src_Pt;
#pragma locate (Core_Src_Pt         = 0x58)
register WORD  *Core_Det_Pt;
#pragma locate (Core_Det_Pt         = 0x5c)
register WORD   Core_Src_Unit;         // must be word by x.zhao at 09/02/2002
#pragma locate (Core_Src_Unit       = 0x60)
register WORD   Core_Src_Len;
#pragma locate (Core_Src_Len        = 0x62)
register WORD   Core_Temp_Loop;
#pragma locate (Core_Temp_Loop      = 0x64)
register WORD   Core_Temp_Loop1;
#pragma locate (Core_Temp_Loop1     = 0x66)
register WORD   Core_Temp_ShortInt;
#pragma locate (Core_Temp_ShortInt  = 0x68)
// 0x6a-0x79

register unsigned char  port_no;
#pragma locate (port_no    = 0x7a)

//端口模块内部可自行临时使用<
register unsigned char   port_check;
#pragma locate (port_check = 0x7b)
register unsigned short  temp_int;
#pragma locate (temp_int   = 0x7c)
register unsigned short  temp_loop;
#pragma locate (temp_loop  = 0x7e)
register unsigned short  temp_loop1;
#pragma locate (temp_loop1 = 0x80)
register unsigned short  temp_lp_int;
#pragma locate (temp_lp_int= 0x82)
register unsigned short *temp_ptS;
#pragma locate (temp_ptS   = 0x84)
register unsigned short *temp_ptD;
#pragma locate (temp_ptD   = 0x88)
register unsigned long  *temp_ptL;
#pragma locate (temp_ptL   = 0x8c)
register BYTE           *temp_ptS_B;
#pragma locate (temp_ptS_B = 0x90)
register BYTE           *temp_ptD_B;
#pragma locate (temp_ptD_B = 0x94)
register BYTE           *temp_fpt_B;
#pragma locate (temp_fpt_B = 0x98)
// > 端口模块内部可自行使用

