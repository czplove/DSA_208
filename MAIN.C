/*****************************************************************************/
/*       FileName  :   MAIN.C                                                */
/*       Content   :   DSA-208 MAIN Module                                   */
/*       Date      :   Fri  02-22-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/

//-不要把这样的东西神话,其实也就是一个一个来的只不过在逻辑组合上有不同,但是肯定还是一个
//-还要记住一点,这个就是一个单片机组成的嵌入式系统,顶多是DSP


#pragma  regconserve                  ;;;什么意思,是要求能够包含编译吗   寄存器 保存

#include _SFR_H_
#include _FUNCS_H_

#include "common.h"
#include "device.h"
#include "reg.h"
#include "ram.h"
#include "rom.h"
#include "comuse.h"

//#pragma  ccb(0xF8D8)










/*---------------------------------------------------------------------------*/
/*                    Definition  of  global  variables                      */
/*---------------------------------------------------------------------------*/

//    none


/*---------------------------------------------------------------------------*/
/*                    Definition  of  local  variables                       */
/*---------------------------------------------------------------------------*/

//    none



/*---------------------------------------------------------------------------*/
/*                        IMPORT            functions                        */
/*---------------------------------------------------------------------------*/


//Set port
extern void set_main();           ;;;告诉编译器这个函数定义在外边
extern void set_init();
extern void set_announce_rpt();

// Protocols
extern void Protocol_Main(void);
extern void Protocol_Init(void);          ;;;这个地方是预处理命令表示这个函数是引用的别的地方的
extern void Protocol_Monitor(void);

//Exchange
extern void Ex_init();         ;;;说明在别的外部有一个定义

 





/*---------------------------------------------------------------------------*/
/*                      predefintion        functions                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/

/***********************************************/
/*   initial_dallas  for   Main  procedure     */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void initial_dallas(void)
{
    far BYTE *ram_base_addr1;
    far BYTE *ram_base_addr2;
    
    ram_base_addr1=(far BYTE *)DALLAS_YEAR;
    if( ((ram_base_addr1[0] & 0xf0)>0x90) || ((ram_base_addr1[0] & 0x0f)>0x09) )
    {
        Rcd_Info_Myself_Tmp[0]=0;  // reserved
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_DALLAS;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_DALLAS_YEAR;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_BCD_DATA_ERR;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=ram_base_addr1[0];  // Err Year
        Rcd_Info_Myself_Tmp[7]=0;                  // New Year
        Store_Rcd_Info_Myself();

        ram_base_addr2=(far BYTE *)DALLAS_CONTROL;
        ram_base_addr2[0]=0x80;
        ram_base_addr1[0]=0;
        ram_base_addr2[0]=0x00;
    }

    ram_base_addr1=(far BYTE *)DALLAS_MONTH;
    if( ( (ram_base_addr1[0] & 0x0f)>0x09) ||
        (  ram_base_addr1[0] ==0x00      ) ||
        (  ram_base_addr1[0] > 0x12      )
      )
    {
        Rcd_Info_Myself_Tmp[0]=0;  // reserved
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_DALLAS;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_DALLAS_MONTH;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_BCD_DATA_ERR;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=ram_base_addr1[0];  // Err Month
        Rcd_Info_Myself_Tmp[7]=1;                  // New Month
        Store_Rcd_Info_Myself();

        ram_base_addr2=(far BYTE *)DALLAS_CONTROL;
        ram_base_addr2[0]=0x80;
        ram_base_addr1[0]=1;
        ram_base_addr2[0]=0x00;
    }

    ram_base_addr1=(far BYTE *)DALLAS_DATE;
    if(((ram_base_addr1[0] & 0xf0)>0x30  || (ram_base_addr1[0] & 0x0f)>0x09) ||
       ( ram_base_addr1[0] ==0x00                                          ) ||
       ((ram_base_addr1[0] & 0xf0)==0x03 && (ram_base_addr1[0] & 0x0f)>0x01)
      )
    {
        Rcd_Info_Myself_Tmp[0]=0;  // reserved
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_DALLAS;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_DALLAS_DATE;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_BCD_DATA_ERR;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=ram_base_addr1[0];  // Err Date
        Rcd_Info_Myself_Tmp[7]=1;                  // New Date
        Store_Rcd_Info_Myself();

        ram_base_addr2=(far BYTE *)DALLAS_CONTROL;
        ram_base_addr2[0]=0x80;
        ram_base_addr1[0]=1;
        ram_base_addr2[0]=0x00;
    }

    ram_base_addr1=(far BYTE *)DALLAS_DAY;
    if((ram_base_addr1[0]>0x07) || (ram_base_addr1[0]==0))
    {
        Rcd_Info_Myself_Tmp[0]=0;  // reserved
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_DALLAS;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_DALLAS_DAY;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_BCD_DATA_ERR;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=ram_base_addr1[0];  // Err Day
        Rcd_Info_Myself_Tmp[7]=1;                  // New Day
        Store_Rcd_Info_Myself();

        ram_base_addr2=(far BYTE *)DALLAS_CONTROL;
        ram_base_addr2[0]=0x80;
        ram_base_addr1[0]=1;
        ram_base_addr2[0]=0x00;
    }

    ram_base_addr1=(far BYTE *)DALLAS_HOUR;
    if( (((ram_base_addr1[0] & 0xf0) >0x20) || ((ram_base_addr1[0] & 0x0f)>0x09)) ||
        (((ram_base_addr1[0] & 0xf0)==0x02) && ((ram_base_addr1[0] & 0x0f)>0x03))
      )
    {
        Rcd_Info_Myself_Tmp[0]=0;  // reserved
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_DALLAS;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_DALLAS_HOUR;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_BCD_DATA_ERR;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=ram_base_addr1[0];  // Err Hour
        Rcd_Info_Myself_Tmp[7]=1;                  // New Hour
        Store_Rcd_Info_Myself();

        ram_base_addr2=(far BYTE *)DALLAS_CONTROL;
        ram_base_addr2[0]=0x80;
        ram_base_addr1[0]=1;
        ram_base_addr2[0]=0x00;
    }

    ram_base_addr1=(far BYTE *)DALLAS_MINUTE;
    if(( (ram_base_addr1[0] & 0xf0)>0x50 ) || ( (ram_base_addr1[0] & 0x0f)>0x09 ))
    {
        Rcd_Info_Myself_Tmp[0]=0;  // reserved
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_DALLAS;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_DALLAS_MINUTE;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_BCD_DATA_ERR;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=ram_base_addr1[0];  // Err Minute
        Rcd_Info_Myself_Tmp[7]=1;                  // New Minute
        Store_Rcd_Info_Myself();

        ram_base_addr2=(far BYTE *)DALLAS_CONTROL;
        ram_base_addr2[0]=0x80;
        ram_base_addr1[0]=1;
        ram_base_addr2[0]=0x00;
    }

    ram_base_addr1=(far BYTE *)DALLAS_SECOND;
    if((((ram_base_addr1[0] & 0xf0)>0x50) || ((ram_base_addr1[0] & 0x0f)>0x09)))
    {
        Rcd_Info_Myself_Tmp[0]=0;  // reserved
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_DALLAS;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_DALLAS_SECOND;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_BCD_DATA_ERR;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=ram_base_addr1[0];  // Err Second
        Rcd_Info_Myself_Tmp[7]=1;                  // New Second
        Store_Rcd_Info_Myself();

        ram_base_addr2=(far BYTE *)DALLAS_CONTROL;
        ram_base_addr2[0]=0x80;
        ram_base_addr1[0]=1;
        ram_base_addr2[0]=0x00;
    }

    Read_Time_From_Dallas(); 

}  // initial_dallas()














/***********************************************/
/*   judge_mainloop_int_dead                   */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void judge_mainloop_int_dead(void)     ;;;判断主循环中断是否死
{
    BYTE  ram_axl;
    RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER=0;

    // EPA1  soft_time_1ms
    RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER++;
    if(RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER>50000)        ;;;判断依据是50S
    {
        disable();       ;;;关中断
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_EPA1_INT;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();      ;;;自己出了问题也要上送吗,要啊 ,你要让人知道
        enable();
        
        int_pend1=int_pend1 | 0x01;            ;;;处理完之后软件再次启动这个中断
        RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER=0;
        RAM_CPU_INT_Rst_Cn[5]++;                 ;;;记录复位次数吗
    }
    
    // EPA0  INT_COM0
    ram_axl=p1_pin | 0xfe;
    if(RAM_CPU_Int_Moni.EPA0_INT_DEAD_COUNTER>500)
    {
        disable();
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_EPA0_INT;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();
        enable();
        
        int_pend=int_pend | 0x80;

        RAM_CPU_Int_Moni.EPA0_INT_DEAD_COUNTER=0;
        RAM_CPU_INT_Rst_Cn[4]++;
    }
    else
    {
        if(ram_axl!=0xff)   RAM_CPU_Int_Moni.EPA0_INT_DEAD_COUNTER++;    ;;;不等于
    }
    
    // EXINT2  INT_COM1
    ram_axl=p3_pin | 0xbf;
    if(RAM_CPU_Int_Moni.EXINT2_INT_DEAD_COUNTER>5)
    {
        disable();
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_EXINT2_INT;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();
        enable();
        
        int_pend1=int_pend1 | 0x20;
        RAM_CPU_Int_Moni.EXINT2_INT_DEAD_COUNTER=0;
        RAM_CPU_INT_Rst_Cn[2]++;
    }
    else
    {
        if(ram_axl!=0xff)   RAM_CPU_Int_Moni.EXINT2_INT_DEAD_COUNTER++;
    }
    
    // EXINT3  INT_COM2
    ram_axl=p3_pin | 0x7f;
    if(RAM_CPU_Int_Moni.EXINT3_INT_DEAD_COUNTER>5)
    {
        disable();
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_EXINT3_INT;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();
        enable();
        
        int_pend1=int_pend1 | 0x40;
        RAM_CPU_Int_Moni.EXINT3_INT_DEAD_COUNTER=0;
        RAM_CPU_INT_Rst_Cn[3]++;
    }
    else
    {
        if(ram_axl!=0xff)   RAM_CPU_Int_Moni.EXINT3_INT_DEAD_COUNTER++;
    }
    
    // EXINT0  INT_CAN
    ram_axl=p2_pin | 0xfb;
    if(RAM_CPU_Int_Moni.EXINT0_INT_DEAD_COUNTER>5)
    {
        disable();
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_EXINT0_INT;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();
        enable();
        
        int_pend=int_pend | 0x08;
        RAM_CPU_Int_Moni.EXINT0_INT_DEAD_COUNTER=0;
        RAM_CPU_INT_Rst_Cn[0]++;
    }
    else
    {
        if(ram_axl!=0xff)   RAM_CPU_Int_Moni.EXINT0_INT_DEAD_COUNTER++;
    }
    
    // EXINT1  INT_CAN1
    ram_axl=p2_pin | 0xef;
    if(RAM_CPU_Int_Moni.EXINT1_INT_DEAD_COUNTER>5)
    {
        disable();
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_EXINT1_INT;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();
        enable();
        
        int_pend=int_pend | 0x10;
        RAM_CPU_Int_Moni.EXINT1_INT_DEAD_COUNTER=0;
        RAM_CPU_INT_Rst_Cn[1]++;
    }
    else
    {
        if(ram_axl!=0xff)   RAM_CPU_Int_Moni.EXINT1_INT_DEAD_COUNTER++;
    }
    
}













/***********************************************/
/*   judge_host_zf_enable                      */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void judge_host_zf_enable(void)   //-规约类型,规定 高字节>80H的规约为上行规约  <80H 为下行
{                                                                       ;;;上行规约是指可以与后台通信 而下行归于是指可以与保护装置通信吗
    BYTE  ram_axl;
    BYTE  ram_axh;
    
    ram_axh=HOST_ZF_enable;
    for(ram_axl=0;ram_axl<14;ram_axl++)
    {
        if((port_info[ram_axl].protocal_type!=PROTOCOL_NONE)&&(byte1(port_info[ram_axl].protocal_type)<0x80)) //-有规约且规约号得高字节小于80H
        {
            if(Portx_Poll_First[ram_axl]!=YES)
                break;        //-立即跳出该层循环,继续执行循环之后的语句
        }
        
    }
    if(ram_axl>=14)                  
        HOST_ZF_enable=YES;    ;;;检查完了所有端口规约,都没有出现小于80H的就给YES,表示只能和后台通信吗
    else
        HOST_ZF_enable=NO;     ;;;发现有小于的规约存在就给这个值表示自发使能吗即下位机可以向上送内容吗
    
    if((ram_axh!=YES)&&(HOST_ZF_enable==YES))    //-原来不等于YES现在出现了
    {
        for(ram_axl=0;ram_axl<4;ram_axl++)
        {
            temp_ptD = (unsigned short*)&YX_transmite_table[ram_axl][0];    ;;;这个地方利用了一个二维数组
            temp_int = 0;
            //while(*temp_ptD < 2048)
            while(*temp_ptD < 4096)           ;;;判断首个单元的内容小于4096
            {
                if((YX_State[(*temp_ptD)/16] & (1<<((*temp_ptD)&0x0f))) != 0)
                {
                    temp_lp_int = YX_state_tr[ram_axl][temp_int/16];
                    temp_lp_int = temp_lp_int | (1 << (temp_int&0x0f));
                    YX_state_tr[ram_axl][temp_int/16] = temp_lp_int;
                }
                else
                {
                    temp_lp_int = YX_state_tr[ram_axl][temp_int/16];
                    temp_lp_int = temp_lp_int & (0xffff ^ (1 << (temp_int&0x0f)));
                    YX_state_tr[ram_axl][temp_int/16] = temp_lp_int;
                }
                temp_ptD ++;
                temp_int ++;
                if(temp_int>1023)     ;;;共有1024个
                break;
            }    
        }
    }
    
    if(HOST_YK_Doing==2)
    {
    	if(Judge_Time_In_MainLoop(HOST_YK_Doing_Begin_Time,40000)==YES)
    	{
    		HOST_YK_Doing=0;
    	}
    }
    else
    {
    	if(HOST_YK_Doing!=0)  // && !=2
    	{
	    	if(Judge_Time_In_MainLoop(HOST_YK_Doing_Begin_Time,3000)==YES)
    		{
    			HOST_YK_Doing=0;
    		}
    	}	
    }
    
    
}













/***********************************************/
/*   byq_dw_yx_translate_to_yc                 */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void byq_dw_yx_translate_to_yc(void)       ;;;变压器档位
{
    BYTE  ram_axl;
    BYTE  ram_axh;
    WORD  ram_bx;
    
    for(ram_axl=0;ram_axl<8;ram_axl++)
    {
        if(Byq_Dw[ram_axl].yc_val_no<=(sizeof(YC_State)/sizeof(YC_State[0])))	//-在所有的数据中查找到这样的内容,并处理
        {
            ram_bx=0;
            for(ram_axh=0;ram_axh<32;ram_axh++)
            {
                if( (YX_State[(Byq_Dw[ram_axl].yx_no[ram_axh])/16] & (1<<(Byq_Dw[ram_axl].yx_no[ram_axh] & 0x000f)))!=0)
                    ram_bx=ram_bx+Byq_Dw[ram_axl].yx_weight[ram_axh];        ;;;累加和
            }
            
            YC_State[Byq_Dw[ram_axl].yc_val_no]=ram_bx;
        }
        
    }
}














/***********************************************/
/*   yx_or_calculate                           */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void yx_or_calculate(void)
{
    BYTE  ram_axl;
    BYTE  ram_axh;
    
    for(ram_axl=0;ram_axl<(sizeof(YX_OR_Table)/sizeof(YX_OR_Table[0]));ram_axl++)        ;;;与64比较
    {
        if(YX_OR_Table[ram_axl][0]<0x1000)                  ;;;取遥信表中的数值决定是否有"配置"
        {
            for(ram_axh=1;ram_axh<9;ram_axh++)
            {
                if( (YX_State[(YX_OR_Table[ram_axl][ram_axh])/16] & (1<<(YX_OR_Table[ram_axl][ram_axh] & 0x000f)))!=0)  ;;;?
                {
                  if((YX_State[(YX_OR_Table[ram_axl][0])/16]&(1<<(YX_OR_Table[ram_axl][0] & 0x000f)))==0)
                   {
                    YX_State[(YX_OR_Table[ram_axl][0])/16]|=(1<<(YX_OR_Table[ram_axl][0] & 0x000f));
                    ram_axh=100;
                   }
                }    
            }
            if(ram_axh<80)
            {
            	YX_State[(YX_OR_Table[ram_axl][0])/16]&=(0xffff-(1<<(YX_OR_Table[ram_axl][0] & 0x000f)));
            }
        }
    }
}













/***********************************************/
/*   judge_bh_to_yx_delay_time                 */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void judge_bh_to_yx_delay_time(void)
{
    WORD  ram_ax;
    WORD  ram_bx;
    WORD  ram_cx;
    WORD  ram_dx;
    
    for(ram_bx=0;ram_bx<512;ram_bx++)
    {
        ram_ax=BH_Event_To_SOE[ram_bx].YX_State_No;       ;;;保护事件转遥信
        if(ram_ax<4096)
        {
		      for(ram_dx=0;ram_dx<512;ram_dx++)
		      {
        		ram_cx=BH_Event_To_SOE_Ret[ram_dx].YX_State_No;
		        if(ram_cx<4096)
        		{
        			if(ram_cx==ram_ax)  goto judge_next;
        		}
        		else
        			break;
          }
            if(PROTOCOL_CDT_YKFG==NO)
             {
              if(Judge_LongTime_In_MainLoop(BH_To_YX_Delay_Begin_Time[ram_bx],BH_EVENT_TO_SOE_YX_RETURN_TIME)==YES)  // 150*2s=5min
               YX_State[ram_ax/16]=YX_State[ram_ax/16] & (0xffff-(1<<(ram_ax%16)));
             }
            else
            if((YX_FG_FLAG==YES)&&(PROTOCOL_CDT_YKFG==YES))
            {
             if((YX_State[ram_ax/16]&(0x0001<<(ram_ax%16)))!=0)
              {
               Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;
               //ms
               *Core_Det_Pt =REG_1Msecond;
               Core_Det_Pt++;                  ;;;这个地方是对指针内容加1,用于移向下一个单元
               //sec min
               *Core_Det_Pt =REG_Second+REG_Minute*256;
               Core_Det_Pt++;
               //hour day
               *Core_Det_Pt =REG_Hour+REG_Date*256;
               Core_Det_Pt++;
               // channel state
               *Core_Det_Pt =ram_ax;
               yx_soe_in_pt++;
               if(yx_soe_in_pt>1023)
                yx_soe_in_pt = 0;               ;;;这是一种能力问题,不是这个问题具体的功效问题
              }
             YX_State[ram_ax/16]=YX_State[ram_ax/16] & (0xffff-(1<<(ram_ax%16)));
            }
judge_next:
			;
        }
        else
        	break;
    }
    YX_FG_FLAG=NO; 
}

/***********************************************/
/*   initial_bh_to_yx                          */
/***********************************************/
//-保护事件转遥信
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void initial_bh_to_yx(void)
{
    WORD  ram_ax;
    WORD  ram_bx;
    
    for(ram_bx=0;ram_bx<512;ram_bx++)
    {
        ram_ax=BH_Event_To_SOE[ram_bx].YX_State_No;	//-这个数值也是事先配置好的
        if(ram_ax<4096)
        {
            YX_State[ram_ax/16]=YX_State[ram_ax/16] & (0xffff - (1<<(ram_ax%16)));	//-不知道为什么这样处理
        }
    }
    
}











/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void initial(void)      // static mean inside use function
{
        WORD  ram_bx;
    far BYTE *ram_base_addr;
    far WORD *ram_base_addr_w;


//............................ initial 296CPU ...............................//
    p1_dir =0x0f;//0x81;
    p1_mode=0x01;
    p1_reg =0x7f;
    
    p2_dir =0x1e;
    p2_mode=0x97;
    p2_reg =0xff;//0x9f;
    
    p3_dir =0xc0;
    p3_mode=0xff;
    p3_reg =0xff;
    

    extint_con=0x0f;//0xcf;   // low level to fall edge ???
  
    t1control=0xc0;      // 1100$0000 B count enable       : enable timer1
                         //             count direction    : count up
                         //             EPA clk direct&mode: clk source-f/4,direct source-UD(bit 6)
                         //             EPA clk prescale   : divide by 1

    epa1_con=0x0048;     // 0000$0000$0100$1000 B
                         // disable remap
                         // timer1 is the reference timer
                         // compare mode
                         // toggle output pin
                         // re-enable
                         // disable reset timer

    timer1   =0;
    epa1_time=timer1+EPA1_PERIOD;         ;;;这个地方就确定了,EPA1可以每1MS产生一个中断,且这个中断不需要再设置,他可以自动重新任命,这就是它理解的优越性,但时间还需要在每次中断和累加
    
    epa0_con=0x10;       ;;;定时器1为基准 捕捉方式  在下降沿捕捉
    epa2_con=0x00;       ;;;                        无捕捉 
    epa3_con=0x0000;
    

    REG_TimeTemp     =0;
    REG_Surplus_Time =0;


    Time_1ms_Counter   =0;
    Time_2048ms_Counter=0;         ;;;仅仅是一个计时寄存器吗


//............................ initial DALLAS ...............................//
    Read_Time_From_Dallas();
    
    ram_base_addr=(far BYTE *)DALLAS_RCD_MYSELF_SAV_PTR_ADDR;
    if((ram_base_addr[0] ^ ram_base_addr[4])!=0xff) goto rcd_info_myself_ptr_err;
    ram_base_addr=(far BYTE *)DALLAS_RCD_MYSELF_TAK_PTR_ADDR;
    if((ram_base_addr[0] ^ ram_base_addr[4])!=0xff) goto rcd_info_myself_ptr_err;
    goto rcd_info_myself_ptr_good;

rcd_info_myself_ptr_err:
    ram_base_addr=(far BYTE *)DALLAS_RCD_MYSELF_SAV_PTR_ADDR;
    ram_base_addr[0]=0x00;
    ram_base_addr[4]=0xff;
    ram_base_addr=(far BYTE *)DALLAS_RCD_MYSELF_TAK_PTR_ADDR;
    ram_base_addr[0]=0x00;
    ram_base_addr[4]=0xff;

    Rcd_Info_Myself_Tmp[0]=0;
    Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_DALLAS;
    Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_RCD_MYSELF_PTR_ADDR;
    Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_PTR_ADDR_XOR_ERR;
    Rcd_Info_Myself_Tmp[4]=0;
    Rcd_Info_Myself_Tmp[5]=0;
    Rcd_Info_Myself_Tmp[6]=0;
    Rcd_Info_Myself_Tmp[7]=0;
    Store_Rcd_Info_Myself();
    
rcd_info_myself_ptr_good:
    ram_base_addr=(far BYTE *)DALLAS_RCD_SYSTEM_SAV_PTR_ADDR;
    if((ram_base_addr[0] ^ ram_base_addr[4])!=0xff) goto rcd_info_system_ptr_err;
    ram_base_addr=(far BYTE *)DALLAS_RCD_SYSTEM_TAK_PTR_ADDR;
    if((ram_base_addr[0] ^ ram_base_addr[4])!=0xff) goto rcd_info_system_ptr_err;

    goto rcd_info_system_ptr_good;

rcd_info_system_ptr_err:
    ram_base_addr=(far BYTE *)DALLAS_RCD_SYSTEM_SAV_PTR_ADDR;
    ram_base_addr[0]=0x00;
    ram_base_addr[4]=0xff;
    ram_base_addr=(far BYTE *)DALLAS_RCD_SYSTEM_TAK_PTR_ADDR;
    ram_base_addr[0]=0x00;
    ram_base_addr[4]=0xff;

    Rcd_Info_Myself_Tmp[0]=0;
    Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_DALLAS;
    Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_RCD_SYSTEM_PTR_ADDR;
    Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_PTR_ADDR_XOR_ERR;
    Rcd_Info_Myself_Tmp[4]=0;
    Rcd_Info_Myself_Tmp[5]=0;
    Rcd_Info_Myself_Tmp[6]=0;
    Rcd_Info_Myself_Tmp[7]=0;
    Store_Rcd_Info_Myself();
    
rcd_info_system_ptr_good:
    
    initial_dallas();
    
    if(RAM_CPU_RST_TAG1!=0x55aa) goto ColdRst;
    if(RAM_CPU_RST_TAG2!=0xaa55) goto ColdRst;
    ram_base_addr_w=(far WORD *)CN_CPU_RST_HOT;

    if(ram_base_addr_w[0]<0xffff)
    {
        ram_base_addr_w[0]++;
    }
    Rcd_Info_Myself_Tmp[0]=0;
    Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
    Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_RST;
    Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPURST_HOT;
    Rcd_Info_Myself_Tmp[4]=0;
    Rcd_Info_Myself_Tmp[5]=0;
    Rcd_Info_Myself_Tmp[6]=byte1(ram_base_addr_w[0]);
    Rcd_Info_Myself_Tmp[7]=byte0(ram_base_addr_w[0]);
    Store_Rcd_Info_Myself();
    goto RstNoInc;
ColdRst:
    ram_base_addr_w=(far WORD *)CN_CPU_RST_COLD;
    if(ram_base_addr_w[0]<0xffff)
    {
        ram_base_addr_w[0]++;
    }
    Rcd_Info_Myself_Tmp[0]=0;
    Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
    Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_RST;
    Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPURST_COLD;
    Rcd_Info_Myself_Tmp[4]=0;
    Rcd_Info_Myself_Tmp[5]=0;
    Rcd_Info_Myself_Tmp[6]=byte1(ram_base_addr_w[0]);
    Rcd_Info_Myself_Tmp[7]=byte0(ram_base_addr_w[0]);
    Store_Rcd_Info_Myself();

    RAM_CPU_RST_TAG1=0x55aa;
    RAM_CPU_RST_TAG2=0xaa55;
RstNoInc:


    p1_reg=0x8f;//0xf1;
    p2_reg=0x9f;//0xff;      // let CAN return from hardware reset, p2 pins as output is only rst_CANx 

//.............................. initial  comuse ...............................//
  
    main_loop=0;

    RAM_CPU_Int_Moni.EXINT0_INT_DEAD_COUNTER=0;
    RAM_CPU_Int_Moni.EXINT1_INT_DEAD_COUNTER=0;
    RAM_CPU_Int_Moni.EXINT2_INT_DEAD_COUNTER=0;
    RAM_CPU_Int_Moni.EXINT3_INT_DEAD_COUNTER=0;
    RAM_CPU_Int_Moni.EPA0_INT_DEAD_COUNTER  =0;
    RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER  =0;
    RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER  =0;             ;;;需要定时对这位清零,而且正常的话一定会清零的否则超过50S没有清 就认为程序跑飞
    
    RAM_CPU_INT_Rst_Cn[0]=0;  // EXINT0_INT
    RAM_CPU_INT_Rst_Cn[1]=0;  // EXINT1_INT
    RAM_CPU_INT_Rst_Cn[2]=0;  // EXINT2_INT
    RAM_CPU_INT_Rst_Cn[3]=0;  // EXINT3_INT
    RAM_CPU_INT_Rst_Cn[4]=0;  // EPA0_INT
    RAM_CPU_INT_Rst_Cn[5]=0;  // EPA1_INT
    RAM_CPU_INT_Rst_Cn[6]=0;  // MAINLOOP
    RAM_CPU_INT_Rst_Cn[7]=0;  // Reserved
    
    SIO_CAN_Need_Reset=0;   // PORTs need reset flag
    

//.............................. initial   PORTs ...............................//
    Initial_CPUCOM();
    
    Initial_CAN(0);
    Initial_CAN(1);               ;;;对端口的初始化

    init_all_port();

    ram_base_addr_w=(far WORD *)(&YX_State);     ;;;强制类型转化  取地址 即把地址转化之后赋给指针
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(YX_State)/sizeof(YX_State[0]);
        ram_bx++)
    {
        ram_base_addr_w[ram_bx]=0;
    }

    ram_base_addr_w=(far WORD *)(&YC_State);	//-这些空间是系统自动分配的,far仅仅是指定了一个大的范围
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(YC_State)/sizeof(YC_State[0]);	//-其实程序里面都是做死的没有必要这么"复杂",但是为了减小耦合性还是这么做了
        ram_bx++)
    {
        ram_base_addr_w[ram_bx]=0;	//-刚启动的时候对数据库清零
    }

    ram_base_addr_w=(far WORD *)(&YM_State);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(YM_State)/sizeof(YM_State[0]);
        ram_bx++)
    {
        ram_base_addr_w[ram_bx*2+0x00]=0;
        ram_base_addr_w[ram_bx*2+0x01]=0;
    }

    ram_base_addr_w=(far WORD *)(&Cn_SIO_CAN_Reset);	//-串行接口
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(Cn_SIO_CAN_Reset)/sizeof(Cn_SIO_CAN_Reset[0]);
        ram_bx++)
    {
        ram_base_addr_w[ram_bx]=0;
    }
    

//............................ initial protocols ............................//
    Ex_init();                // must initial before all Ports !

    set_init();               // include set's port_initial & protocol_initial
    p2_img = p2_pin & 0x08;               
    if(p2_img == 0)
        sp_baud = CPU_SIO_BAUD_TO_LED;	//-通过检查引脚的状态来决定使用的波特率,怪怪不得可以通过灯的闪动快慢来判断运行是否正常


    CAN0_Init();
    Protocol_Init();
    
    for(port_no=0;port_no<12;port_no++)
    {
        P554_Port_Transing_Lastbyte[port_no]=NO;	//-初始状态为0
    }    

    for(port_no=0;port_no<16;port_no++)
        port_mirror[port_no] = 0x0f;

//    if need monitor any PORT info , initial 'port_mirror' here
//    port_mirror[11]=0x3a;
    port_mirror[14]=0x3a;

    for(main_loop=0;main_loop<256;main_loop++)
        unit_set_flag[main_loop] = 0x55;	//-初始为55的目的

    for(port_no=0;port_no<14;port_no++)
        Portx_Poll_First[port_no] = NO;
    
    initial_bh_to_yx();
    YX_FG_FLAG=NO;
    HOST_ZF_enable=NO;
    HOST_YK_Doing =0;
    
//.............................. initial INTs ...............................//
    int_mask =0xf8;  // bit 7-0: EPA0  , SIORec , SIOTran, Exint1,         ;;;没有那么复杂运用的肯定是有限的
    int_pend =0x00;  //          Exint0, Reserve, T2_Ovr , T1_Ovr.

    int_mask1=0x61;  // bit 7-0: NMI      , Exint3 , Exint2 , EPAOvr2_3,
    int_pend1=0x00;  //          EPAOvr1_0, EPA3   , EPA2   , EPA1     .
}  // initial()














































;;;C语言首先要用C编译器生成汇编代码
;;;主函数开始了
;;;主函数放在最后的原因是这样减少了很多函数的声明


;;;现在对这些基本语句还缺少阅读能力

/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/


                 ;;;程序开始在这个地方,这时人为逻辑设置的,我们可以通过编译器让单片机知道这些,并按此执行
;;;这个程序还复杂在我对要完成的功能还不清楚,对他拥有的资源还不清楚,要对他们也有提高才能进一步认识程序

/*===========================================================================*/
void main(void)
{
main_start:                        //-标号
    disable();                                       ;;;关中断,这个是内部库函数代码你不要管,只要运用就行了
    p4_reg = 0xfc;     // let CS0->FLASH          ,,片选吗 是的就是片选,给信号选中芯片

    //prepare_status!!!!
    buscon0=0xc1;      // 0xc1
    buscon1=0xc1;      // f80000 fbffff
    buscon2=0xc0;      // RAM         
    sp  = 0xff00;
	//-由于现在我并没有这样的硬件原理图,所以有些东西现在的层次并不需要去深究,现在需要的就是纯粹的软件处理通讯

    for(main_loop=0;main_loop<120;main_loop++)          ;;;main_loop这名字再花也就是一个变量名等同于A,没有单片机得概念,这种表达编译器可以翻译
    {
        *((far WORD *)(SOFT_ERR_FLAG+main_loop*2))=0;        ;;;切忌这些符号首先是满足PC上的C语言的,其次才有提高的
    }                                                        ;;;一个很简单没有任何意义和嚼头的事情,还想得这么复杂,菜


    initial();                                       ;;;程序再大他也是由一句一句的逻辑串接而成的,没有任何东四是不可想象的
    enable();                                        ;;;开中断
    
    int_pend =0x80;                                 ;;;清中断悬挂   这个应该是立即原件产生中断
    int_pend1=0x60;                                 ;;;这个地方是通过软件产生相应的中断吗
    
    set_announce_rpt();   ;;;设置_发表_报告

    if( ( ((p1_pin & 0x08)==0)&&((p4_pin & 0x04)!=0) )   ;;;((安位与)==0)逻辑与 ((安位与)!=0)
                                                      ||( ((p1_pin & 0x08)!=0)&&((p4_pin & 0x04)==0) ) )
    {
        Dsa208_Work_State=NO;     ;;;刚上电的时候就通过引脚状态来判断系统的工作状态如何
    }
    else
    {
        Dsa208_Work_State=YES;	//-没有也无所谓
    }











	//-总循环开始了,说明运行状态已经建立好了,数据库在更新了
       ;;;现在不要求把整个系统都了然与心,但必须能够做到,一部分一部分的详细注释
       ;;;当上面对程序做好初始化之后,下面就进入了主循环程序段
    while(1)                                    ;;;本程序的目的就是永久循环等待;切忌C语言编写没有对硬件的过多要求,只是有更好,并不影响功能
    {
        if((main_loop % 4)==0)               ;;;求余数
        	p1_reg = p1_reg ^ 0x80;  // for QH                ;;;求和吗,^安位异或
        
        if( ( ((p1_pin & 0x08)==0)&&((p4_pin & 0x04)!=0) )
          ||( ((p1_pin & 0x08)!=0)&&((p4_pin & 0x04)==0) ) )
        {
            Dsa208_Work_State=NO;          ;;;状态不好就复位,即从头来
        }
        else
        {
            if(Dsa208_Work_State==NO)  goto main_start;
        }
        
        
        RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER=0;     ;;;结构体变量名.成员名    这就是对结构体的引用切忌引用的不确定是哪一位而是一个成员可能还是数值成员
                                                      ;;;上面这个,就仅仅起到监视程序执行的功能,其它没有实际意义
        main_loop++;                                  ;;;寄存器加一
        //设置口
        set_main();    ;;;难道这个地方就是一切的设置入口吗,可能在这个里面向后台发送了报表内容
	//-很多东西肯定是定时发送的,那么就肯定是安排在中断内,而不是一般的循环内


        //-难道作为通讯管理机,他没有什么头不头的,有的仅仅是根据标志位的处理
        if(Dsa208_Work_State!=NO)     ;;;这个判断的最终处理过程是完全一样的,只是状态不同,进入的时间不同而已
        {                             ;;;系统正常的处理过程
            CAN0_Main();              ;;;需要有自己解决问题的方法,和能力,这叫自主学习,没有人能够"教"你
            Protocol_Main();	//-这是对每个端口的一一处理
            
            if(main_loop>180)
            {
                main_loop = 0;
        
                p4_reg   =p4_reg ^ 0x01;
      
                CAN0_Monitor();           //-难道这个管理机就是以CAN网通讯为基础的,然后在此基础之上才增加?监控还可以对出了问题的东西进行初始化
                Protocol_Monitor();
                
                if(p2_img != (p2_pin & 0x08))
                {
                    p2_img = p2_pin & 0x08;
                    if((p2_img & 0x08) == 0x00)
                        sp_baud = CPU_SIO_BAUD_TO_LED;
                    else
                        sp_baud = CPU_SIO_BAUD_TO_PC;
                }
            
                judge_host_zf_enable();
                byq_dw_yx_translate_to_yc();	//-这个过程作为一个特殊处理,我现在不需要去管他,这个仅仅是单独功能之一
                yx_or_calculate();	//-同样目前可以不处理
                judge_bh_to_yx_delay_time();
            }
        }
        else      ;;;系统状态不正常的处理过程
        {
            if(main_loop>1200)           ;;;通过这个决定处理过程,也就是等于应用了这个信号延时有效,并不是一出现就有效的
            {
                main_loop = 0;
        
                p4_reg   =p4_reg ^ 0x01;
      
                if(p2_img != (p2_pin & 0x08))
                {
                    p2_img = p2_pin & 0x08;
                    if((p2_img & 0x08) == 0x00)
                        sp_baud = CPU_SIO_BAUD_TO_LED;	//-通过指示灯显耀的频率判断运行状态就在这里了
                    else
                        sp_baud = CPU_SIO_BAUD_TO_PC;
                }
            
                judge_host_zf_enable();           ;;;主动判断给标志位
                byq_dw_yx_translate_to_yc();
                yx_or_calculate();
                judge_bh_to_yx_delay_time();
            }
        }
        
        


        
        
        // use port_no
        Judge_P554_CAN_Reset();
        judge_mainloop_int_dead();          //-应该是类看门狗程序,判断中断是否停止,相当于自检并上送情况
    }
}

;;;任何一个大的系统也就是由"心 眼 手 耳 等"组成,值不多具体内容具体处理等中断是最有效的方法
;;;其实系统就是这么简单,只是面对不同的情况进行不同的处理.