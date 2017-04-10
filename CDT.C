/*****************************************************************************/
/*       FileName  :   CDT.C                                                 */
/*       Content   :   DSA-208 CDT Module                                    */
/*       Date      :   Fri  02-22-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/



#pragma  regconserve

#include _SFR_H_
#include _FUNCS_H_

#include "common.h"
#include "device.h"
#include "reg.h"
#include "ram.h"
#include "rom.h"
#include "comuse.h"


//-从现在开始我要像原来学习汇编语言一样,把能够想到的所有情况全部写下来,最后再来一点一点整理和弄会



#define CDT_PORT     port_no

//send
#define cdt_yx_chg_send         port_flag[CDT_PORT][0]		//-这个还是使用的独立空间,所有值还是这个文件中给的,前面仅仅是宏而以
#define cdt_yx_send             port_flag[CDT_PORT][1]
#define cdt_control_code        port_flag[CDT_PORT][2]
    // 0x61:YC  0xf4:YX  0x26:SOE  0xf8:Protect
#define cdt_send_info_turn      port_flag[CDT_PORT][3]
    // 0: EB90   1: control  2: info 
#define cdt_send_order          port_flag[CDT_PORT][4]
#define cdt_send_frame_len      port_flag[CDT_PORT][5]
#define cdt_send_stage          port_flag[CDT_PORT][6]
    // 0: 1: 2: YC           3: YX, SOE, YK, Protect
#define cdt_yk_verify_send      port_flag[CDT_PORT][7]


//receive
#define cdt_rec_syn_flag        port_flag[CDT_PORT][10]
#define cdt_rec_stage           port_flag[CDT_PORT][11]
#define cdt_rec_frame_len       port_flag[CDT_PORT][12]
#define cdt_rec_frame_type      port_flag[CDT_PORT][13]
    // 0x7161: YC,YK    0x71f4: YX  0x7126: SOE   0x7185: DD 
    // 0x71f8: protect   
#define cdt_rec_info_num        port_flag[CDT_PORT][14]
#define cdt_yx_fresh            port_flag[CDT_PORT][15]

//转发表号
#define CDT_TRAN_YC_TABLE_NO    port_flag[CDT_PORT][16]
#define CDT_TRAN_YX_TABLE_NO    port_flag[CDT_PORT][17]
#define CDT_TRAN_YM_TABLE_NO    port_flag[CDT_PORT][18]

#define CDT_RLY_REPORT          port_flag[CDT_PORT][19]
#define CDT_LSA_SELFCHK         port_flag[CDT_PORT][20]
#define CDT_PAGE_NUM            port_flag[CDT_PORT][21]
#define CDT_PAGE_ORDER          port_flag[CDT_PORT][22]
#define CDT_ZZZZ1               port_flag[CDT_PORT][23]
#define CDT_time_tmp_MS         port_flag[CDT_PORT][24]
#define CDT_time_tmp_MSL        byte0(port_flag[CDT_PORT][24])
#define CDT_time_tmp_MSH        byte1(port_flag[CDT_PORT][24])
#define CDT_time_tmp_SEC        byte0(port_flag[CDT_PORT][25])
#define CDT_time_tmp_MIN        byte1(port_flag[CDT_PORT][25])

//#define CDT_bh_zf_msg_len       port_flag[CDT_PORT][26]
//#define CDT_bh_zf_soe_buf       ( (WORD *)&(port_flag[CDT_PORT][32]) )

#define CDT_TRAN_YC_NUM         port_info[CDT_PORT].scan_unit
#define CDT_TRAN_YX_NUM         port_info[CDT_PORT].scan_panel
#define CDT_TRAN_YM_NUM         byte0(port_info[CDT_PORT].rebound)
#define CDT_TRAN_FRAME_IN_1PAGE port_info[CDT_PORT].YK_enable

#define CDT_YC_NORMAL           0x61	//-重要遥测（A帧）  遥控选择
#define CDT_YC_IMPORTANT        0xC2	//-次要遥测（B帧）  遥控执行 
#define CDT_YC_LESSER           0xB3	//-一般遥测（C帧）  遥控撤消 
#define CDT_YX_NORMAL           0xF4	//-遥信状态（D1帧）  升降选择 
#define CDT_YX_EVENT            0x26	//-事件顺序记录（E帧）  升降撤消 
#define CDT_YM_NORMAL           0x85	//-电能脉冲数值（D2帧）  升降执行 
#define CDT_PROTECT_LSA         0xF8	//-以下的这些可能是自定义的
#define CDT_PROTECT_LFP         0xF9

#define CDT_YK_SELECT           0xE2
#define CDT_YK_REPLY            0xE3



void CDT_deal_YX_state(void);


void CDT_BCH_check()
{
  port_check = 0;
  temp_ptD_B = &port_send[CDT_PORT][0];
  for(temp_loop=0;temp_loop<5;temp_loop++)
  {
    temp_loop1 = port_check ^ (*temp_ptD_B);	//-这里其实就是不变?
    port_check = BCH_table[temp_loop1];
    temp_ptD_B++;                 //   set the BCH code   
  }
  port_check   = port_check ^ 0xff;
  *temp_ptD_B  = port_check;

}

//禁用 delta_len(temp_int) temp_ptS temp_ptD
unsigned char CDT_check_BCH()
{
  port_check = 0;
  for(temp_loop=0;temp_loop<5;temp_loop++)
  {
    temp_loop1 = port_check ^ byte0(port_report[temp_loop]);
    port_check = BCH_table[temp_loop1];
  }
  port_check = 0xff ^ port_check;
  if(port_report[5] ==  port_check)
     return 0xaa;
  else
     return 0x55;

}

void CDT_send()		//-数据已经准备好了,我想这儿就是最终数据处理,然后交给设备自动发送
{
  CDT_BCH_check();	//-这里就是计算校验码吗

  port_send_len[CDT_PORT] = 6;	//-难道CDT使用的是双校验码,控制字和信息字都有
  port_send_begin();  	//-最终开始发送
}
//-现在我还没有看清来龙去脉,所以需要痛苦的一点一点来解决,我有基础.
void CDT_send_EB90()
{
  temp_ptD_B = &port_send[CDT_PORT][0];
 
  for(temp_loop=0;temp_loop<6;temp_loop++)
  {	//-这个地方是准备信息内容的一部分
    *temp_ptD_B = SYN_WORD_CDT_EB90[temp_loop];
     temp_ptD_B ++;
  }

  port_send_len[CDT_PORT] = 6;
  port_send_begin();  
  
}


//-组织发送控制字.
//-bit7  当E=0 时使用表2已定义的帧类别；当E=1 时帧类别可另行定义，以便扩展功能。
//-bit6  当L=0时表示本帧信息字数n为0，既本帧没有信息字；当L=1 时表示本帧有信息。
//-bit5  源站址定义位
//-bit4  目的站址定义位 
//-bit3  0
//-bit2  0
//-bit1  0
//-bit0  1
void CDT_send_control_byte()
{
   temp_ptD_B = &port_send[CDT_PORT][0];
  *temp_ptD_B = PROTOCOL_CDTBB_CONTROL_BYTE[CDT_PORT];	//-已定义的类型帧 本帧有信息 
   temp_ptD_B ++;
  *temp_ptD_B = cdt_control_code;	//-帧类型
   temp_ptD_B ++;
  *temp_ptD_B = cdt_send_frame_len;	//-信息字数
   temp_ptD_B ++;
  *temp_ptD_B = PROTOCOL_CDTBB_SOURCE_ADDR[CDT_PORT];	//-源地址
   temp_ptD_B ++;
  *temp_ptD_B = PROTOCOL_CDTBB_DESTIN_ADDR[CDT_PORT];	//-目的地址

   CDT_send();   
}

void CDT_send_YC()
{
  unsigned short temp_short;
  
  temp_ptD_B = &port_send[CDT_PORT][0];
//  if(cdt_send_order<1)
//  {
//    *temp_ptD_B = 0x8a;
//     temp_ptD_B ++;
//     temp_short = YC_State[YC_f_value_selected[0]];
//    *temp_ptD_B = temp_short%10 + (temp_short/10)%10*16;
//     temp_short = temp_short/100;
//     temp_ptD_B ++;
//    *temp_ptD_B = temp_short%10 + (temp_short/10)%10*16;
//     temp_ptD_B ++;
//     temp_short = YC_State[YC_f_value_selected[1]];
//    *temp_ptD_B = temp_short%10 + (temp_short/10)%10*16;
//     temp_short = temp_short/100;
//     temp_ptD_B ++;
//    *temp_ptD_B = temp_short%10 + (temp_short/10)%10*16;
//     CDT_send();   
//     return;
//  }

//  *temp_ptD_B =  cdt_send_order-1;
//   temp_int   = (cdt_send_order-1)*2;
//   temp_ptD_B ++;

  *temp_ptD_B = CDT_PAGE_ORDER*CDT_TRAN_FRAME_IN_1PAGE+cdt_send_order+MON_FUN_NO[CDT_PORT][0];
   temp_int   = ((*temp_ptD_B)-MON_FUN_NO[CDT_PORT][0])*2;
   temp_ptD_B ++;

  for(temp_loop=0;temp_loop<2;temp_loop++)
  {
     if(CDT_TRAN_YC_TABLE_NO<4) 
         temp_lp_int = YC_transmite_table[CDT_TRAN_YC_TABLE_NO][temp_int];
     else
         temp_lp_int = temp_int;

     temp_short = YC_State[temp_lp_int];
     if(temp_short>0x7fff) 
     {
        temp_short = 0x10000 - temp_short;
        temp_short = 0x1000  - temp_short;
        temp_short&= 0x0fff;
     }
     else if(temp_short>0x0fff)
          {
              if(temp_short<5000)
                  temp_short = 0x1000 - (5000 - temp_short);
              else
              {
                  temp_short = temp_short - 5000;
                  temp_short&= 0x0fff;
              }    
          }
    if(CDT_TRAN_YC_TABLE_NO<4)
     {
      for(temp_loop1=0;temp_loop1<32;temp_loop1++)
       {
       	if(temp_lp_int==yc_offset[CDT_TRAN_YC_TABLE_NO][temp_loop1].yc_no)
       	 {
       	  temp_short-=yc_offset[CDT_TRAN_YC_TABLE_NO][temp_loop1].offset;
       	  break;
       	 }
       }
     }
    else
     {
      for(temp_loop1=0;temp_loop1<32;temp_loop1++)
       {
       	if(temp_lp_int==yc_offset[0][temp_loop1].yc_no)
       	 {
       	  temp_short-=yc_offset[0][temp_loop1].offset;
       	  break;
       	 }
       }      
     }
    *temp_ptD_B  = byte0(temp_short);
     temp_ptD_B  ++;
    *temp_ptD_B  = byte1(temp_short);
     temp_ptD_B  ++;
     temp_int    ++;
  }  

  CDT_send();   
}
//-靠,这个规约也太可靠了吧,每6个字节就加一个校验
void CDT_send_YX()	//-这个地方的发送肯定是向后台发送的,因为没有哪个下位机需要这个东西,现在的问题是是什么启动了这样的发送
{
   temp_ptD_B = &port_send[CDT_PORT][0];
  *temp_ptD_B = cdt_send_order + 0xf0+MON_FUN_NO[CDT_PORT][1];	//-这个是组织功能码的,YX从F0H~FFH,一帧发送16个信息字吗
   temp_ptD_B ++;
   temp_int = cdt_send_order*2;	//-由于一次发送都是以4个字节为单位的,而暂存区又是以字为单位的,所以需要*2
   
   if(CDT_TRAN_YX_TABLE_NO<4)
      temp_lp_int = YX_state_tr[CDT_TRAN_YX_TABLE_NO][temp_int];
   else
      temp_lp_int = YX_State[temp_int];
  *temp_ptD_B =byte0(temp_lp_int);
   temp_ptD_B ++;
  *temp_ptD_B =byte1(temp_lp_int);
   temp_ptD_B ++;
   temp_int ++;

   if(CDT_TRAN_YX_TABLE_NO<4)
      temp_lp_int = YX_state_tr[CDT_TRAN_YX_TABLE_NO][temp_int];
   else
      temp_lp_int = YX_State[temp_int];
  *temp_ptD_B =byte0(temp_lp_int);
   temp_ptD_B ++;
  *temp_ptD_B =byte1(temp_lp_int);
	//-上面准备了一个字的信息数据,下面就可以发送了,在正式进入发送流程前还需要一个校验码
  CDT_send();   
}

void CDT_send_YM()
{
   temp_ptD_B = &port_send[CDT_PORT][0];
   temp_int   = CDT_PAGE_ORDER*CDT_TRAN_FRAME_IN_1PAGE+cdt_send_order;
  *temp_ptD_B = temp_int+0xa0+MON_FUN_NO[CDT_PORT][3];	//-A0H~DFH
   temp_ptD_B ++;
   if(CDT_TRAN_YM_TABLE_NO<4)
      temp_ptL = (near UL *)&YM_State[YM_transmite_table[CDT_TRAN_YM_TABLE_NO][temp_int]]; 	//-这些要理解可能还有很多知识,不是这个层次的事情,先放放
   else
      temp_ptL = (near UL *)&YM_State[temp_int];    
  *temp_ptD_B =byte0(*temp_ptL);
   temp_ptD_B ++;
  *temp_ptD_B =byte1(*temp_ptL);
   temp_ptD_B ++;
  *temp_ptD_B =byte2(*temp_ptL);
   temp_ptD_B ++;
  *temp_ptD_B =byte3(*temp_ptL);

  if( (CDT_PAGE_ORDER+1)>=CDT_PAGE_NUM )
      cdt_yx_send = 0;
  CDT_send();   
}

void CDT_send_YK_verify_from_transmit()
{
    BYTE ram_axl;
    
    port_send[CDT_PORT][0]=0xe1;
    temp_ptS_B = &port_transmit[CDT_PORT][16];
    if(*temp_ptS_B==CORE_CODE2_YK_CLOSE) 
        port_send[CDT_PORT][1]=0xcc;
    else if(*temp_ptS_B==CORE_CODE2_YK_TRIP)
        port_send[CDT_PORT][1]=0x33;
    else
        port_send[CDT_PORT][1]=0xff;
    port_send[CDT_PORT][2]=(*(temp_ptS_B+1))+MON_FUN_NO[CDT_PORT][2];
    port_send[CDT_PORT][3]=port_send[CDT_PORT][1];
    port_send[CDT_PORT][4]=(*(temp_ptS_B+1))+MON_FUN_NO[CDT_PORT][2];

    CDT_send();   
}

void CDT_send_YX_chg()
{
   if(CDT_TRAN_YM_TABLE_NO < 4)
     temp_ptS = &yx_change_tr[CDT_TRAN_YX_TABLE_NO][yx_chg_out_pt[CDT_PORT]].offset_no;
   else
     temp_ptS = &yx_change[yx_chg_out_pt[CDT_PORT]].offset_no;    
   temp_ptD_B = &port_send[CDT_PORT][0];
  *temp_ptD_B = byte0(*temp_ptS) + 0xf0+MON_FUN_NO[CDT_PORT][1];
   temp_ptD_B ++;
   temp_ptS ++;
  *temp_ptD_B = byte0(*temp_ptS);
   temp_ptD_B ++;
  *temp_ptD_B = byte1(*temp_ptS);
   temp_ptS ++;
   temp_ptD_B ++;
  *temp_ptD_B = byte0(*temp_ptS);
   temp_ptD_B ++;
  *temp_ptD_B = byte1(*temp_ptS);

   CDT_send();   
}

//-功能码成对出现前者用80H、后者用81H
//-每对信息字在同一帧内连续发送3遍
void CDT_send_YX_SOE()	//-作为通讯管理机本身并没有实际采样能力,而且下位机也不需要MON发送这样的信息,所以肯定是向后台发送的
{

   if(CDT_TRAN_YX_TABLE_NO < 4)	//-必须小于4,就是这样一个二维数组
     temp_ptS = &yx_event_tr[CDT_TRAN_YX_TABLE_NO][yx_soe_out_pt[CDT_PORT]].soe_ms;	//-这个里面记录的数值来自数据库,等待发送给上位机
   else
     temp_ptS = &yx_event[yx_soe_out_pt[CDT_PORT]].soe_ms;
   temp_ptD_B = &port_send[CDT_PORT][0];	//-需要发送的数据还是保存在这个空间内
   //-难道由于CDT规定信息和数据只能是4字节,如果有的话,所以需要作为两次发送
   if((cdt_send_order&1)==0)
   {	//-这个应该是奇偶的判断,那是为什么需要这样呢
     *temp_ptD_B = 0x80;
      temp_ptD_B ++;
     *temp_ptD_B = byte0(*temp_ptS);
      temp_ptD_B ++;
     *temp_ptD_B = byte1(*temp_ptS);	//-毫秒是用16位表示的
      temp_ptD_B ++;
      temp_ptS ++;
     *temp_ptD_B = byte0(*temp_ptS);	//-秒和分各用8位
      temp_ptD_B ++;
     *temp_ptD_B = byte1(*temp_ptS);	
   }
   else
   {
      temp_ptS +=2;
     *temp_ptD_B = 0x81;
      temp_ptD_B ++;
     *temp_ptD_B = byte0(*temp_ptS);
      temp_ptD_B ++;
     *temp_ptD_B = byte1(*temp_ptS);	//-难道表示时和天
      temp_ptD_B ++;
      temp_ptS ++;
     *temp_ptD_B = byte0((*temp_ptS)+MON_FUN_NO[CDT_PORT][1]*32);
      temp_ptD_B ++;
     *temp_ptD_B = byte1((*temp_ptS)+MON_FUN_NO[CDT_PORT][1]*32);    //-SOE对象号和性质（合/分） 
   }

   if(cdt_send_order>=5)	//-有些东西,而且是很多都是没有理由的,大家就是这么规定的
   {
      yx_soe_out_pt[CDT_PORT] ++;	//-SOE帧已发送出去的帧数,,这个地方实现了连续发送3遍才算成功
      yx_soe_out_pt[CDT_PORT] &= 0x3ff;
   }         
   CDT_send();    
}




void CDT_send_bh_bank()
{
  unsigned char *temp_ptsx;
  
  temp_ptD_B = &port_send[CDT_PORT][0];
 *temp_ptD_B =  cdt_send_order;
  temp_ptD_B ++;

  temp_ptsx = &BH_Bank_Report[cdt_send_order*4];

  for(temp_loop=0;temp_loop<4;temp_loop++)
  {
     *temp_ptD_B    = *temp_ptsx;
      temp_ptsx     ++;
      temp_ptD_B    ++;
  }  

  CDT_send();   
}

void CDT_send_bh_report()
{
  temp_ptD_B = &port_send[CDT_PORT][0];
 *temp_ptD_B =  cdt_send_order;
  temp_ptD_B ++;

  temp_ptS_B  = &port_transmit[CDT_PORT][16];
  temp_ptS_B += cdt_send_order*4;
  for(temp_loop=0;temp_loop<4;temp_loop++)
  {
     *temp_ptD_B    = *temp_ptS_B;
      temp_ptS_B    ++;
      temp_ptD_B    ++;
  }  

  if(cdt_send_order >= (cdt_send_frame_len-1))
  {
     port_transmit_flag[CDT_PORT] = 0x55;
     CDT_RLY_REPORT = 0x00;
  }

  CDT_send();   
}

//-本规约采用可变帧长度、多种帧类别循环传送、变位遥信优先传送，               
//-重要遥测量更新循环时间较短，区分循环量、随机量和插入量采用不同形式传送信
//-息，以满足电网调度安全监控系统对远动信息的实时性和可靠性的要求
BYTE CDT_insert_frame_proc()
{
    WORD ram_bx;

    // avoid invalid info STOP UP the transmit channal
    if(port_transmit_flag[CDT_PORT] == 0xaa)	//-难道aa表示接受到新内容,55表示正在处理吗
    {
        port_transmit_flag[CDT_PORT] = 0x55;
        if(port_transmit[CDT_PORT][8]==CORE_CODE_YK_VERIFY) 
        	port_transmit_flag[CDT_PORT]=0xaa;
        else
        {
            if( (port_transmit[CDT_PORT][2]==(PROTOCOL_LSA%0x100))
              &&(port_transmit[CDT_PORT][3]==(PROTOCOL_LSA/0x100)) )
            {  
                port_transmit_flag[CDT_PORT] = 0xaa;
            }    
            if( (port_transmit[CDT_PORT][2]==(PROTOCOL_DFP201%0x100))
              &&(port_transmit[CDT_PORT][3]==(PROTOCOL_DFP201/0x100)) )
            {
                port_transmit_flag[CDT_PORT] = 0xaa;
            }    
        }    
    }

    if((BH_Report_Bank_Tak_Ptr[CDT_PORT] != BH_Report_Bank_Sav_Ptr)&&(port_info[CDT_PORT].protect_enable==0x00aa))
    {
        Core_Src_Unit      = BH_Report_Bank_Tak_Ptr[CDT_PORT];
        core_get_bh_bank_report();
        BH_Report_Bank_Tak_Ptr[CDT_PORT]=Core_Src_Unit;
        if((BH_Bank_Report[510]==(PROTOCOL_LSA%0x100))
         &&(BH_Bank_Report[511]==(PROTOCOL_LSA/0x100)))
            cdt_control_code=CDT_PROTECT_LSA;
        else    
            cdt_control_code=CDT_PROTECT_LFP;
                       
        cdt_send_frame_len = (Core_Src_Len+3)/4;
    }
    else 
    {
        if((port_transmit_flag[CDT_PORT] == 0xaa)&&(port_transmit[CDT_PORT][8]!=CORE_CODE_YK_VERIFY))
        {
            if( ((port_transmit[CDT_PORT][2] == (PROTOCOL_LSA%0x100))
               &&(port_transmit[CDT_PORT][3] == (PROTOCOL_LSA/0x100)))  ||
                ((port_transmit[CDT_PORT][2] == (PROTOCOL_DSA301%0x100))
               &&(port_transmit[CDT_PORT][3] == (PROTOCOL_DSA301/0x100)))  )
            {  
                cdt_control_code   = CDT_PROTECT_LSA;
            }       
            else
            {
                if( (port_transmit[CDT_PORT][2] == (PROTOCOL_DFP201%0x100))
                  &&(port_transmit[CDT_PORT][3] == (PROTOCOL_DFP201/0x100)) )
                {  
                    cdt_control_code   = CDT_PROTECT_LFP;
                }    
            }       
            CDT_RLY_REPORT     = 0x01;
            byte0(ram_bx)      = port_transmit[CDT_PORT][9];
            byte1(ram_bx)      = port_transmit[CDT_PORT][10];
            cdt_send_frame_len = (ram_bx+3)/4;         
        }
        else
        {
            if(yx_soe_ex_pt[CDT_PORT] != yx_soe_out_pt[CDT_PORT])	//-难道是有更新之后,判断到有新值所以自动发送新值
            {	//-是的刚本通道检查到数据库有更新之后就自动向上位机即后台发送更新报文,知道结束
                cdt_control_code   = CDT_YX_EVENT;	//-猜的不错的话,这里应该是处理的一些信息之后,才下达了这样的命令
                if(CDT_TRAN_YX_TABLE_NO<4)
                {
                    cdt_send_frame_len = 6;
                }
                else
                {	//-下面这些都是不正常的处理,为通信过程中的错误加的
                    if(((yx_event[yx_soe_out_pt[CDT_PORT]].soe_chn_sta)&0x0fff)<(CDT_TRAN_YX_NUM*16))
                    {
                        cdt_send_frame_len = 6;
                    }
                    else
                    {
                        cdt_send_frame_len = 0;
                        yx_soe_out_pt[CDT_PORT] ++;		//-每发出一个就加1
                        yx_soe_out_pt[CDT_PORT] &= 0x3ff;
                    }
                }    
            }
            else
            {
            	return NO;
            }
        }
    }
    return YES;        
}   

void CDT_assemble_send_rpt()	//-CDT_组合_发送_报告
{
  BYTE  ram_axl;
    
  if((port_send_pt[CDT_PORT]<=port_send_len[CDT_PORT]) && (port_send_len[CDT_PORT]!=0))	//-本口需要发送的字节数
    return;	//-表示进入了发送流程且还没有发送完成就不进行以下判断

  if(cdt_send_order>=cdt_send_frame_len)	//-实际发送信息字>=设定的信息字个数就从新组织帧的发送
  {
    if((CDT_PAGE_ORDER+2)>CDT_PAGE_NUM)
    {
        cdt_send_stage++;
        if(cdt_send_stage>30)
            cdt_send_stage = 0;
        CDT_PAGE_NUM   = 0;
        CDT_PAGE_ORDER = 0;
    }
    else
    {
        CDT_PAGE_ORDER ++;
    }

    cdt_send_info_turn = 0;
    cdt_send_order     = 0;
    cdt_send_frame_len = 1;
  }
  //-难道是把CDT的三部分分开处理发送的,0表示同步头1表示控制字2表示信息字吗???
  switch(cdt_send_info_turn)	//-CDT_发送_标志_转换
  {
    case 0: //syn
       cdt_yx_chg_send = 0;
       cdt_yk_verify_send = 0;
       cdt_send_info_turn++;
       CDT_send_EB90();
       break;
    case 1: //control
       cdt_send_info_turn++;
       if(CDT_insert_frame_proc()!=YES)		//-返回YES说明正常 ,,这个的处理就说明了有些发送是有优先级的
       {
           if((cdt_send_stage%3)!=0)
           {
               if(CDT_PAGE_NUM==0)
               {
                   cdt_control_code = CDT_YC_NORMAL;	//-表示帧的类型
                   if((CDT_TRAN_FRAME_IN_1PAGE==0)||(CDT_TRAN_YC_NUM<=(CDT_TRAN_FRAME_IN_1PAGE*2)))
                       CDT_PAGE_NUM=1;
                   else    
                       CDT_PAGE_NUM=(CDT_TRAN_YC_NUM+CDT_TRAN_FRAME_IN_1PAGE*2-1)/(CDT_TRAN_FRAME_IN_1PAGE*2);
                   CDT_PAGE_ORDER = 0;
                   if(CDT_PAGE_NUM>1)
                       cdt_send_frame_len = CDT_TRAN_FRAME_IN_1PAGE;
                   else
                       cdt_send_frame_len = (CDT_TRAN_YC_NUM+1)/2;
               }
               else    
               {
                   cdt_control_code = CDT_YC_NORMAL;
                   if((CDT_PAGE_ORDER+1)<CDT_PAGE_NUM)
                       cdt_send_frame_len = CDT_TRAN_FRAME_IN_1PAGE;
                   else
                       cdt_send_frame_len = (CDT_TRAN_YC_NUM-CDT_PAGE_ORDER*CDT_TRAN_FRAME_IN_1PAGE*2+1)/2;
               }    
           }
           else 
           {
               if((cdt_yx_send>2)&&(CDT_TRAN_YM_NUM !=0))
               {
                   if(CDT_PAGE_NUM==0)
                   {
                       cdt_control_code   = CDT_YM_NORMAL;
                       if((CDT_TRAN_FRAME_IN_1PAGE==0)||(CDT_TRAN_YM_NUM<=CDT_TRAN_FRAME_IN_1PAGE))
                           CDT_PAGE_NUM=1;
                       else    
                           CDT_PAGE_NUM=(CDT_TRAN_YM_NUM+CDT_TRAN_FRAME_IN_1PAGE-1)/CDT_TRAN_FRAME_IN_1PAGE;

                       CDT_PAGE_ORDER = 0;
                       if(CDT_PAGE_NUM>1)
                           cdt_send_frame_len = CDT_TRAN_FRAME_IN_1PAGE;
                       else
                       {
                           cdt_send_frame_len = CDT_TRAN_YM_NUM;
                       }    
                   }
                   else
                   {
                       cdt_control_code   = CDT_YM_NORMAL;
                       if((CDT_PAGE_ORDER+1)<CDT_PAGE_NUM)
                           cdt_send_frame_len = CDT_TRAN_FRAME_IN_1PAGE;
                       else
                           cdt_send_frame_len = CDT_TRAN_YM_NUM-CDT_PAGE_ORDER*CDT_TRAN_FRAME_IN_1PAGE;
                   }
               }
               else
               {
                   cdt_yx_send ++;
                   cdt_control_code   = CDT_YX_NORMAL;
                   if(yx_chg_ex_pt[CDT_PORT] != yx_chg_out_pt[CDT_PORT])
                       cdt_send_frame_len = 0;
                   else    
                       cdt_send_frame_len = (CDT_TRAN_YX_NUM+1)/2;
               }
           }
       }
       if(HOST_ZF_enable!=YES)	//-这个地方我猜是,作废的意思,也就是说刚才发的同步头没有用了,从新再来
          cdt_send_frame_len = 0;
       
       CDT_send_control_byte();		//-果真为了可靠,每一部分都有自己的校验码

       break;
    case 2:  //-信息字	,,难道信息字可以不只一个以6个为单位,可以连续发送信息字吗? 是的信息字的长度是可变的,每个信息字包含6个字节内容
       switch(cdt_control_code)		//-上行,问题是是哪确定了这个值,确定这个值的机制
       {
          case CDT_YC_NORMAL:	//-这个地方的判断就是接收到的内容,但是他作为中间单元可能接收到两方面的内容
               ram_axl=0;
               
               if(  (yx_chg_ex_pt[CDT_PORT] != yx_chg_out_pt[CDT_PORT])
                 &&((cdt_send_order+3-cdt_yx_chg_send)<cdt_send_frame_len)
                 )
               {
                   if(CDT_TRAN_YX_TABLE_NO<4)
                   {
                       CDT_send_YX_chg();
                       cdt_yx_chg_send++;
                       ram_axl=1;
                   }
                   else
                   {
                       if((byte0(yx_change[yx_chg_out_pt[CDT_PORT]].offset_no))<((CDT_TRAN_YX_NUM+1)/2))
                       {
                           CDT_send_YX_chg();
                           cdt_yx_chg_send++;
                           ram_axl=1;
                       }
                       else
                       {
                           cdt_yx_chg_send=3;
                       }
                   }
                   
                   if(cdt_yx_chg_send>2)
                   {
                       yx_chg_out_pt[CDT_PORT] ++;
                       yx_chg_out_pt[CDT_PORT] &= 0xff;
                       cdt_yx_chg_send = 0;
                       if(yx_chg_ex_pt[CDT_PORT] == yx_chg_out_pt[CDT_PORT])
                       {
                           CDT_deal_YX_state();
                       }
                   }
               }
               else
               {
                   if(port_transmit_flag[CDT_PORT] == 0xaa)
                   {
                       if(port_transmit[CDT_PORT][8]==CORE_CODE_YK_VERIFY)
                       {
                           if((cdt_send_order+3-cdt_yk_verify_send)<cdt_send_frame_len)
                           {
                               CDT_send_YK_verify_from_transmit();
                               cdt_yk_verify_send++;
                               if(cdt_yk_verify_send>2)
                               {
                                   port_transmit_flag[CDT_PORT] = 0x55;
                                   cdt_yk_verify_send=0;
                                   HOST_YK_Doing_Begin_Time=Time_1ms_Counter;
                                   HOST_YK_Doing=2;
                               }
                               ram_axl=1;
                           }
                       }
                   }
               }
               if(ram_axl==0)
               {
                   CDT_send_YC();
               }
               break;
          case CDT_PROTECT_LSA:		//-具体的这些内容,到时候在一一解决,现在不深究,这个深度已经够了
               if(CDT_RLY_REPORT==0x01)
                   CDT_send_bh_report();
               else
                   CDT_send_bh_bank();
               break;
          case CDT_YX_EVENT:
               CDT_send_YX_SOE();
               break;   
          case CDT_YM_NORMAL:
               CDT_send_YM();
               break;   
          case CDT_YX_NORMAL:
               CDT_send_YX();
               break;
          default:
               break;     
       }     
       cdt_send_order ++;	//-以4为单位是记录的信息字块得个数吗,,差不多实际记录的是发送的信息字数目
       break;
    default:
       break;
  }
     
}

void CDT_transmit_YK()
{
  BYTE *temp_pt;
  
  temp_pt = &port_deal_buf[CDT_PORT][0];
  for(temp_loop=0;temp_loop<5;temp_loop++)
  {
     if( (*temp_pt!=*(temp_pt+5))||(*temp_pt!=*(temp_pt+10)) )
         temp_loop = 100;    
     temp_pt++;      
  }
  
  if(temp_loop<6)
  {
      exchange_buf[1] = CDT_PORT;         // source port
      exchange_buf[2] = PROTOCOL_CDT_BB%0x100;  // source protocol
      exchange_buf[3] = PROTOCOL_CDT_BB/0x100;  // source protocol
      
      temp_pt = &port_deal_buf[CDT_PORT][0];
      if(*temp_pt==0xe0)
          exchange_buf[8] = CORE_CODE_YK_CHOOSE;  // CMD    operate type
      else
          if(*temp_pt==0xe2)
              exchange_buf[8] = CORE_CODE_YK_EXECUTE;  // CMD    operate type
          else
              exchange_buf[8] = CORE_CODE_YK_CANCEL;  // CMD    operate type
      
      exchange_buf[9] = 0x02;             // LEN_L
      exchange_buf[10]= 0x00;             // LEN_H
      
      temp_pt = &port_deal_buf[CDT_PORT][1];
      if(exchange_buf[8]==CORE_CODE_YK_CHOOSE)
      {
          if(*temp_pt==0xcc)
              exchange_buf[16] = CORE_CODE2_YK_CLOSE;  // CMD    action type
          else
              exchange_buf[16] = CORE_CODE2_YK_TRIP;   // CMD    action type
      }
      else
      {
          if(exchange_buf[8]==CORE_CODE_YK_EXECUTE)
              exchange_buf[16] = CORE_CODE2_YK_EXECUTE;  // CMD    action type
          else
              exchange_buf[16] = CORE_CODE2_YK_CANCEL;   // CMD    action type
      }
      
      exchange_buf[5] =port_deal_buf[CDT_PORT][2]-MON_FUN_NO[CDT_PORT][2];  // YK   object No.
      exchange_buf[17]=0xff;  
//      Ex_YK_CDTBB_ObjectNo_To_UnitAddr();        // input  [5] YK  ObjNo, [17] 0xff;
                                                   // output [5] Unit Addr, [17] YK No.
      if(Ex_YK_CDTBB_ObjectNo_To_UnitAddr()==YES)  // if  YK object No. not exist do not transmit_info
      {
          port_transmit[exchange_target_port][0]=PORT_EXCHANGE_STA_START;
          Ex_Produce_Transmit_Info();
      }
  }
}

void CDT_exchange_report()
{
  #define   LSA_CODE_ACK                    0x06
  #define   LSA_CODE_NAK                    0x15
  unsigned char  *temp_pt,*temp_pts;
  unsigned short  temp_port,temp_word,sumword;

  temp_pt = &port_deal_buf[CDT_PORT][0];
  
  temp_word= *temp_pt;
  for(temp_port=0;temp_port<14;temp_port++)
  {
      if((port_info[temp_port].protocal_type==PROTOCOL_LSA)
       ||(port_info[temp_port].protocal_type==PROTOCOL_CAN_DSA)
       ||(port_info[temp_port].protocal_type==PROTOCOL_DSA301))
      {
         if(   temp_word>=(byte0(port_info[temp_port].scan_panel))
            && temp_word<=(byte1(port_info[temp_port].scan_panel))
           )
               temp_port += 100;
      }        
  }
  
  if(temp_port<90)
     return;
     
  temp_port -= 101;
  if(port_transmit_flag[temp_port] == 0xaa)
     return;
  port_transmit_flag[temp_port] = 0xaa;

  temp_pts = &port_transmit[temp_port][0];

  *temp_pts = 0x00;             temp_pts++; //0
  *temp_pts = CDT_PORT;         temp_pts++; //1
  *temp_pts = PROTOCOL_CDT_BB%0x100;  temp_pts++; //2
  *temp_pts = PROTOCOL_CDT_BB/0x100;  temp_pts++; //3
  *temp_pts = *temp_pt;         temp_pts++; //4
  *temp_pts = 0x00;             temp_pts++; //5
  *temp_pts = 0x00;             temp_pts++; //6
  *temp_pts = 0x00;             temp_pts++; //7
  *temp_pts = *(temp_pt+1);     temp_pts++; //8
   
  if( ( (*(temp_pt+1))==LSA_CODE_ACK ) || ( (*(temp_pt+1))==LSA_CODE_NAK ) )
  {
      *temp_pts = 0x02;          temp_pts++;   //9
      *temp_pts = 0x00;          temp_pts+=6;  //10
      *temp_pts = *(temp_pt+0);  temp_pts++;   //16
      *temp_pts = *(temp_pt+1);                //17
  }
  else
  {
      temp_word  = *(temp_pt+2)+3;
      *temp_pts = byte0(temp_word);          temp_pts++;   //9
      *temp_pts = byte1(temp_word);          temp_pts+=6;  //10
      temp_pt   = &port_deal_buf[CDT_PORT][0];
      sumword=0;
      temp_word=temp_word-2;
      for(temp_port=0;temp_port<temp_word;temp_port++)
      { 
          if(temp_port!=0) sumword += *temp_pt;
          *temp_pts = *temp_pt;
           temp_pts++; 
           temp_pt++;
      }
      
      *temp_pts=byte0(sumword);
       temp_pts++; 
      *temp_pts=byte1(sumword);
  }
}

void CDT_exchange_lsa_FG_report()
{
  BYTE *temp_pt,*temp_pts;
  WORD  temp_port,temp_word,sumword;

  for(temp_port=0;temp_port<14;temp_port++)
  {
   if((port_info[temp_port].protocal_type==PROTOCOL_LSA)
     ||(port_info[temp_port].protocal_type==PROTOCOL_CAN_DSA)
     ||(port_info[temp_port].protocal_type==PROTOCOL_DSA301))
    {
     if(port_transmit_flag[temp_port] != 0xaa)
      {
       port_transmit_flag[temp_port] = 0xaa;
       temp_pts = &port_transmit[temp_port][0];
       *temp_pts = 0x00;             temp_pts++; //0
       *temp_pts = CDT_PORT;      temp_pts++; //1
       *temp_pts = PROTOCOL_CDT_BB%0x100;     temp_pts++; //2
       *temp_pts = PROTOCOL_CDT_BB/0x100;     temp_pts++; //3
       *temp_pts = 0;  temp_pts++; //4
       *temp_pts = 0x00;             temp_pts++; //5
       *temp_pts = 0x00;             temp_pts++; //6
       *temp_pts = 0x00;             temp_pts++; //7
       *temp_pts = 0x4f;     temp_pts++; //8
       temp_word = 6;
       *temp_pts = byte0(temp_word);          temp_pts++;   //9
       *temp_pts = byte1(temp_word);          temp_pts+=6;  //10
       sumword=0;
       *temp_pts =0;
       sumword += *temp_pts;
       temp_pts++;
       *temp_pts =0x4f;
       sumword += *temp_pts;
       temp_pts++;
       *temp_pts =0x03;
       sumword += *temp_pts;
       temp_pts++;
       *temp_pts =0xff;
       sumword += *temp_pts;      
       temp_pts++;
       *temp_pts=byte0(sumword);
       temp_pts++; 
       *temp_pts=byte1(sumword);      	
      }
    }        
  }

}

//void CDT_exchange_dfp201_FG_report()
//{
//  BYTE *temp_pt,*temp_pts;
//  WORD  temp_port,temp_word,sumword;

//  temp_pt   = &port_deal_buf[CDT_PORT][0];
//  temp_word = *temp_pt;
//  for(temp_port=0;temp_port<12;temp_port++)
//  {
//      if(port_info[temp_port].protocal_type==PROTOCOL_DFP201)
//      {
//         if(   (temp_word>=byte0(port_info[temp_port].scan_panel))
//            && (temp_word<=byte1(port_info[temp_port].scan_panel))
//           )
//               temp_port += 100;
//      }        
//  }
  
//  if(temp_port<90)
//     return;
     
//  temp_port -= 101;
//  if(port_transmit_flag[temp_port] == 0xaa)
//     return;
//  port_transmit_flag[temp_port] = 0xaa;

//  temp_pts = &port_transmit[temp_port][0];

//  *temp_pts = 0x00;                          temp_pts++; //0
//  *temp_pts = CDT_PORT;                   temp_pts++; //1
//  *temp_pts = PROTOCOL_CDT_BB%0x100;     temp_pts++; //2
//  *temp_pts = PROTOCOL_CDT_BB/0x100;     temp_pts++; //3
//  *temp_pts = temp_pt[0];                    temp_pts++; //4
//  *temp_pts = 0x00;                          temp_pts++; //5
//  *temp_pts = 0x00;                          temp_pts++; //6
//  *temp_pts = 0x00;                          temp_pts++; //7
//  *temp_pts = temp_pt[2];                    temp_pts++; //8
   
//  temp_word = (WORD)temp_pt[4]+5;
//  *temp_pts = byte0(temp_word);              temp_pts++; //9
//  *temp_pts = byte1(temp_word);              temp_pts+=6;//10
//  temp_pt   = &port_deal_buf[CDT_PORT][0];
//  for(temp_port=0;temp_port<temp_word;temp_port++)
//  { 
//      *temp_pts = *temp_pt;
//       temp_pts++; 
//       temp_pt ++;
//  }
//}

void CDT_exchange_lfp_FG_report()
{
  BYTE *temp_pt,*temp_pts;
  WORD  temp_port,temp_word,sumword;

  for(temp_port=0;temp_port<12;temp_port++)
  {
      if(port_info[temp_port].protocal_type==PROTOCOL_LFP)
      {
       temp_port += 100;
      }        
  }
  
  if(temp_port<90)
     return;
     
  temp_port -= 101;
  if(port_transmit_flag[temp_port] == 0xaa)
     return;
  port_transmit_flag[temp_port] = 0xaa;

  temp_pts = &port_transmit[temp_port][0];

  *temp_pts = 0x00;                          temp_pts++; //0
  *temp_pts = CDT_PORT;                   temp_pts++; //1
  *temp_pts = PROTOCOL_CDT_BB%0x100;     temp_pts++; //2
  *temp_pts = PROTOCOL_CDT_BB/0x100;     temp_pts++; //3
  *temp_pts = 0xff;                    temp_pts++; //4
  *temp_pts = 0x00;                          temp_pts++; //5
  *temp_pts = 0x00;                          temp_pts++; //6
  *temp_pts = 0x00;                          temp_pts++; //7
  *temp_pts = 0x46;                    temp_pts++; //8

  temp_word = 0x0007;
  *temp_pts = byte0(temp_word);              temp_pts++; //9
  *temp_pts = byte1(temp_word);              temp_pts+=6;//10
  sumword =0;
  *temp_pts = 0xff;
  sumword += *temp_pts;
  temp_pts++;
  *temp_pts = 0x46;
  sumword += *temp_pts;
  temp_pts++;
  *temp_pts = 0x01;
  sumword += *temp_pts;
  temp_pts++;
  *temp_pts = 0x00;
  sumword += *temp_pts;
  temp_pts++;
  *temp_pts = 0x04;
  sumword += *temp_pts;
  temp_pts++;
  *temp_pts = byte0(sumword);
  temp_pts++;
  *temp_pts = byte1(sumword);  
}

//禁用 delta_len(temp_int) temp_ptS temp_ptD
void CDT_analysis_control()
{
  cdt_rec_frame_len = port_report[2];
  cdt_rec_stage     = 2;
  byte0(cdt_rec_frame_type) = port_report[1];
  byte1(cdt_rec_frame_type) = port_report[0];
  cdt_rec_info_num = 0;
}

//禁用 delta_len(temp_int) temp_ptS temp_ptD
void CDT_analysis_info()
{
  BYTE *temp_pt;
  
  switch(byte0(cdt_rec_frame_type))
  {
     case 0x61:
     case 0xc2:
     case 0xb3:
          if((port_report[0]>0xDF)&&(port_report[0]<0xE4))//YK
          {
              temp_pt = &port_deal_buf[CDT_PORT][cdt_rec_info_num*5];
              for(temp_loop=0;temp_loop<5;temp_loop++)
              {
                 *temp_pt = port_report[temp_loop];
                  temp_pt ++;
              }
              if(cdt_rec_info_num==2)
              {
                  if((HOST_YK_Doing==0)&&(port_report[0]==0xe0))
                  {
                      HOST_YK_Doing=1;
                      HOST_YK_Port_No=CDT_PORT;
                      HOST_YK_Doing_Begin_Time=Time_1ms_Counter;
                      if(port_deal_buf[CDT_PORT][2]<200)
                       CDT_transmit_YK();
                      else      //soe  fg
                       {
                        port_transmit_flag[CDT_PORT]=0xaa;
                        YX_FG_WAIT_TIME=Time_1ms_Counter;
                        YX_FG_WAIT_FLAG=YES;
                        port_transmit[CDT_PORT][8]=CORE_CODE_YK_VERIFY;
                        port_transmit[CDT_PORT][16]=port_deal_buf[CDT_PORT][1];
                        port_transmit[CDT_PORT][17]=port_deal_buf[CDT_PORT][2]-MON_FUN_NO[CDT_PORT][2];
                        YX_FG_FLAG=YES;
                       }
                  }
                  else
                  {
                      if( (HOST_YK_Doing==2) && (port_report[0]!=0xe0) && (HOST_YK_Port_No==CDT_PORT) )
                      {
                          HOST_YK_Doing=0;
                          if(port_deal_buf[CDT_PORT][2]<200)
                           CDT_transmit_YK();
                      }    
                  }        
              }    
          }
          break;

     case 0x7a:
          mon_ok_flag[CDT_PORT]=YES;
          mon_err_start_time[CDT_PORT]=Time_2048ms_Counter;
          if(unit_info[255].yx_num!=0)
       	   YX_State[unit_info[255].yx_start]&=(~(0x0001<<CDT_PORT));
          if(port_report[0]==0xee)
          {
              CDT_time_tmp_MSL=port_report[1];
              CDT_time_tmp_MSH=port_report[2];
              CDT_time_tmp_SEC=port_report[3];
              CDT_time_tmp_MIN=port_report[4];
          }
          else
          {
              if(port_report[0]==0xef)
              {
                  if(byte0(port_info[CDT_PORT].time_enable)!=0x55)
                  {
                      disable();
                  
                      REG_1Msecond=(CDT_time_tmp_MS % 1000) + P554_XTAL16M_CLOSE_RTS_DELAY[port_info[CDT_PORT].bauds]*20;
                      REG_Second=CDT_time_tmp_SEC % 60;
                      REG_Minute=CDT_time_tmp_MIN % 60;
                      REG_Hour  =port_report[1] % 24;
                      REG_Date  =port_report[2] % 32;
                      REG_Month =port_report[3] % 13;
                      REG_Year  =byte0(port_info[CDT_PORT].time_enable)+1830+(port_report[4] % 100);
                  
                      Clock_Process();
                  
                      Write_Time_To_Dallas();
                  
                      enable();
                  }    
              }
          }
          break;

     case 0xf8:
           temp_pt = &port_deal_buf[CDT_PORT][port_report[0]*4];
          *temp_pt = port_report[1];temp_pt++;
          *temp_pt = port_report[2];temp_pt++;
          *temp_pt = port_report[3];temp_pt++;
          *temp_pt = port_report[4];
          if(port_report[0]>=(cdt_rec_frame_len-1))
          {
            CDT_exchange_report();
            cdt_rec_frame_len = 0;
          }    
          break;
          
     case 0x3d:
          CDT_exchange_lsa_FG_report();
//          CDT_exchange_dfp201_FG_report();
          CDT_exchange_lfp_FG_report();
          break;
     
     default:
          break;
  } 
  cdt_rec_info_num ++;
  if(cdt_rec_info_num >= cdt_rec_frame_len)
     cdt_rec_syn_flag = 0x55;
}

void CDT_deal_rpt()
{
//temp_loop   同步字数
//temp_int    待查字长
//temp_ptS    当前位置
//temp_ptD    回零位置
//temp_lp_int 同步字
//port_check  已过滤字数
#define synword_num  temp_loop  
#define delta_len    temp_int
#define synword      temp_lp_int  
#define filter_num   port_check

  synword_num   = 0;
  byte0(synword)= SYN_WORD_CDT_EB90[0];
  filter_num    = 0;

  disable();
  if(port_recv_dl[CDT_PORT]>port_recv_pt[CDT_PORT])
     delta_len = (port_recv_pt[CDT_PORT]+512) - port_recv_dl[CDT_PORT];
  else
     delta_len = port_recv_pt[CDT_PORT] - port_recv_dl[CDT_PORT];
  enable();
  
  if(delta_len<6)
     return;

  temp_ptS_B = &port_recv[CDT_PORT][port_recv_dl[CDT_PORT]];
  temp_ptD_B = &port_recv[CDT_PORT][511];

  if(cdt_rec_syn_flag != 0xaa)
  {// find EB
    while(synword_num<6)
    {   
      while((*temp_ptS_B)!=(byte0(synword)))
      {
         synword_num = 0;
         temp_ptS_B ++;
         filter_num ++;
         if(temp_ptS_B>temp_ptD_B)
            temp_ptS_B -= 512;
         delta_len  --;
         if(delta_len<6)
         {
            port_recv_dl[CDT_PORT] += filter_num;
            if(port_recv_dl[CDT_PORT]>511)
                port_recv_dl[CDT_PORT] -= 512;
            return;
         }
         byte0(synword) = SYN_WORD_CDT_EB90[0];
      }
      synword_num++;
      byte0(synword)=SYN_WORD_CDT_EB90[synword_num];
      temp_ptS_B++;
      filter_num++;
      if(temp_ptS_B>temp_ptD_B)
         temp_ptS_B -= 512;
      delta_len--;
      if(delta_len<(unsigned short)(6-synword_num))
      {
         filter_num -= synword_num;
         port_recv_dl[CDT_PORT] += filter_num;
         if(port_recv_dl[CDT_PORT]>511)
            port_recv_dl[CDT_PORT] -= 512;
         return;
      }
    }  // <6
    port_recv_dl[CDT_PORT] += filter_num;
    if(port_recv_dl[CDT_PORT]>511)
       port_recv_dl[CDT_PORT] -= 512;
    cdt_rec_stage = 1; 
    cdt_rec_syn_flag = 0xaa;
  }

  while(delta_len>=6)
  {
//禁用 delta_len(temp_int) temp_ptS temp_ptD
     for(temp_loop1=0;temp_loop1<6;temp_loop1++)
     {
       port_report[temp_loop1] = *temp_ptS_B;
       temp_ptS_B++;
       if(temp_ptS_B>temp_ptD_B)
          temp_ptS_B -= 512;
     }
     if(CDT_check_BCH() == 0xaa)
     {
       port_recv_dl[CDT_PORT] += 6;
       if(port_recv_dl[CDT_PORT]>511)
          port_recv_dl[CDT_PORT] -= 512;
       delta_len -= 6;

       if(cdt_rec_stage==1)
         CDT_analysis_control();
       else if(cdt_rec_stage==2)
                CDT_analysis_info();
            else
            {
                cdt_rec_syn_flag = 0x55;
                return;
            }
 
       if(cdt_rec_syn_flag != 0xaa)
         return;
     }
     else
     {
       cdt_rec_syn_flag = 0x55;
       return;
     }
  }
}


void CDT_deal_YX_state()	//-这个是在初始化过程中有处理
{
  if(CDT_TRAN_YX_TABLE_NO>3)	//-这个的数值来自ROM中的配置值
     return;
  if(yx_chg_tr_ex_pt[CDT_TRAN_YX_TABLE_NO]!=yx_chg_in_pt)	//-好像和上面是一个意思,仅仅是为了可靠而已
     return;
 
  temp_ptD = (unsigned short*)&YX_transmite_table[CDT_TRAN_YX_TABLE_NO][0];
  temp_int = 0;
  //while(*temp_ptD < 2048)
  while(*temp_ptD < 4096)  // all YX now MAX 4096
  {
     if((YX_State[(*temp_ptD)/16] & (1<<((*temp_ptD)&0x0f))) != 0)
     {	//-现在已经精简到不能再少了,所以任何一个知识点都不能发过现在就解决
        temp_lp_int = YX_state_tr[CDT_TRAN_YX_TABLE_NO][temp_int/16];
        temp_lp_int = temp_lp_int | (1 << (temp_int&0x0f));
        YX_state_tr[CDT_TRAN_YX_TABLE_NO][temp_int/16] = temp_lp_int;
     }
     else
     {
        temp_lp_int = YX_state_tr[CDT_TRAN_YX_TABLE_NO][temp_int/16];
        temp_lp_int = temp_lp_int & (0xffff ^ (1 << (temp_int&0x0f)));
        YX_state_tr[CDT_TRAN_YX_TABLE_NO][temp_int/16] = temp_lp_int;
     }
     temp_ptD ++;
     temp_int ++;
     if(temp_int>511)
        break;
  }

}


void CDT_deal_YX_chg()
{
  unsigned short temp_chg;
  unsigned short old_chg_channel;
  unsigned short *temp_chg_pt;
  unsigned short  chg_bit_buf;   
  if(CDT_TRAN_YX_TABLE_NO>3)
  {
     yx_chg_ex_pt[CDT_PORT] = yx_chg_in_pt;
     return;
  }

  old_chg_channel = 0xff;
  while(yx_chg_tr_ex_pt[CDT_TRAN_YX_TABLE_NO]!=yx_chg_in_pt)
  {
     temp_ptS = &yx_change[yx_chg_tr_ex_pt[CDT_TRAN_YX_TABLE_NO]].offset_no;
     temp_lp_int = 1;
     temp_loop = 0;
     chg_bit_buf = *(temp_ptS+3);	//-如chg_bit0对应的位值为1，则表示该位YX产生变位
     while(chg_bit_buf!=0)
     {
       if((chg_bit_buf & temp_lp_int)!=0)
       {	//-一位一位的检查不等于0说明有变化
         chg_bit_buf ^= temp_lp_int;
         temp_ptD = (unsigned short*)&YX_transmite_table[CDT_TRAN_YX_TABLE_NO][0];
         temp_int = (*temp_ptS)*32 + temp_loop;
         temp_chg =  0;
         while((*temp_ptD != temp_int) && (temp_chg<512))
         {
             temp_ptD ++;
             if(*temp_ptD == 0xffff)
                 temp_chg = 1000;
             temp_chg++;
         }
         if(temp_chg<512)
         {
             if((*(temp_ptS+1) & temp_lp_int) != 0)
                 YX_state_tr[CDT_TRAN_YX_TABLE_NO][temp_chg/16] |= (1<<(temp_chg&0x0f));
             else
                 YX_state_tr[CDT_TRAN_YX_TABLE_NO][temp_chg/16] &= ( 0xffff - (1<<(temp_chg&0x0f)));
             if(old_chg_channel != (temp_chg/32))
             {
                 if(old_chg_channel != 0xff)
                 {
                     temp_chg_pt    =&yx_change_tr[CDT_TRAN_YX_TABLE_NO][yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO]].offset_no;
                    *temp_chg_pt    = old_chg_channel;
                     temp_chg_pt++;
                    *temp_chg_pt    = YX_state_tr[CDT_TRAN_YX_TABLE_NO][old_chg_channel*2];
                     temp_chg_pt++;
                    *temp_chg_pt    = YX_state_tr[CDT_TRAN_YX_TABLE_NO][old_chg_channel*2+1];
                     yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO] ++;
                     yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO] &= 0xff;
                 }
                 
                 old_chg_channel = temp_chg/32;
             }
         } 
       }
       temp_lp_int <<= 1;
       temp_loop++;
       if(temp_loop>16)
          break;
     }

     temp_lp_int = 1;
     temp_loop = 0;
     chg_bit_buf = *(temp_ptS+4);	//-如chg_bit1对应的位值为1，则表示该位YX产生变位
     while(chg_bit_buf != 0)
     {
       if((chg_bit_buf & temp_lp_int)!=0)
       {
         chg_bit_buf ^= temp_lp_int;
         temp_ptD = (unsigned short *)&YX_transmite_table[CDT_TRAN_YX_TABLE_NO][0];
         temp_int = *temp_ptS*32 + 16 + temp_loop;
         temp_chg = 0;
         while((*temp_ptD != temp_int)&&(temp_chg<512))
         {
             temp_ptD ++;
             if(*temp_ptD == 0xffff)
                 temp_chg = 1000;
             temp_chg++;
         }
         if(temp_chg<512)
         {
             if((*(temp_ptS+2) & temp_lp_int) != 0)
                 YX_state_tr[CDT_TRAN_YX_TABLE_NO][temp_chg/16] |= (1<<(temp_chg&0x0f));
             else
                 YX_state_tr[CDT_TRAN_YX_TABLE_NO][temp_chg/16] &= ( 0xffff - (1<<(temp_chg&0x0f)));
             if(old_chg_channel != (temp_chg/32))
             {
                 if(old_chg_channel != 0xff)
                 {
                     temp_chg_pt    =&yx_change_tr[CDT_TRAN_YX_TABLE_NO][yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO]].offset_no;
                    *temp_chg_pt    = old_chg_channel;
                     temp_chg_pt++;
                    *temp_chg_pt    = YX_state_tr[CDT_TRAN_YX_TABLE_NO][old_chg_channel*2];
                     temp_chg_pt++;
                    *temp_chg_pt    = YX_state_tr[CDT_TRAN_YX_TABLE_NO][old_chg_channel*2+1];
                     yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO] ++;
                     yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO] &= 0xff;
                 }
                 
                 old_chg_channel = temp_chg/32;
             }
         } 
       }
       temp_lp_int<<= 1;
       temp_loop++;
       if(temp_loop>16)
          break;
     }  
     
     yx_chg_tr_ex_pt[CDT_TRAN_YX_TABLE_NO] ++;	//-统计变位YX帧的个数
     yx_chg_tr_ex_pt[CDT_TRAN_YX_TABLE_NO] &= 0xff;        
                
  }  

  if(old_chg_channel != 0xff)
  {
     temp_chg_pt     =&yx_change_tr[CDT_TRAN_YX_TABLE_NO][yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO]].offset_no;
    *temp_chg_pt    = old_chg_channel;
     temp_chg_pt++;
    *temp_chg_pt    = YX_state_tr[CDT_TRAN_YX_TABLE_NO][old_chg_channel*2];
     temp_chg_pt++;
    *temp_chg_pt    = YX_state_tr[CDT_TRAN_YX_TABLE_NO][old_chg_channel*2+1];
     yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO] ++;
     yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO] &= 0xff;
  }


  yx_chg_ex_pt[CDT_PORT] = yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO];

}


void CDT_deal_YX_soe()
{
  if(CDT_TRAN_YX_TABLE_NO>3)
  {
     yx_soe_ex_pt[CDT_PORT] = yx_soe_in_pt;
     return;
  }

  while(yx_soe_tr_ex_pt[CDT_TRAN_YX_TABLE_NO]!=yx_soe_in_pt)	//-应该是把这儿送的数值,跟外边变化的进行比较,有差异的话就跟上yx_soe_in_pt的变化
  {
     temp_ptS = &yx_event[yx_soe_tr_ex_pt[CDT_TRAN_YX_TABLE_NO]].soe_ms;	//-和外面联系上了,这个地方就是记录了外面的数值
     temp_int = *(temp_ptS + 3) & 0x7fff;	//-难道这里是取出了对象号,最高位是记录的状态值
     temp_ptD = (unsigned short *)&YX_transmite_table[CDT_TRAN_YX_TABLE_NO][0];	//-遥信转发表
     temp_lp_int = 0;
     while(*temp_ptD != temp_int)	//-寻找到对象号和性质一样的东西
     {
        temp_ptD ++;
        if(*temp_ptD > 0x0fff)    // now max YX num 4096 in Dbase
           temp_lp_int = 1000;
        temp_lp_int++;
        if(temp_lp_int>512)
           break;
     }
     if(temp_lp_int<512)	//-这个数值表示现在更新的SOE的对象号,没有直接把对象号取出来而是比较得到的
     {
        temp_ptD = &yx_event_tr[CDT_TRAN_YX_TABLE_NO][yx_soe_tr_in_pt[CDT_TRAN_YX_TABLE_NO]].soe_ms;
       *temp_ptD = *temp_ptS; temp_ptD++; temp_ptS++;	//-毫秒
       *temp_ptD = *temp_ptS; temp_ptD++; temp_ptS++;	//-秒 分
       *temp_ptD = *temp_ptS; temp_ptD++; temp_ptS++;	//-时 天
       *temp_ptD = *temp_ptS; *temp_ptD = (*temp_ptD & 0x8000) + temp_lp_int;
        yx_soe_tr_in_pt[CDT_TRAN_YX_TABLE_NO]++;	//-指向当前记录的单元号
        yx_soe_tr_in_pt[CDT_TRAN_YX_TABLE_NO] &= 0x3ff;
     }
     yx_soe_tr_ex_pt[CDT_TRAN_YX_TABLE_NO] ++;	//-一个一个检查跟上新增加的数值
     yx_soe_tr_ex_pt[CDT_TRAN_YX_TABLE_NO] &= 0x3ff;
  }

  yx_soe_ex_pt[CDT_PORT] = yx_soe_tr_in_pt[CDT_TRAN_YX_TABLE_NO];	//-表示当前记录到了哪个单元

}


void CDT_deal_info()
{
  CDT_deal_YX_chg();
  CDT_deal_YX_soe();
}




void CDT_Init()		//-所有之前先干的
{
  cdt_send_order      = 0;
  cdt_send_frame_len  = 0;
  cdt_send_info_turn  = 0; 
  cdt_send_stage      = 0;
  cdt_yx_send         = 0;
  cdt_yx_chg_send     = 0;
  cdt_yk_verify_send  = 0;

  cdt_rec_syn_flag    = 0x55;
  cdt_rec_stage       = 0;
  cdt_rec_frame_len   = 0;

  CDT_TRAN_YC_TABLE_NO = byte0(port_info[CDT_PORT].table_no);	//-对应的口就是检查到这个规约的口
  CDT_TRAN_YX_TABLE_NO = byte1(port_info[CDT_PORT].table_no);	//-把对应的配置数放在需要的空间呢
  CDT_TRAN_YM_TABLE_NO = byte1(port_info[CDT_PORT].table_no);

  if(CDT_TRAN_YX_TABLE_NO<4)	//-现在还不知道为什么配置什么数据但是我知道了配置数据之后的处理
  {
     yx_chg_out_pt[CDT_PORT] = yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO];
     yx_chg_ex_pt[CDT_PORT]  = yx_chg_tr_in_pt[CDT_TRAN_YX_TABLE_NO];
     yx_soe_out_pt[CDT_PORT] = yx_soe_tr_in_pt[CDT_TRAN_YX_TABLE_NO];	//-可能表示送出的SOE
     yx_soe_ex_pt[CDT_PORT]  = yx_soe_tr_in_pt[CDT_TRAN_YX_TABLE_NO];	//-表示改变的SOE,初始值是一样的,那么应该正常情况下是改变的先变化然后送出的跟上
  }
  else
  {
     yx_chg_out_pt[CDT_PORT] = yx_chg_in_pt;
     yx_chg_ex_pt[CDT_PORT]  = yx_chg_in_pt;
     yx_soe_out_pt[CDT_PORT] = yx_soe_in_pt;
     yx_soe_ex_pt[CDT_PORT]  = yx_soe_in_pt;
  }

  cdt_yx_fresh   = 0;
  CDT_deal_YX_state();
  
  CDT_RLY_REPORT = 0x00;
  CDT_PAGE_NUM   = 0;
  CDT_PAGE_ORDER = 0;
  YX_FG_WAIT_FLAG=NO;
}







//-主程序中,一个端口一个端口的进行检查,检查到有匹配的规约的时候就进行这个端口的操作,他们操作的对象
//-应该是每个端口上挂接得设备,而这些设备处理的数据是内部数据库
void CDT_Main()
{
  CDT_deal_info();
  CDT_deal_rpt();
  CDT_assemble_send_rpt();
  if((Judge_Time_In_MainLoop(YX_FG_WAIT_TIME,100)==YES)&&(YX_FG_WAIT_FLAG==YES))
   {
    YX_FG_WAIT_FLAG=NO;
    port_transmit_flag[CDT_PORT]=0xaa;         ;;;这儿也给了转发标志
   }
}














void CDT_Monitor()	//-监视说白了就是检查是否正常,若异常的话就重新初始化
{
  // monitor send err
  if(port_send_pt[CDT_PORT]<port_send_len[CDT_PORT])
     port_mon[CDT_PORT]++;
  else
     port_mon[CDT_PORT] = 0;
  if(port_mon[CDT_PORT]>200)
  {
    (*((far BYTE *)(SOFT_ERR_FLAG+0x0080+CDT_PORT*4)))++;
    disable();
    init_port();
    enable();
    port_mon[CDT_PORT] = 0;
  }

  // for 'long long time' fresh YX
  cdt_yx_fresh++;
  if(cdt_yx_fresh>40)
  {
     CDT_deal_YX_state();
     cdt_yx_fresh = 0;
  }
}
