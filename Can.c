/*****************************************************************************/
/*       FileName  :   CAN.C                                                 */
/*       Content   :   DSA-208 CAN  Module                                   */
/*       Date      :   Sat  08-16-2003                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/
             //-CMU代表第二类设备
/**第二类设备周期定时查询第一、三类设备，查询间隔时间T3(10s-30s)一般应较长，以空下时间完成其它突发进程。
查询结果填入第二类设备自己的RAM中，以备上级通信设备查询时立即上送。
通信出错的判据是判断在通信软件无误的情况下，由硬件引起的通信中断。第二类设备应以能否收到周期定时的信息
查询结果作为通信出错的判据，而不定期的取装置信息和设定装置，不应作为通信出错的判据。<通信出错的判据>
第二类设备须定时向第一、三类设备发送校时报文，校时间隔不大于10分钟。
第二类设备在通信进程中，经常会碰到“所问非所答”的情况：
a)	收到”正忙”报文，一般情况应放弃本次进程。
b)	收到COS报文，立即回以确认报文。
c)	收到状态变化报文，存入任务队列，以后从任务队列中取出依次执行。
**/


;;;对于CAN口应该最简单的就是接受+发送和一个处理过程,为了网络的顺畅应该快速的进行接受+发送
;;;具体的处理发送缓存区中来慢慢处理.
;;;在1MS中断中进行发送CAN0_Port_Transmit();即判断要不要发送是实时的,
;;;而接收是在中断exint0_int中进行的,可能是有新内容接收到就促发这样的中断进行接收[CAN0_Port_Receive()]
;;;和CPU的不同之处出现了,那是放在同一个固定中断中的是查询方式,而这样是放在两个不同形式的
;;;中断之中,一个是查询,一个是中断.
;;;对数据的处理方法和CPU也是不同的,CPU是定时处理,而这里不再是定时处理了,而是周期处理CAN0_Main();
;;;其实,是一样的因为周期只是时间不确定的定时.切忌这个处理就不分什么接收与发送了,根据标志位办



/*---------------------------------------------------------------------------*/
/*                    Definition  of  global  variables                      */
/*---------------------------------------------------------------------------*/

//    none


/*---------------------------------------------------------------------------*/
/*                    Definition  of  local  variables                       */
/*---------------------------------------------------------------------------*/

//    none
                    ;;;以下定义的都是宏;;Frame是帧的意思
//-----------------  CAN definition  ----------------------
#define CAN_VERTIME_FRAME                         0x00       ;;;对时报文
#define CAN_SHORT_FRAME                           0x08       ;;;数据短报文
#define CAN_LONG_FRAME                            0x38       ;;;数据长报文  ;;;其实通信报文就这三种格式
#define CAN_CPUASKCMU_FRAME                       0x20

#define CAN_COMM_BEERR_NUM                           4   // must <200 
#define CAN_COMM_BEGOOD_NUM                          2

#define CAN_BROADCAST_ADDR                        0xff           ;;;广播地址

#define AND_MAX_CAN_NUMBER                        0x3f        ;;;数据缓冲区的最大范围?

#define BH_GUIHAO_IN_CAN_INTERNAL_PROTOCOL        0x01        ;;;保护_柜号__内部的_规约


//  CAN0 address: 0x00        ,MON 
//                0x01-0x3f   ,CPU_CAN0
//                0xff        ,Broadcast
#define MIN_CAN0_ADDR                             0x01             ;;;定义的CAN0的源地址的范围是1~63
#define MAX_CAN0_ADDR                             0x3f
//  CAN0 address: 0x00        ,MON   so CPU program needn't modify protocol 
//                0x40        ,as MON_CAN1 use
//                0x41-0x7f   ,CPU_CAN0
//                0xff        ,Broadcast
#define MIN_CAN1_ADDR                             0x41
#define MAX_CAN1_ADDR                             0x7f


    
//#define TIME_ASK_YC_VALUE                           30   //2004.09.05 HU HONGBING --yc faster 
//#define TIME_ASK_YX_VALUE                           61   
//#define TIME_ASK_YM_VALUE                           72   
//#define TIME_ASK_DD_VALUE                           80   
#define TIME_ASK_YC_VALUE                            5   
#define TIME_ASK_YX_VALUE                           10   
#define TIME_ASK_YM_VALUE                           15   
#define TIME_ASK_DD_VALUE                           15         ;;;直流量 计时单位是2048MS

#define TIME_CAN_VERIFY_TIME_VALUE                  15    

#define CAN_WAIT_CPU_MSG_TIME                     800   // 800ms

#define CAN_NOW_POL_NONE                          0x00
#define CAN_NOW_POL_YCALL                         0x01
#define CAN_NOW_POL_YXALL                         0x02
#define CAN_NOW_POL_YMALL                         0x03
#define CAN_NOW_POL_DD_AC                         0x04


//  CAN_MON_Status       
#define CAN_MON_IDLE                              0xf0
#define CAN_MON_WAITCPUREPLY                      0xf1
#define CAN_MON_TRANSINGTOCPU                     0xf2


//  CAN_CODE
#define CODE_CAN_RD_CMU_MEM                       0x01
#define CODE_CAN_WR_CMU_MEM_BYTE                  0x02
#define CODE_CAN_WR_CMU_MEM_WORD                  0x03
#define CODE_CAN_RD_CMU_INFO_RCD_MYSELF           0x04
#define CODE_CAN_RD_CMU_INFO_RCD_SYSTEM           0x05
#define CODE_CAN_WR_CMU_INFO_RCD_MYSELF           0x06
#define CODE_CAN_WR_CMU_INFO_RCD_SYSTEM           0x07
#define CODE_CAN_RD_CMU_INFO_CN_RESET             0x08
#define CODE_CAN_WR_CMU_INFO_CN_RESET             0x09



#define CODE_CAN_STATUS_CHANGE                    0x01        ;;; CPU，CSU主动上送状态变化，CMU不用确认
#define CODE_CAN_COS_BEGIN                        0x10        ;;; CPU主动上送COS，CMU需确认  ;;;10H，11H，12H，13H  这些代表遥信的组数,且这还多了一个非标配14
#define CODE_CAN_COS_END                          0x14  //0x13
#define CODE_CAN_COS_ACK                          0x1f        ;;; CMU确认：  若CPU在500ms内未收到确认，则继续主动上送。
#define CODE_CAN_STH_RESET                        0x30        ;;; CMU复位CPU，CPU需回答（这个复位是清标志位）
#define CODE_CAN_STH_RESET_ACK                    0x31        ;;; CPU确认
#define CODE_CAN_BRDCST_RST_CPU                   0x32        ;;; CMU广播复位CPU数据，CPU不需回答
#define CODE_CAN_BRDCST_RST_RLY                   0x33        ;;;CMU广播复归CPU保护动作信号，CPU不需回答
#define CODE_CAN_FREZSETDD                        0x34        ;;;CMU设置电度信息
#define CODE_CAN_PAUSE_CPU_AUTO                   0x35        ;;; CMU广播或非广播暂停CPU主动上送  ;;;要学习人家用英文定义,这样才有效
#define CODE_CAN_RST_CPU_COMM                     0x36        ;;;CMU广播或非广播复位通信参数
#define CODE_CAN_YK_CHOOSE                        0x40        ;;; CMU遥控CPU
#define CODE_CAN_YK_CONFIRM                       0x41        ;;;遥控执行CMU->CPU
#define CODE_CAN_CONFIRMMSG                       0x80
#define CODE_CAN_CANT_REPLYINFO                   0xf0        ;;; CMU，CPU，CSU应答其它单元“无法上送所请求信息”

#define CANT_RPLCPUBUSY                           0x01
#define CANT_RPLCPUNOINFO                         0x02
#define CANT_RPLCPUNOSUPT                         0x03
#define CANT_RPLWAITCPUTIMEOUT                    0x04
#define CANT_RPLMONTRANFAIL                       0x05
#define CANT_RPLRECERTUERRCODE                    0x06        // MON RECE RTU 
#define CANT_RPLRECECPUERRCODE                    0x07        // MON RECE CPU 
#define CANT_RPLWAITRTUTIMEOUT                    0x08

         ;;;以下是长报文的命令代码
#define CODE_CAN_YC_ALL                           0x60       ;;;查询全遥测 回答全YC
#define CODE_CAN_YC_CHG                           0x61       ;;; 查询变化YC              回答变化YC
#define CODE_CAN_YX_ALL                           0x62       ;;;查询全YX                回答全YX
#define CODE_CAN_YX_ALL_CLRCOSSOE                 0x63       ;;;查询全YX并清COS、SOE   回答全YX
#define CODE_CAN_YM_ALL                           0x64       ;;;查询全YM                回答全YM
#define CODE_CAN_DD_AC                            0x65       ;;;查询全DD_AC             回答全DD_AC         ;;;有功无功
#define CODE_CAN_SOE                              0x66       ;;;查询SOE                 回答SOE
#define CODE_CAN_LIFE_NOTIME                      0x67       ;;;查询无时标工况           回答无时标工况
#define CODE_CAN_LIFE_TIME                        0x6c       ;;;查询带时标工况           回答带时标工况
#define CODE_CAN_EVENT                            0x68       ;;;查询保护事件             回答保护事件
#define CODE_CAN_W_R_RTU_DATA                     0x69       ;;;操作参数                 回答参数
#define CODE_CAN_W_R_RLY_DATA                     0x6a       ;;;操作保护                 回答保护
#define CODE_CAN_REQUIRE_CPU                      0x6b       ;;;查询CPU                 回答查询
#define CODE_CAN_EQUIP_INFO                       0x6d       ;;;查询装置信息             回答装置信息
#define CODE_CAN_WAVRCDDATA                       0x6e       ;;;申请录波数据             回答录波数据
#define CODE_CAN_EVENT_EXT                        0x6f  // 2004-07-10
#define CODE_CAN_ACK_LONG_FRAME                   0x80
#define CODE_CAN_SOE_ACK                          0x76       ;;;确认接收SOE
#define CODE_CAN_LIFE_NOT_ACK                     0x77       ;;;确认接收工况变化(不带时标)
#define CODE_CAN_EVENT_ACK                        0x78       ;;;确认接收保护事件
#define CODE_CAN_LIFE_T_ACK                       0x7c       ;;;确认接收工况变化(带时标)
#define CODE_CAN_EVENT_EXT_ACK                    0x7f  // 2004-07-10


#define CAN_NONEED_WAIT_CPU                       0xff
#define CAN_WAIT_CPU_RESETCPUACK                  0x00
#define CAN_WAIT_CPU_YKVERIFY                     0x01         ;;;这些可能是应用层内部的定义,与外部没有关系甚至于链路层都没有关系
#define CAN_WAIT_CPU_YKRESULT                     0x02
#define CAN_WAIT_CPU_YC                           0x03
#define CAN_WAIT_CPU_YCCHG                        0x04
#define CAN_WAIT_CPU_YX                           0x05
#define CAN_WAIT_CPU_YXCLR                        0x06
#define CAN_WAIT_CPU_YM                           0x07
#define CAN_WAIT_CPU_DDAC                         0x08
#define CAN_WAIT_CPU_SOE                          0x09
#define CAN_WAIT_CPU_LIFENOTM                     0x0a
#define CAN_WAIT_CPU_RLYEVENT                     0x0b
#define CAN_WAIT_CPU_EVENT_EXT                    0x0c  // 2004-07-10
#define CAN_WAIT_CPU_CPUPOL                       0x0c
#define CAN_WAIT_CPU_LIFETIME                     0x0d
#define CAN_WAIT_CPU_EQPINFO                      0x0e
#define CAN_WAIT_CPU_WAVRCD                       0x0f

#define CAN_WAIT_RTU_YKCONFIRM                    0x10

#define CAN_WAIT_CPU_RLYEVENTNO                   0x20
#define CAN_WAIT_CPU_RLYSETTING                   0x21
#define CAN_WAIT_CPU_RLYMEASURE                   0x22
#define CAN_WAIT_CPU_RSTRLYSIGN                   0x23
#define CAN_WAIT_CPU_RLYSETVERI                   0x24
#define CAN_WAIT_RTU_CHGSETACK                    0x25
#define CAN_WAIT_CPU_RLYOUTLIMT                   0x26
#define CAN_WAIT_CPU_RLYSETGRUP                   0x27
#define CAN_WAIT_CPU_SETGRUPVER                   0x28
#define CAN_WAIT_RTU_SETGRPCONF                   0x29
#define CAN_WAIT_CPU_RLYSETINFOH                  0x2a
#define CAN_WAIT_CPU_RLYSETINFOL                  0x2b


#define CAN_NONEEDTRANSTOHOST                     0xff

#define   LSA_CODE_ACK                    0x06
#define   LSA_CODE_NAK                    0x15
        ;;;?这些应该是应用层的定义所以,与外部没有关系
#define   LSA_CODE_C2_ASK_EVENT_RCD       0x40             ;;;ASK是取的意思,,取保护事件
#define   LSA_CODE_C3_ASK_SETTING         0x53            ;;;取保护定值
#define   LSA_CODE_C4_ASK_MEASURE         0x4d            ;;;取保护测量值
#define   LSA_CODE_C5_BH_FUGUI            0x4f            ;;;保护信号复归
#define   LSA_CODE_C6_SETTING_MODI        0x54            ;;;改定值
#define   LSA_CODE_C7_VERIFY_TIME         0x04            
#define   LSA_CODE_C11_ASK_OUTLIMIT_RCD   0x81            ;;;取保护越限记录
#define   LSA_CODE_C12_ASK_SETGRP         0x82            ;;;取保护定值组号
#define   LSA_CODE_C13_SETGRP_MODI        0x83            ;;;设置保护定值组号
#define   LSA_CODE_C14_SETGRP_MODI_ACK    0x84            ;;;确认修改定值组号
#define   LSA_CODE_C14_RLY_OPERATE_ACT    0x8f  // 2004-07-10

#define   LSA_CODE_R1_REPLY_EVENT_RCD     0x40            ;;;上送保护事件R1
#define   LSA_CODE_R2_REPLY_SETTING       0x53            ;;;上送保护定值
#define   LSA_CODE_R3_REPLY_MEASURE       0x4d            ;;;上送保护测量值
#define   LSA_CODE_R4_REPLY_SELFCHK       0x51            ;;;
#define   LSA_CODE_R5_REPLY_SETTING_VERI  0x54            ;;;定值返校
#define   LSA_CODE_R5_REPLY_SET_EXT_VERI  0x55  // 2004-07-10;;;
#define   LSA_CODE_R11_REPLY_OUTLIMIT_RCD 0x81            ;;;上送保护越限记录
#define   LSA_CODE_R12_REPLY_SETGRP       0x82            ;;;上送保护定值组号
#define   LSA_CODE_R13_REPLY_SETGRP_VERI  0x83            ;;;定值组号返校

#define   CAN0_Comm_Yx_LastSta			( (unsigned  char *)&(port_flag[12][32]) )	//-这个的意思和我刚才想的意思有偏差
						//-实际意思应该是取12行32列的地址





/*---------------------------------------------------------------------------*/
/*                        IMPORT            functions                        */
/*---------------------------------------------------------------------------*/
















/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BYTE CAN_MON_common_proc(void)
{
    if((CAN0_ReceProc_Buf[0]==CAN_SHORT_FRAME) && (CAN0_ReceProc_Buf[2]==CODE_CAN_CANT_REPLYINFO))
    {
        // inform other COM CAN can't reply
        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
        return YES;
    }
    else
    {
        return NO;
    }
}

//-说的那么好听数据库中有数据了,不知道这个数据库有多复杂呢,实际上呢,就是那么回事










/************************************************/
/* CAN_judge_state_to_yx                        */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void CAN_judge_state_to_yx(void)
{
	
    	BYTE *the_ram_addr_byte;
	    BYTE  the_ram_axh;	
	    
	the_ram_addr_byte=(BYTE *)&YX_State[IED_TX_STATE_START_WORD+(byte0(port_info[12].mirror_unit))/16];	   ;;;这个取出来的是CAN0配的绝对地址在YX中的状态,一位表示一台装置
	if(HOST_ZF_enable==YES)       ;;;表示主动再发的意思吗
    {
    	for(temp_loop=0;temp_loop<8;temp_loop++)
     	{
    		if(CAN0_Comm_Yx_LastSta[temp_loop]!=the_ram_addr_byte[temp_loop])       ;;;首先8台一比发现有不同的就继续,若都没有变化就继续查下一组
    		{				
    			for(temp_loop1=0;temp_loop1<8;temp_loop1++)
    			{
    				the_ram_axh=(1<<(temp_loop1%8));
    				if(((CAN0_Comm_Yx_LastSta[temp_loop])&(the_ram_axh))!=((the_ram_addr_byte[temp_loop])&(the_ram_axh)))   ;;;一台一台的比
    				{
    					Core_Temp_Loop=IED_TX_STATE_START_WORD+(byte0(port_info[12].mirror_unit))/16+temp_loop/2;    ;;;这个地方又把16个组合成了一块
    					Core_Det_Pt = (WORD *)&YX_State[Core_Temp_Loop];
					    Core_Src_Pt = &yx_change[yx_chg_in_pt].offset_no;             ;;;确定变位YX的偏移量,这个是相对于数组首说的
					   *Core_Src_Pt = Core_Temp_Loop/2;          Core_Src_Pt++; // offset
			    		if(((Core_Temp_Loop)& 0x01)==0)
					    {
					       *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX0      ;;;这个应该是8台装置的组合
			    		   *Core_Src_Pt = *(Core_Det_Pt+0x01);   Core_Src_Pt++; // YX1
			    		   if(((temp_loop)&0x01)==0)
					       *Core_Src_Pt = (WORD)(the_ram_axh);    
					       else
					       *Core_Src_Pt = (WORD)(the_ram_axh*0x100);
					       										  Core_Src_Pt++;   // CHG0
					       *Core_Src_Pt = 0x00;                                 // CHG1
			    		} 
					    else
 					    {
			    		   *Core_Src_Pt = *(Core_Det_Pt-0x01);   Core_Src_Pt++; // YX0
					       *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX1
					       *Core_Src_Pt = 0x00;                  Core_Src_Pt++; // CHG0
					       if(((temp_loop)&0x01)==0)
			    		   *Core_Src_Pt = (WORD)(the_ram_axh);   // CHG1             ;;;这些YX不知道实际意义所以不知道目的
			    		   else
			    		   *Core_Src_Pt = (WORD)(the_ram_axh*0x100);
					    }
   
					    yx_chg_in_pt ++;
			    		yx_chg_in_pt &= 0xff;         ;;;确保循环
    					// insert SOE
					    Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;  ;;;插入变化的遥信事件时也需要记录时间
						//ms
					   *Core_Det_Pt = REG_1Msecond;       ;;;这个就是整个系统的计时单位,也可以猜测REG就是用于整个系统的
					    Core_Det_Pt++;
						//sec min
					   *Core_Det_Pt = (WORD)REG_Minute*0x100 + REG_Second;
					    Core_Det_Pt++;
					    //hour day
					   *Core_Det_Pt = (WORD)REG_Date*256 + REG_Hour;
					    Core_Det_Pt++;
					    // channel state
					   
					   *Core_Det_Pt = Core_Temp_Loop*16+temp_loop1;      ;;;也有一种认为设置BUG的可能性,为了只让自己知道
					   if(((temp_loop)&0x01)==1)
					   *Core_Det_Pt = Core_Temp_Loop*16+temp_loop1+8;
					   if(((the_ram_addr_byte[temp_loop])&(the_ram_axh))!=0)
					   {
					  	 *Core_Det_Pt|=0x8000;
					   }
						yx_soe_in_pt++;                ;;;变化的遥信最多记录1024个多了就覆盖前面的了
					    if(yx_soe_in_pt>1023)
						    yx_soe_in_pt = 0;
					}    
    		
    			}
    			CAN0_Comm_Yx_LastSta[temp_loop]=the_ram_addr_byte[temp_loop];
    		}
     	}
    }
    else
    {
    	for(temp_loop=0;temp_loop<8;temp_loop++)
     	{
    		CAN0_Comm_Yx_LastSta[temp_loop]=the_ram_addr_byte[temp_loop];        ;;;保存64台装置的状态
     	}	
    
    }	
	
}	













/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BYTE CAN_MON_stachg_task_proc(void)
{
    WORD  dumyword;

    byte1(dumyword)=0;
    for(byte0(dumyword)=MIN_CAN0_ADDR;byte0(dumyword)<=MAX_CAN0_ADDR;byte0(dumyword)++)     ;;;从1~63检查单元
    {
        if(CAN0_IED_StaChg[byte0(dumyword)]!=0)          ;;;判断装置当前状态,一切正常为0
        {
            if((CAN0_IED_StaChg[byte0(dumyword)] & 0x01)!=0)
            {
                byte1(dumyword)=CODE_CAN_LIFE_TIME;  // life     ;;;查询带时标工况           回答带时标工况
                CAN0_IED_StaChg[byte0(dumyword)]=CAN0_IED_StaChg[byte0(dumyword)] & 0xfe;    ;;;处理完之后就把这个标志清楚
            }
            else
            {
                if((CAN0_IED_StaChg[byte0(dumyword)] & 0x04)!=0)
                {
                    byte1(dumyword)=CODE_CAN_EVENT;  // RLY Event       ;;;查询保护事件             回答保护事件
                    CAN0_IED_StaChg[byte0(dumyword)]=CAN0_IED_StaChg[byte0(dumyword)] & 0xfb;   ;;;这个和我看的规约有点不同,他的每位意义不一样
                }
                else
                {
                    if((CAN0_IED_StaChg[byte0(dumyword)] & 0x08)!=0)
                    {
                        byte1(dumyword)=CODE_CAN_SOE;  // SOE          ;;;查询SOE                 回答SOE
                        CAN0_IED_StaChg[byte0(dumyword)]=CAN0_IED_StaChg[byte0(dumyword)] & 0xf7;
                    }
                    else
                    {
                        if((CAN0_IED_StaChg[byte0(dumyword)] & 0x10)!=0)
                        {
                            byte1(dumyword)=CODE_CAN_YC_CHG;  // YC_CHG        ;;; 查询变化YC              回答变化YC
                            CAN0_IED_StaChg[byte0(dumyword)]=CAN0_IED_StaChg[byte0(dumyword)] & 0xef;
                        }
						            else
						            {// 2004-07-10         ;;;这个不是标配里的是宁加的,说明了只要能交流就行
	                        if((CAN0_IED_StaChg[byte0(dumyword)] & 0x20)!=0)
    	                    {
        	                    byte1(dumyword)=CODE_CAN_EVENT_EXT;  // RLY_EVENT_EXT
            	                CAN0_IED_StaChg[byte0(dumyword)]=CAN0_IED_StaChg[byte0(dumyword)] & 0xdf;
                	        }
                	      }    
                    }
                }
            }
            if(byte1(dumyword)!=0)             ;;;判断处理的结果,等于0说明没有任何情况需要处理的
            {
                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;      ;;;表示帧的类型
                CAN0_TranProc_Buf[1]=0x00;              // S
                CAN0_TranProc_Buf[2]=0x00;              // Group
                CAN0_TranProc_Buf[3]=byte1(dumyword);   // CMD         ;;;发送这帧的内容,或者说这个内容就是后台发来的要求向CPU取信息
                CAN0_TranProc_Buf[4]=0x00;              // LenL
                CAN0_TranProc_Buf[5]=0x00;              // LenH
                CAN0_TranProc_Buf[6]=byte1(dumyword);   // SumL    add from 'group'的累加和
                CAN0_TranProc_Buf[7]=0x00;              // SumH    ;;;这些数组内的内容就可以放入发送缓冲器中去了
                
                if(byte1(dumyword)==CODE_CAN_LIFE_TIME)
                {
                    REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_LIFETIME;        ;;;对应的内容给对应的值
                }
                else
                {
                    if(byte1(dumyword)==CODE_CAN_EVENT)
                    {
                        REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_RLYEVENT;
                    }
                    else
                    {
                        if(byte1(dumyword)==CODE_CAN_SOE)
                        {
                            REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_SOE;
                        }
                        else
                        {// 2004-07-10
                        	if(byte1(dumyword)==CODE_CAN_YC_CHG)
                        	{
                            	REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YCCHG;
                          }	
                          else
                            {
                            	REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_EVENT_EXT;
                            }
                        }
                    }
                }
                
                REG_CAN0_Port.TRAN_CPU_INFO_ADDR=byte0(dumyword);         ;;;向哪台CPU发信息
                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;              ;;;帧内容配置
                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;       ;;;相对于长报文而言的
                
                REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;          ;;;需要回答
                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;     ;;;转变思路再也不是两者之间的通信了,而是一个系统;需要发送到主机吗
                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;          ;;;这个标志就可以决定是否发送了,发送方向是CMU-->CPU
                
                return YES;
            }
        }
    }
    
    return NO;
}







/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BYTE CAN_MON_inquire_task_proc(void)        ;;;CAN_现在_需要_任务_过程
{
    if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_VERICPUCLK_TIME,TIME_CAN_VERIFY_TIME_VALUE)==YES)
    {
        CAN0_TranProc_Buf[0]=CAN_VERTIME_FRAME;           ;;;难道这个地方处理到0单元了,帧类型?
        
        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
        
        REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
        
        RAM_CAN0_Para.CAN_BEGIN_VERICPUCLK_TIME=Time_2048ms_Counter;
        return YES;         ;;;返回数值YES
    }
    
refind_addr:
    if(RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR>MAX_CAN0_ADDR)        ;;;          >63
    {
        if((Portx_Poll_First[12]!=YES)&&(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_DD_AC))        ;;;            4
        {
            Portx_Poll_First[12]=YES;
        }    
       	if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YCALL)            ;;;    1
       	{
		       if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YXALL_TIME,TIME_ASK_YX_VALUE)==YES)
		        {
           		RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YXALL;
           		RAM_CAN0_Para.CAN_BEGIN_POL_YXALL_TIME=Time_2048ms_Counter;
    		    }
    		   else
            {
			        if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME,TIME_ASK_YC_VALUE)==YES)
			         {
			            RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YCALL;
            		  RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME=Time_2048ms_Counter;
        		   }
        		   else  return NO;
    		    }
		    }
		else
        {	
	       	if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YXALL)
    	    {
			    if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YMALL_TIME,TIME_ASK_YM_VALUE)==YES)
    			{
			        RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YMALL;
			        RAM_CAN0_Para.CAN_BEGIN_POL_YMALL_TIME=Time_2048ms_Counter;
			    }
	    		else
    	        {
				    if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME,TIME_ASK_YC_VALUE)==YES)
				    {
			   	        RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YCALL;
            			RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME=Time_2048ms_Counter;
	        		}
	        		else  return NO;
    			}
		    }
			else
			{
		       	if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YMALL)
    		   	{
				    if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_DD_AC_TIME,TIME_ASK_DD_VALUE)==YES)
    				{
			            RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_DD_AC;
			           	RAM_CAN0_Para.CAN_BEGIN_POL_DD_AC_TIME=Time_2048ms_Counter;
				    }
		    		else
    		        {
					    if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME,TIME_ASK_YC_VALUE)==YES)
					    {
			    	        RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YCALL;
            				RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME=Time_2048ms_Counter;
		        		}
		        		else  return NO;
    				}
				}
	    		else
   		        {
					if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME,TIME_ASK_YC_VALUE)==YES)
					{
		    		    RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YCALL;
           				RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME=Time_2048ms_Counter;
	        		}
	        		else  return NO;
   				}
			}
        }
        RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR=MIN_CAN0_ADDR; // 1
    }
    else  // 1 type pol has not complete
    {
        if(RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR==REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx)
        {
            RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
            goto refind_addr;
        }
        // other ifs
            
        if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YCALL)
        {
            if(unit_info[RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit)].yc_val_num==0)
            {
                RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
                goto refind_addr;
            }
                
            CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
            CAN0_TranProc_Buf[1]=0x00;              // S
            CAN0_TranProc_Buf[2]=0x00;              // Group
            CAN0_TranProc_Buf[3]=CODE_CAN_YC_ALL;   // CMD
            CAN0_TranProc_Buf[4]=0x00;              // LenL
            CAN0_TranProc_Buf[5]=0x00;              // LenH
            CAN0_TranProc_Buf[6]=CODE_CAN_YC_ALL;   // SumL    add from 'group'
            CAN0_TranProc_Buf[7]=0x00;              // SumH
                
            REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
            REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YC;
        }
        else
        {
            if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YXALL) 
            {
                if(unit_info[RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit)].yx_num==0)
                {
                    RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
                    goto refind_addr;
                }
               
                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                CAN0_TranProc_Buf[1]=0x00;              // S
                CAN0_TranProc_Buf[2]=0x00;              // Group
                CAN0_TranProc_Buf[3]=CODE_CAN_YX_ALL;   // CMD
                CAN0_TranProc_Buf[4]=0x00;              // LenL
                CAN0_TranProc_Buf[5]=0x00;              // LenH
                CAN0_TranProc_Buf[6]=CODE_CAN_YX_ALL;   // SumL    add from 'group'
                CAN0_TranProc_Buf[7]=0x00;              // SumH
               
                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YX;
            }
            else
            {
                if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YMALL) 
                {
                    if(unit_info[RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit)].ym_num==0)
                    {
                        RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
                        goto refind_addr;
                    }
                
                    CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                    CAN0_TranProc_Buf[1]=0x00;              // S
                    CAN0_TranProc_Buf[2]=0x00;              // Group
                    CAN0_TranProc_Buf[3]=CODE_CAN_YM_ALL;   // CMD
                    CAN0_TranProc_Buf[4]=0x00;              // LenL
                    CAN0_TranProc_Buf[5]=0x00;              // LenH
                    CAN0_TranProc_Buf[6]=CODE_CAN_YM_ALL;   // SumL    add from 'group'
                    CAN0_TranProc_Buf[7]=0x00;              // SumH
                
                    REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                    REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YM;
                }
                else  // CAN_Now_Pol_DD_AC
                {
                    if(unit_info[RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit)].dc_num==0)
                    {
                        RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
                        goto refind_addr;
                    }
                
                    CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                    CAN0_TranProc_Buf[1]=0x00;              // S
                    CAN0_TranProc_Buf[2]=0x00;              // Group
                    CAN0_TranProc_Buf[3]=CODE_CAN_DD_AC;    // CMD
                    CAN0_TranProc_Buf[4]=0x00;              // LenL
                    CAN0_TranProc_Buf[5]=0x00;              // LenH
                    CAN0_TranProc_Buf[6]=CODE_CAN_DD_AC;    // SumL    add from 'group'
                    CAN0_TranProc_Buf[7]=0x00;              // SumH
               
                    REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                    REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_DDAC;
                }
            }
        }
            
        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR;
        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                
        REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;
        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;

        RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
            
        return YES;
    }
    
    return NO;
}









    
    
    
/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/

//-这个肯定就是在中断中定时接收处理了,和保护里面的是一样的处理
/*===========================================================================*/
void CAN0_Port_Receive(void)    // in Exint0_int           ;;;在这个中断中需要执行
{
    register WORD  reg_ibx;                        ;;;把这个变量放在片内寄存器中,具体放在哪我不管只要在这个内就行
        near BYTE *the_ram_base_addr;               ;;;这个仅仅起到局域变量的作用


    //   store CAN_Frame to COM0

    byte0(reg_ibx)=*(far BYTE *)(CAN0_RECE_BUF+0x03);   // sourcer addr         ;;;源地址   两个内容相比
    if( (byte0(reg_ibx)<MIN_CAN0_ADDR) || (byte0(reg_ibx)>MAX_CAN0_ADDR) )      ;;;也就是说接在CAN0上的CPU 地址只能在这个范围内超过不行
    {
        return;       ;;;超过就不处理
    }       
    if(((BYTE)(RAM_CAN0_Para.CAN_PORT_RECE_BUF_SAV_PTR+1))!=RAM_CAN0_Para.CAN_PORT_RECE_BUF_TAK_PTR)   ;;;这种判断依据仅仅是防错,相等不行是溢出了
    {   // Buf len 256.   if overflow no Rece it to CAN0_RecePort_Buf
        the_ram_base_addr=(near BYTE *)&CAN0_RecePort_Buf+RAM_CAN0_Para.CAN_PORT_RECE_BUF_SAV_PTR*12; ;;;先取缓冲区的基址,再加上偏移量(每12个数据一块)

        the_ram_base_addr[0x00]=*(far BYTE *)(CAN0_RECE_BUF+0x01);       ;;;对从总线上接受到的帧进行保存,这是接收到的信息的第一次且唯一一次接收
        the_ram_base_addr[0x01]=*(far BYTE *)(CAN0_RECE_BUF+0x02);       ;;;这个地方就是把接收到的内容取到内部存储器中供处理用,SJA1000本身的资源有限
        the_ram_base_addr[0x02]=*(far BYTE *)(CAN0_RECE_BUF+0x03);       ;;;由于接收到的内容已经确定,所以没有记录"帧信息" 
        the_ram_base_addr[0x03]=*(far BYTE *)(CAN0_RECE_BUF+0x04);
        the_ram_base_addr[0x04]=*(far BYTE *)(CAN0_RECE_BUF+0x05);
        the_ram_base_addr[0x05]=*(far BYTE *)(CAN0_RECE_BUF+0x06);
        the_ram_base_addr[0x06]=*(far BYTE *)(CAN0_RECE_BUF+0x07);
        the_ram_base_addr[0x07]=*(far BYTE *)(CAN0_RECE_BUF+0x08);
        the_ram_base_addr[0x08]=*(far BYTE *)(CAN0_RECE_BUF+0x09);
        the_ram_base_addr[0x09]=*(far BYTE *)(CAN0_RECE_BUF+0x0a);
        the_ram_base_addr[0x0a]=*(far BYTE *)(CAN0_RECE_BUF+0x0b);
        the_ram_base_addr[0x0b]=*(far BYTE *)(CAN0_RECE_BUF+0x0c);  ;;;接收缓冲器虽然可读大小只有13B窗口,但是他有一个FIFO的空间64B
                     ;;;并没有什么绝对的对,有的只是能用

        byte0(reg_ibx)=0;       ;;;没有什么特殊的含义,作为活动寄存器他又有了新任务
        if(the_ram_base_addr[0x00]==0x00)  // slave reply            ;;;检查目标地址是否正确,从机_回复 即只要收到这样的报文立即回以确认报文,处理那是后话
        {
	        if((Now_CANx_Sw_Channel_Sta[the_ram_base_addr[0x02]] & 0x10)!=0)  // may rece on CAN0 ;;;是表示现在可以接收这个单元的报文吗
    	    {
	    	    if( (*(far BYTE *)(CAN0_RECE_BUF+0x01)== 0x00) &&                          ;;;帧的格式是固定的,需要人为组合长报文
    	    	   ((*(far BYTE *)(CAN0_RECE_BUF+0x04) & 0x38)==CAN_SHORT_FRAME) )
	        	{
    	        	byte1(reg_ibx)=*(far BYTE *)(CAN0_RECE_BUF+0x06);   // CMD
	    	        if((byte1(reg_ibx)>=CODE_CAN_COS_BEGIN) &&
    	    	       (byte1(reg_ibx)<=CODE_CAN_COS_END  ))        // COS auto   ;;;判断这帧的命令功能,如果在这个范围内就是这个功能;并相应处理发确认报文
        	    	{
            	    	if(((RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR+1) & AND_CAN_COSACK_QUELEN)    ;;;范围是0~15
                	    !=RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR)
	                	{
    	                	CAN0_COSACK_Buf[RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR]=*(far BYTE *)(CAN0_RECE_BUF+0x03);    ;;;这个地方是记录接收帧的源地址吗
        	                	          ;;;记录发来全遥信的源地址
	            	        RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR =(RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR+1) & AND_CAN_COSACK_QUELEN;
    	            	           ;;;这些也决定是否发送内容
	    	            }
    	    	        else
        	    	    {
            	    	    byte0(reg_ibx)=1;    
	                	}
		            }
    		    }   
    	    }
        	else
	        {
   		        byte0(reg_ibx)=1;        ;;;没有成功接收就填1
        	}
        }
        else
        {
        	if((the_ram_base_addr[0x00]==Vqc_Info[0].DSA_243_Addr)||         ;;;无功补偿装置的地址已经在数组中固定,SJA1000的接收是来者不拒的,接收完了在区别
        	   (the_ram_base_addr[0x00]==Vqc_Info[1].DSA_243_Addr))
        	{
	        	Now_CANx_Sw_Channel_Sta[the_ram_base_addr[0x02]] &= 0x3f;       ;;;如果是发给两个无功补偿装置的报文管理机也接收处理
		        Now_CANx_Sw_Channel_Sta[the_ram_base_addr[0x02]] += 0x40;    // may send on CAN0  ;;;给标志位赋值
        	}
        	else
	        {
   		        byte0(reg_ibx)=1;                ;;;是不是只要不是发来的内容就记录开来,即目标地址不对
        	}
        }


         
      if(byte0(reg_ibx)==0)     RAM_CAN0_Para.CAN_PORT_RECE_BUF_SAV_PTR++;      ;;;只要是发给这个单元的就记录下来,可能是由于端口太多了,不能及时处理,所以先记录,成功接收一个就加1
    }
        
} 
















     ;;;要有自己同一的命名习惯
//-这个地方的处理是对接收到的数据块处理,比定时接收小一个等级
/*===========================================================================*/
void CAN_Port_ReceAsm(void)    // in Main_loop        ;;;接收的过程应该和处理的过程是分开的这样才能保证实时性
{
    register      WORD  reg_iax;                     ;;;这个地方的转化就因该属于是应用层的范畴了
    register near BYTE *reg_ibx;                 ;;;?虽然放在内部寄存器中但也还是局部变量
             BYTE  ram_the_save_ptr;
             BYTE  ram_the_take_ptr;
             WORD  ram_ax;
             WORD  ram_bx;
             WORD  ram_cx;
             WORD  ram_dx;
			 long  ram_ulx;	
             BYTE  ram_array[80];             ;;;这个开辟的是临时阵列
             
         far BYTE *ram_the_base_addr;
         far BYTE *ram_the_base_addr_b;
         far WORD *ram_the_base_addr_w;
         far WORD *ram_the_base_addr_w1;


    ram_the_save_ptr=RAM_CAN0_Para.CAN_PORT_RECE_BUF_SAV_PTR;	//-记录了已经接收到的个数
    ram_the_take_ptr=RAM_CAN0_Para.CAN_PORT_RECE_BUF_TAK_PTR;        ;;;方便了块处理,减少了关联性,且可以中断继续接收

    while(ram_the_take_ptr!=ram_the_save_ptr)    ;;;不等说明有新的帧接收到,用了一个循环知道处理完所有的接收内容之后才结束,一次不止处理一帧信息
    {	//-本规约报文帧长度是固定的12个字节
        reg_ibx=(near BYTE *)&CAN0_RecePort_Buf+ram_the_take_ptr*12;    ;;;取得现在要处理内容的首地址,对与13字节的报文只取了后12个,然后继续从零开始编址
        
        if(*(near BYTE *)(reg_ibx+0x00)==CAN_BROADCAST_ADDR)         //-判断是广播地址吗
        {
            // broadcast process                   ;;;若是的对与CMU 没有处理
        }
        else  // destin addr=0     ;;;本单元指定地址为0
        {
            if(*(near BYTE *)(reg_ibx+0x00)==0x00)       //-判断是否是要发给我CMU的帧,相等说明是发给我通讯管理机得
            {
                byte0(reg_iax)=*(near BYTE *)(reg_ibx+0x03) & 0x38;     //-取出接收到帧的类型
                if(byte0(reg_iax)==CAN_VERTIME_FRAME)                   //-从现在开始英语随处不在,它也就是一个工具而已,判断是否是对时报文
                {
                    // verify time process     ;;;核实时间的过程,由于是CMU向下对时 所以其他发来的肯定就无效处理了
                }
                else                   ;;;其实,通信帧的类型只有三个
                {
                    if(byte0(reg_iax)==CAN_SHORT_FRAME)  // CAN_SHORT_FRAME
                    {
                        byte0(reg_iax)=*(near BYTE *)(reg_ibx+0x02);   // addr      ;;;取源地址即哪个单元发来的
                        CAN0_CurrentSta_Buf[byte0(reg_iax)]=*(near BYTE *)(reg_ibx+0x04) & 0x7f;   //-取通信状态S,即发来帧的单元
                    
                        byte1(reg_iax)=*(near BYTE *)(reg_ibx+0x05);        // CMD
                        if(byte1(reg_iax)==CODE_CAN_STATUS_CHANGE)          // status auto ;;;判断是否是CPU，CSU主动上送状态变化，CMU不用确认的帧
                        {	//-那么可能改变单元号的时候,就需要清零处理
                            CAN0_IED_StaChg[byte0(reg_iax)]=*(near BYTE *)(reg_ibx+0x06)	//-这类信息应该是HOST发来的
                                                            | CAN0_IED_StaChg[byte0(reg_iax)];	//-因为有变化值就是1,所以直接把变化量加上就行
                        }
                        else
                        {
                            if((byte1(reg_iax)>=CODE_CAN_COS_BEGIN) &&      //-0x10~14
                               (byte1(reg_iax)<=CODE_CAN_COS_END  ))        // COS auto 上送,CMU需确认
                            {	//-当定义单元为1时并不是说这个单元在内部的绝对地址就是1还要加上,配置时的偏移量
                                byte0(ram_ax)=byte0(port_info[PORT_NO_CAN_0].mirror_unit)+*(near BYTE *)(reg_ibx+0x02);  // Dbase addr ;;;得在数据库中绝对地址
                                byte1(ram_ax)=*(near BYTE *)(reg_ibx+0x05)-CODE_CAN_COS_BEGIN;     //-这帧的命令上送YX的组数

				               if(byte1(ram_ax) > 0)  // at least 1 YX COS byte ;;;决定是否有YX送上
                                {  //-首先把所有这个单元的YX值复制下来(根据配置来),然后把变化了的修改,最后把数据更新到YX_state[]中,只更新的最大的即保证变化的都改了就行
                                    byte1(ram_bx)=unit_info[byte0(ram_ax)].yx_num;  // yx_num as words;;;取出配置的遥信数
                                    ram_the_base_addr_w1=(far WORD *)&(YX_State[unit_info[byte0(ram_ax)].yx_start]);  //-遥信的首地址,初始化的时候全给0,当配置好了之后每台装置对应的空间是固定的
                                    for(byte0(ram_bx)=0;byte0(ram_bx)<byte1(ram_bx);byte0(ram_bx)++)   //-从第0个开始检查直到配置的最后一个
                                    {     //-把装置配的YX单元的数值内容都存进port_report[]中,现在接收到更新数据为什么,需要把上次的值保存一下呢,不直接替代
                                        port_report[byte0(ram_bx)*2+0x00]=byte0(*ram_the_base_addr_w1);     //-这个里面的值从头给
                                        port_report[byte0(ram_bx)*2+0x01]=byte1(*ram_the_base_addr_w1);
                                        ram_the_base_addr_w1++;       ;;;这个数组中已经记录了遥信的内容,这个地方再一次保存
                                    }    
                                	//-首先把所有的旧值读出的原因可能是并一定所有的值都会更新,可能仅仅只有一部分更新
                                    // fill new YX value
                                    byte1(ram_bx)=byte1(ram_bx)*2;     //-配置的时候遥信是按字算的分组的时候遥信是按字节给的,所以乘以2
                                    
                                    
                                    if(byte1(ram_ax)>3)	//at least 4 yx Cos byte,add by kkk
                                	  {
                                		   byte0(reg_iax)=*( BYTE *)(reg_ibx+0x06);   ;;;切忌这个BYTE不是byte他是类型定义的产物,取出遥信组号
                                		   if(byte0(reg_iax)==0x03)  // yx_num
                                       {
                                        	port_report[0]=*( BYTE *)(reg_ibx+0x07); // YX_Value    ;;;由处理缓冲区直接给值
                                        	port_report[1]=*( BYTE *)(reg_ibx+0x08); // YX_Value
                                        	byte1(reg_iax)=byte0(reg_iax)+1;
                                   	   }
                                		
                                		   if(byte0(reg_iax)==0x0c)  // yx_num
                                    		{
                                        	port_report[2]=*( BYTE *)(reg_ibx+0x07); // YX_Value
                                        	port_report[3]=*( BYTE *)(reg_ibx+0x08); // YX_Value
                                        	byte1(reg_iax)=byte0(reg_iax)+1;
                                   		  }
                                	  }    //add by kkk         ;;;这个是特殊功能增加的,不是原来的标配...
                                	  else
                                	  {
	                                    byte0(reg_iax)=*( BYTE *)(reg_ibx+0x06);  // COS:YX_No  ;;;取出YX组号即变化的是哪个YX
	                                    if(byte0(reg_iax)<byte1(ram_bx))  // yx_num        ;;;这个地方是标配,用组号,小于说明没有溢出
	                                    {
	                                        port_report[byte0(reg_iax)]=*( BYTE *)(reg_ibx+0x07); // YX_Value  ;;;变化量替代原来的值,若没有变化就没有必要替换
	                                        byte1(reg_iax)=byte0(reg_iax);         //-给YX组号,记录那组写入了新值
	                                    }
	                                
	                                    if(byte1(ram_ax)>1)  // at least 2 YX COS byte   ;;;标配中至少有两组
	                                    {
	                                        byte0(reg_iax)=*( BYTE *)(reg_ibx+0x08);  // COS:YX_No
	                                        if(byte0(reg_iax)<byte1(ram_bx))  // yx_num   ;;;第几组YX与人为安排的总组数比较
	                                        {
	                                            port_report[byte0(reg_iax)]=*( BYTE *)(reg_ibx+0x09); // YX_Value  ;;;只要实际YX在允许的范围内就替代对应的值
	                                            if(byte1(reg_iax)<byte0(reg_iax)) byte1(reg_iax)=byte0(reg_iax); ;;;上组的遥信号与这次的比较若这组的大接过接力棒
	                                        }
	    
	                                        if(byte1(ram_ax)>2)  // at least 3 YX COS byte
	                                        {
	                                            byte0(reg_iax)=*( BYTE *)(reg_ibx+0x0a);  // COS:YX_No
	                                            if(byte0(reg_iax)<byte1(ram_bx))  // yx_num  ;;;就是用小于号因为是从0开始计数的
	                                            {
	                                                port_report[byte0(reg_iax)]=*( BYTE *)(reg_ibx+0x0b); // YX_Value
	                                                if(byte1(reg_iax)<byte0(reg_iax)) 
	                                                byte1(reg_iax)=byte0(reg_iax);
	                                            }
	                                        }
	                                    }
                                	  }
                                
                                    Core_Src_Pt_B=&port_report[0];  //-为了通用空间是大大的但不一定都要使用,首地址
                                    Core_Src_Unit=byte0(ram_ax);                    ;;;数据库中的绝对地址(unit_info)
                                    Core_Src_Len =byte1(reg_iax)+1;                 //-记录共有几组遥性一组是1个B
                                    if((Core_Src_Len & 0x01)!=0) Core_Src_Len++;    ;;;四舍五入,这个数值是B可能为了给字而又不丢失数据就进一位

	                                //-可能是处于程序模块化的原因,本来可以直接向数据库填新值,但是并没有这样做,而是先在临时控制组织,最终在统一更新
                                    core_update_YX();           ;;;猜测这段的动能是把从保护装置中接收到的遥信上送到后台?
                                }
                            }
                            else     // other short frame   ;;;接收其它类型的短帧
                            {
                                if((REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE==YES)   &&
                                   (REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR  ==*(near BYTE *)(reg_ibx+0x02)))    ;;;源地址,总共12字节
                                {
                                    CAN0_ReceProc_Buf[0]=CAN_SHORT_FRAME;
                                    CAN0_ReceProc_Buf[1]=*(near BYTE *)(reg_ibx+0x04);        ;;;Data0
                                    CAN0_ReceProc_Buf[2]=*(near BYTE *)(reg_ibx+0x05);
                                    CAN0_ReceProc_Buf[3]=*(near BYTE *)(reg_ibx+0x06);
                                    CAN0_ReceProc_Buf[4]=*(near BYTE *)(reg_ibx+0x07);
                                    CAN0_ReceProc_Buf[5]=*(near BYTE *)(reg_ibx+0x08);
                                    CAN0_ReceProc_Buf[6]=*(near BYTE *)(reg_ibx+0x09);
                                    CAN0_ReceProc_Buf[7]=*(near BYTE *)(reg_ibx+0x0a);
                                    CAN0_ReceProc_Buf[8]=*(near BYTE *)(reg_ibx+0x0b);
                                
                                    REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=YES;	//-有CPU的信息需要处理
                                    REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE  =NO;	//-接收_CPU_回答信息_能够,,值为NO说明现在已经接收到了信息还没处理不能再接收了
                                }
                            }
                        }
                    }   
                    else  // not CAN_SHORT_FRAME
                    {
                        if(byte0(reg_iax)==CAN_LONG_FRAME)     // long frame
                        {
                            if((REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE==YES)   &&
                               (REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR  ==*(near BYTE *)(reg_ibx+0x02)))
                            {
                                byte0(reg_iax)=*(near BYTE *)(reg_ibx+0x01) & 0x3f;   // frame No.
                                if((byte0(reg_iax)==0) || (byte0(reg_iax)==REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR))
                                {
                                    if(byte0(reg_iax)==0)    // 1st frame      ;;;起始帧(长帧)
                                    {
		                                Core_Src_Unit=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);   ;;;得绝对地址,这些都是相对数组而言的
		                                   if(unit_info[Core_Src_Unit].unit_type==0x0101)   ;;;得到的地址是数组unit_info中的绝对地址
    		                                {
			                                Core_Src_Pt_B=&port_report[0];         ;;;把指针指向这个数组
			                                Core_Src_Len =0x02; // byte num
			                                
		                                	port_report[0]=byte0(YX_State[unit_info[Core_Src_Unit].yx_start]);
		                                	port_report[1]=byte1(YX_State[unit_info[Core_Src_Unit].yx_start]);
        		                        	if(((*(near BYTE *)(reg_ibx+0x05)) & 0x08)==0)       ;;;状态内容,查询当前有无事件信号未复归
        		                        		 port_report[1]&=0x7f;    ;;;C语言中区分大小写的
            		                    	else                                           
            		                    		 port_report[1]|=0x80;
		                                    core_update_YX();
                		                    }
                                        CAN0_CurrentSta_Buf[*(near BYTE *)(reg_ibx+0x02)]         ;;;记录这个单元状态的信息
                                            =*(near BYTE *)(reg_ibx+0x05) & 0x7f;             
                                        REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMESUM=*(near BYTE *)(reg_ibx+0x04);  ;;;记录长报文的总帧数 Num
                            
                                        if(REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMESUM<=64)     ;;;一个长报文的大小必须小于等于64
                                        {
                                            byte0(reg_iax)=*(near BYTE *)(reg_ibx+0x08);
                                            byte1(reg_iax)=*(near BYTE *)(reg_ibx+0x09);        ;;;取K值,通过这种方法组合成字
                                            if(reg_iax<=(WORD)(REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMESUM-1)*8)  ;;;正常情况应该小于等于
                                            {
                                                CAN0_ReceProc_Buf[0]=CAN_LONG_FRAME;
                                                CAN0_ReceProc_Buf[1]=*(near BYTE *)(reg_ibx+0x05);   // S 
                                                CAN0_ReceProc_Buf[2]=*(near BYTE *)(reg_ibx+0x06);   // Group
                                                CAN0_ReceProc_Buf[3]=*(near BYTE *)(reg_ibx+0x07);   // CMD
                                                CAN0_ReceProc_Buf[4]=*(near BYTE *)(reg_ibx+0x08);   // LenL
                                                CAN0_ReceProc_Buf[5]=*(near BYTE *)(reg_ibx+0x09);   // LenH
                                    
                                                if(REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMESUM>1)
                                                {
                                                    CAN0_ReceProc_Buf[6]=*(near BYTE *)(reg_ibx+0x0a);
                                                    CAN0_ReceProc_Buf[7]=*(near BYTE *)(reg_ibx+0x0b);   ;;;如果大于1帧就补完这些内容(属于第一数据位的)
                                                    REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR=1;   ;;;表示当前已经接收了一帧内容吗
                                                }
                                                else  // only 1 frame,no need verify suml,sumh
                                                {
                                                    REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=YES;   ;;;表示希望处理吗
                                                    REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE  =NO;    ;;;是已经接收了,不能再再接收了吗,除非处理好?
                                                }
                                            }   
                                        }
                                    }
                                    else  // later frame         ;;;不是0帧的处理过程
                                    {
                                        reg_iax=byte0(reg_iax)*8;
                                        CAN0_ReceProc_Buf[reg_iax+0x00]=*(near BYTE *)(reg_ibx+0x04);
                                        CAN0_ReceProc_Buf[reg_iax+0x01]=*(near BYTE *)(reg_ibx+0x05);
                                        CAN0_ReceProc_Buf[reg_iax+0x02]=*(near BYTE *)(reg_ibx+0x06);
                                        CAN0_ReceProc_Buf[reg_iax+0x03]=*(near BYTE *)(reg_ibx+0x07);
                                        CAN0_ReceProc_Buf[reg_iax+0x04]=*(near BYTE *)(reg_ibx+0x08);
                                        CAN0_ReceProc_Buf[reg_iax+0x05]=*(near BYTE *)(reg_ibx+0x09);
                                        CAN0_ReceProc_Buf[reg_iax+0x06]=*(near BYTE *)(reg_ibx+0x0a);
                                        CAN0_ReceProc_Buf[reg_iax+0x07]=*(near BYTE *)(reg_ibx+0x0b);
                                
                                        REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR
                                            =REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR+1;       ;;;又接收了一帧当然也加1了
                                    
                                        if(REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR>=
                                           REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMESUM)         ;;;判断是否所有的帧都接收完成了呢??
                                        {
                                            // here may add check Sum ,if error,restart receiving.
                                            // but now it do not check Sum.
                                            REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=YES;
                                            REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE  =NO;       ;;;这种组合就是在没有处理完之前不允许再接收了
                                        }
                                    }
                                }
                                else  // receive frame is not currnet frame wanted
                                {
                                    REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR=0;        ;;;可能就是一种出错的处理
                                }
                            }    
                        }
                        else
                        {
                        	                         ;;;除了这三钟形式,通信帧没有其它形式了
                        }
                    }
                }
            }
            else              ;;;若不是发给我CMU的帧就做以下处理
            {
                byte0(reg_iax)=*(near BYTE *)(reg_ibx+0x00);      ;;;取目标地址
                byte0(ram_cx)=0xff;               ;;;仅仅起标志作用
                if(byte0(reg_iax)==Vqc_Info[0].DSA_243_Addr) byte0(ram_cx)=0x00;   //-判断是否是电压无功控制,,?电压无功控制是两个特别的装置吗
                if(byte0(reg_iax)==Vqc_Info[1].DSA_243_Addr) byte0(ram_cx)=0x01;   //-且是哪台无功补偿装置
                if(byte0(ram_cx)<2)          ;;;若是两个无功控制之一,那么值就小于2
                {
                    if((*(near BYTE *)(reg_ibx+0x03))==CAN_LONG_FRAME)  
                    {
                        if( ((*(near BYTE *)(reg_ibx+0x01))==0x00)  // frame no
                          &&((*(near BYTE *)(reg_ibx+0x04))==0x01)  // frame num
                          )          ;;;帧号为0且帧数为1的处理
                        {
                            if((*(near BYTE *)(reg_ibx+0x07))==CODE_CAN_YC_ALL)  // ask YC ;;;查询全遥测回答全遥测
                            {
                                ram_array[0]=0x0a;  // frame num    ;;;前面的ram_array可能就是命令阵列,这个里面的内容并不是直接发送的还需要统一整理
                                ram_array[1]=0x00;  // S
                                ram_array[2]=0x00;  // Group
                                ram_array[3]=CODE_CAN_YC_ALL;  // CMD
                                ram_array[4]=0x42;  // LENL         ;;;由于这个最多安排32个遥测量,所以分两行传送,K就等于66
                                ram_array[5]=0x00;  // LENH
                                ram_array[6]=0x00;  // Line_No

                                for(ram_bx=0;ram_bx<16;ram_bx++)
                                {
                                    ram_ax=YC_State[Vqc_Info[byte0(ram_cx)].YC_NO_HighSide[ram_bx]];
									                 if((ram_ax & 0x0800)!=0) ram_ax|=0xf000;     ;;;难道YC数据位11是符号位,这个地方就仅仅是扩充吗
									                   ram_ulx=(short)ram_ax;
									                   
									                  if((ram_bx>0)&&(ram_bx<7))  // U    ;;;一点一点来解决最后把他们串起来,应用和开创不是一个阶层上的
									                    {                           ;;;这个局域内放的是电压数据
									                     	ram_ax =ram_ulx*128/75;
										                   if(((ram_bx==4)||(ram_bx==5))&&((Vqc_Info[byte0(ram_cx)].Reserved & 0x01)!=0))  ;;;假如是线电压的处理
											                     ram_ax =ram_ulx*739/250;                           ;;;预留位影响线电压的处理方法
									                    }
									                  else    ;;;有颜色的显示才是机器默认的类型
									                    {
										                        if((ram_bx>6)&&(ram_bx<11))  // I
									                           {
										                    	      ram_ax =ram_ulx*256/75;
									                       	   }
										                        else
										                          {
										                      	    if((ram_bx>10)&&(ram_bx<13))  // P,Q
										                     	        {
										                    		        ram_ax =ram_ulx*616/375;
										                      	      }
										                       	    else
										                            {
											                      	     if(ram_bx==13)        // cosQ       ;;;根据不同的数据类型给与相应的处理
												                             {
												                    	         ram_ax =ram_ulx*256/125;
											                                }
										                            }
									                         	  }
									                    }
                                    ram_array[ram_bx*2+7]=byte0(ram_ax);       ;;;取一个字的低位
                                    ram_array[ram_bx*2+8]=byte1(ram_ax);       ;;;取同一个字的高位
                                }    
                                
                                
                                ram_array[39]=0x01;  // Line_No         ;;;给行号
                                for(ram_bx=0;ram_bx<16;ram_bx++)
                                {
                                    ram_ax=YC_State[Vqc_Info[byte0(ram_cx)].YC_NO_LowSide[ram_bx]];
									if((ram_ax & 0x0800)!=0) ram_ax|=0xf000;
									ram_ulx=(short)ram_ax;
									if((ram_bx>0)&&(ram_bx<7))  // U
									{
										ram_ax =ram_ulx*128/75;
										if(((ram_bx==4)||(ram_bx==5))&&((Vqc_Info[byte0(ram_cx)].Reserved & 0x01)!=0))
											ram_ax =ram_ulx*739/250;
									}
									else
									{
										if((ram_bx>6)&&(ram_bx<11))  // I
										{
											ram_ax =ram_ulx*256/75;
										}
										else
										{
											if((ram_bx>10)&&(ram_bx<13))  // P,Q
											{
												ram_ax =ram_ulx*616/375;
											}
											else 
											{
												if(ram_bx==13)        // cosQ
												{
													ram_ax =ram_ulx*256/125;
												}
											}
										}
									}
                                    ram_array[ram_bx*2+40]=byte0(ram_ax);
                                    ram_array[ram_bx*2+41]=byte1(ram_ax);
                                }   
                                
                                
                                 
                                ram_ax=0;                           ;;;这就是一个临时寄存器,供以下累加用
                                for(ram_bx=2;ram_bx<72;ram_bx++)
                                {
                                    ram_ax+=ram_array[ram_bx];        ;;;求需要的累加和
                                }
                                ram_array[72]=byte0(ram_ax);           ;;;保存16位累加和
                                ram_array[73]=byte1(ram_ax);
                                
                                
                                for(ram_bx=0;ram_bx<10;ram_bx++)
                                {
                                    if((BYTE)(CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr+1)==   // Que_len=256
                                              CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr     )
                                    {
                                        CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr++;
                                    }
                                    ram_the_base_addr_b=(far BYTE *)&CAN0_CPUASKCMU_Trans_Buf[0]
                                                       +CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr*12;   ;;;用12个数据就足以说明一切
                                    ram_the_base_addr_b[0x00]=*(near BYTE *)(reg_ibx+0x02) ;  // destin addr  ;;;原来发来请求的地址在这变为了目的地址
                                    ram_the_base_addr_b[0x01]= ram_bx;                        // frame_no ;;;当前帧号
                                    ram_the_base_addr_b[0x02]= byte0(reg_iax);                // source addr
                                    ram_the_base_addr_b[0x03]= CAN_LONG_FRAME;                // 
                                    ram_the_base_addr_b[0x04]= ram_array[ram_bx*8+0x00];      // 数据区
                                    ram_the_base_addr_b[0x05]= ram_array[ram_bx*8+0x01];      // 
                                    ram_the_base_addr_b[0x06]= ram_array[ram_bx*8+0x02];      // 
                                    ram_the_base_addr_b[0x07]= ram_array[ram_bx*8+0x03];      // 
                                    ram_the_base_addr_b[0x08]= ram_array[ram_bx*8+0x04];      // 
                                    ram_the_base_addr_b[0x09]= ram_array[ram_bx*8+0x05];      // 
                                    ram_the_base_addr_b[0x0a]= ram_array[ram_bx*8+0x06];      // 
                                    ram_the_base_addr_b[0x0b]= ram_array[ram_bx*8+0x07];      // 
                                    CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr++;   ;;;又保存了一个等待发送
                                }
                            }
                            else
                            {
                                if((*(near BYTE *)(reg_ibx+0x07))==CODE_CAN_YX_ALL)  // ask YX
                                {
                                    ram_array[0]=0x02;  // frame num
                                    ram_array[1]=0x00;  // S
                                    ram_array[2]=0x00;  // Group
                                    ram_array[3]=CODE_CAN_YX_ALL;  // CMD
                                    ram_array[4]=0x08;  // LENL
                                    ram_array[5]=0x00;  // LENH
                                    ram_dx=0;
                                    for(ram_bx=0;ram_bx<16;ram_bx++)
                                    {
                                        ram_ax=YX_State[(Vqc_Info[byte0(ram_cx)].YX_NO[ram_bx])/16];
                                        if((ram_ax & (0x0001<<((Vqc_Info[byte0(ram_cx)].YX_NO[ram_bx])%16)))!=0)
                                        {
                                            ram_dx=ram_dx | (0x0001<<ram_bx);
                                        }
                                    }    
                                    ram_array[6]=0x00;  // YX_No
                                    ram_array[7]=byte0(ram_dx);
                                    ram_array[8]=0x01;  // YX_No
                                    ram_array[9]=byte1(ram_dx);
                                    ram_dx=0;
                                    for(ram_bx=16;ram_bx<32;ram_bx++)
                                    {
                                        ram_ax=YX_State[(Vqc_Info[byte0(ram_cx)].YX_NO[ram_bx])/16];
                                        if((ram_ax & (0x0001<<((Vqc_Info[byte0(ram_cx)].YX_NO[ram_bx])%16)))!=0)
                                        {
                                            ram_dx=ram_dx | (0x0001<<(ram_bx & 0x0f));
                                        }
                                    }    
                                    ram_array[10] =0x02;  // YX_No
                                    ram_array[11]=byte0(ram_dx);
                                    ram_array[12]=0x03;  // YX_No
                                    ram_array[13]=byte1(ram_dx);

                                    ram_ax=0;
                                    for(ram_bx=2;ram_bx<14;ram_bx++)
                                    {
                                        ram_ax+=ram_array[ram_bx];
                                    }
                                    ram_array[14]=byte0(ram_ax);
                                    ram_array[15]=byte1(ram_ax);
                                
                                    for(ram_bx=0;ram_bx<2;ram_bx++)
                                    {
                                        if((BYTE)(CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr+1)==   // Que_len=256
                                                  CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr     )
                                        {
                                            CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr++;
                                        }
                                        ram_the_base_addr_b=(far BYTE *)&CAN0_CPUASKCMU_Trans_Buf[0]
                                                           +CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr*12;
                                        ram_the_base_addr_b[0x00]=*(near BYTE *)(reg_ibx+0x02) ;  // destin addr
                                        ram_the_base_addr_b[0x01]= ram_bx;                        // frame_no
                                        ram_the_base_addr_b[0x02]= byte0(reg_iax);                // source addr
                                        ram_the_base_addr_b[0x03]= CAN_LONG_FRAME;                // 
                                        ram_the_base_addr_b[0x04]= ram_array[ram_bx*8+0x00];      // 
                                        ram_the_base_addr_b[0x05]= ram_array[ram_bx*8+0x01];      // 
                                        ram_the_base_addr_b[0x06]= ram_array[ram_bx*8+0x02];      // 
                                        ram_the_base_addr_b[0x07]= ram_array[ram_bx*8+0x03];      // 
                                        ram_the_base_addr_b[0x08]= ram_array[ram_bx*8+0x04];      // 
                                        ram_the_base_addr_b[0x09]= ram_array[ram_bx*8+0x05];      // 
                                        ram_the_base_addr_b[0x0a]= ram_array[ram_bx*8+0x06];      // 
                                        ram_the_base_addr_b[0x0b]= ram_array[ram_bx*8+0x07];      // 
                                        CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr++;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if(((*(near BYTE *)(reg_ibx+0x03))==CAN_SHORT_FRAME)&&((*(near BYTE *)(reg_ibx+0x01))==0x00)) ;;;是短帧的处理过程
                        {
                            if(  ( (*(near BYTE *)(reg_ibx+0x05))==CODE_CAN_YK_CHOOSE )        ;;;CMU接收到的应该是遥控选择反校帧
                               &&( (*(near BYTE *)(reg_ibx+0x06))==(*(near BYTE *)(reg_ibx+0x08)) )  // action type 
                               &&( (*(near BYTE *)(reg_ibx+0x07))==(*(near BYTE *)(reg_ibx+0x09)) )  // YK_NO
                               &&(((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_CLOSE)||((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_TRIP)) 
                               &&( (*(near BYTE *)(reg_ibx+0x07))<0x02)    ;;;这个地方就决定了Switch的值只能为0或1  @#@#$%
                              )   ;;;逻辑运算符的优先级 或<与<非
                            {
                            	if(port_transmit_flag[PORT_NO_P554_11]==0x55)           ;;;等于55表示又内容需要发送吗,或需要准备发送数据
                            	{
                            		port_transmit[PORT_NO_P554_11][0]=PORT_EXCHANGE_STA_START;   ;;;切忌管理机不能发送任何命令,他只是转发而已
                            		port_transmit[PORT_NO_P554_11][1]=PORT_NO_CAN_0;                 ;;;记录端口号
                            		port_transmit[PORT_NO_P554_11][2]=PROTOCOL_CAN_DSA%0x100;
                            		port_transmit[PORT_NO_P554_11][3]=PROTOCOL_CAN_DSA/0x100;        ;;;规约号
                            		
                            		port_transmit[PORT_NO_P554_11][8] =CORE_CODE_YK_CHOOSE;        ;;;记录接收到报文的具体内容
                            		port_transmit[PORT_NO_P554_11][9] =0x02;
                            		port_transmit[PORT_NO_P554_11][10]=0x00;
                            		port_transmit[PORT_NO_P554_11][16]=(*(near BYTE *)(reg_ibx+0x06));   ;;;记录具体的操作,合还是分
                            		if((*(near BYTE *)(reg_ibx+0x07))==0x00)     ;;;开关是0的处理
                            		{
                            		    port_transmit[PORT_NO_P554_11][5] =Vqc_Info[byte0(ram_cx)].YK0_208_YK_BOARD_Addr;
                            		    port_transmit[PORT_NO_P554_11][17]=Vqc_Info[byte0(ram_cx)].YK0_208_YK_BOARD_YKNO;
                            		}
                            		else
                            		{
                            		    port_transmit[PORT_NO_P554_11][5] =Vqc_Info[byte0(ram_cx)].YK1_208_YK_BOARD_Addr;
                            		    port_transmit[PORT_NO_P554_11][17]=Vqc_Info[byte0(ram_cx)].YK1_208_YK_BOARD_YKNO;
                            		}
                            		
                            		port_transmit_flag[PORT_NO_P554_11]=0xaa;       ;;;准备好转发数据之后就可以转发数据了吗
                            		
                            		CAN_246_YK_Begin_WAIT_VERIFY=Time_2048ms_Counter;
                            		
                            		CAN_246_Trans_YK_Buf[0]=CAN_246_TRANS_YK_STA_WAIT_VERIFY;
                            		CAN_246_Trans_YK_Buf[1]=(*(near BYTE *)(reg_ibx+0x02));      // CAN246_ADDR ;;;记录哪台装置发来的帧
                            		CAN_246_Trans_YK_Buf[2]=PORT_NO_P554_11;
                            		CAN_246_Trans_YK_Buf[3]=port_transmit[PORT_NO_P554_11][5];   // YK_BUS_ADDR
                            		CAN_246_Trans_YK_Buf[4]=(*(near BYTE *)(reg_ibx+0x06));      // Action_Type   ;;;那台装置发来的合或分的类型
                            		CAN_246_Trans_YK_Buf[5]=port_transmit[PORT_NO_P554_11][17];  // YK_No
                            		CAN_246_Trans_YK_Buf[6]=byte0(reg_iax);                      // CAN243_ADDR
                            		CAN_246_Trans_YK_Buf[7]=(*(near BYTE *)(reg_ibx+0x09));      // CAN_YK_No
                            	}
                            }
                            else
                            {
                                if(  ( (*(near BYTE *)(reg_ibx+0x05))==CODE_CAN_YK_CONFIRM )
                                   &&( (*(near BYTE *)(reg_ibx+0x06))==(*(near BYTE *)(reg_ibx+0x08)) )  // action type 
                                   &&( (*(near BYTE *)(reg_ibx+0x07))==(*(near BYTE *)(reg_ibx+0x09)) )  // YK_No
                                   &&(((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_EXECUTE)||((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_CANCEL)) 
                                   &&( (*(near BYTE *)(reg_ibx+0x07))<0x02)
                                  )
                                {
                                	(*((far BYTE *)(SOFT_ERR_FLAG+0x0008)))++;
                                	if((port_transmit_flag[PORT_NO_P554_11]==0x55)
                                	 &&(CAN_246_Trans_YK_Buf[0]==CAN_246_TRANS_YK_STA_WAIT_CONFIRM)
                                	 &&(                byte0(reg_iax)==CAN_246_Trans_YK_Buf[6])    // CAN243_ADDR
                                	 &&((*(near BYTE *)(reg_ibx+0x07))==CAN_246_Trans_YK_Buf[7])    // YK_No
                                	 )
                            	    {
                            		    port_transmit[PORT_NO_P554_11][0]=PORT_EXCHANGE_STA_START;
                                		port_transmit[PORT_NO_P554_11][1]=PORT_NO_CAN_0;
                                		port_transmit[PORT_NO_P554_11][2]=PROTOCOL_CAN_DSA%0x100;
                                		port_transmit[PORT_NO_P554_11][3]=PROTOCOL_CAN_DSA/0x100;
                            		
                                		if((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_EXECUTE)
                                		    port_transmit[PORT_NO_P554_11][8]=CORE_CODE_YK_EXECUTE;
                                		else
                                		    port_transmit[PORT_NO_P554_11][8]=CORE_CODE_YK_CANCEL;
                                		      
                                		port_transmit[PORT_NO_P554_11][9] =0x02;
                    	        		port_transmit[PORT_NO_P554_11][10]=0x00;
                        	    		port_transmit[PORT_NO_P554_11][16]=(*(near BYTE *)(reg_ibx+0x06));
                            			if((*(near BYTE *)(reg_ibx+0x07))==0x00)
                            			{
                            			    port_transmit[PORT_NO_P554_11][5] =Vqc_Info[byte0(ram_cx)].YK0_208_YK_BOARD_Addr;
                            		    	port_transmit[PORT_NO_P554_11][17]=Vqc_Info[byte0(ram_cx)].YK0_208_YK_BOARD_YKNO;
	                            		}
    	                        		else
        	                    		{
            	                		    port_transmit[PORT_NO_P554_11][5] =Vqc_Info[byte0(ram_cx)].YK1_208_YK_BOARD_Addr;
                	            		    port_transmit[PORT_NO_P554_11][17]=Vqc_Info[byte0(ram_cx)].YK1_208_YK_BOARD_YKNO;
                    	        		}
                            		
                        	    		port_transmit_flag[PORT_NO_P554_11]=0xaa;
                        	    		
                                        Rcd_Info_System_Tmp[0]=0;                         // reserved
                                        Rcd_Info_System_Tmp[1]=RCD_INFO_SYSTEM_AREA0_CAN;
                                        Rcd_Info_System_Tmp[2]=CAN_246_Trans_YK_Buf[6];   // YK_NO
			                            if((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_EXECUTE)
			                                Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_246_YK_EXECUTE;
			                            else    
			                                Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_246_YK_CANCEL;
            			                Rcd_Info_System_Tmp[4]=CAN_246_Trans_YK_Buf[2];  // source port
                        			    Rcd_Info_System_Tmp[5]=CAN_246_Trans_YK_Buf[3];  // 485 YK_BUS_ADDR
			                            Rcd_Info_System_Tmp[6]=CAN_246_Trans_YK_Buf[4];  // 485 Action_Type
            			                Rcd_Info_System_Tmp[7]=CAN_246_Trans_YK_Buf[5];  // 485 YK_NO
                        
                        			    Store_Rcd_Info_System();
                        	    		
                        	    		CAN_246_Trans_YK_Buf[0]=CAN_246_TRANS_YK_STA_IDLE;
                        	    		CAN_246_Trans_YK_Buf[1]=0xff;
                        	    		CAN_246_Trans_YK_Buf[2]=0xff;
                        	    		CAN_246_Trans_YK_Buf[3]=0xff;
                        	    		CAN_246_Trans_YK_Buf[4]=0xff;
                        	    		CAN_246_Trans_YK_Buf[5]=0xff;
                        	    		CAN_246_Trans_YK_Buf[6]=0xff;
                        	    		CAN_246_Trans_YK_Buf[7]=0xff;
                            		  }
                            	  }
                            }
                            
                        }
                    }
                }
            }
        }
        
        ram_the_take_ptr++;         //-表示又处理了一件事先,其实应该是舍弃了一个字节的内容吧
    }
    
    RAM_CAN0_Para.CAN_PORT_RECE_BUF_TAK_PTR=ram_the_save_ptr;	//-这个处理了的个数

} 













/*===========================================================================*/
void CAN0_Port_Transmit(void)    //  in short 1ms process
{
    register WORD  reg_ibx;
             BYTE  ram_need_port_trans;           ;;;用于记录是否可以最终发送了
             BYTE  ram_the_array[13];             ;;;帧记录区
         far BYTE *ram_the_base_addr;
             
    ram_need_port_trans=0;
    
    if(REG_CAN0_Port.F_TRANS_STATUS!=0)     //  transing,not know complete  ;;;如果正在发送就进入下面的判断过程
    {
        if( ( (Now_CANx_Sw_Channel_Sta[0x00]<0x80)&&((*(far BYTE *)(CAN0_STATUS+0) & 0x0c)==0x0c) )     ;;;判断发送缓冲器的状态看是否可以写入新数据
          ||( (Now_CANx_Sw_Channel_Sta[0x00]>0x7f)&&((*(far BYTE *)(CAN1_STATUS+0) & 0x0c)==0x0c) ) )
        {
            REG_CAN0_Port.F_TRANS_STATUS=0;   ;;;当下达了发送命令之后监测到发送结束了,那么就给0说明发送结束

        	if(Now_CANx_Sw_Channel_Sta[0x00]<0x80)      ;;;选择到底是CAN0还是1口
        	  {
	            CAN0_Comm_CurrentSta[0]=GOOD;        ;;;当前是好的
    	        if(Portx_Poll_First[12]!=YES)       ;;;Portx_Poll_First各口均工作正常后HOST_ZF_enable变0xaa
        	    {
            	    CAN0_Comm_Error_LastSta[0]=GOOD;
	            }
            
    	        if(CAN0_Comm_Error_LastSta[0]==ERR)
        	    {
            	    if(CN_CAN0_Comm_Error[0]>1)
                	{
                    	CN_CAN0_Comm_Error[0]=CN_CAN0_Comm_Error[0]-2;
	                }
    	            else
        	        {
            	        CN_CAN0_Comm_Error[0]=0;
                	}
                
	                if(CN_CAN0_Comm_Error[0]==0)
    	            {
        	            CAN0_Comm_Error_LastSta[0]=GOOD;
            	        CAN0_CurrentSta_Buf[0]=CAN0_CurrentSta_Buf[0] & 0x7f;
                	    // store 0 -> communicate_right in    event_rcd RAM
	                }
	            }    
	            else  // CAN0_Comm_Error_LastSta[0]==GOOD
    	        {
        	        CN_CAN0_Comm_Error[0]=0;
            	}
            }
          else
            {
	            CAN0_Comm_CurrentSta[MAX_CAN_NUMBER-1]=GOOD;             ;;;64
    	        if(Portx_Poll_First[12]!=YES)
        	    {
            	    CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]=GOOD;
	            }
            
    	        if(CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]==ERR)
        	    {
            	    if(CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]>1)
                	{
                    	CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]-=2;
	                }
    	            else
        	        {
            	        CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]=0;
                	}
                
	                if(CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]==0)
    	            {
        	            CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]=GOOD;
            	        CAN0_CurrentSta_Buf[MAX_CAN_NUMBER-1]=CAN0_CurrentSta_Buf[MAX_CAN_NUMBER-1] & 0x7f;
                	    // store 0 -> communicate_right in    event_rcd RAM
	                }
	            }    
	            else  // CAN0_Comm_Error_LastSta[0]==GOOD
    	        {
        	        CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]=0;
            	}
            }
            
            if(REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_TRANSINGTOCPU)        ;;;F2 正向CPU发信息
            {    // to be different from sending COSACK
                if(REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME==YES)        ;;;难道还有连续问题吗
                {
                    if(REG_CAN0_Proc.CAN_MON_NEED_REPLY==YES)      ;;;如果是最后一帧了就要看,有没有必要回答
                    {
                        RAM_CAN0_Para.CAN_BEGINWAITCPUREPLY_TIME=Time_1ms_Counter;
                        
                        REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=NO;
                        REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR=REG_CAN0_Port.TRAN_CPU_INFO_ADDR;      ;;;表示需要哪台CPU的回答
                        REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR=0;     ;;;清楚有关标志位
                        
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_WAITCPUREPLY;       ;;;等待CPU的回答
                        REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE=YES;             ;;;开通接收CPU回答的通道 
                    }
                    else
                    {
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;     ;;;表示不需要等待CPU回答吗
                    }
                }
            }
        }
        else   // can_status indicate not trans complete  ;;;不能写数据的处理
        {
            REG_CAN0_Port.F_TRANS_STATUS++;       ;;;从开始下达发送命令开始经过20MS还没有发送出去的就发送
            if(REG_CAN0_Port.F_TRANS_STATUS>20)    // 20ms
            {
                // may store info to indicate trans abort      ;;;超时发送不成功的就放弃这个内容
                if(Now_CANx_Sw_Channel_Sta[0x00]<0x80)          ;;;仅仅决定是CAN0还是1口需要下达这样的命令
                	*(far BYTE *)(CAN0_COMMAND+0)=0xe2;  // yao zhe trans_CAN  ;;;夭折发送 ;如果不是正在处理等待中的发送被取消
                else
                	*(far BYTE *)(CAN1_COMMAND+0)=0xe2;  // yao zhe trans_CAN
                	
                REG_CAN0_Port.F_TRANS_STATUS=0;
                
                if(Now_CANx_Sw_Channel_Sta[0x00]<0x80)  // fail at CAN0
                {
	                CAN0_Comm_CurrentSta[0]=ERR;         ;;;记录CAN0发送帧发生了错误,没有能够发送出去
    	            if(Portx_Poll_First[12]!=YES)       ;;;会不会是保证只记录一次呢
        	        {
            	        CAN0_Comm_Error_LastSta[0]=ERR;
                	}
                
	                if(CAN0_Comm_Error_LastSta[0]==GOOD)
    	            {
        	            if(CN_CAN0_Comm_Error[0]<CAN_COMM_BEERR_NUM)            ;;;4
            	        {
                	        CN_CAN0_Comm_Error[0]++;
                    	}
                    
	                    if(CN_CAN0_Comm_Error[0]>=CAN_COMM_BEERR_NUM)
    	                {
        	                CAN0_Comm_Error_LastSta[0]=ERR;
	                       	if(CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]==ERR)
                        	{
    	        	            for(reg_ibx=0;reg_ibx<MAX_CAN_NUMBER;reg_ibx++)
        	        	        {
        	            	        CAN0_Comm_CurrentSta[reg_ibx]=CAN0_Comm_CurrentSta[reg_ibx] | 0x80;
        	            	    }    
                        	}
	                    }
    	            }
        	        else  // F_RAM_CAN.CAN_Comm_Error_LastSta=ERR 
            	    {
                	    CN_CAN0_Comm_Error[0]=CAN_COMM_BEERR_NUM;
                    	for(reg_ibx=1;reg_ibx<(MAX_CAN_NUMBER-1);reg_ibx++)
	                    {
       	            		Now_CANx_Sw_Channel_Sta[reg_ibx]=0xa0;
                    	}
                	}
                }
                else
                {
	                CAN0_Comm_CurrentSta[MAX_CAN_NUMBER-1]=ERR;
    	            if(Portx_Poll_First[12]!=YES)
        	        {
            	        CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]=ERR;
                	}
                
	                if(CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]==GOOD)
    	            {
        	            if(CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]<CAN_COMM_BEERR_NUM)
            	        {
                	        CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]++;
                    	}
                    
	                    if(CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]>=CAN_COMM_BEERR_NUM)
    	                {
        	                CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]=ERR;
                        
                        	if(CAN0_Comm_Error_LastSta[0]==ERR)
                        	{
	            	            for(reg_ibx=0;reg_ibx<MAX_CAN_NUMBER;reg_ibx++)
    	            	        {
           	        	        	CAN0_Comm_CurrentSta[reg_ibx]=CAN0_Comm_CurrentSta[reg_ibx] | 0x80;
                	        	}
                	        }	
	                    }
    	            }
        	        else  // F_RAM_CAN.CAN_Comm_Error_LastSta=ERR 
            	    {
                	    CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]=CAN_COMM_BEERR_NUM;
                    	for(reg_ibx=1;reg_ibx<(MAX_CAN_NUMBER-1);reg_ibx++)
	                    {
               	    		Now_CANx_Sw_Channel_Sta[reg_ibx]=0x50;
                    	}
                	}
                }
                
                CAN0_Comm_CurrentSta[REG_CAN0_Port.TRAN_CPU_INFO_ADDR]=UNKNOW;
                
                if(REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_TRANSINGTOCPU)
                {// to be different from sending COSACK
                    REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                }
            }
        }
    }
    else
    {//  must put into the_array        ;;;是阵列的意思吗
        if(RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR!=RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR)  // COS_ACK 1st
        {
            ram_the_array[0x00]=0x88;
            ram_the_array[0x01]=CAN0_COSACK_Buf[RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR];  ;;;目的地址
            ram_the_array[0x02]=0x00;                                                       ;;;当前帧号
            ram_the_array[0x03]=0x00;             // MON Addr                               ;;;源地址
            ram_the_array[0x04]=CAN_SHORT_FRAME;  // short frame                            ;;;帧类型
            ram_the_array[0x05]=0x00;             // S                                      ;;;装置当前状态
            ram_the_array[0x06]=CODE_CAN_COS_ACK;                                           ;;;对CPU发来的COS表示收到 下发的确认报文 要在500MS之内发出
            ram_the_array[0x07]=0x00;
            ram_the_array[0x08]=0x00;
            ram_the_array[0x09]=0x00;
            ram_the_array[0x0a]=0x00;
            ram_the_array[0x0b]=0x00;
            ram_the_array[0x0c]=0x00;
            
            ram_need_port_trans=1;                ;;;准备好了帧数据用1表示现在可以发送
            RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR                                         ;;;完成一次发送指针就向下移动一位
                =(RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR+1) & AND_CAN_COSACK_QUELEN;       ;;;范围是0~15
        }
        else
        {
            if(REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_TRANSINGTOCPU)       // normal MSG 2nd ;;;处理程序中决定了这步的过程
            {
                ram_the_array[0x00]=0x88;
                if(CAN0_TranProc_Buf[0]==CAN_SHORT_FRAME)              ;;;缓冲区中的短报文就是在这个地方组织的,组织成标准帧的形式,进行发送
                {
                    ram_the_array[0x01]=REG_CAN0_Port.TRAN_CPU_INFO_ADDR;   ;;;目标地址
                    ram_the_array[0x02]=0;                                  ;;;当前帧号
                    ram_the_array[0x03]=0;                                  ;;;源地址
                    ram_the_array[0x04]=CAN_SHORT_FRAME;                    ;;;帧类型
                    
                    ram_the_array[0x05]=CAN0_TranProc_Buf[1];               ;;;实际的数据单元8字节
                    ram_the_array[0x06]=CAN0_TranProc_Buf[2];
                    ram_the_array[0x07]=CAN0_TranProc_Buf[3];
                    ram_the_array[0x08]=CAN0_TranProc_Buf[4];
                    ram_the_array[0x09]=CAN0_TranProc_Buf[5];
                    ram_the_array[0x0a]=CAN0_TranProc_Buf[6];
                    ram_the_array[0x0b]=CAN0_TranProc_Buf[7];
                    ram_the_array[0x0c]=CAN0_TranProc_Buf[8];
                    
                    ram_need_port_trans=1;                                ;;;最终给SJA1000下达发送命令就是这个吗?
                    REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=YES;             ;;;多个标志表示是YES了吗
                }
                else
                {
                    if((CAN0_TranProc_Buf[0]==CAN_VERTIME_FRAME) && (REG_Year%100 < 70))//fjh 2006 07 20
                    {
                        //Clock_Process();
                        ram_the_array[0x01]=CAN_BROADCAST_ADDR;       ;;;广播地址
                        ram_the_array[0x02]=0;
                        ram_the_array[0x03]=0;
                        ram_the_array[0x04]=CAN_VERTIME_FRAME;        ;;;对时报文
                    
                        ram_the_array[0x05]=REG_Year % 100;
                        ram_the_array[0x06]=REG_Month;
                        ram_the_array[0x07]=REG_Date;
                        ram_the_array[0x08]=REG_Hour;
                        ram_the_array[0x09]=REG_Minute;
                        ram_the_array[0x0a]=REG_Second;
                        ram_the_array[0x0b]=byte1(REG_1Msecond);
                        ram_the_array[0x0c]=byte0(REG_1Msecond);       ;;;没什么特别的前面就是含有变量的宏
                    
                        ram_need_port_trans=1;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=YES;      ;;;难道这些地方表示了填写了数据可以发送了吗
                        
			        	if(Now_CANx_Sw_Channel_Sta[ram_the_array[0x01]]>0x7f) 
			        	{
			        		Now_CANx_Sw_Channel_Sta[ram_the_array[0x01]]=0x50;
			        	}
			        	else
			        	{
			        		Now_CANx_Sw_Channel_Sta[ram_the_array[0x01]]=0xa0;
			        	}
                    }
                    else  // CAN_LONG_FRAME        ;;;决定长报文的组织地方
                    {
                        ram_the_array[0x01]=REG_CAN0_Port.TRAN_CPU_INFO_ADDR;               ;;;目的地址
                        ram_the_array[0x02]=REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR;      ;;;当前帧号 
                        ram_the_array[0x03]=0;                                              ;;;源地址  
                        ram_the_array[0x04]=CAN_LONG_FRAME;                                 ;;;帧类型
                    
                        if(REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR==0)     ;;;一种组织长报文的方法判断
                        {
                            ram_the_array[0x05]=REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM;    ;;;总帧数
                            ram_the_array[0x06]=CAN0_TranProc_Buf[1];
                            ram_the_array[0x07]=CAN0_TranProc_Buf[2];
                            ram_the_array[0x08]=CAN0_TranProc_Buf[3];
                            ram_the_array[0x09]=CAN0_TranProc_Buf[4];
                            ram_the_array[0x0a]=CAN0_TranProc_Buf[5];
                            ram_the_array[0x0b]=CAN0_TranProc_Buf[6];
                            ram_the_array[0x0c]=CAN0_TranProc_Buf[7];               ;;;剩下的帧内容,之前在判断处理的时候都准备好了,这只是组织发送
                        }
                        else
                        {
                            reg_ibx=REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR*8;
                            ram_the_array[0x05]=CAN0_TranProc_Buf[reg_ibx+0x00];
                            ram_the_array[0x06]=CAN0_TranProc_Buf[reg_ibx+0x01];
                            ram_the_array[0x07]=CAN0_TranProc_Buf[reg_ibx+0x02];
                            ram_the_array[0x08]=CAN0_TranProc_Buf[reg_ibx+0x03];
                            ram_the_array[0x09]=CAN0_TranProc_Buf[reg_ibx+0x04];
                            ram_the_array[0x0a]=CAN0_TranProc_Buf[reg_ibx+0x05];
                            ram_the_array[0x0b]=CAN0_TranProc_Buf[reg_ibx+0x06];
                            ram_the_array[0x0c]=CAN0_TranProc_Buf[reg_ibx+0x07];
                        }
                        
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR++;      ;;;8个数据一保存在这加上统一的头
                        if(REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR
                            >=REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM)  ;;;达到总帧数
                        {
                            REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=YES;     ;;;表示现在这个是一个长报文的最后一帧了
                        }
                        ram_need_port_trans=1;    
                    }
                }
            }
            else
            {
                if(CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr!=CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr)         // CPU ask CMU 3rd   ;;;这些过程是CPU 问CMU,CMU的回答
                {
                    ram_the_base_addr=(far BYTE *)&CAN0_CPUASKCMU_Trans_Buf[0]+CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr*12;
                    
                    ram_the_array[0x00]=0x88;
                    ram_the_array[0x01]=ram_the_base_addr[0x00];
                    ram_the_array[0x02]=ram_the_base_addr[0x01];
                    ram_the_array[0x03]=ram_the_base_addr[0x02];             
                    ram_the_array[0x04]=ram_the_base_addr[0x03];  
                    ram_the_array[0x05]=ram_the_base_addr[0x04];             
                    ram_the_array[0x06]=ram_the_base_addr[0x05];
                    ram_the_array[0x07]=ram_the_base_addr[0x06];
                    ram_the_array[0x08]=ram_the_base_addr[0x07];
                    ram_the_array[0x09]=ram_the_base_addr[0x08];
                    ram_the_array[0x0a]=ram_the_base_addr[0x09];
                    ram_the_array[0x0b]=ram_the_base_addr[0x0a];
                    ram_the_array[0x0c]=ram_the_base_addr[0x0b];
            
                    ram_need_port_trans=1;
                    CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr++;
                }
            }
        }
                                                                
           ;;;CPU写入,BSP读出,读出之后就进入了CAN总线
        if(ram_need_port_trans!=0)                                  ;;;最终开始发送是在这个地方吗,如果等于0就就直接结束了
        {
        	if(Now_CANx_Sw_Channel_Sta[ram_the_array[0x01]]>0x7f)     ;;;判断是CAN0还是1口发送
        	 {
	            *(far BYTE *)(CAN1_TRANS_BUF+0x00)=ram_the_array[0x00];       ;;;这个地方就把要发送的内容,写入到了最终单元,一个命令就可以自动发送
	            temp_buf[0]=ram_the_array[0x00];
    	        *(far BYTE *)(CAN1_TRANS_BUF+0x01)=ram_the_array[0x01];
    	        temp_buf[1]=ram_the_array[0x01];
        	    *(far BYTE *)(CAN1_TRANS_BUF+0x02)=ram_the_array[0x02];
        	 temp_buf[2]=ram_the_array[0x02];
            	*(far BYTE *)(CAN1_TRANS_BUF+0x03)=ram_the_array[0x03];
            	temp_buf[3]=ram_the_array[0x03];
	            *(far BYTE *)(CAN1_TRANS_BUF+0x04)=ram_the_array[0x04];
	            temp_buf[4]=ram_the_array[0x04];
    	        *(far BYTE *)(CAN1_TRANS_BUF+0x05)=ram_the_array[0x05];
    	        temp_buf[5]=ram_the_array[0x05];
        	    *(far BYTE *)(CAN1_TRANS_BUF+0x06)=ram_the_array[0x06];
        	    temp_buf[6]=ram_the_array[0x06];
            	*(far BYTE *)(CAN1_TRANS_BUF+0x07)=ram_the_array[0x07];
            	temp_buf[7]=ram_the_array[0x07];
	            *(far BYTE *)(CAN1_TRANS_BUF+0x08)=ram_the_array[0x08];
	            temp_buf[8]=ram_the_array[0x08];
    	        *(far BYTE *)(CAN1_TRANS_BUF+0x09)=ram_the_array[0x09];
    	        temp_buf[9]=ram_the_array[0x09];
        	    *(far BYTE *)(CAN1_TRANS_BUF+0x0a)=ram_the_array[0x0a];
        	    temp_buf[10]=ram_the_array[0x0a];
            	*(far BYTE *)(CAN1_TRANS_BUF+0x0b)=ram_the_array[0x0b];
            	temp_buf[11]=ram_the_array[0x0b];
	            *(far BYTE *)(CAN1_TRANS_BUF+0x0c)=ram_the_array[0x0c];
	            temp_buf[12]=ram_the_array[0x0c];
            
    	        *(far BYTE *)(CAN1_COMMAND+0)=0xe1;                             ;;;向SJA1000下达发送请求命令
    	        Now_CANx_Sw_Channel_Sta[0x00]=0x80;  // now sending on CAN1
        	 }
        	else
        	{
	            *(far BYTE *)(CAN0_TRANS_BUF+0x00)=ram_the_array[0x00];       ;;;写入发送缓冲区等待发送,最后的地方了,发送之前
	            temp_buf[0]=ram_the_array[0x00];
    	        *(far BYTE *)(CAN0_TRANS_BUF+0x01)=ram_the_array[0x01];
    	        temp_buf[1]=ram_the_array[0x01];
        	    *(far BYTE *)(CAN0_TRANS_BUF+0x02)=ram_the_array[0x02];
        	    temp_buf[2]=ram_the_array[0x02];
            	*(far BYTE *)(CAN0_TRANS_BUF+0x03)=ram_the_array[0x03];
            	temp_buf[3]=ram_the_array[0x03];
	            *(far BYTE *)(CAN0_TRANS_BUF+0x04)=ram_the_array[0x04];
	            temp_buf[4]=ram_the_array[0x04];
    	        *(far BYTE *)(CAN0_TRANS_BUF+0x05)=ram_the_array[0x05];
    	        temp_buf[5]=ram_the_array[0x05];
        	    *(far BYTE *)(CAN0_TRANS_BUF+0x06)=ram_the_array[0x06];
        	    temp_buf[6]=ram_the_array[0x06];
            	*(far BYTE *)(CAN0_TRANS_BUF+0x07)=ram_the_array[0x07];
            	temp_buf[7]=ram_the_array[0x07];
	            *(far BYTE *)(CAN0_TRANS_BUF+0x08)=ram_the_array[0x08];
	            temp_buf[8]=ram_the_array[0x08];
    	        *(far BYTE *)(CAN0_TRANS_BUF+0x09)=ram_the_array[0x09];
    	        temp_buf[9]=ram_the_array[0x09];
        	    *(far BYTE *)(CAN0_TRANS_BUF+0x0a)=ram_the_array[0x0a];
        	    temp_buf[10]=ram_the_array[0x0a];
            	*(far BYTE *)(CAN0_TRANS_BUF+0x0b)=ram_the_array[0x0b];
            	temp_buf[11]=ram_the_array[0x0b];
	            *(far BYTE *)(CAN0_TRANS_BUF+0x0c)=ram_the_array[0x0c];
	            temp_buf[12]=ram_the_array[0x0c];
            
    	        *(far BYTE *)(CAN0_COMMAND+0)=0xe1;          ;;;这里面保留的都默认填1 ;;;向SJA1000下达发送命令,他会自动进行发送
    	        Now_CANx_Sw_Channel_Sta[0x00]=0x40;  // now sending on CAN0       ;;;表示现在正在通过哪个CAN口发送
            }
            REG_CAN0_Port.F_TRANS_STATUS=1;
        }
    }
} 













//-其实和我已经看到的原理是一样的,只不过过程有些不同而已
/*===========================================================================*/
void CAN_MSG_Proc(void)            //-对接收到的报文做进一步分析处理,已经从帧的格式中脱离出来了
{
         WORD  dumyword;
         WORD  sumword ;
         WORD  sumword1;
    near BYTE *the_ram_address;
    far  BYTE *the_ram_addr;
    far  WORD *the_ram_address_w1;

    if(REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_IDLE)  // if=CAN_MON_TransingToCPU,do nothing ;;;可能判断到这样的状态 使得这个系统做后续处理,即什么也不做
    {
        if(port_transmit_flag[PORT_NO_CAN_0]==0xaa)        ;;;等于AA表示接收了新值,需要进一步处理
        {
            port_transmit_flag[PORT_NO_CAN_0]=0x55;        ;;;表示有内容需要发送
            
            switch(port_transmit[PORT_NO_CAN_0][8])     ;;;难道为了所有格式统一,他内部使用了折中的应用层
            {
                case CORE_CODE_YK_CHOOSE:
                    if(port_transmit[PORT_NO_CAN_0][5]==MAX_CAN0_ADDR)       ;;;前面表示的是用的是CAN0口,后面是
                    {
                        exchange_target_port=port_transmit[PORT_NO_CAN_0][1];

                        exchange_buf[1] = PORT_NO_CAN_0;           ;;;表示是CAN0口
                        exchange_buf[2] = PROTOCOL_CAN_DSA%0x100;
                        exchange_buf[3] = PROTOCOL_CAN_DSA/0x100;  ;;;记录协议类型
                        exchange_buf[5] = MAX_CAN0_ADDR;           ;;;最大地址号,即哪台装置发来的
                        exchange_buf[8] = CORE_CODE_YK_VERIFY;     ;;;对报文的进一步处理内容放在了这里
                        exchange_buf[9] = 0x02;
                        exchange_buf[10]= 0x00;
                        exchange_buf[16]= port_transmit[PORT_NO_CAN_0][16];  // the same as CORE_CODE2_YK_CLOSE/TRIP/ERROR
                        exchange_buf[17]= port_transmit[PORT_NO_CAN_0][17];  // YK No.
                        if(exchange_target_port<14)
                            exchange_buf[17] += unit_info[byte0(port_info[12].mirror_unit) + exchange_buf[5]].yk_start;
                        Ex_Produce_Transmit_Info();        ;;;交换一下内容让他准备发送
                        if(port_transmit[PORT_NO_CAN_0][17]==1)  //VQC208 ON/OFF 2004.09.02   ;;;可能这些就没有整体逻辑就是一个宁加的特例
                         {
                          dumyword=unit_info[MAX_CAN1_ADDR].channel_no;
                          sumword=dumyword/16;
                          sumword1=dumyword%16;
                          if(port_transmit[PORT_NO_CAN_0][16]==CORE_CODE2_YK_CLOSE)      ;;;遥合
                           YX_State[sumword]|=0x0001<<sumword1;         ;;;先移位再或
                          else
                           if(port_transmit[PORT_NO_CAN_0][16]==CORE_CODE2_YK_TRIP)      ;;;遥分
                            YX_State[sumword]&=(~(0x0001<<sumword1));
                         }
                    }
                    else
                    {
	                    if((port_transmit[PORT_NO_CAN_0][5]>=MIN_CAN0_ADDR)&&(port_transmit[PORT_NO_CAN_0][5]<=MAX_CAN0_ADDR))
    	                {
        	                CAN0_TranProc_Buf[0]=CAN_SHORT_FRAME;
            	            CAN0_TranProc_Buf[1]=0x00;              // S
                	        CAN0_TranProc_Buf[2]=0x40;              // CMD           ;;;是发送遥控选择的报文吗(CMU->CPU)
                    	    CAN0_TranProc_Buf[3]=port_transmit[PORT_NO_CAN_0][16];    // action type
                        	CAN0_TranProc_Buf[4]=port_transmit[PORT_NO_CAN_0][17];    // YK no.
	                        CAN0_TranProc_Buf[5]=CAN0_TranProc_Buf[3];                ;;;内容是一样的
    	                    CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[4];    
        	                CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][5];        ;;;装置的地址      
            	            CAN0_TranProc_Buf[8]=~(CAN0_TranProc_Buf[7]);             ;;;取反在哪有见过
                
                	        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=port_transmit[PORT_NO_CAN_0][5];    ;;;里面存放的是地址信息
                    	    REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;                     ;;;表示这些内容是为长帧准备的吗
                        	REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
	                        REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;                           ;;;这些应该都是些控制位,
    	                    REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YKVERIFY;        ;;;表示需要CPU返回遥控确认报文
        	                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=port_transmit[PORT_NO_CAN_0][1];  // source port;;;有共性,肯定也有特性
            	            REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                	    }
                	  }    
                    break;       ;;;结束不再判断遇到了这个
                    
                case CODE_CAN_WAVRCDDATA:
                    if((port_transmit[PORT_NO_CAN_0][5]>=MIN_CAN0_ADDR)&&(port_transmit[PORT_NO_CAN_0][5]<=MAX_CAN0_ADDR))
                    {
                        CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                        CAN0_TranProc_Buf[1]=0x00;              // S
                        CAN0_TranProc_Buf[2]=0x00;              // Group
                        CAN0_TranProc_Buf[3]=CODE_CAN_WAVRCDDATA;              // CMD
                        CAN0_TranProc_Buf[4]=0x02;              // LenL
                        CAN0_TranProc_Buf[5]=0x00;              // LenH
                        CAN0_TranProc_Buf[6]=port_transmit[PORT_NO_CAN_0][20]; 
                        CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][21];    
                        sumword=CAN0_TranProc_Buf[2]+CAN0_TranProc_Buf[3]+CAN0_TranProc_Buf[4]
                               +CAN0_TranProc_Buf[6]+CAN0_TranProc_Buf[7];
                        CAN0_TranProc_Buf[8]=byte0(sumword);
                        CAN0_TranProc_Buf[9]=byte1(sumword);              
                
                        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=port_transmit[PORT_NO_CAN_0][5];
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=2;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
                        REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;
                        REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_WAVRCD;
                        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=port_transmit[PORT_NO_CAN_0][1];  // source port
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                    }
                    break;
                    
                case CORE_CODE_YK_EXECUTE:
                    if(port_transmit[PORT_NO_CAN_0][5]==MAX_CAN0_ADDR)
                    {
                     if(port_transmit[PORT_NO_CAN_0][17]==0)
                      {
           	            CAN0_TranProc_Buf[0]=CAN_SHORT_FRAME;              ;;;发送信号报文缓冲区,这个对应帧类型,下面八个是数据Data0~Data7,这些都是应用层
               	        CAN0_TranProc_Buf[1]=0x00;              // S
                   	    CAN0_TranProc_Buf[2]=CODE_CAN_BRDCST_RST_RLY;  // CMD    ;;;CMU广播复归CPU保护动作信号，CPU不需回答
                       	CAN0_TranProc_Buf[3]=0x00;
                        CAN0_TranProc_Buf[4]=0x00;
   	                    CAN0_TranProc_Buf[5]=0x00;
       	                CAN0_TranProc_Buf[6]=0x00;
           	            CAN0_TranProc_Buf[7]=0x00;
               	        CAN0_TranProc_Buf[8]=0x00;
                	
                   	    REG_CAN0_Port.TRAN_CPU_INFO_ADDR=CAN_BROADCAST_ADDR;        ;;;记录这帧的地址
                       	REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                   
   	                    REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;          ;;;表示需不需要CPU回答
       	                //REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YKRESULT;
           	            REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port
               	        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;

			for(byte0(sumword)=0;byte0(sumword)<12;byte0(sumword)++) // 2004-07-29
 			 {
      			  if((port_info[byte0(sumword)].protocal_type==PROTOCOL_NAZI94)&&(port_transmit_flag[byte0(sumword)] != YES))
      			   {
			    port_transmit_flag[byte0(sumword)] = 0xaa;
			    the_ram_addr=&port_transmit[byte0(sumword)][0];
			    the_ram_addr[1]=port_transmit[PORT_NO_CAN_0][1];
			    the_ram_addr[8]=0x80;
			    the_ram_addr[9]=0x04;
			    the_ram_addr[10]=0x00;
			    the_ram_addr[16]=0xff;
			    the_ram_addr[17]=0x80;
			    the_ram_addr[18]=0x01;
			    the_ram_addr[19]=0x00;
      			   }
      			  if((port_info[byte0(sumword)].protocal_type==PROTOCOL_IEC103_RCS)&&(port_transmit_flag[byte0(sumword)] != YES))
      			   {
			    port_transmit_flag[byte0(sumword)] = 0xaa;
			    the_ram_addr=&port_transmit[byte0(sumword)][0];
			    the_ram_addr[1]=port_transmit[PORT_NO_CAN_0][1];
			    the_ram_addr[8]=0x14;
			    the_ram_addr[9]=0x04;
			    the_ram_addr[10]=0x00;
			    the_ram_addr[16]=0xff;
			    the_ram_addr[17]=0xff;
			    the_ram_addr[18]=0x00;
			    the_ram_addr[19]=0x00;
      			   }         
  			 }
  		     }
                    }
                    else
                    {
	                    if((port_transmit[PORT_NO_CAN_0][5]==REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx)&&
    	                   (REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx!=0xff))
        	            {
            	            CAN0_TranProc_Buf[0]=CAN_SHORT_FRAME;
                	        CAN0_TranProc_Buf[1]=0x00;              // S
                    	    CAN0_TranProc_Buf[2]=0x41;              // CMD
                        	CAN0_TranProc_Buf[3]=0xaa;              // action type
	                        CAN0_TranProc_Buf[4]=port_transmit[PORT_NO_CAN_0][17];    // YK no.
    	                    CAN0_TranProc_Buf[5]=CAN0_TranProc_Buf[3];              
        	                CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[4];    
            	            CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][5];              
                	        CAN0_TranProc_Buf[8]=~(CAN0_TranProc_Buf[7]);              
                	
                    	    REG_CAN0_Port.TRAN_CPU_INFO_ADDR=port_transmit[PORT_NO_CAN_0][5];
                        	REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
	                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
    	                    REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;
        	                REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YKRESULT;
            	            REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port
                	        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                        
                    	    Rcd_Info_System_Tmp[0]=0;  // reserved
                        	Rcd_Info_System_Tmp[1]=RCD_INFO_SYSTEM_AREA0_CAN;
	                        Rcd_Info_System_Tmp[2]=CAN0_TranProc_Buf[7];
    	                    Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_YK_EXECUTE;
        	                Rcd_Info_System_Tmp[4]=port_transmit[PORT_NO_CAN_0][1];  // source port
            	            Rcd_Info_System_Tmp[5]=0;  
                	        Rcd_Info_System_Tmp[6]=0;  
                    	    Rcd_Info_System_Tmp[7]=0; 
                        
                        	Store_Rcd_Info_System();
                        }	
                    }
                    break;
                    
                case CORE_CODE_YK_CANCEL:
                    if((port_transmit[PORT_NO_CAN_0][5]>=MIN_CAN0_ADDR)&&(port_transmit[PORT_NO_CAN_0][5]<=MAX_CAN0_ADDR))
                    {
                        CAN0_TranProc_Buf[0]=CAN_SHORT_FRAME;
                        CAN0_TranProc_Buf[1]=0x00;              // S
                        CAN0_TranProc_Buf[2]=0x41;              // CMD
                        CAN0_TranProc_Buf[3]=0x55;              // action type
                        CAN0_TranProc_Buf[4]=port_transmit[PORT_NO_CAN_0][17];    // YK no.
                        CAN0_TranProc_Buf[5]=CAN0_TranProc_Buf[3];              
                        CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[4];    
                        CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][5];              
                        CAN0_TranProc_Buf[8]=~(CAN0_TranProc_Buf[7]);              
                
                        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=port_transmit[PORT_NO_CAN_0][5];
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
                        REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;
                        REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YKRESULT;
                        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                    }
                    break;
                    
                case LSA_CODE_ACK: 
                    Rcd_Info_System_Tmp[0]=0;  // reserved
                    Rcd_Info_System_Tmp[1]=RCD_INFO_SYSTEM_AREA0_CAN;            // CAN PORT
                    Rcd_Info_System_Tmp[2]=REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx;     // CAN ADDR
                    Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_SET_MODI;       // do  what
                    Rcd_Info_System_Tmp[4]=port_transmit[PORT_NO_CAN_0][1];      // source port
                    Rcd_Info_System_Tmp[5]=0;
                    Rcd_Info_System_Tmp[6]=0;  
                    Rcd_Info_System_Tmp[7]=0; 
                        
                    Store_Rcd_Info_System();
                case LSA_CODE_NAK:
                    if(REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx<=MAX_CAN0_ADDR)
                    {
                        CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                        CAN0_TranProc_Buf[1]=0x00;              // S
                        CAN0_TranProc_Buf[2]=0x00;              // Group
                        CAN0_TranProc_Buf[3]=CODE_CAN_W_R_RLY_DATA;              // CMD
                        CAN0_TranProc_Buf[4]=0x02;              // LenL
                        CAN0_TranProc_Buf[5]=0x00;              // LenH
                        CAN0_TranProc_Buf[6]=BH_GUIHAO_IN_CAN_INTERNAL_PROTOCOL; // replace port_transmit[PORT_NO_CAN_0][16]
                        CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][17];    
                        sumword=CAN0_TranProc_Buf[2]+CAN0_TranProc_Buf[3]+CAN0_TranProc_Buf[4]
                               +CAN0_TranProc_Buf[6]+CAN0_TranProc_Buf[7];
                        CAN0_TranProc_Buf[8]=byte0(sumword);
                        CAN0_TranProc_Buf[9]=byte1(sumword);              
                
                        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx;
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=2;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
                        REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                        //REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YKRESULT;
                        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                    }
                    break;

                case LSA_CODE_C14_SETGRP_MODI_ACK:
                    if((port_transmit[PORT_NO_CAN_0][19]==REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx)&&
                       (REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx!=0xff))
                    {
                        CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                        CAN0_TranProc_Buf[1]=0x00;              // S
                        CAN0_TranProc_Buf[2]=0x00;              // Group
                        CAN0_TranProc_Buf[3]=CODE_CAN_W_R_RLY_DATA;              // CMD
                        CAN0_TranProc_Buf[4]=0x05;              // LenL
                        CAN0_TranProc_Buf[5]=0x00;              // LenH
                        CAN0_TranProc_Buf[6]=BH_GUIHAO_IN_CAN_INTERNAL_PROTOCOL; // replace port_transmit[PORT_NO_CAN_0][16]
                        CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][17];    // CMD
                        CAN0_TranProc_Buf[8]=port_transmit[PORT_NO_CAN_0][18];    // LEN
                        CAN0_TranProc_Buf[9]=port_transmit[PORT_NO_CAN_0][19];    
                        CAN0_TranProc_Buf[10]=port_transmit[PORT_NO_CAN_0][20];    
                        byte0(sumword)=port_transmit[PORT_NO_CAN_0][21];
                        CAN0_TranProc_Buf[11]=byte0(sumword);
                        byte1(sumword)=port_transmit[PORT_NO_CAN_0][22];
                        CAN0_TranProc_Buf[12]=byte1(sumword);
                        sumword=sumword+CAN0_TranProc_Buf[3]+CAN0_TranProc_Buf[4]+CAN0_TranProc_Buf[5]+CAN0_TranProc_Buf[6]; // group=0
                        CAN0_TranProc_Buf[13]=byte0(sumword);
                        CAN0_TranProc_Buf[14]=byte1(sumword);
                
                        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=CAN0_TranProc_Buf[9];
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=2;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
                        REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;

                        Rcd_Info_System_Tmp[0]=0;  // reserved
                        Rcd_Info_System_Tmp[1]=RCD_INFO_SYSTEM_AREA0_CAN;
                        Rcd_Info_System_Tmp[2]=CAN0_TranProc_Buf[9];
                        Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_SETGROUP_MODI;
                        Rcd_Info_System_Tmp[4]=port_transmit[PORT_NO_CAN_0][1];   // source port
                        Rcd_Info_System_Tmp[5]=0;
                        Rcd_Info_System_Tmp[6]=0;  
                        Rcd_Info_System_Tmp[7]=0; 
                        
                        Store_Rcd_Info_System();
                    }
                    break;

                default:  // other all as BH msg
                    CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                    CAN0_TranProc_Buf[1]=0x00;              // S
                    CAN0_TranProc_Buf[2]=0x00;              // Group
                    CAN0_TranProc_Buf[3]=CODE_CAN_W_R_RLY_DATA;              // CMD
                    byte0(sumword)=port_transmit[PORT_NO_CAN_0][9] ;
                    byte1(sumword)=port_transmit[PORT_NO_CAN_0][10];
                    if(sumword>504) sumword=504;
                    CAN0_TranProc_Buf[4]=byte0(sumword);    // LenL
                    CAN0_TranProc_Buf[5]=byte1(sumword);    // LenH
                    CAN0_TranProc_Buf[6]=BH_GUIHAO_IN_CAN_INTERNAL_PROTOCOL; // replace port_transmit[PORT_NO_CAN_0][16]          
                    for(dumyword=7;dumyword<(sumword+6);dumyword++)
                    {
                        CAN0_TranProc_Buf[dumyword]=port_transmit[PORT_NO_CAN_0][dumyword+10];
                    }
                    byte0(sumword1)=CAN0_TranProc_Buf[dumyword-2];
                    byte1(sumword1)=CAN0_TranProc_Buf[dumyword-1];
                    sumword1=sumword1+CAN0_TranProc_Buf[3]+CAN0_TranProc_Buf[4]
                                     +CAN0_TranProc_Buf[5]+CAN0_TranProc_Buf[6]; // group=0
                    CAN0_TranProc_Buf[dumyword+0]=byte0(sumword1);
                    CAN0_TranProc_Buf[dumyword+1]=byte1(sumword1);
                    
                    //2004-07-10
                    if(port_transmit[PORT_NO_CAN_0][8]==LSA_CODE_C14_RLY_OPERATE_ACT)
                    {
	                    if((port_transmit[PORT_NO_CAN_0][19]==REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx)&&
    	                   (REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx!=0xff))
	                    {
		                    REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
    		                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port

		                    Rcd_Info_System_Tmp[0]=0;  // reserved
        		            Rcd_Info_System_Tmp[1]=RCD_INFO_SYSTEM_AREA0_CAN;            // CAN PORT
                		    Rcd_Info_System_Tmp[2]=REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx;     // CAN ADDR
		                    Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_SET_MODI;       // do  what
        		            Rcd_Info_System_Tmp[4]=port_transmit[PORT_NO_CAN_0][1];      // source port
                		    Rcd_Info_System_Tmp[5]=0;
		                    Rcd_Info_System_Tmp[6]=0;  
        		            Rcd_Info_System_Tmp[7]=0; 
                        
                		    Store_Rcd_Info_System();
                    	}
                    	else
                    		break;
                    }
                    else
                    {
	                    REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;
    	                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=port_transmit[PORT_NO_CAN_0][1];  // source port
                    }
                    REG_CAN0_Port.TRAN_CPU_INFO_ADDR=CAN0_TranProc_Buf[9];
                    REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                    REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=(sumword+8+7)/8;
                    REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;

       	            REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                    break;
            }
            REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx=0xff;
            
        }
        else
        {
            if(CAN_MON_stachg_task_proc()!=YES)      ;;;返回YES说明有内容需要发送,发送是自动定时中断的
            {
                if(CAN_MON_inquire_task_proc()!=YES)       ;;;这个里面也有一个调用
                {
                    // other task
                }
            }    
        }
    }
    else // not CAN_MON_IDLE	CAN_MON_空闲
    {
        if(REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_WAITCPUREPLY)
        {
            if(REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC==YES)
            {
                if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YC) goto rcd_ser_good;      ;;;判断CPU需要回答的内容
                if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YX) goto rcd_ser_good;
                if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YM) goto rcd_ser_good;
                goto no_rcd_ser_good;
rcd_ser_good:
                CAN0_Comm_CurrentSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=GOOD;
                if(CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]!=ERR)
                    CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=GOOD;
                if(Portx_Poll_First[12]!=YES)
                {
                    CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=0;
                    CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=GOOD;
                }
                else
                {
                    if(CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]==ERR)
                    {
                        if(CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]>1)
                        {
                            CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]--;
                            CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]--;
                        }
                        else
                        {
                            CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=0;
                        }
                    
                        if(CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]==0)
                        {
                            CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=GOOD;
                            if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                            the_ram_addr=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
                            the_ram_addr[0]=PROTOCOL_LSA%0x100;     // protocol type
                            the_ram_addr[1]=0x51;                   // CMD      code
                            the_ram_addr[2]=0x00;                   // frame    no
                            the_ram_addr[3]=0x12;                   // valid    bytes  in this frame
                            the_ram_addr[4]=the_ram_addr[0]
                                           +the_ram_addr[1]
                                           +the_ram_addr[2]
                                           +the_ram_addr[3];        // verify   byte
                            the_ram_addr[5]=PROTOCOL_LSA/0x100;     // Reserved
            
                            the_ram_addr[6]=0x01;                   // Frame    Num
                            the_ram_addr[7]=0x00;                   // Reserved
                            the_ram_addr[8]=CAN0_AS_BH_MON_ADDR_SET;// CAN      GuiHao
                            the_ram_addr[9]=0x51;
                            the_ram_addr[10]=0x0d;
                            byte0(sumword)=REG_Year%100;
                            the_ram_addr[11]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                            byte0(sumword)=(REG_Year%10000)/100;
                            the_ram_addr[12]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                            the_ram_addr[13]=(REG_Month/10)*0x10+REG_Month%10;
                            the_ram_addr[14]=(REG_Date/10)*0x10+REG_Date%10;
                            the_ram_addr[15]=(REG_Hour/10)*0x10+REG_Hour%10;
                            the_ram_addr[16]=(REG_Minute/10)*0x10+REG_Minute%10;
                            the_ram_addr[17]=(REG_Second/10)*0x10+REG_Second%10;
                            byte0(sumword)=REG_1Msecond%100;
                            the_ram_addr[18]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                            byte0(sumword)=(REG_1Msecond%1000)/100;
                            the_ram_addr[19]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                            the_ram_addr[20]=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                            the_ram_addr[21]=0x05;
                            sumword=0;
                            for(byte0(dumyword)=9;byte0(dumyword)<22;byte0(dumyword)++)
                            {
                             	sumword+=the_ram_addr[byte0(dumyword)];
                            }
                            the_ram_addr[22]=byte0(sumword);
                            the_ram_addr[23]=byte1(sumword);
            
                            BH_Report_Bank_Sav_Ptr++;
                            if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
                            
                             // BH to SOE
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE[dumyword].ActionType==the_ram_addr[21]))
                                        	{
                                            	BH_To_YX_Delay_Begin_Time[dumyword]=Time_2048ms_Counter;
                    
	                                            dumyword =BH_Event_To_SOE[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] | (1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]=(byte1(dumyword) & 0x0f)+0x80; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                 
                                        dumyword++;
                                    }
                                    
                                    // BH to SOE Ret
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE_Ret[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE_Ret[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE_Ret[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE_Ret[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE_Ret[dumyword].ActionType==the_ram_addr[21]))
                                        	{
	                                            dumyword =BH_Event_To_SOE_Ret[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] & (0xffff-1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]= byte1(dumyword) & 0x0f; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                                        dumyword++;
                                    }
                            
                            
                            
                            
                            
                            
                            
                        }
                    }
                    else
                    {
                        CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=0;
                    }    
                }

no_rcd_ser_good:
                // different status process 
                if(CAN_MON_common_proc()!=YES)         ;;;这个调用为什么没有源代码?????有了
                {
                    if(REG_CAN0_Proc.CAN_NEEDTRANSTOHOST==CAN_NONEEDTRANSTOHOST)        ;;;这个地方和主机有关即后台
                    {
                        byte0(dumyword)=0;
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_LIFETIME) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_LIFE_TIME)       ;;;冗余对接收到的内容进行处理
                            {
                                // save Life_Time
//                                if(port_info[12].protect_enable==0xaa)
                                {
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                                    the_ram_addr=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
            
                                    the_ram_addr[0]=PROTOCOL_LSA%0x100;     // protocol type
                                    the_ram_addr[1]=0x51;                   // CMD      code
                                    the_ram_addr[2]=0x00;                   // frame    no
                                    the_ram_addr[3]=0x12;                   // valid    bytes  in this frame
                                    the_ram_addr[4]=the_ram_addr[0]
                                                   +the_ram_addr[1]
                                                   +the_ram_addr[2]
                                                   +the_ram_addr[3];        // verify   byte
                                    the_ram_addr[5]=PROTOCOL_LSA/0x100;     // Reserved
            
                                    the_ram_addr[6]=0x01;                   // Frame    Num
                                    the_ram_addr[7]=0x00;                   // Reserved
                                    the_ram_addr[8]=CAN0_AS_BH_MON_ADDR_SET;// CAN      GuiHao
                                    for(byte1(dumyword)=9;byte1(dumyword)<(9+13+2);byte1(dumyword)++)
                                    {
                                        the_ram_addr[byte1(dumyword)]=CAN0_ReceProc_Buf[byte1(dumyword)-2];
                                    }
            
                                    BH_Report_Bank_Sav_Ptr++;
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
                            	
                            	 // BH to SOE
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE[dumyword].ActionType==the_ram_addr[21]))
                                        	{
                                            	BH_To_YX_Delay_Begin_Time[dumyword]=Time_2048ms_Counter;
                    
	                                            dumyword =BH_Event_To_SOE[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] | (1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]=(byte1(dumyword) & 0x0f)+0x80; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                 
                                        dumyword++;
                                    }
                                    
                                    // BH to SOE Ret
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE_Ret[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE_Ret[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE_Ret[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE_Ret[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE_Ret[dumyword].ActionType==the_ram_addr[21]))
                                        	{
	                                            dumyword =BH_Event_To_SOE_Ret[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] & (0xffff-1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]= byte1(dumyword) & 0x0f; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                                        dumyword++;
                                    }
                            	
                            	
                            
                            
                                }
                            
                                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                                CAN0_TranProc_Buf[1]=0x00;                  // S
                                CAN0_TranProc_Buf[2]=0x00;                  // Group
                                CAN0_TranProc_Buf[3]=CODE_CAN_LIFE_T_ACK;   // CMD
                                CAN0_TranProc_Buf[4]=0x00;   
                                CAN0_TranProc_Buf[5]=0x00;   
                                CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[3];  // SumL 
                                CAN0_TranProc_Buf[7]=0x00;   
                                
                                REG_CAN0_Port.TRAN_CPU_INFO_ADDR=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                                REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                                
                                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
                                REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                                
                                byte0(dumyword)=1;
                            }
                        }

                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_RLYEVENT) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_EVENT)
                            {
                                // save EVENT
//                                if(port_info[12].protect_enable==0xaa)
                                {
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                                    the_ram_addr=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
            
                                    the_ram_addr[0]=PROTOCOL_LSA%0x100;     // protocol type
                                    the_ram_addr[1]=0x40;                    // CMD      code
                                    the_ram_addr[2]=0x00;                    // frame    no
                                    the_ram_addr[3]=0x18;                    // valid    bytes  in this frame
                                    the_ram_addr[4]=the_ram_addr[0]
                                                   +the_ram_addr[1]
                                                   +the_ram_addr[2]
                                                   +the_ram_addr[3];         // verify   byte
                                    the_ram_addr[5]=PROTOCOL_LSA/0x100;     // Reserved
            
                                    the_ram_addr[6]=0x01;             // Frame    Num
                                    the_ram_addr[7]=0x00;             // Reserved
                                    the_ram_addr[8]=CAN0_AS_BH_MON_ADDR_SET;    // CAN      GuiHao
                                    for(byte1(dumyword)=9;byte1(dumyword)<(9+19+2);byte1(dumyword)++)
                                    {
                                        the_ram_addr[byte1(dumyword)]=CAN0_ReceProc_Buf[byte1(dumyword)-2];
                                    }
            
                                    BH_Report_Bank_Sav_Ptr++;
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                        			if((BH_Act_Alarm_Type[the_ram_addr[24]]==0x02)&&(bh_event_alarm_now_action_warn==NO))
                        			    bh_event_alarm_warn  =1;
                        			if((BH_Act_Alarm_Type[the_ram_addr[24]]==0x01)&&(bh_event_alarm_now_action_warn==NO))
                        			    bh_event_alarm_action=1;

                                    // BH to SOE
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE[dumyword].RLYAddr   ==the_ram_addr[20])
                                    	     &&(BH_Event_To_SOE[dumyword].ActionType==the_ram_addr[24]))
                                        	{
                                            	BH_To_YX_Delay_Begin_Time[dumyword]=Time_2048ms_Counter;
                    
	                                            dumyword =BH_Event_To_SOE[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] | (1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]=(byte1(dumyword) & 0x0f)+0x80; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                 
                                        dumyword++;
                                    }
                                    
                                    // BH to SOE Ret
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE_Ret[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE_Ret[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE_Ret[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE_Ret[dumyword].RLYAddr   ==the_ram_addr[20])
                                    	     &&(BH_Event_To_SOE_Ret[dumyword].ActionType==the_ram_addr[24]))
                                        	{
	                                            dumyword =BH_Event_To_SOE_Ret[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] & (0xffff-1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]= byte1(dumyword) & 0x0f; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                                        dumyword++;
                                    }
                                }
                                
                                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                                CAN0_TranProc_Buf[1]=0x00;                  // S
                                CAN0_TranProc_Buf[2]=0x00;                  // Group
                                CAN0_TranProc_Buf[3]=CODE_CAN_EVENT_ACK;    // CMD
                                CAN0_TranProc_Buf[4]=0x00;   
                                CAN0_TranProc_Buf[5]=0x00;   
                                CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[3];  // SumL 
                                CAN0_TranProc_Buf[7]=0x00;   
                                
                                REG_CAN0_Port.TRAN_CPU_INFO_ADDR=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                                REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                                
                                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
                                REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                                
                                byte0(dumyword)=1;
                            }
                        }
						// 2004-07-10                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_EVENT_EXT) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_EVENT_EXT)
                            {
                                // save EVENT
//                                if(port_info[12].protect_enable==0xaa)
                                {
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                                    the_ram_addr=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
            
                                    the_ram_addr[0]=PROTOCOL_LSA%0x100;      // protocol type
                                    the_ram_addr[1]=0x41;                    // CMD      code
                                    the_ram_addr[2]=0x00;                    // frame    no
                                    the_ram_addr[3]=0x19;                    // valid    bytes  in this frame
                                    the_ram_addr[4]=the_ram_addr[0]
                                                   +the_ram_addr[1]
                                                   +the_ram_addr[2]
                                                   +the_ram_addr[3];         // verify   byte
                                    the_ram_addr[5]=PROTOCOL_LSA/0x100;     // Reserved
            
                                    the_ram_addr[6]=0x01;             // Frame    Num
                                    the_ram_addr[7]=0x00;             // Reserved
                                    the_ram_addr[8]=CAN0_AS_BH_MON_ADDR_SET;    // CAN      GuiHao
                                    for(byte1(dumyword)=9;byte1(dumyword)<(9+20+2);byte1(dumyword)++)
                                    {
                                        the_ram_addr[byte1(dumyword)]=CAN0_ReceProc_Buf[byte1(dumyword)-2];
                                    }
            
                                    BH_Report_Bank_Sav_Ptr++;
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                        			//if((BH_Act_Alarm_Type[the_ram_addr[24]]==0x02)&&(bh_event_alarm_now_action_warn==NO))
                        			//    bh_event_alarm_warn  =1;
                        			//if((BH_Act_Alarm_Type[the_ram_addr[24]]==0x01)&&(bh_event_alarm_now_action_warn==NO))
                        			//    bh_event_alarm_action=1;

                                    // BH to SOE
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE[dumyword].DeviceType==0x80+the_ram_addr[25])  // DeviceType=LSA_EXTxx
					    	            {
                            	            if((BH_Event_To_SOE[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE[dumyword].RLYAddr   ==the_ram_addr[20])
                                    	     &&(BH_Event_To_SOE[dumyword].ActionType==the_ram_addr[24]))
                                        	{
                                            	BH_To_YX_Delay_Begin_Time[dumyword]=Time_2048ms_Counter;
                    
	                                            dumyword =BH_Event_To_SOE[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] | (1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]=(byte1(dumyword) & 0x0f)+0x80; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                 
                                        dumyword++;
                                    }
                                    
                                    // BH to SOE Ret
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE_Ret[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE_Ret[dumyword].DeviceType==0x80+the_ram_addr[25])  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE_Ret[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE_Ret[dumyword].RLYAddr   ==the_ram_addr[20])
                                    	     &&(BH_Event_To_SOE_Ret[dumyword].ActionType==the_ram_addr[24]))
                                        	{
	                                            dumyword =BH_Event_To_SOE_Ret[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] & (0xffff-1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]= byte1(dumyword) & 0x0f; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                                        dumyword++;
                                    }
                                }
                                
                                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                                CAN0_TranProc_Buf[1]=0x00;                  // S
                                CAN0_TranProc_Buf[2]=0x00;                  // Group
                                CAN0_TranProc_Buf[3]=CODE_CAN_EVENT_EXT_ACK;    // CMD
                                CAN0_TranProc_Buf[4]=0x00;   
                                CAN0_TranProc_Buf[5]=0x00;   
                                CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[3];  // SumL 
                                CAN0_TranProc_Buf[7]=0x00;   
                                
                                REG_CAN0_Port.TRAN_CPU_INFO_ADDR=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                                REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                                
                                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
                                REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                                
                                byte0(dumyword)=1;
                            }
                        }// 2004-07-10
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_SOE) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_SOE)
                            {
                                byte1(dumyword)=(CAN0_ReceProc_Buf[5]*256+CAN0_ReceProc_Buf[4])/8;
                                if(byte1(dumyword)>32) byte1(dumyword)=32;

                                for(byte0(dumyword)=0;byte0(dumyword)<byte1(dumyword);byte0(dumyword)++)
                                {
                                    the_ram_address=(near BYTE *)&CAN0_ReceProc_Buf[6+byte0(dumyword)*8];
                                    Core_Src_Unit=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);  // Dbase addr
                                    if( the_ram_address[6]< ((unit_info[byte0(Core_Src_Unit)].yx_num)*16) )
                                    {
                                        port_report[0]=the_ram_address[0];
                                        port_report[1]=the_ram_address[1];
                                        port_report[2]=the_ram_address[2];
                                        port_report[3]=the_ram_address[3];
                                        port_report[4]=the_ram_address[4];
                                        port_report[5]=the_ram_address[5];
                                        port_report[6]=the_ram_address[6];
                                        port_report[7]=the_ram_address[7];
                                        Core_Src_Pt_B=&(port_report[0]);
                                        core_insert_SOE();
                                    }
                                }


                                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                                CAN0_TranProc_Buf[1]=0x00;                  // S
                                CAN0_TranProc_Buf[2]=0x00;                  // Group
                                CAN0_TranProc_Buf[3]=CODE_CAN_SOE_ACK;      // CMD
                                CAN0_TranProc_Buf[4]=0x00;   
                                CAN0_TranProc_Buf[5]=0x00;   
                                CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[3];  // SumL 
                                CAN0_TranProc_Buf[7]=0x00;   
                                
                                REG_CAN0_Port.TRAN_CPU_INFO_ADDR=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                                REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                                
                                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
                                REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                                
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YCCHG) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_YC_CHG)
                            {
                                byte0(dumyword)=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);
                                the_ram_address=(near BYTE *)&YC_State[unit_info[byte0(dumyword)].yc_val_start];
                                byte1(dumyword)=CAN0_ReceProc_Buf[4]/33*16;
                                byte0(dumyword)=unit_info[byte0(dumyword)].yc_val_num;
                                
                                if(byte1(dumyword)>byte0(dumyword))  // need save yc_num
                                {
                                    byte1(dumyword)=byte0(dumyword);
                                }
                                
                                byte0(sumword)=6;
                                for(byte0(dumyword)=0;byte0(dumyword)<byte1(dumyword);byte0(dumyword)++)
                                {
                                    if(byte0(dumyword)&0x0f==0)  byte0(sumword)++;
                                    byte1(sumword)=byte0(dumyword)*2+byte0(sumword);
                                    the_ram_address[0]=CAN0_ReceProc_Buf[byte1(sumword)];
                                    the_ram_address[1]=CAN0_ReceProc_Buf[byte1(sumword)+1];
                                    the_ram_address=the_ram_address+2;
                                }
                                
                                //if((RAM_CAN0_Para.CAN_RECEYCCHG_BUF_SAV_PTR+1) & AND_CAN_YCCHG_QUELEN
                                //    ==RAM_CAN0_Para.CAN_RECEYCCHG_BUF_TAK_PTR)
                                //{
                                //  RAM_CAN0_Para.CAN_RECEYCCHG_BUF_TAK_PTR
                                //  =(RAM_CAN0_Para.CAN_RECEYCCHG_BUF_TAK_PTR+1) & AND_CAN_YCCHG_QUELEN;
                                //}
                                   
                                //CAN0_ReceYCCHG_Buf[RAM_CAN0_Para.CAN_RECEYCCHG_BUF_SAV_PTR]
                                //    =REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                    
                                //RAM_CAN0_Para.CAN_RECEYCCHG_BUF_SAV_PTR
                                //=(RAM_CAN0_Para.CAN_RECEYCCHG_BUF_SAV_PTR+1) & AND_CAN_YCCHG_QUELEN;
                                
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YC) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_YC_ALL)
                            {
                                byte0(dumyword)=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);
                                the_ram_address=(near BYTE *)&YC_State+(unit_info[byte0(dumyword)].yc_val_start*DATABASE_YC_INFO_LEN);
                                byte1(dumyword)=CAN0_ReceProc_Buf[4]/33*16;
                                byte0(dumyword)=unit_info[byte0(dumyword)].yc_val_num;
                                
                                if(byte1(dumyword)>byte0(dumyword))  // need save yc_num
                                {
                                    byte1(dumyword)=byte0(dumyword);
                                }
                                
                                byte0(sumword)=6;
                                for(byte0(dumyword)=0;byte0(dumyword)<byte1(dumyword);byte0(dumyword)++)
                                {
                                    if((byte0(dumyword)&0x0f)==0)  byte0(sumword)++;
                                    byte1(sumword)=byte0(dumyword)*2+byte0(sumword);
                                    the_ram_address[0]=CAN0_ReceProc_Buf[byte1(sumword)];
                                    the_ram_address[1]=CAN0_ReceProc_Buf[byte1(sumword)+1];
                                    the_ram_address=the_ram_address+2;
                                }
                                
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YX) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_YX_ALL)
                            {
                                Core_Src_Unit=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);  // Dbase addr
                                Core_Src_Len =CAN0_ReceProc_Buf[4]/2;  // bytes num
                                if(Core_Src_Len>(unit_info[byte0(Core_Src_Unit)].yx_num)*2)
                                    Core_Src_Len=(unit_info[byte0(Core_Src_Unit)].yx_num)*2;
                                
                                for(byte0(dumyword)=0;byte0(dumyword)<Core_Src_Len;byte0(dumyword)++)
                                {
                                    port_report[byte0(dumyword)]=CAN0_ReceProc_Buf[byte0(dumyword)*2+7];
                                }

                                Core_Src_Pt_B=&port_report[0];
                                if((Core_Src_Len & 0x01)!=0) 
                                {
                                	port_report[Core_Src_Len]=0;
                                	Core_Src_Len++;
                                }	

                                if(unit_info[Core_Src_Unit].unit_type==0x0101)
                                {
                                	if((CAN0_ReceProc_Buf[1] & 0x08)==0)  port_report[1]&=0x7f;
                                	else                                  port_report[1]|=0x80;
                                }
                                core_update_YX();
                                
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YM) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_YM_ALL)
                            {
                                byte0(dumyword)=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);
                                the_ram_address=(near BYTE *)&YM_State[unit_info[byte0(dumyword)].ym_start];
                                byte1(dumyword)=CAN0_ReceProc_Buf[4]/17*4;
                                byte0(dumyword)=unit_info[byte0(dumyword)].ym_num;
                                
                                if(byte1(dumyword)>byte0(dumyword))  // need save ym_num
                                {
                                    byte1(dumyword)=byte0(dumyword);
                                }
                                
                                byte0(sumword)=6;
                                for(byte0(dumyword)=0;byte0(dumyword)<byte1(dumyword);byte0(dumyword)++)
                                {
                                    if((byte0(dumyword)&0x03)==0)  byte0(sumword)++;
                                    byte1(sumword)=byte0(dumyword)*4+byte0(sumword);
                                    the_ram_address[0]=CAN0_ReceProc_Buf[byte1(sumword)];
                                    the_ram_address[1]=CAN0_ReceProc_Buf[byte1(sumword)+1];
                                    the_ram_address[2]=CAN0_ReceProc_Buf[byte1(sumword)+2];
                                    the_ram_address[3]=CAN0_ReceProc_Buf[byte1(sumword)+3];
                                    the_ram_address=the_ram_address+4;
                                }
                                
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_DDAC) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_DD_AC)
                            {
                                byte0(dumyword)=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);
                                the_ram_address=(near BYTE *)&YM_State[unit_info[byte0(dumyword)].dc_start];
                                byte1(dumyword)=CAN0_ReceProc_Buf[4]/17*4;
                                byte0(dumyword)=unit_info[byte0(dumyword)].dc_num;
                                
                                if(byte1(dumyword)>byte0(dumyword))  // need save ym_num
                                {
                                    byte1(dumyword)=byte0(dumyword);
                                }
                               
                                byte0(sumword)=6;
                                for(byte0(dumyword)=0;byte0(dumyword)<byte1(dumyword);byte0(dumyword)++)
                                {
                                    if((byte0(dumyword)&0x03)==0)  byte0(sumword)++;
                                    byte1(sumword)=byte0(dumyword)*4+byte0(sumword);
                                    the_ram_address[0]=CAN0_ReceProc_Buf[byte1(sumword)];
                                    the_ram_address[1]=CAN0_ReceProc_Buf[byte1(sumword)+1];
                                    the_ram_address[2]=CAN0_ReceProc_Buf[byte1(sumword)+2];
                                    the_ram_address[3]=CAN0_ReceProc_Buf[byte1(sumword)+3];
                                    the_ram_address=the_ram_address+4;
                                }
                                
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if(byte0(dumyword)==0)
                        {
                            REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                            // mean receive sth,but is not needed,if restart receive
                            // time arrived and not received will add err 1 times.
                        }
                    }
                    else  //  not CAN_NoNeedTransToRTU
                    {
                        if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YKVERIFY)
                        {
                            if((CAN0_ReceProc_Buf[0]==CAN_SHORT_FRAME) && (CAN0_ReceProc_Buf[2]==CODE_CAN_YK_CHOOSE))
                            {
                                REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                RAM_CAN0_Para.CAN_BEGINWAITRTUREPLY_TIME=Time_2048ms_Counter;
                                
                                exchange_target_port=REG_CAN0_Proc.CAN_NEEDTRANSTOHOST;

                                exchange_buf[1] = PORT_NO_CAN_0;
                                exchange_buf[2] = PROTOCOL_CAN_DSA%0x100;
                                exchange_buf[3] = PROTOCOL_CAN_DSA/0x100;
                                exchange_buf[5] = REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                exchange_buf[8] = CORE_CODE_YK_VERIFY;
                                exchange_buf[9] = 0x02;
                                exchange_buf[10]= 0x00;
                                exchange_buf[16]= CAN0_ReceProc_Buf[3];  // the same as CORE_CODE2_YK_CLOSE/TRIP/ERROR
                                exchange_buf[17]= CAN0_ReceProc_Buf[4];  // YK No.
                                if(exchange_target_port<14)
                                    exchange_buf[17] += unit_info[byte0(port_info[12].mirror_unit) + exchange_buf[5]].yk_start;
                                Ex_Produce_Transmit_Info();
                            }
                            
                        }
                        else
                        {
                            if((CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME) && (CAN0_ReceProc_Buf[3]==CODE_CAN_W_R_RLY_DATA))
                            {// 2004-07-10
                                if((CAN0_ReceProc_Buf[7]==LSA_CODE_R5_REPLY_SETTING_VERI)||
                                   (CAN0_ReceProc_Buf[7]==LSA_CODE_R13_REPLY_SETGRP_VERI)||
                                   (CAN0_ReceProc_Buf[7]==LSA_CODE_R5_REPLY_SET_EXT_VERI))
                                {
                                    REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                    RAM_CAN0_Para.CAN_BEGINWAITRTUREPLY_TIME=Time_2048ms_Counter;
                                }    
                                exchange_target_port=REG_CAN0_Proc.CAN_NEEDTRANSTOHOST;

                                exchange_buf[1] = PORT_NO_CAN_0;
                                exchange_buf[2] = PROTOCOL_LSA%0x100;
                                exchange_buf[3] = PROTOCOL_LSA/0x100;         // in CDT,CDT9702,HOSTZF these exange baowen be looked as LSA port
                                exchange_buf[4] = CAN0_AS_BH_MON_ADDR_SET;
                                exchange_buf[5] = REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                exchange_buf[8] = CAN0_ReceProc_Buf[7]; // CMD
                                
                                if((exchange_buf[8]!=LSA_CODE_ACK) && (exchange_buf[8]!=LSA_CODE_NAK))
                                {
                                    dumyword = CAN0_ReceProc_Buf[8]+1+2; // LEN
                                    exchange_buf[9] = byte0(dumyword);
                                    exchange_buf[10]= byte1(dumyword);
                                    for(sumword=16;sumword<(dumyword+16);sumword++)
                                        exchange_buf[sumword] = CAN0_ReceProc_Buf[sumword-10];
                                    exchange_buf[16]=exchange_buf[4];    //hhb 2004.10.21    BH GUIHAO
                                }
                                else
                                {
                                    exchange_buf[9]  = 0x02; // LEN
                                    exchange_buf[10] = 0x00;
                                    exchange_buf[16] = exchange_buf[4];
                                    exchange_buf[17] = exchange_buf[8];
                                }
                                
                                Ex_Produce_Transmit_Info();
                            }
                            else
                            {
                            	if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_WAVRCD)
                            	{
                            		if((CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME) && (CAN0_ReceProc_Buf[3]==CODE_CAN_WAVRCDDATA))
                            		{
                            			exchange_target_port=REG_CAN0_Proc.CAN_NEEDTRANSTOHOST;
                            			
                            			if(port_transmit_flag[exchange_target_port]!=0xaa)
                            			{
                            				port_transmit_flag[exchange_target_port]=0xaa;
                            				
                            				the_ram_addr=(BYTE *)&(port_transmit[exchange_target_port][0]);
                            				the_ram_addr[1]=PORT_NO_CAN_0;
                            				the_ram_addr[2]=PROTOCOL_CAN_DSA%0x100;
                            				the_ram_addr[3]=PROTOCOL_CAN_DSA/0x100;
                            				the_ram_addr[4]=CAN0_AS_BH_MON_ADDR_SET;
                            				the_ram_addr[5]=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                            				the_ram_addr[8]=CODE_CAN_WAVRCDDATA;
                            				
                            				byte0(sumword)=CAN0_ReceProc_Buf[4];
                            				byte1(sumword)=CAN0_ReceProc_Buf[5];
                            				sumword+=2;
                            				
                            				for(dumyword=0;dumyword<sumword;dumyword++)
                            				{
                            					the_ram_addr[dumyword+21]=CAN0_ReceProc_Buf[dumyword+4];
                            				}

                            				sumword+=5;
                            				the_ram_addr[16]=byte0(sumword);
                            				the_ram_addr[17]=byte1(sumword);
                            				the_ram_addr[18]=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
//                            				the_ram_addr[19]=0x01;
                                                        the_ram_addr[19]=CAN0_AS_BH_MON_ADDR_SET;  //hhb 2004.10.21
                            				the_ram_addr[20]=0x6e;

                            				the_ram_addr[9] =byte0(sumword);
                            				the_ram_addr[10]=byte1(sumword);
                            			}
                            		}
                            	}
                            }
                        }
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                    }
                    
                }
            }
            else
            {
                if(Judge_Time_In_MainLoop(RAM_CAN0_Para.CAN_BEGINWAITCPUREPLY_TIME,CAN_WAIT_CPU_MSG_TIME)==YES)   ;;;等待CPU回答的时间是800MS
                {
                    if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YC) goto rcd_ser_err;
                    if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YX) goto rcd_ser_err;
                    if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YM) goto rcd_ser_err;
                    goto no_rcd_ser_err;
rcd_ser_err:
                    CAN0_Comm_CurrentSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=ERR;
                    // may let ser err times and ser err last time to judge ser err 
	                if(CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]!=GOOD)
    	                CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=ERR;
                    if(Portx_Poll_First[12]!=YES)
                    {
                        CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=CAN_COMM_BEERR_NUM;
                        CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=ERR;
                        CAN0_CurrentSta_Buf[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]
                            =CAN0_CurrentSta_Buf[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR] | 0x80;
                    }
                    else
                    {
                        if(CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]==GOOD)
                        {
                            if(CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]<CAN_COMM_BEERR_NUM)
                            {
                                CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]++;
                            }
                        
                            if(CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]>=CAN_COMM_BEERR_NUM)
                            {
                                CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=ERR;
                                CAN0_CurrentSta_Buf[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]
                                    =CAN0_CurrentSta_Buf[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR] | 0x80;
                                
                                if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
                                the_ram_addr=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
                                the_ram_addr[0]=PROTOCOL_LSA%0x100;     // protocol type
                                the_ram_addr[1]=0x51;                   // CMD      code
                                the_ram_addr[2]=0x00;                   // frame    no
                                the_ram_addr[3]=0x12;                   // valid    bytes  in this frame
                                the_ram_addr[4]=the_ram_addr[0]
                                               +the_ram_addr[1]
                                               +the_ram_addr[2]
                                               +the_ram_addr[3];        // verify   byte
                                the_ram_addr[5]=PROTOCOL_LSA/0x100;     // Reserved
            
                                the_ram_addr[6]=0x01;                   // Frame    Num
                                the_ram_addr[7]=0x00;                   // Reserved
                                the_ram_addr[8]=CAN0_AS_BH_MON_ADDR_SET;// CAN      GuiHao
                                the_ram_addr[9]=0x51;
                                the_ram_addr[10]=0x0d;
                                byte0(sumword)=REG_Year%100;
                                the_ram_addr[11]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                                byte0(sumword)=(REG_Year%10000)/100;
                                the_ram_addr[12]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                                the_ram_addr[13]=(REG_Month/10)*0x10+REG_Month%10;
                                the_ram_addr[14]=(REG_Date/10)*0x10+REG_Date%10;
                                the_ram_addr[15]=(REG_Hour/10)*0x10+REG_Hour%10;
                                the_ram_addr[16]=(REG_Minute/10)*0x10+REG_Minute%10;
                                the_ram_addr[17]=(REG_Second/10)*0x10+REG_Second%10;
                                byte0(sumword)=REG_1Msecond%100;
                                the_ram_addr[18]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                                byte0(sumword)=(REG_1Msecond%1000)/100;
                                the_ram_addr[19]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                                the_ram_addr[20]=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                the_ram_addr[21]=0x04;
                                sumword=0;
                                for(byte0(dumyword)=9;byte0(dumyword)<22;byte0(dumyword)++)
                                {
                                	sumword+=the_ram_addr[byte0(dumyword)];
                                }
                                the_ram_addr[22]=byte0(sumword);
                                the_ram_addr[23]=byte1(sumword);
            
                                BH_Report_Bank_Sav_Ptr++;
                                if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
                                
                                 // BH to SOE
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE[dumyword].ActionType==the_ram_addr[21]))
                                        	{
                                            	BH_To_YX_Delay_Begin_Time[dumyword]=Time_2048ms_Counter;
                    
	                                            dumyword =BH_Event_To_SOE[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] | (1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]=(byte1(dumyword) & 0x0f)+0x80; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                 
                                        dumyword++;
                                    }
                                    
                                    // BH to SOE Ret
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE_Ret[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE_Ret[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE_Ret[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE_Ret[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE_Ret[dumyword].ActionType==the_ram_addr[21]))
                                        	{
	                                            dumyword =BH_Event_To_SOE_Ret[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] & (0xffff-1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]= byte1(dumyword) & 0x0f; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                                        dumyword++;
                                    }
                                
                                
                            }
                        }
                        else  // CAN0_Comm_Error_LastSta=ERR
                        {
                            CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=CAN_COMM_BEERR_NUM;
                            if((Now_CANx_Sw_Channel_Sta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR] & 0x10)==0)
                            	Now_CANx_Sw_Channel_Sta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=0x50;
                            else
                            	Now_CANx_Sw_Channel_Sta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=0xa0;	
                        }
                    }    
no_rcd_ser_err:
                    //if(F_REG_CAN_PROC.CAN_NeedTransToRTU=CAN_NeedTransToCOMp)then
                    //{
                    //}
                    
                    REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                }
            }
        }
        else  // REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_TransingToCPU
        {
            // do nothing
        }
    }
}


















/************************************************/
/* CAN0_init     function                       */
/************************************************/
//-怎么会在主程序中就进行初始化呢,是不是说明这个程序是一定有的并不是配置的
/*===========================================================================*/
void CAN0_Init(void)
{
        WORD  ram_bx;
    far BYTE *ram_base_addr;

    ram_base_addr=(far BYTE *)(&CAN0_CurrentSta_Buf);	//-初始状态赋0
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(CAN0_CurrentSta_Buf)/sizeof(CAN0_CurrentSta_Buf[0]);
        ram_bx++)
    {
        ram_base_addr[ram_bx]=0;
    }
    
    ram_base_addr=(far BYTE *)(&CAN0_IED_StaChg);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(CAN0_IED_StaChg)/sizeof(CAN0_IED_StaChg[0]);
        ram_bx++)
    {
        ram_base_addr[ram_bx]=0;	//-初始化就是对大量标志数组赋0,方法就是一个人的风格
    }
    
    ram_base_addr=(far BYTE *)(&CAN0_Comm_Error_LastSta);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(CAN0_Comm_Error_LastSta)/sizeof(CAN0_Comm_Error_LastSta[0]);
        ram_bx++)
    {
        ram_base_addr[ram_bx]=UNKNOW;
    }
    
    ram_base_addr=(far BYTE *)(&CAN0_Comm_CurrentSta);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(CAN0_Comm_CurrentSta)/sizeof(CAN0_Comm_CurrentSta[0]);
        ram_bx++)
    {
        ram_base_addr[ram_bx]=UNKNOW;
    }
    
    ram_base_addr=(far BYTE *)(&Now_CANx_Sw_Channel_Sta);	//-初始赋50又是和用意呢
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(Now_CANx_Sw_Channel_Sta)/sizeof(Now_CANx_Sw_Channel_Sta[0x00]);
        ram_bx++)
    {
        ram_base_addr[ram_bx]=0x50;
    }
    if((Vqc_Info[0].DSA_243_Addr!=0x00)&&(Vqc_Info[0].DSA_243_Addr!=0xff))	//-检查电压无功控制 地址
        Now_CANx_Sw_Channel_Sta[Vqc_Info[0].DSA_243_Addr]|=0x30;
    if((Vqc_Info[1].DSA_243_Addr!=0x00)&&(Vqc_Info[1].DSA_243_Addr!=0xff))
        Now_CANx_Sw_Channel_Sta[Vqc_Info[1].DSA_243_Addr]|=0x30;	//-初始状态是 配置的检查电压无功控制地址可以接收
    
    CAN_Not_Idle_Cn=0;
    
    REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE  =NO;
    REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=NO;
    REG_CAN0_Port.F_TRANS_STATUS             =0;

    REG_CAN0_Proc.CAN_MON_STATUS             =CAN_MON_IDLE;
    
    CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr=0;
    CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr=0;
    
    RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR=0;	//-给结构体赋初值
    RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR=0;
    
    RAM_CAN0_Para.CAN_RECEYCCHG_BUF_SAV_PTR=0;
    RAM_CAN0_Para.CAN_RECEYCCHG_BUF_TAK_PTR=0;
    
    RAM_CAN0_Para.CAN_PORT_RECE_BUF_SAV_PTR=0;
    RAM_CAN0_Para.CAN_PORT_RECE_BUF_TAK_PTR=0;
    
    
    RAM_CAN0_Para.CAN_BEGIN_VERICPUCLK_TIME=Time_2048ms_Counter+TIME_CAN_VERIFY_TIME_VALUE+1;
    RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME =Time_2048ms_Counter+TIME_ASK_YC_VALUE+1;           ;;; 访问有效的时间5+1
    RAM_CAN0_Para.CAN_BEGIN_POL_YXALL_TIME =Time_2048ms_Counter+TIME_ASK_YX_VALUE+1;           ;;;  10
    RAM_CAN0_Para.CAN_BEGIN_POL_YMALL_TIME =Time_2048ms_Counter+TIME_ASK_YM_VALUE+1;           ;;;   15
    RAM_CAN0_Para.CAN_BEGIN_POL_DD_AC_TIME =Time_2048ms_Counter+TIME_ASK_DD_VALUE+1;           ;;;   15 //-这些有些时间就是判断有效时间段得
    
    RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YCALL;
    CAN_NOW_POL_YCALL_FLAG=YES;
    RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR=MIN_CAN0_ADDR;
    REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx =0xff;        ;;;不需要判断什么
    
    CAN_246_Trans_YK_Buf[0]=CAN_246_TRANS_YK_STA_IDLE;         ;;; 0
    CAN_246_Trans_YK_Buf[1]=0xff;
	CAN_246_Trans_YK_Buf[2]=0xff;
	CAN_246_Trans_YK_Buf[3]=0xff;
	CAN_246_Trans_YK_Buf[4]=0xff;
	CAN_246_Trans_YK_Buf[5]=0xff;
	CAN_246_Trans_YK_Buf[6]=0xff;
	CAN_246_Trans_YK_Buf[7]=0xff;
}










;;;这段程序也许就是对接收到的报文的处理程序或发送处理程序,但由于通讯管理机通讯的复杂性
;;;这儿并不是唯一的处理过程,但这儿应该是独一的过程.
;;;我需要更多理论和实际要求,就必须大量看书和干活.
/*===========================================================================*/
void CAN0_Main(void)                           ;;;在这里只需要声明外部需要外部变量,而不需要把定义的变量说明了可以外部公用
{
    BYTE  the_ram_axl;
    BYTE *the_ram_addr_byte;

    CAN_Port_ReceAsm();                 ;;;是把接收到的信息转化成自己内部信息,他处理的信息已经不是从总线上来的了,而是第一缓冲区的,这个就相当于CPU中的接收过程  接收整理
    CAN_MSG_Proc();                     ;;;对经过初步分析提取的信息做进一步处理,这个是CPU中T1中断的处理过程,CMU放到了同一个时限内
    
    if(REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx!=0xff)     ;;;阻止调查地址_来自12个口的哪个吗
    {
        if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGINWAITRTUREPLY_TIME,30)==YES)    ;;;判断是否超过 2048MS*30 这个范围61.44 S
        {
            REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx=0xff;   ;;;等于FF就没有必要再判断了
        }
    }
    
    the_ram_addr_byte=(BYTE *)&YX_State[IED_TX_STATE_START_WORD+(byte0(port_info[12].mirror_unit))/16];  // so BUS mirror start addr must be x*16;;;为什么要去掉前32个
    for(the_ram_axl=0;the_ram_axl<64;the_ram_axl++)
    {
        if(CAN0_Comm_Error_LastSta[the_ram_axl]==GOOD)
            the_ram_addr_byte[the_ram_axl/8]=the_ram_addr_byte[the_ram_axl/8] & (0xff-(1<<(the_ram_axl%8)));  ;;;把同一个字节里面的数字一位一位清零
        if(CAN0_Comm_Error_LastSta[the_ram_axl]==ERR)   ;;;好用0表示,不好用1表示
            the_ram_addr_byte[the_ram_axl/8]=the_ram_addr_byte[the_ram_axl/8] | (1<<(the_ram_axl%8)); ;;;共用8个字节表示64台装置的状态
    }
    CAN_judge_state_to_yx();
    temp_int=Time_2048ms_Counter;       ;;;接收2048的计数
    if(REG_CAN0_Proc.CAN_MON_STATUS!=CAN_MON_IDLE)
    {
        if(CAN_Time2048ms_Last_Value!=temp_int)
            CAN_Not_Idle_Cn++;
        if(CAN_Not_Idle_Cn>300) // 10 min
        {
            REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE  =NO;
            REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=NO;
            REG_CAN0_Port.F_TRANS_STATUS             =0;

            REG_CAN0_Proc.CAN_MON_STATUS             =CAN_MON_IDLE;
            port_transmit_flag[PORT_NO_CAN_0]=0x55;         ;;;超过10分钟就给55说明不好了

            RAM_CAN0_Para.CAN_BEGIN_VERICPUCLK_TIME=Time_2048ms_Counter+TIME_CAN_VERIFY_TIME_VALUE+1;     ;;; +15+1
            RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME =Time_2048ms_Counter+TIME_ASK_YC_VALUE+1;
            RAM_CAN0_Para.CAN_BEGIN_POL_YXALL_TIME =Time_2048ms_Counter+TIME_ASK_YX_VALUE+1;
            RAM_CAN0_Para.CAN_BEGIN_POL_YMALL_TIME =Time_2048ms_Counter+TIME_ASK_YM_VALUE+1;
            RAM_CAN0_Para.CAN_BEGIN_POL_DD_AC_TIME =Time_2048ms_Counter+TIME_ASK_DD_VALUE+1;
    
            RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_NONE;
            RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR=MIN_CAN0_ADDR;
            REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx =0xff;
            
            CAN_Not_Idle_Cn=0;          ;;;从头开始计什么数

            Rcd_Info_Myself_Tmp[0]=0;                                         // reserved
            Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CAN;                 // PORT NO.        ;;;记录哪个端口号  为 12 CAN0
            Rcd_Info_Myself_Tmp[2]=0;                                         // UNIT ADDR
            Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_PORT_STA_RET_IDLE;   // do   what     ;;;01
            Rcd_Info_Myself_Tmp[4]=0;
            Rcd_Info_Myself_Tmp[5]=0;    
            Rcd_Info_Myself_Tmp[6]=0;  
            Rcd_Info_Myself_Tmp[7]=0; 
                        
            Store_Rcd_Info_Myself();
        }
    }
    
    CAN_Time2048ms_Last_Value=temp_int;        ;;;记录原始值判断变化
    
    if(CAN_246_Trans_YK_Buf[0]==CAN_246_TRANS_YK_STA_WAIT_VERIFY)      ;;;01
    {
        if(Judge_LongTime_In_MainLoop(CAN_246_YK_Begin_WAIT_VERIFY,15)==YES)        ;;;判断是否超过了15*2048的时间 30S
        {
        	CAN_246_Trans_YK_Buf[0]=CAN_246_TRANS_YK_STA_IDLE;
        	CAN_246_Trans_YK_Buf[1]=0xff;
    		CAN_246_Trans_YK_Buf[2]=0xff;
    		CAN_246_Trans_YK_Buf[3]=0xff;
    		CAN_246_Trans_YK_Buf[4]=0xff;
    		CAN_246_Trans_YK_Buf[5]=0xff;
    		CAN_246_Trans_YK_Buf[6]=0xff;
    		CAN_246_Trans_YK_Buf[7]=0xff;
        }
    }
    
    if(CAN_246_Trans_YK_Buf[0]==CAN_246_TRANS_YK_STA_WAIT_CONFIRM)
    {
        if(Judge_LongTime_In_MainLoop(CAN_246_YK_Begin_WAIT_CONFIRM,30)==YES)      ;;;60S
        {
        	CAN_246_Trans_YK_Buf[0]=CAN_246_TRANS_YK_STA_IDLE;
    		CAN_246_Trans_YK_Buf[1]=0xff;
    		CAN_246_Trans_YK_Buf[2]=0xff;
    		CAN_246_Trans_YK_Buf[3]=0xff;
    		CAN_246_Trans_YK_Buf[4]=0xff;
    		CAN_246_Trans_YK_Buf[5]=0xff;
    		CAN_246_Trans_YK_Buf[6]=0xff;
    		CAN_246_Trans_YK_Buf[7]=0xff;
        }
    }
    
    if(CAN_246_Trans_YK_Buf[0]==CAN_246_TRANS_YK_STA_TRANSING_TO_CAN)
    {
        if((BYTE)(CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr+1)==   // Que_len=256
            CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr     )
        {
            CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr++;
        }
                                    
        the_ram_addr_byte=(far BYTE *)&CAN0_CPUASKCMU_Trans_Buf[0]
                         +CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr*12;
        the_ram_addr_byte[0x00]=CAN_246_Trans_YK_Buf[1]      ;  // CAN246_ADDR
        the_ram_addr_byte[0x01]=0                            ;  // frame No
        the_ram_addr_byte[0x02]=CAN_246_Trans_YK_Buf[6]      ;  // CAN243_ADDR
        the_ram_addr_byte[0x03]=CAN_SHORT_FRAME              ;
        the_ram_addr_byte[0x04]=0                            ;  // S
        the_ram_addr_byte[0x05]=CODE_CAN_YK_CHOOSE           ;  // CMD
        if((CAN_246_Trans_YK_Buf[4]==CORE_CODE2_YK_CLOSE)||(CAN_246_Trans_YK_Buf[4]==CORE_CODE2_YK_TRIP))
        {
        	the_ram_addr_byte[0x06]=CAN_246_Trans_YK_Buf[4];
        	the_ram_addr_byte[0x0a]=0x06;
        }
        else
        {
        	the_ram_addr_byte[0x06]=0xff;
        	the_ram_addr_byte[0x0a]=0x00;
        }
        
        the_ram_addr_byte[0x07]=CAN_246_Trans_YK_Buf[7];  // 
        the_ram_addr_byte[0x08]=the_ram_addr_byte[0x06];  // 
        the_ram_addr_byte[0x09]=the_ram_addr_byte[0x07];  // reserved
        the_ram_addr_byte[0x0b]=the_ram_addr_byte[0x0a];  // 
                                
        CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr++;
        
        CAN_246_YK_Begin_WAIT_CONFIRM=Time_2048ms_Counter;           ;;;有效时间计时开始
        CAN_246_Trans_YK_Buf[0]=CAN_246_TRANS_YK_STA_WAIT_CONFIRM;   ;;;02
    }
}















/************************************************/
/* CAN0_Monitor     function                    */
/************************************************/
/*===========================================================================*/
void CAN0_Monitor(void)
{
    if(((SIO_CAN_Need_Reset & 0x1000)!=0) || ((*(far BYTE *)(CAN0_BaseAddr+CAN_MODE) & 0x01)!=0) 
    ||((*(far BYTE *)(CAN0_BaseAddr+CAN_STATUS) & 0x80)!=0))
    {
        disable();
        Initial_CAN(0);
        enable();
        SIO_CAN_Need_Reset=SIO_CAN_Need_Reset & 0xefff;	//-正常情况下位12应该为0
        (*((far BYTE *)(SOFT_ERR_FLAG+0x00b0)))++;
    }
 
    
    ;;;当然有两种情况了啊 两个CAN口呢,过程应该完全相同
 
 
    if(((SIO_CAN_Need_Reset & 0x2000)!=0) || ((*(far BYTE *)(CAN1_BaseAddr+CAN_MODE) & 0x01)!=0) 
    ||((*(far BYTE *)(CAN1_BaseAddr+CAN_STATUS) & 0x80)!=0))
    {
        disable();
        Initial_CAN(1);
        enable();
        SIO_CAN_Need_Reset=SIO_CAN_Need_Reset & 0xdfff;
        (*((far BYTE *)(SOFT_ERR_FLAG+0x00b1)))++;
    }
}



