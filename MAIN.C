/*****************************************************************************/
/*       FileName  :   MAIN.C                                                */
/*       Content   :   DSA-208 MAIN Module                                   */
/*       Date      :   Fri  02-22-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/

//-��Ҫ�������Ķ�����,��ʵҲ����һ��һ������ֻ�������߼�������в�ͬ,���ǿ϶�����һ��
//-��Ҫ��סһ��,�������һ����Ƭ����ɵ�Ƕ��ʽϵͳ,������DSP


#pragma  regconserve                  ;;;ʲô��˼,��Ҫ���ܹ�����������   �Ĵ��� ����

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
extern void set_main();           ;;;���߱���������������������
extern void set_init();
extern void set_announce_rpt();

// Protocols
extern void Protocol_Main(void);
extern void Protocol_Init(void);          ;;;����ط���Ԥ���������ʾ������������õı�ĵط���
extern void Protocol_Monitor(void);

//Exchange
extern void Ex_init();         ;;;˵���ڱ���ⲿ��һ������

 





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
static void judge_mainloop_int_dead(void)     ;;;�ж���ѭ���ж��Ƿ���
{
    BYTE  ram_axl;
    RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER=0;

    // EPA1  soft_time_1ms
    RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER++;
    if(RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER>50000)        ;;;�ж�������50S
    {
        disable();       ;;;���ж�
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_EPA1_INT;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();      ;;;�Լ���������ҲҪ������,Ҫ�� ,��Ҫ����֪��
        enable();
        
        int_pend1=int_pend1 | 0x01;            ;;;������֮�������ٴ���������ж�
        RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER=0;
        RAM_CPU_INT_Rst_Cn[5]++;                 ;;;��¼��λ������
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
        if(ram_axl!=0xff)   RAM_CPU_Int_Moni.EPA0_INT_DEAD_COUNTER++;    ;;;������
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
static void judge_host_zf_enable(void)   //-��Լ����,�涨 ���ֽ�>80H�Ĺ�ԼΪ���й�Լ  <80H Ϊ����
{                                                                       ;;;���й�Լ��ָ�������̨ͨ�� �����й�����ָ�����뱣��װ��ͨ����
    BYTE  ram_axl;
    BYTE  ram_axh;
    
    ram_axh=HOST_ZF_enable;
    for(ram_axl=0;ram_axl<14;ram_axl++)
    {
        if((port_info[ram_axl].protocal_type!=PROTOCOL_NONE)&&(byte1(port_info[ram_axl].protocal_type)<0x80)) //-�й�Լ�ҹ�Լ�ŵø��ֽ�С��80H
        {
            if(Portx_Poll_First[ram_axl]!=YES)
                break;        //-���������ò�ѭ��,����ִ��ѭ��֮������
        }
        
    }
    if(ram_axl>=14)                  
        HOST_ZF_enable=YES;    ;;;����������ж˿ڹ�Լ,��û�г���С��80H�ľ͸�YES,��ʾֻ�ܺͺ�̨ͨ����
    else
        HOST_ZF_enable=NO;     ;;;������С�ڵĹ�Լ���ھ͸����ֵ��ʾ�Է�ʹ������λ������������������
    
    if((ram_axh!=YES)&&(HOST_ZF_enable==YES))    //-ԭ��������YES���ڳ�����
    {
        for(ram_axl=0;ram_axl<4;ram_axl++)
        {
            temp_ptD = (unsigned short*)&YX_transmite_table[ram_axl][0];    ;;;����ط�������һ����ά����
            temp_int = 0;
            //while(*temp_ptD < 2048)
            while(*temp_ptD < 4096)           ;;;�ж��׸���Ԫ������С��4096
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
                if(temp_int>1023)     ;;;����1024��
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
static void byq_dw_yx_translate_to_yc(void)       ;;;��ѹ����λ
{
    BYTE  ram_axl;
    BYTE  ram_axh;
    WORD  ram_bx;
    
    for(ram_axl=0;ram_axl<8;ram_axl++)
    {
        if(Byq_Dw[ram_axl].yc_val_no<=(sizeof(YC_State)/sizeof(YC_State[0])))	//-�����е������в��ҵ�����������,������
        {
            ram_bx=0;
            for(ram_axh=0;ram_axh<32;ram_axh++)
            {
                if( (YX_State[(Byq_Dw[ram_axl].yx_no[ram_axh])/16] & (1<<(Byq_Dw[ram_axl].yx_no[ram_axh] & 0x000f)))!=0)
                    ram_bx=ram_bx+Byq_Dw[ram_axl].yx_weight[ram_axh];        ;;;�ۼӺ�
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
    
    for(ram_axl=0;ram_axl<(sizeof(YX_OR_Table)/sizeof(YX_OR_Table[0]));ram_axl++)        ;;;��64�Ƚ�
    {
        if(YX_OR_Table[ram_axl][0]<0x1000)                  ;;;ȡң�ű��е���ֵ�����Ƿ���"����"
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
        ram_ax=BH_Event_To_SOE[ram_bx].YX_State_No;       ;;;�����¼�תң��
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
               Core_Det_Pt++;                  ;;;����ط��Ƕ�ָ�����ݼ�1,����������һ����Ԫ
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
                yx_soe_in_pt = 0;               ;;;����һ����������,��������������Ĺ�Ч����
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
//-�����¼�תң��
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void initial_bh_to_yx(void)
{
    WORD  ram_ax;
    WORD  ram_bx;
    
    for(ram_bx=0;ram_bx<512;ram_bx++)
    {
        ram_ax=BH_Event_To_SOE[ram_bx].YX_State_No;	//-�����ֵҲ���������úõ�
        if(ram_ax<4096)
        {
            YX_State[ram_ax/16]=YX_State[ram_ax/16] & (0xffff - (1<<(ram_ax%16)));	//-��֪��Ϊʲô��������
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
    epa1_time=timer1+EPA1_PERIOD;         ;;;����ط���ȷ����,EPA1����ÿ1MS����һ���ж�,������жϲ���Ҫ������,�������Զ���������,��������������Խ��,��ʱ�仹��Ҫ��ÿ���жϺ��ۼ�
    
    epa0_con=0x10;       ;;;��ʱ��1Ϊ��׼ ��׽��ʽ  ���½��ز�׽
    epa2_con=0x00;       ;;;                        �޲�׽ 
    epa3_con=0x0000;
    

    REG_TimeTemp     =0;
    REG_Surplus_Time =0;


    Time_1ms_Counter   =0;
    Time_2048ms_Counter=0;         ;;;������һ����ʱ�Ĵ�����


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
    RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER  =0;             ;;;��Ҫ��ʱ����λ����,���������Ļ�һ��������ķ��򳬹�50Sû���� ����Ϊ�����ܷ�
    
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
    Initial_CAN(1);               ;;;�Զ˿ڵĳ�ʼ��

    init_all_port();

    ram_base_addr_w=(far WORD *)(&YX_State);     ;;;ǿ������ת��  ȡ��ַ ���ѵ�ַת��֮�󸳸�ָ��
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(YX_State)/sizeof(YX_State[0]);
        ram_bx++)
    {
        ram_base_addr_w[ram_bx]=0;
    }

    ram_base_addr_w=(far WORD *)(&YC_State);	//-��Щ�ռ���ϵͳ�Զ������,far������ָ����һ����ķ�Χ
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(YC_State)/sizeof(YC_State[0]);	//-��ʵ�������涼��������û�б�Ҫ��ô"����",����Ϊ�˼�С����Ի�����ô����
        ram_bx++)
    {
        ram_base_addr_w[ram_bx]=0;	//-��������ʱ������ݿ�����
    }

    ram_base_addr_w=(far WORD *)(&YM_State);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(YM_State)/sizeof(YM_State[0]);
        ram_bx++)
    {
        ram_base_addr_w[ram_bx*2+0x00]=0;
        ram_base_addr_w[ram_bx*2+0x01]=0;
    }

    ram_base_addr_w=(far WORD *)(&Cn_SIO_CAN_Reset);	//-���нӿ�
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
        sp_baud = CPU_SIO_BAUD_TO_LED;	//-ͨ��������ŵ�״̬������ʹ�õĲ�����,�ֲֹ��ÿ���ͨ���Ƶ������������ж������Ƿ�����


    CAN0_Init();
    Protocol_Init();
    
    for(port_no=0;port_no<12;port_no++)
    {
        P554_Port_Transing_Lastbyte[port_no]=NO;	//-��ʼ״̬Ϊ0
    }    

    for(port_no=0;port_no<16;port_no++)
        port_mirror[port_no] = 0x0f;

//    if need monitor any PORT info , initial 'port_mirror' here
//    port_mirror[11]=0x3a;
    port_mirror[14]=0x3a;

    for(main_loop=0;main_loop<256;main_loop++)
        unit_set_flag[main_loop] = 0x55;	//-��ʼΪ55��Ŀ��

    for(port_no=0;port_no<14;port_no++)
        Portx_Poll_First[port_no] = NO;
    
    initial_bh_to_yx();
    YX_FG_FLAG=NO;
    HOST_ZF_enable=NO;
    HOST_YK_Doing =0;
    
//.............................. initial INTs ...............................//
    int_mask =0xf8;  // bit 7-0: EPA0  , SIORec , SIOTran, Exint1,         ;;;û����ô�������õĿ϶������޵�
    int_pend =0x00;  //          Exint0, Reserve, T2_Ovr , T1_Ovr.

    int_mask1=0x61;  // bit 7-0: NMI      , Exint3 , Exint2 , EPAOvr2_3,
    int_pend1=0x00;  //          EPAOvr1_0, EPA3   , EPA2   , EPA1     .
}  // initial()














































;;;C��������Ҫ��C���������ɻ�����
;;;��������ʼ��
;;;��������������ԭ�������������˺ܶຯ��������


;;;���ڶ���Щ������仹ȱ���Ķ�����

/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/


                 ;;;����ʼ������ط�,��ʱ��Ϊ�߼����õ�,���ǿ���ͨ���������õ�Ƭ��֪����Щ,������ִ��
;;;������򻹸������Ҷ�Ҫ��ɵĹ��ܻ������,����ӵ�е���Դ�������,Ҫ������Ҳ����߲��ܽ�һ����ʶ����

/*===========================================================================*/
void main(void)
{
main_start:                        //-���
    disable();                                       ;;;���ж�,������ڲ��⺯�������㲻Ҫ��,ֻҪ���þ�����
    p4_reg = 0xfc;     // let CS0->FLASH          ,,Ƭѡ�� �ǵľ���Ƭѡ,���ź�ѡ��оƬ

    //prepare_status!!!!
    buscon0=0xc1;      // 0xc1
    buscon1=0xc1;      // f80000 fbffff
    buscon2=0xc0;      // RAM         
    sp  = 0xff00;
	//-���������Ҳ�û��������Ӳ��ԭ��ͼ,������Щ�������ڵĲ�β�����Ҫȥ�,������Ҫ�ľ��Ǵ������������ͨѶ

    for(main_loop=0;main_loop<120;main_loop++)          ;;;main_loop�������ٻ�Ҳ����һ����������ͬ��A,û�е�Ƭ���ø���,���ֱ�����������Է���
    {
        *((far WORD *)(SOFT_ERR_FLAG+main_loop*2))=0;        ;;;�м���Щ��������������PC�ϵ�C���Ե�,��β�����ߵ�
    }                                                        ;;;һ���ܼ�û���κ�����ͽ�ͷ������,�������ô����,��


    initial();                                       ;;;�����ٴ���Ҳ����һ��һ����߼����Ӷ��ɵ�,û���κζ����ǲ��������
    enable();                                        ;;;���ж�
    
    int_pend =0x80;                                 ;;;���ж�����   ���Ӧ��������ԭ�������ж�
    int_pend1=0x60;                                 ;;;����ط���ͨ������������Ӧ���ж���
    
    set_announce_rpt();   ;;;����_����_����

    if( ( ((p1_pin & 0x08)==0)&&((p4_pin & 0x04)!=0) )   ;;;((��λ��)==0)�߼��� ((��λ��)!=0)
                                                      ||( ((p1_pin & 0x08)!=0)&&((p4_pin & 0x04)==0) ) )
    {
        Dsa208_Work_State=NO;     ;;;���ϵ��ʱ���ͨ������״̬���ж�ϵͳ�Ĺ���״̬���
    }
    else
    {
        Dsa208_Work_State=YES;	//-û��Ҳ����ν
    }











	//-��ѭ����ʼ��,˵������״̬�Ѿ���������,���ݿ��ڸ�����
       ;;;���ڲ�Ҫ�������ϵͳ����Ȼ����,�������ܹ�����,һ����һ���ֵ���ϸע��
       ;;;������Գ������ó�ʼ��֮��,����ͽ�������ѭ�������
    while(1)                                    ;;;�������Ŀ�ľ�������ѭ���ȴ�;�м�C���Ա�дû�ж�Ӳ���Ĺ���Ҫ��,ֻ���и���,����Ӱ�칦��
    {
        if((main_loop % 4)==0)               ;;;������
        	p1_reg = p1_reg ^ 0x80;  // for QH                ;;;�����,^��λ���
        
        if( ( ((p1_pin & 0x08)==0)&&((p4_pin & 0x04)!=0) )
          ||( ((p1_pin & 0x08)!=0)&&((p4_pin & 0x04)==0) ) )
        {
            Dsa208_Work_State=NO;          ;;;״̬���þ͸�λ,����ͷ��
        }
        else
        {
            if(Dsa208_Work_State==NO)  goto main_start;
        }
        
        
        RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER=0;     ;;;�ṹ�������.��Ա��    ����ǶԽṹ��������м����õĲ�ȷ������һλ����һ����Ա���ܻ�����ֵ��Ա
                                                      ;;;�������,�ͽ����𵽼��ӳ���ִ�еĹ���,����û��ʵ������
        main_loop++;                                  ;;;�Ĵ�����һ
        //���ÿ�
        set_main();    ;;;�ѵ�����ط�����һ�е����������,����������������̨�����˱�������
	//-�ܶණ���϶��Ƕ�ʱ���͵�,��ô�Ϳ϶��ǰ������ж���,������һ���ѭ����


        //-�ѵ���ΪͨѶ������,��û��ʲôͷ��ͷ��,�еĽ����Ǹ��ݱ�־λ�Ĵ���
        if(Dsa208_Work_State!=NO)     ;;;����жϵ����մ�����������ȫһ����,ֻ��״̬��ͬ,�����ʱ�䲻ͬ����
        {                             ;;;ϵͳ�����Ĵ�������
            CAN0_Main();              ;;;��Ҫ���Լ��������ķ���,������,�������ѧϰ,û�����ܹ�"��"��
            Protocol_Main();	//-���Ƕ�ÿ���˿ڵ�һһ����
            
            if(main_loop>180)
            {
                main_loop = 0;
        
                p4_reg   =p4_reg ^ 0x01;
      
                CAN0_Monitor();           //-�ѵ����������������CAN��ͨѶΪ������,Ȼ���ڴ˻���֮�ϲ�����?��ػ����ԶԳ�������Ķ������г�ʼ��
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
                byq_dw_yx_translate_to_yc();	//-���������Ϊһ�����⴦��,�����ڲ���Ҫȥ����,��������ǵ�������֮һ
                yx_or_calculate();	//-ͬ��Ŀǰ���Բ�����
                judge_bh_to_yx_delay_time();
            }
        }
        else      ;;;ϵͳ״̬�������Ĵ�������
        {
            if(main_loop>1200)           ;;;ͨ�����������������,Ҳ���ǵ���Ӧ��������ź���ʱ��Ч,������һ���־���Ч��
            {
                main_loop = 0;
        
                p4_reg   =p4_reg ^ 0x01;
      
                if(p2_img != (p2_pin & 0x08))
                {
                    p2_img = p2_pin & 0x08;
                    if((p2_img & 0x08) == 0x00)
                        sp_baud = CPU_SIO_BAUD_TO_LED;	//-ͨ��ָʾ����ҫ��Ƶ���ж�����״̬����������
                    else
                        sp_baud = CPU_SIO_BAUD_TO_PC;
                }
            
                judge_host_zf_enable();           ;;;�����жϸ���־λ
                byq_dw_yx_translate_to_yc();
                yx_or_calculate();
                judge_bh_to_yx_delay_time();
            }
        }
        
        


        
        
        // use port_no
        Judge_P554_CAN_Reset();
        judge_mainloop_int_dead();          //-Ӧ�����࿴�Ź�����,�ж��ж��Ƿ�ֹͣ,�൱���Լ첢�������
    }
}

;;;�κ�һ�����ϵͳҲ������"�� �� �� �� ��"���,ֵ����������ݾ��崦�����ж�������Ч�ķ���
;;;��ʵϵͳ������ô��,ֻ����Բ�ͬ��������в�ͬ�Ĵ���.