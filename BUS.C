#pragma  regconserve

#include _SFR_H_
#include _FUNCS_H_

#include "common.h"
#include "device.h"
#include "reg.h"
#include "ram.h"
#include "rom.h"
#include "comuse.h"

//-----------------  BUS definition  ----------------------
#define BUS_PORT_NO                               0x0b


//  BUS_CODE
#define BUS_CMD_CODE_YC_SELECT                    0x01
#define BUS_CMD_CODE_YC_SELECT_REPLY              0x09
#define BUS_CMD_CODE_YC_ALL                       0x02
#define BUS_CMD_CODE_YC_ALL_REPLY                 0x0a
#define BUS_CMD_CODE_YC_XS_DOWN                   0x03
#define BUS_CMD_CODE_YC_ZY_DOWN                   0x04
#define BUS_CMD_CODE_YC_XS_VERI                   0x0b
#define BUS_CMD_CODE_YC_ZY_VERI                   0x0c
#define BUS_CMD_CODE_YC_VERI_CONFIRM              0x06
#define BUS_CMD_CODE_DC_ALL                       0x05
#define BUS_CMD_CODE_DC_ALL_REPLY                 0x0d

#define BUS_CMD_CODE_YX_SELECT                    0x11
#define BUS_CMD_CODE_YX_SELECT_REPLY              0x19
#define BUS_CMD_CODE_YX_ALL                       0x12
#define BUS_CMD_CODE_YX_COS                       0x1a
#define BUS_CMD_CODE_YX_SOE                       0x1b
#define BUS_CMD_CODE_YX_APPLY_SET                 0x1c
#define BUS_CMD_CODE_YX_SZ_DOWN                   0x15
#define BUS_CMD_CODE_YX_SZ_VERI                   0x1d
#define BUS_CMD_CODE_YX_VERI_CONFIRM              0x16

#define BUS_CMD_CODE_YK_SELECT                    0x30
#define BUS_CMD_CODE_YK_EXCUTE                    0x32
#define BUS_CMD_CODE_YK_CANCEL                    0x33
#define BUS_CMD_CODE_YK_VERIFY                    0x38

#define BUS_CMD_CODE_YM_ALL                       0x40
#define BUS_CMD_CODE_YM_ALL_REPLY                 0x48

#define BUS_CMD_CODE_BRDCAST_TIME                 0x51
#define BUS_CMD_CODE_ASK_VERI_TIME                0x5a

#define BUS_CMD_CODE_YX_EVENT_CONFIRM             0xc0

#define BUS_POLL_TURN_YC                          0x01
#define BUS_POLL_TURN_DC                          0x02
#define BUS_POLL_TURN_YX                          0x03
#define BUS_POLL_TURN_YM                          0x04






#define BUS_PORT     port_no

#define BUS_YC_LOOP  4    /* 1 times perloop */
#define BUS_DC_LOOP  32   /* 1 times perloop */

//-难道这里就是所谓的再定义(为了形象)
#define bus_state                              port_flag[BUS_PORT][0]
#define bus_rpt_state                          port_flag[BUS_PORT][1]
#define bus_img                                port_flag[BUS_PORT][2]
#define bus_img_cnt                            port_flag[BUS_PORT][3]
#define bus_reply                              port_flag[BUS_PORT][4]
#define bus_reply_cnt                          port_flag[BUS_PORT][5]
#define bus_loop                               port_flag[BUS_PORT][6]
#define bus_moniter                            port_flag[BUS_PORT][7]
#define bus_temp                               port_flag[BUS_PORT][8]
#define bus_yk_source                          port_flag[BUS_PORT][9]
#define bus_delay                              port_flag[BUS_PORT][10]
#define bus_bark_time                          port_flag[BUS_PORT][11]

#define bus_broadcast_begin_time               port_flag[BUS_PORT][12]
#define bus_need_exchg_to_host                 port_flag[BUS_PORT][13]
#define bus_assemble_ym_send_page              byte0(port_flag[BUS_PORT][120])
#define YM_SET_FLAG                            byte1(port_flag[BUS_PORT][120])
#define ym_loop                                byte0(port_flag[BUS_PORT][121])
#define ym_add                                 byte1(port_flag[BUS_PORT][121])
#define ym_no_reply                            byte0(port_flag[BUS_PORT][122])
#define bus_gps_addr                           byte1(port_flag[BUS_PORT][122])
#define ym_set_time                            port_flag[BUS_PORT][123]

#define bus_unit_poll_disable                  ( (unsigned  char *)&(port_flag[BUS_PORT][24]) ) // 8 bytes
#define bus_unit_tx_state                      ( (unsigned  char *)&(port_flag[BUS_PORT][28]) ) // 8 bytes
#define bus_cn_unit_tx_err                     ( (unsigned  char *)&(port_flag[BUS_PORT][32]) )
#define bus_unit_err_begin_poll_clk            ( (unsigned short *)&(port_flag[BUS_PORT][64]) )


    


void bus_transmit_head()
{
  for(temp_loop=0;temp_loop<6;temp_loop++)
    port_send[BUS_PORT][temp_loop] = SYN_WORD_BUS[temp_loop];
}


void bus_send()
{

  port_check = 0;
  temp_ptS_B = &(port_send[BUS_PORT][6]);
  for(temp_loop=0;temp_loop<(WORD)(port_send[BUS_PORT][8]+3);temp_loop++)
  {
    port_check +=  *temp_ptS_B;   //-求校验和,这个和的结果是不包括固定头的
    temp_ptS_B ++;
  }
  *temp_ptS_B = port_check;  //-开始发送时才计算校验和,是从单元地址到最后一个数据值,的累加和的低8位数据

  port_send_len[BUS_PORT] = port_send[BUS_PORT][8] + 11;
	//-到上面为止一个信息的内容已经全了,现在可以发送了
  port_send_begin();  

  port_recv_dl[BUS_PORT]=0;
  port_recv_pt[BUS_PORT]=0; //-一个是固定指针,一个是活动指针,,是指处理完一帧信息之后就报无效报文舍弃吗
}

void bus_transmit_yk_cmd()
{
	if(port_transmit[BUS_PORT][0]==0x32)
	{
        Rcd_Info_System_Tmp[0]=0;  // reserved
        Rcd_Info_System_Tmp[1]=BUS_PORT;                            // PORT NO.
        Rcd_Info_System_Tmp[2]=port_transmit[BUS_PORT][5];          // UNIT ADDR
        Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_YK_EXECUTE;    // do   what
        Rcd_Info_System_Tmp[4]=SETPORT;                             // source port
        Rcd_Info_System_Tmp[5]=port_transmit[BUS_PORT][17];
        Rcd_Info_System_Tmp[6]=0;  
        Rcd_Info_System_Tmp[7]=0; 
        Store_Rcd_Info_System();
	}
	else
	{
	    if((port_transmit[BUS_PORT][0]==PORT_EXCHANGE_STA_START)&&(port_transmit[BUS_PORT][8]==0x32))
	    {
	        Rcd_Info_System_Tmp[0]=0;  // reserved
    	    Rcd_Info_System_Tmp[1]=BUS_PORT;                            // PORT NO.
        	Rcd_Info_System_Tmp[2]=port_transmit[BUS_PORT][5];          // UNIT ADDR
	        Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_YK_EXECUTE;    // do   what
    	    Rcd_Info_System_Tmp[4]=port_transmit[BUS_PORT][1];          // source port
        	Rcd_Info_System_Tmp[5]=port_transmit[BUS_PORT][17];
		    Rcd_Info_System_Tmp[6]=0;  
        	Rcd_Info_System_Tmp[7]=0; 
        	Store_Rcd_Info_System();
	    }
	}
	
    bus_transmit_head();
    temp_ptD_B = &port_send[BUS_PORT][6];
   *temp_ptD_B = port_transmit[BUS_PORT][5];
    temp_ptD_B++;
   *temp_ptD_B = port_transmit[BUS_PORT][8];
    temp_ptD_B++;
   *temp_ptD_B = 2;
    temp_ptD_B++;
   *temp_ptD_B = port_transmit[BUS_PORT][17];
    temp_ptD_B++;
   *temp_ptD_B = port_transmit[BUS_PORT][16];
    temp_ptD_B++;

    bus_need_exchg_to_host=NO;
    bus_send();
}

void bus_transmit_whole_package()
{
  bus_transmit_head();

  temp_ptD_B = &port_send[BUS_PORT][6];
  temp_ptS_B = &port_transmit[BUS_PORT][1];
  temp_loop1 = *(temp_ptS_B + 2) + 3; 
  for(temp_loop=0;temp_loop<temp_loop1;temp_loop++)
  {
      *temp_ptD_B = *temp_ptS_B;
       temp_ptD_B ++;
       temp_ptS_B ++;
  }
  bus_need_exchg_to_host=NO;
  bus_send();

//  if(port_send[BUS_PORT][7]==0x05)  bus_need_exchg_to_host=YES;
//  else   bus_need_exchg_to_host=NO;
  switch(port_send[BUS_PORT][7])
  {
    case 0x02: case 0x03: case 0x04:
       bus_reply     = 0xaa;
       bus_reply_cnt = 0;
       break;
    default:
       bus_reply     = 0x55;
       bus_reply_cnt = 0;
       break;
  } 

}

//-现在急需要一个突破口
void bus_ask_yc()
{
  bus_transmit_head();
  port_send[BUS_PORT][6] = bus_state;
  port_send[BUS_PORT][7] = 0x01;
  port_send[BUS_PORT][8] = 0x01;
  port_send[BUS_PORT][9] = 0x01;

  bus_state |= 0x80;
  bus_img = bus_state;
  bus_img_cnt = 0;
  bus_need_exchg_to_host=NO;
  bus_send();
}

void bus_ask_dc()
{
  bus_transmit_head();
  port_send[BUS_PORT][6] = bus_state;
  port_send[BUS_PORT][7] = 0x05;
  port_send[BUS_PORT][8] = 0x02;
  port_send[BUS_PORT][9] = 0x00;
  port_send[BUS_PORT][10]= 0x00;

  bus_state |= 0x80;
  bus_img = bus_state;
  bus_img_cnt = 0;
  bus_need_exchg_to_host=NO;
  bus_send();
}

void bus_ask_ym()
{
  bus_transmit_head();
  port_send[BUS_PORT][6] = bus_state;
  port_send[BUS_PORT][7] = 0x40;
  port_send[BUS_PORT][8] = 0x02;
  port_send[BUS_PORT][9] = 0x00;
  port_send[BUS_PORT][10]= 0x00;

  bus_state |= 0x80;
  bus_img = bus_state;
  bus_img_cnt = 0;
  bus_need_exchg_to_host=NO;
  bus_send();
}
//-连这个都搞不好我来
void bus_ask_yx()
{
  bus_transmit_head();    //-加上固定的头
  port_send[BUS_PORT][6] = bus_state; //-这个的作用,难道不是为了实际值,而仅仅是为了下达命令
  port_send[BUS_PORT][7] = 0x11;
  port_send[BUS_PORT][8] = 0x0;

  bus_state |= 0x80;
  bus_img = bus_state;
  bus_img_cnt = 0;
  bus_need_exchg_to_host=NO;
  bus_send();
}
//-MON查询IED  YX ,这个地方的查询也是可靠性更高的互动性查询
void bus_confirm_yx()	//-现在如果猜的不错的话,应该是MON向IED下发报文
{
  bus_transmit_head();
  port_send[BUS_PORT][6] = bus_state;
  port_send[BUS_PORT][7] = 0xc0;  //-难道这个是传说中的功能码 
  port_send[BUS_PORT][8] = 0x1;   //-这个1的意思是下面多了一个字节的内容,那么这个就是长度咯
  port_send[BUS_PORT][9] = temp_int;	//-数据位

  bus_state &= 0x7f;
  bus_img = bus_state;
  bus_img_cnt = 0;
  bus_need_exchg_to_host=NO;	//-因为不需要host的参与所以总线,不需要交换给host
  bus_send();
}

void bus_brd_time()
{
  bus_transmit_head();
  port_send[BUS_PORT][6] = 0xff;
  port_send[BUS_PORT][7] = 0x51;	//-MON_TO_IED校时。  功能码
  port_send[BUS_PORT][8] = 0x06;

  disable();
  port_send[BUS_PORT][9] =  byte0(REG_1Msecond);
  port_send[BUS_PORT][10]=  byte1(REG_1Msecond);
  port_send[BUS_PORT][11]=  byte0(REG_Second);
  port_send[BUS_PORT][12]=  byte0(REG_Minute);
  port_send[BUS_PORT][13]=  byte0(REG_Hour);
  port_send[BUS_PORT][14]=  byte0(REG_Date);
  enable();

  bus_need_exchg_to_host=NO;
  bus_send();
}
//-这些现在大体上知道什么意思就行了,深入了解这个层次还没有到
void bus_assemble_yx_set()	//-MON对IED  YX设置。
{
  bus_transmit_head();
  Core_Src_Unit = bus_state & 0x7f;
  port_send[BUS_PORT][6] = Core_Src_Unit;
  port_send[BUS_PORT][7] = 0x15;	//-这里还看出了一个问题,在这个规约里,相同的功能码,意思不一定相同,可能随着主体的变化而变化
  port_send[BUS_PORT][8] = 208;
  Core_Src_Pt_B = &port_send[BUS_PORT][9];
  core_get_yx_set_unit();

  temp_ptS_B    = &port_send[BUS_PORT][8];
  temp_ptD_B    = &port_efficacy[BUS_PORT][2];
  for(temp_loop=0;temp_loop<209;temp_loop++)
  {		//-这段程序的目的就是保留向IED设置的值,以便IED发来返校时校验
     *temp_ptD_B = *temp_ptS_B;
      temp_ptS_B ++;
      temp_ptD_B ++;
  }

  bus_state |= 0x80;
  bus_img    = bus_state;
  bus_img_cnt= 0; 
  bus_need_exchg_to_host=NO;
  bus_send();
}


void bus_ym_set()
{
  far BYTE *ram_base_addr;
  
  ram_base_addr=(far BYTE *)DALLAS_YM_SAV_PTR_ADDR; 
  bus_transmit_head();
  port_send[BUS_PORT][6] = ym_add;
  port_send[BUS_PORT][7] = 0x41; 
  port_send[BUS_PORT][8] = 0x81;
   
  port_send[BUS_PORT][9] = bus_assemble_ym_send_page;//page
  for(temp_loop=0;temp_loop<32;temp_loop++)
   {
    port_send[BUS_PORT][4*temp_loop+10]=ram_base_addr[unit_info[ym_add].ym_start+4*temp_loop+bus_assemble_ym_send_page*128];
    port_send[BUS_PORT][4*temp_loop+11]=ram_base_addr[unit_info[ym_add].ym_start+4*temp_loop+1+bus_assemble_ym_send_page*128];
    port_send[BUS_PORT][4*temp_loop+12]=ram_base_addr[unit_info[ym_add].ym_start+4*temp_loop+2+bus_assemble_ym_send_page*128];
    port_send[BUS_PORT][4*temp_loop+13]=ram_base_addr[unit_info[ym_add].ym_start+4*temp_loop+3+bus_assemble_ym_send_page*128];
   }

  bus_need_exchg_to_host=NO;
  bus_send();
}
//-这个是接收到YC设置请求后,下发的回应报文.
void bus_assemble_yc_set()
{
  bus_transmit_head();
  Core_Src_Unit = bus_state & 0x7f;         //-这个里面怎么会记录的是单元地址呢???
  port_send[BUS_PORT][6] = Core_Src_Unit;	//-单元地址
  port_send[BUS_PORT][7] = 0x03;			//-MON设置IED  YC(选择上传)。  ,,MON指的是通讯管理机,IED指的是下位机
  port_send[BUS_PORT][8] = 12;              //-报文长度
  Core_Src_Pt_B = &port_send[BUS_PORT][9];
  core_get_yc_set_unit();

  temp_ptS_B    = &port_send[BUS_PORT][8];  
  temp_ptD_B    = &port_efficacy[BUS_PORT][2];   //-效力,效能在这表示什么呢
  for(temp_loop=0;temp_loop<13;temp_loop++)
  {
     *temp_ptD_B = *temp_ptS_B;   //-是备份吗
      temp_ptS_B ++;
      temp_ptD_B ++;
  }

  bus_state |= 0x80;  //-通讯地址+128是什么用意呢?
  bus_img = bus_state;
  bus_img_cnt = 0; 
  bus_need_exchg_to_host=NO;  //-呵呵,突发奇想,是表示总线需要交给主服务吗
  bus_send();
}










/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void ym_save_to_dallas(void)
{
    WORD  ram_ax;
    far BYTE *ram_base_addr;

    ram_base_addr=(far BYTE *)DALLAS_YM_SAV_PTR_ADDR;     ;;;给的是绝对地址
    for(ram_ax=0;ram_ax<256;ram_ax++)
    {
      ram_base_addr[4*ram_ax]=byte0(YM_State[ram_ax]); 
      ram_base_addr[4*ram_ax+1]=byte1(YM_State[ram_ax]);
      ram_base_addr[4*ram_ax+2]=byte2(YM_State[ram_ax]);
      ram_base_addr[4*ram_ax+3]=byte3(YM_State[ram_ax]);   
    }
    
}

void bus_judge_poll_disable(void)
{
    BYTE the_ram_axl;
//    BYTE the_ram_axh;

    for(the_ram_axl=0;the_ram_axl<63;the_ram_axl++)
    {
        if( (bus_unit_tx_state[the_ram_axl/0x08] & (1<<(the_ram_axl%0x08)))!=0x00 )  // err
        {
            if(Judge_LongTime_In_MainLoop(bus_unit_err_begin_poll_clk[the_ram_axl],90)==YES) // 90*2=180s
            {
                bus_unit_err_begin_poll_clk[the_ram_axl]=Time_2048ms_Counter;
                bus_unit_poll_disable[the_ram_axl/0x08]=bus_unit_poll_disable[the_ram_axl/0x08] & ( 0xff - (1<<(the_ram_axl%0x08)) );
            }
        }
    }
    
}


void bus_scheduler()
{
  if(port_send_pt[BUS_PORT]<port_send_len[BUS_PORT])
  {
     return; 
  }   
  if((bus_state & 0x80) != 0)
  {
     return;
  }   
  if(bus_reply != 0x55)
  {
     return;
  }   

  if(bus_delay<25)
  {
     bus_delay ++;
     return;
  }

// 'compile or  not'   refer to common.h
#ifdef BH_EVENT_ALARM_ENABLE
  if(bh_event_alarm_warn!=0)
  {
      if((bh_event_alarm_warn==0x01)&&(bh_event_alarm_now_action_warn!=YES))
      {
        bh_event_alarm_warn=0x02;
        bh_event_alarm_now_action_warn=YES;
        bus_reply     = 0xaa;
        bus_reply_cnt = 0;

        bus_transmit_head();
//        port_send[BUS_PORT][6]=0x3f;
//        port_send[BUS_PORT][7]=0x30;
//        port_send[BUS_PORT][8]=0x02;
//        port_send[BUS_PORT][9]=0x00;
//        port_send[BUS_PORT][10]=CORE_CODE2_YK_TRIP;
        port_send[BUS_PORT][6]=bh_event_alarm.warn_yk_add;          //hu hongbing  2004.11.10
        port_send[BUS_PORT][7]=0x30;
        port_send[BUS_PORT][8]=0x02;
        port_send[BUS_PORT][9]=bh_event_alarm.warn_yk_no;
        port_send[BUS_PORT][10]=bh_event_alarm.warn_yk_hf;
        bus_need_exchg_to_host=NO;
        bus_send();
        return;
      }
      if(bh_event_alarm_warn==0x03)
      {
        bh_event_alarm_warn=0x04;
        bh_event_alarm_now_action_warn=NO;
        bh_event_alarm_warn_Begin_Clk=Time_2048ms_Counter;
      
        bus_transmit_head();
        port_send[BUS_PORT][6]=bh_event_alarm.warn_yk_add;
        port_send[BUS_PORT][7]=0x32;
        port_send[BUS_PORT][8]=0x02;
        port_send[BUS_PORT][9]=bh_event_alarm.warn_yk_no;
        port_send[BUS_PORT][10]=CORE_CODE2_YK_EXECUTE;
        bus_need_exchg_to_host=NO;
        bus_send();
        return;
      }
      if((bh_event_alarm_warn==0x04)&&(bh_event_alarm_now_action_warn!=YES)
       &&(Judge_LongTime_In_MainLoop(bh_event_alarm_warn_Begin_Clk,(port_info[BUS_PORT].YK_enable)/2)==YES))
      {
        bh_event_alarm_warn=0x05;
        bh_event_alarm_now_action_warn=YES;
        bus_reply     = 0xaa;
        bus_reply_cnt = 0;
      
        bus_transmit_head();
        port_send[BUS_PORT][6]=bh_event_alarm.warn_FG_yk_add;          //hu hongbing  2004.11.10
        port_send[BUS_PORT][7]=0x30;
        port_send[BUS_PORT][8]=0x02;
        port_send[BUS_PORT][9]=bh_event_alarm.warn_FG_yk_no;
        port_send[BUS_PORT][10]=bh_event_alarm.warn_FG_yk_hf;
        bus_need_exchg_to_host=NO;
        bus_send();
        return;
      }
      if(bh_event_alarm_warn==0x06)
      {
        bh_event_alarm_warn=0x00;
        bh_event_alarm_now_action_warn=NO;
        
        bus_transmit_head();
        port_send[BUS_PORT][6]=bh_event_alarm.warn_FG_yk_add;
        port_send[BUS_PORT][7]=0x32;
        port_send[BUS_PORT][8]=0x02;
        port_send[BUS_PORT][9]=bh_event_alarm.warn_FG_yk_no;
        port_send[BUS_PORT][10]=CORE_CODE2_YK_EXECUTE;
        bus_need_exchg_to_host=NO;
        bus_send();
        return;
      }
  }    
  if(bh_event_alarm_action!=0)
  {
      if((bh_event_alarm_action==0x01)&&(bh_event_alarm_now_action_warn!=YES))
      {
          bh_event_alarm_action=0x02;
          bh_event_alarm_now_action_warn=YES;
          bus_reply     = 0xaa;
          bus_reply_cnt = 0;

          bus_transmit_head();
//          port_send[BUS_PORT][6]=0x3f;
//          port_send[BUS_PORT][7]=0x30;
//          port_send[BUS_PORT][8]=0x02;
//          port_send[BUS_PORT][9]=0x00;
//          port_send[BUS_PORT][10]=CORE_CODE2_YK_CLOSE;
          port_send[BUS_PORT][6]=bh_event_alarm.action_yk_add;   //hu hongbing 2004.11.10
          port_send[BUS_PORT][7]=0x30;
          port_send[BUS_PORT][8]=0x02;
          port_send[BUS_PORT][9]=bh_event_alarm.action_yk_no;
          port_send[BUS_PORT][10]=bh_event_alarm.action_yk_hf;
          bus_need_exchg_to_host=NO;
          bus_send();
          return;
      }
      if(bh_event_alarm_action==0x03)
      {
          bh_event_alarm_action=0x04;
          bh_event_alarm_now_action_warn=NO;
          bh_event_alarm_action_Begin_Clk=Time_2048ms_Counter;
      
          bus_transmit_head();
          port_send[BUS_PORT][6]=bh_event_alarm.action_yk_add;
          port_send[BUS_PORT][7]=0x32;
          port_send[BUS_PORT][8]=0x02;
          port_send[BUS_PORT][9]=bh_event_alarm.action_yk_no;
          port_send[BUS_PORT][10]=CORE_CODE2_YK_EXECUTE;
          bus_need_exchg_to_host=NO;
          bus_send();
          return;
      }
      if((bh_event_alarm_action==0x04)&&(bh_event_alarm_now_action_warn!=YES)
       &&(Judge_LongTime_In_MainLoop(bh_event_alarm_action_Begin_Clk,60)==YES))
      {
          bh_event_alarm_action=0x05;
          bh_event_alarm_now_action_warn=YES;
          bus_reply     = 0xaa;
          bus_reply_cnt = 0;
      
          bus_transmit_head();
          port_send[BUS_PORT][6]=bh_event_alarm.action_FG_yk_add;   //hu hongbing 2004.11.10
          port_send[BUS_PORT][7]=0x30;
          port_send[BUS_PORT][8]=0x02;
          port_send[BUS_PORT][9]=bh_event_alarm.action_FG_yk_no;
          port_send[BUS_PORT][10]=bh_event_alarm.action_FG_yk_hf;
          bus_need_exchg_to_host=NO;
          bus_send();
          return;
      }
      if(bh_event_alarm_action==0x06)
      {
          bh_event_alarm_action=0x00;
          bh_event_alarm_now_action_warn=NO;
      
          bus_transmit_head();
          port_send[BUS_PORT][6]=bh_event_alarm.action_FG_yk_add;
          port_send[BUS_PORT][7]=0x32;
          port_send[BUS_PORT][8]=0x02;
          port_send[BUS_PORT][9]=bh_event_alarm.action_FG_yk_no;
          port_send[BUS_PORT][10]=CORE_CODE2_YK_EXECUTE;
          bus_need_exchg_to_host=NO;
          bus_send();
          return;
      }  
  }
#endif
// 'compile or  not'   end
  
  if(port_transmit_flag[BUS_PORT] == 0xaa)
  {
     switch(port_transmit[BUS_PORT][0])
     {
       case 0x30:
            bus_yk_source = port_transmit[BUS_PORT][1];
            bus_transmit_yk_cmd();
            bus_reply     = 0xaa;
            bus_reply_cnt = 0;
            break;
       case 0x32: case 0x33:
            bus_transmit_yk_cmd();
            bus_reply     = 0x55;
            bus_reply_cnt = 0;
            break;
       case WHOLE_PACKAGE:
            bus_transmit_whole_package();
            break;
       case PORT_EXCHANGE_STA_START:   // =0x01    common transmit info   by x.zhao
            switch(port_transmit[BUS_PORT][8])
            {
                case CORE_CODE_YK_CHOOSE:
                     port_transmit[BUS_PORT][8]=0x30;
                     bus_yk_source = port_transmit[BUS_PORT][1];
                     bus_transmit_yk_cmd();
                     bus_reply     = 0xaa;
                     bus_reply_cnt = 0;
                     break;
                case CORE_CODE_YK_EXECUTE:
                     port_transmit[BUS_PORT][8]=0x32;
                     bus_transmit_yk_cmd();
                     bus_reply     = 0x55;
                     bus_reply_cnt = 0;
                     break;
                case CORE_CODE_YK_CANCEL:
                     port_transmit[BUS_PORT][8]=0x33;
                     bus_transmit_yk_cmd();
                     bus_reply     = 0x55;
                     bus_reply_cnt = 0;
                     break;
                default:
                     break;
            }    
            break;
       default:
            break;
     }
     port_transmit_flag[BUS_PORT] = 0x55;
     return;
  }
  
        
  if((bus_rpt_state%4) != 0) //ask yx
  {
    bus_state++;

    while( (unit_info[bus_state].yx_num==0)
         ||( (bus_unit_poll_disable[bus_state/8] & (1<<(bus_state%8)))!=0 ) )
    {
      bus_state++;
      if(bus_state>0x2f)
      {
        bus_rpt_state++;
        if((bus_rpt_state % BUS_DC_LOOP)==0)  // dc or ym
        {
            bus_rpt_state = 0;
            bus_state = 0x1f;
            bus_img   = bus_state | 0x80;
            Portx_Poll_First[BUS_PORT] = YES;
            break;
        }
        else 
            if((bus_rpt_state % BUS_YC_LOOP)==0)
            {
                bus_state=0x0f;
                bus_img = bus_state | 0x80;
                break;
            }
            else
            {
                bus_state=0x20;  
                bus_img  = bus_state | 0x80;
            }
        }
    }
    if((bus_rpt_state%4)!=0)
    {
      bus_ask_yx();
      if( (bus_unit_tx_state[(bus_state & 0x7f)/8] & (1<<(bus_state%8)))!=0 )
      {
          bus_unit_poll_disable[(bus_state & 0x7f)/8]=bus_unit_poll_disable[(bus_state & 0x7f)/8] | (1<<(bus_state%8));
      }
      return;
    }
  }

  if((bus_rpt_state % BUS_DC_LOOP)==0) //ask dc or ym
  {
    bus_state++;

    while( ((unit_info[bus_state].dc_num==0)
          &&(unit_info[bus_state].ym_num==0))
         ||( (bus_unit_poll_disable[bus_state/8] & (1<<(bus_state%8)))!=0 ) )
    {
        bus_state++;
        if(bus_state>0x3f)
        {
            if(Judge_LongTime_In_MainLoop(bus_broadcast_begin_time,5)==YES)  // 1 min
            {
                if((unit_info[8].unit_type!=0x0008)&&(unit_info[8].unit_type!=0x0009))  // ought brd time
                {
                    bus_brd_time();  // add at 06/22/2002 pm
                    bus_state=0x1f;
                    bus_img = bus_state | 0x80;
                }
                else  // ought ask time
                {
                    bus_transmit_head();
                    port_send[BUS_PORT][6] = 0x08+bus_gps_addr;
                    if((unit_info[9].unit_type==0x0008)||(unit_info[9].unit_type==0x0009))
                     bus_gps_addr++;
                    if(bus_gps_addr>=2)
                     bus_gps_addr=0;
                    port_send[BUS_PORT][7] = 0x5a;
                    port_send[BUS_PORT][8] = 0x02;
                    port_send[BUS_PORT][9] = 0x0b;
                    if(unit_info[8].unit_type==0x0008)
                        port_send[BUS_PORT][10]= 0x11;
                    else  // 0x0009    
                        port_send[BUS_PORT][10]= 0x12;

                    bus_state   = 0x88;
                    bus_img     = bus_state;
                    bus_img_cnt = 0;
                    bus_need_exchg_to_host=NO;
                    bus_send();
                }
                bus_broadcast_begin_time=Time_2048ms_Counter;
            }
            else
            {
                bus_state=0x1f;
                bus_img  =bus_state | 0x80;
            }
               
            bus_rpt_state++;
            return;  
        }
    }
    if(bus_state <0x30)
    {
      bus_ask_ym();
      if( (bus_unit_tx_state[(bus_state & 0x7f)/8] & (1<<(bus_state%8)))!=0 )
      {
          bus_unit_poll_disable[(bus_state & 0x7f)/8]=bus_unit_poll_disable[(bus_state & 0x7f)/8] | (1<<(bus_state%8));
      }
    }  
    else 
    {
        if(bus_state<0x40)
        {
            bus_ask_dc();
            if( (bus_unit_tx_state[(bus_state & 0x7f)/8] & (1<<(bus_state%8)))!=0 )
            {
                bus_unit_poll_disable[(bus_state & 0x7f)/8]=bus_unit_poll_disable[(bus_state & 0x7f)/8] | (1<<(bus_state%8));
            }
        }    
        else
        {
            // ask nothing
//          bus_brd_time();
            bus_rpt_state=1;
            bus_state=0x1f;
            bus_img = bus_state | 0x80;
            bus_img_cnt = 0;
        }    
    }
    return;
  }


  if((bus_rpt_state % BUS_YC_LOOP)==0) //ask yc
  {
    bus_state++;

    while( (unit_info[bus_state].yc_line_num==0)
         ||( (bus_unit_poll_disable[bus_state/8] & (1<<(bus_state%8)))!=0 ) )
    {
      bus_state++;
      if(bus_state>0x1f)
      {
        bus_rpt_state++;
        bus_state=0x1f;
        bus_img=bus_state | 0x80;
        return;  
      }
    }
    bus_ask_yc();
    if( (bus_unit_tx_state[(bus_state & 0x7f)/8] & (1<<(bus_state%8)))!=0 )
    {
        bus_unit_poll_disable[(bus_state & 0x7f)/8]=bus_unit_poll_disable[(bus_state & 0x7f)/8] | (1<<(bus_state%8));
    }
  }
}

void bus_deal_rpt()		//-总线处理回复
{
//temp_loop   同步字数
//temp_loop1  报文长度
//temp_int    待查字长
//temp_ptS    当前位置
//temp_ptD    回零位置
//temp_lp_int 同步字
//port_check  已过滤字数
#define synword_num  temp_loop  
#define delta_len    temp_int
#define synword      temp_lp_int  
#define filter_num   port_check

  WORD  temp_bus_recv_pt;

  synword_num   = 0;     //-同步字数
  synword       = 0xeb;  //-同步字
  filter_num    = 0;     //-已过滤字数

  temp_bus_recv_pt=port_recv_pt[BUS_PORT];

  if(port_recv_dl[BUS_PORT]>temp_bus_recv_pt)
     delta_len = (temp_bus_recv_pt+512) - port_recv_dl[BUS_PORT];
  else
     delta_len = temp_bus_recv_pt - port_recv_dl[BUS_PORT];
  //-虽然现在还不知道上面程序的具体内容,但是可以肯定的是就是求绝对长度
  if(delta_len<10)       //-小于10个字节现在就不处理
     return;
  temp_ptS_B = &port_recv[BUS_PORT][port_recv_dl[BUS_PORT]];
  temp_ptD_B = &port_recv[BUS_PORT][511];  //-大胆猜测S D可能就是不同型号机器做在一个程序里的,当然也可能是错的
  while(synword_num<6)  //-同步字数小于6的处理是怎么回事啊
  {
    while(*temp_ptS_B != synword)   //-synchronize word   同步字 已经设定为了0xeb
    {
      synword_num = 0;
      temp_ptS_B ++;
      filter_num ++;
      if(temp_ptS_B>temp_ptD_B)
        temp_ptS_B -= 512;
      delta_len  --;
      if(delta_len<4)
      {
         port_recv_dl[BUS_PORT] += filter_num;
         if(port_recv_dl[BUS_PORT]>511)
             port_recv_dl[BUS_PORT] -= 512;
         return;
      }
      synword = 0xeb;
    }
    synword_num++;
    synword = SYN_WORD_BUS[synword_num];
    temp_ptS_B++;
    filter_num++;
    if(temp_ptS_B>temp_ptD_B)
       temp_ptS_B -= 512;
    delta_len--;
    if(delta_len<4)
    {
       filter_num -= synword_num;
       port_recv_dl[BUS_PORT] += filter_num;
       if(port_recv_dl[BUS_PORT]>511)
          port_recv_dl[BUS_PORT] -= 512;
       return;
    }
  }  // <6


  port_report[0] = *temp_ptS_B; /*unit_no*/
  temp_ptS_B++;
  if(temp_ptS_B>temp_ptD_B)
     temp_ptS_B -= 512;
  port_report[1] = *temp_ptS_B; /*rpt_type*/
  temp_ptS_B++;
  if(temp_ptS_B>temp_ptD_B)
     temp_ptS_B -= 512;

  if(*temp_ptS_B>0xf0)
     *temp_ptS_B = 0;
  if(delta_len<(WORD)(*temp_ptS_B+5))     //unit + tp + len + sum + tail 
  {
    filter_num -= 6;
    port_recv_dl[BUS_PORT] += filter_num;
    if(port_recv_dl[BUS_PORT]>511)
       port_recv_dl[BUS_PORT] -= 512;
    return;
  }


  temp_int = *temp_ptS_B+5;      
  for(temp_loop=2;temp_loop<temp_int;temp_loop++)
  {
     port_report[temp_loop] = *temp_ptS_B;
     temp_ptS_B++;
     if(temp_ptS_B>temp_ptD_B)
        temp_ptS_B -= 512;
  }
  port_recv_dl[BUS_PORT] += (temp_int+6); //+eb90
  if(port_recv_dl[BUS_PORT]>511)   //-我突然想到了保护装置中的,同步的问题,这里有类似,这里有却别,甚至"创新"吗
     port_recv_dl[BUS_PORT] -= 512;

  //check sum
  port_check = 0;
  temp_loop1 =  port_report[2]+3;
  for(temp_loop=0; temp_loop<temp_loop1;temp_loop++)
    port_check += port_report[temp_loop];  //-这个地方计算校验和
  
  if(port_check != port_report[temp_loop1])  //-判断计算的校验和和接收到的校验和是否相等,如果一样的话,那么认为报文有效,继续下面的处理
     return;   
//-下面是对报文的处理
// add at 06/26/2002
  if(port_report[0]<0x40)
  {
  	if( (bus_unit_tx_state[port_report[0]/8] & (1<<(port_report[0]%8)) )!=0 ) //  already err
  	{
  	    if(bus_cn_unit_tx_err[port_report[0]]>1)
  	        bus_cn_unit_tx_err[port_report[0]]=bus_cn_unit_tx_err[port_report[0]]-2;
  	    else
  	        bus_cn_unit_tx_err[port_report[0]]=0;    
  	    if(bus_cn_unit_tx_err[port_report[0]]==0)
  	    {
  	        bus_unit_tx_state[port_report[0]/8]=bus_unit_tx_state[port_report[0]/8] & (0xff-(1<<(port_report[0]%8)));
  	        bus_unit_poll_disable[port_report[0]/8]=bus_unit_poll_disable[port_report[0]/8] & (0xff-(1<<(port_report[0]%8)));
  	    }
  	}
  	else
  	{
  	    bus_cn_unit_tx_err[port_report[0]]=0x00;
  	}
  }

  if((bh_event_alarm_warn==0x02)||(bh_event_alarm_warn==0x05))
  {
      bh_event_alarm_warn++;
      bus_reply            = 0x55;
      bus_reply_cnt        = 0;
      bus_delay = 0;
      return;
  }
  if((bh_event_alarm_action==0x02)||(bh_event_alarm_action==0x05))
  { //-这些重复性的东西,有点简单的让人受不了,因为虽然简单,但是还是不会,呵呵
      bh_event_alarm_action++;
      bus_reply            = 0x55;
      bus_reply_cnt        = 0;
      bus_delay = 0;
      return;
  }
	//-下面就是各种量的,对应更新处理程序的判断入口部分      
  switch(port_report[1])   //-1里面放的特征码吗??? 是的,在系统受到报文之后把同步头已经去掉了
  { //-前面进一步排错之后,下面实际处理并开出了
     case 0x51:
        bus_state=0x1f;
        bus_img = bus_state | 0x80;

        disable();   //-为什么关中断呢
        REG_1Msecond=port_report[4]*0x100+port_report[3];
        REG_Second=port_report[5];
        REG_Minute=port_report[6];
        REG_Hour  =port_report[7];
        if(port_report[2]==6)
        {
            if((port_report[8]==1)&&(REG_Date>27))
            {
                if(REG_Month<12) 
                {
                    REG_Month++;
                }    
                else
                {
                    REG_Month=1;
                    REG_Year++;
                }    
            }    
            else
            {
                if((port_report[8]>27)&&(REG_Date==1))
                {
                    if(REG_Month==1) 
                    {
                        REG_Month=12;
                        REG_Year--;
                    }   
                    else
                    {
                        REG_Month--;
                    }    
                }    
            }
        }
        else
        {
            REG_Month=port_report[9];
            REG_Year=port_report[11]*0x100+port_report[10];
        }
            
        REG_Date  =port_report[8];
        Clock_Process();
        Write_Time_To_Dallas();
        enable();

        bus_delay = 0; 
        break;
     case 0x09:    //-IED（YC板）应答  被选上传YC量。,是否就是说这些内容是从机发送过来的
        if((bus_state&0x70)==0x10)
            bus_state &= 0x7f;
        Core_Src_Unit =  port_report[0];   //-发送来的单元地址也决定了,一个偏移量,,这些都是在ROM中事先配置好的
        Core_Src_Pt_B = &port_report[3];
        Core_Src_Len  =  port_report[2]/2;
        core_update_YC();  //-开来处理还是进行了统一处理,这些地方作为一个规约就是一个信息的处理
        bus_delay = 0; 
        break;
     case 0x0d:                        //-难道这个是内部虚拟地址
        if((bus_state&0x70)==0x30)
            bus_state &= 0x7f;
        Core_Src_Unit =  port_report[0];   //-单元地址
        Core_Src_Pt_B = &port_report[3];   //-把这个口中的数据(可能是最近接收到的实时值),更新内部对应信息,从下面看,这个里面记录的是遥测存放地址
        Core_Src_Len  =  port_report[2]/2; //-除以2的目的是把字节个数变为字个数
        core_update_DC();   //-这个地方就是把从机上送上来的量进行对应处理了
        bus_delay = 0; 
        break;
     case 0x0a: case 0x0c:  //-IED上送MON所有YC量。MON上送HOST所有YC量。 从这里看就是下级给上级上送YC信息
        if((set_transmit_flag==0x55)&&(port_report[2]<220))	//-总字节数必须小于220个
        {
            set_transmit[1]=BUS_PORT;
            set_transmit[2]=PROTOCOL_INTERNAL_BUS%0x100;
            set_transmit[3]=PROTOCOL_INTERNAL_BUS/0x100;
            set_transmit[5]=port_report[0];			//-单元地址
            if(port_report[1]==0x0a)  
            {
                set_transmit[8]=0x02;	//-HOST取IED所有YC量。MON取IED所有YC量。
            }
            else
            {		//-IED上送MON增益调节返校。MON上送HOST增益调节返校。
                set_transmit[8]=0x04;	//-HOST设置IED增益调节。MON设置IED增益调节。
            }
            
            set_transmit[9] =port_report[2]; // lenL	//-从这里可以很好的看的出来规约变化了
            set_transmit[10]=0x00;           // lenH

            for(temp_bus_recv_pt=0;temp_bus_recv_pt<port_report[2];temp_bus_recv_pt++) //-单位是字节
            {
                set_transmit[temp_bus_recv_pt+16]=port_report[temp_bus_recv_pt+3];
            }

            set_transmit_flag = 0xaa;		//-表示有数据发送了
        }
 
        bus_reply     = 0x55;
        bus_reply_cnt = 0;
        bus_delay     = 0;
        break;
     case 0x0b:   //Apply_yc_set  //-IED请求YC(选择上传)设置。 这是一个只有命令无数据的报文,当然也不需要数据
        bus_img_cnt   = 0;
        bus_assemble_yc_set();
        bus_reply     = 0xaa;
        bus_reply_cnt = 0;
        bus_delay = 0;
        break;
     case 0x0e:				//-IED上送  YC(选择上传)返校。   ,,怎么都来这么多的返校啊,可靠性吗
        temp_int = port_report[2]+1;
        for(temp_lp_int=2; temp_lp_int<temp_int;temp_lp_int++)
           if(port_report[temp_lp_int] != port_efficacy[BUS_PORT][temp_lp_int])		//-为返校预留的?
              temp_lp_int += 400;		//-只要出现不等就跳走
        if(temp_lp_int<300)		//-在这个范围内就是正常的
        {
          bus_transmit_head();
          bus_state     = bus_state & 0x7f;
          port_send[BUS_PORT][6] = bus_state;
          port_send[BUS_PORT][7] = 0xc0;	//-MON_TO_IED  YX COS,SOE确认。
          port_send[BUS_PORT][8] = 0x1;		//-上面是IED发来的返校,这个就是MON做出的确认
          port_send[BUS_PORT][9] = 0x0e;
          unit_set_flag[bus_state] = 0xaa;
          bus_need_exchg_to_host=NO;
          bus_send();
          bus_reply     = 0x55;
          bus_reply_cnt = 0;
        }
        bus_delay = 0;
        
        break;
     case 0x1a: case 0x19:		//-MON应答HOST  YX(MON数据库)。
        if((bus_state&0x70)==0x20)
            bus_state &= 0x7f;

        Core_Src_Unit =  port_report[0];
        Core_Src_Pt_B = &port_report[3];
        Core_Src_Len  =  port_report[2];
        core_update_YX();    //-猜测这里是更新,下面是接到回文之后的确认
        if(port_report[1]==0x1a)	//-IED_TO_MON  YX COS。
        {
           temp_int      = 0x1a;
           bus_confirm_yx();
        } 
        bus_delay = 0;
        break;
     case 0x1b:		//-IED_TO_MON  YX SOE。
        if((bus_state&0x70)==0x20)		//-这个地方是判断地址范围,不同范围的地址装置安排不同
            bus_state &= 0x7f;

        Core_Src_Unit =  port_report[0];
        Core_Src_Pt_B = &port_report[3];
        core_insert_SOE();
        temp_int      = 0x1b;
        bus_confirm_yx();	//-上面接收到有效的SOE事件,这里就给下位机一个信息
        bus_delay = 0;		//-大部分情况下都需要3者的配合所以标志位会更多,厉害的人就是小菜
        break; 
     case 0x1c: //apply yx set		IED请求MON  YX设置。 ,,从这里可能可以看的出来,一切语句的主语是MON
       if(ym_no_reply==NO)
        {
        bus_img_cnt   = 0;
        bus_assemble_yx_set();
        bus_reply     = 0xaa;
        bus_reply_cnt = 0;
        bus_delay = 0;
        }
       else
        ym_no_reply=NO;
        break;
     case 0x1d: //YX set verify		,IED_TO_MON  YX设置返校。
        temp_int = port_report[2]+3;
        for(temp_lp_int=2; temp_lp_int<temp_int;temp_lp_int++)
           if(port_report[temp_lp_int] != port_efficacy[BUS_PORT][temp_lp_int])
              temp_lp_int += 400;
        if(temp_lp_int<300)
        {
          bus_transmit_head();
          bus_state     = bus_state & 0x7f;
          port_send[BUS_PORT][6] = bus_state;
          port_send[BUS_PORT][7] = 0x16;
          port_send[BUS_PORT][8] = 0x0;
          port_send[BUS_PORT][9] = bus_state + 0x16;

          bus_need_exchg_to_host=NO;
          bus_send();
          bus_reply     = 0xaa;
          bus_reply_cnt = 0;
      
        }
        bus_delay = 0;		//-难道不简单吗,不还是刚才那些东西吗
        break;
     case 0x5a:		//-MON_TO_GPS申请校时。 ,,现在大胆的猜测,这版程序是被高手精简过的,但是很是留下了很多借口
        bus_brd_time();		//-校时,向所有IED下发时间
        bus_reply     = 0x55;
        bus_reply_cnt = 0;
        bus_delay = 0;
        break;
     case 0x48:		//-IED应答MON  YM。
        if((bus_state&0x70)==0x20)
          bus_state &= 0x7f;
        Core_Src_Unit =  port_report[0];
        Core_Src_Pt_B = &port_report[3];
        Core_Src_Len=   (port_report[2]-1)/4;
        core_update_YM();		//-就是表示向MON发送YM信息  即MON把信息记录在指定的存储空间中
        bus_delay = 0;
        break; 
     case 0x38:case 0x32:		//-MON_TO_IED  YK 执行/取消。 HOST_TO_MON  YK 执行/取消。 MON_TO_HOST  YK 选择返校。 IED_TO_MON  YK 选择返校。
        if((CAN_246_Trans_YK_Buf[0]==CAN_246_TRANS_YK_STA_WAIT_VERIFY)
         &&(port_report[0]==CAN_246_Trans_YK_Buf[3])
         &&(port_report[3]==CAN_246_Trans_YK_Buf[5]))		//-遥控_号
        {
            if(port_transmit_flag[PORT_NO_CAN_0]==0x55)
            {
                CAN_246_Trans_YK_Buf[0]=CAN_246_TRANS_YK_STA_TRANSING_TO_CAN;		//-不懂,好像是把信息送到了特定区域,还有后续处理
                CAN_246_Trans_YK_Buf[4]=port_report[4];  //type
            }
        }
        else
        {
            if(CAN_246_Trans_YK_Buf[0]==CAN_246_TRANS_YK_STA_WAIT_VERIFY)
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
            
            exchange_target_port = bus_yk_source;
            exchange_buf[1] = BUS_PORT;
            exchange_buf[2] = PROTOCOL_INTERNAL_BUS%0x100;
            exchange_buf[3] = PROTOCOL_INTERNAL_BUS/0x100;
            exchange_buf[5] = port_report[0];		//-单元地址
            exchange_buf[8] = CORE_CODE_YK_VERIFY;
            exchange_buf[9] = 0x02;
            exchange_buf[10]= 0x00;
            exchange_buf[17]= port_report[3];  //channel
            exchange_buf[16]= port_report[4];  //type
      
            if(exchange_target_port<14)
            {
                exchange_buf[17] += unit_info[byte0(port_info[BUS_PORT].mirror_unit) + port_report[0]].yk_start;
            }    

            Ex_Produce_Transmit_Info();
        }
        bus_reply    = 0x55;
        bus_reply_cnt= 0;
        bus_delay    = 0;
        break;
         
     default:
        bus_delay = 0;
        break;
  }
}

void bus_para_init()
{
  bus_state     = 0x1f;  //begin from yx
  bus_rpt_state = 0x01;
  bus_img       = bus_state | 0x80;
  bus_img_cnt   = 0;
  bus_reply     = 0x55;
  bus_reply_cnt = 0;
  bus_delay     = 10;
  bus_bark_time = 0;

  bh_event_alarm_now_action_warn=NO;
  bh_event_alarm_warn  =0;
  bh_event_alarm_action=0;

  bus_broadcast_begin_time=Time_2048ms_Counter;
}
















;;;以下是规约三要素函数
void Bus_Init()
{
    BYTE the_ram_axl;
    
    bus_para_init();
    
    for(the_ram_axl=0;the_ram_axl<64;the_ram_axl++)
    {
        bus_cn_unit_tx_err[the_ram_axl]=0;
        bus_unit_err[the_ram_axl] = 0;
    }
    for(the_ram_axl=0;the_ram_axl<8;the_ram_axl++)
    {
        bus_unit_tx_state[the_ram_axl]=0;
        bus_unit_poll_disable[the_ram_axl]=0;
    } 
    bus_err_rec_pt = 0;
    YM_SET_FLAG=YES;
    ym_set_time=Time_1ms_Counter;
    ym_loop=0;
    ym_add=0x20;
    ym_no_reply=NO;
    bus_gps_addr=0;
   
}















void Bus_Main()
{
    BYTE  the_ram_axl;
    BYTE *the_ram_addr_byte;
    
    bus_loop ++;
    bus_judge_poll_disable();

    if(port_recv_dl[BUS_PORT]!=port_recv_pt[BUS_PORT])
      bus_deal_rpt();
  if(YM_SET_FLAG==YES)
   {
     if(unit_info[ym_add].ym_num==0)
      {
       ym_loop+=2;
       if(ym_add<0x2f)
        ym_add++;
      }
     else
      {
       if((Judge_Time_In_MainLoop(ym_set_time,800)==YES)&&(ym_loop<0x20))
        {
         ym_set_time=Time_1ms_Counter;
         bus_assemble_ym_send_page=(ym_loop%2);
         bus_ym_set();
         ym_no_reply=YES;
         ym_loop++;
         if((ym_loop%2)==0)
          ym_add++;                   
        }
      }
     if(ym_loop>=0xc0)
       YM_SET_FLAG=NO;
   }
  else
   {
    bus_scheduler();            ;;;总线_调度
    if((YM_SET_FLAG==NO)&&(Judge_Time_In_MainLoop(ym_set_time,3000)==YES))
     {
      ym_set_time=Time_1ms_Counter;
      ym_save_to_dallas();
     }
   }
//    bus_scheduler();
    the_ram_addr_byte=(BYTE *)&YX_State[IED_TX_STATE_START_WORD+(byte0(port_info[BUS_PORT].mirror_unit))/16];  // so BUS mirror start addr must be x*16
    for(the_ram_axl=0;the_ram_axl<8;the_ram_axl++)
    {
        the_ram_addr_byte[the_ram_axl]=bus_unit_tx_state[the_ram_axl];
    }
    
}













void Bus_Monitor()
{
  BYTE  the_ram_dxl;
  
  bus_moniter ++;
  if(port_send_pt[BUS_PORT]<port_send_len[BUS_PORT])
     port_mon[BUS_PORT]++;
  else
     port_mon[BUS_PORT] = 0;
  if(port_mon[BUS_PORT]>200)
  {
    (*((far BYTE *)(SOFT_ERR_FLAG+0x0080+BUS_PORT*4)))++;
    disable();
    init_port();
    enable();
    bus_temp ++;
    port_mon[BUS_PORT] = 0;
  }

  if((bus_img == bus_state)&&(bus_reply == 0x55))
  {
    bus_img_cnt++;
    if(bus_img_cnt>10)
    {
      (*((far BYTE *)(SOFT_ERR_FLAG+0x0081+BUS_PORT*4)))++;

      if( (bus_unit_tx_state[(bus_state & 0x7f)/8] & (1<<(bus_state%8)))==0 ) //  not already err
      {
          bus_cn_unit_tx_err[(bus_state & 0x7f)]++;
          if(bus_cn_unit_tx_err[(bus_state & 0x7f)]>0x03)
          {
              bus_unit_tx_state[(bus_state & 0x7f)/8]=bus_unit_tx_state[(bus_state & 0x7f)/8] | (1<<(bus_state%8));
              bus_unit_poll_disable[(bus_state & 0x7f)/8]=bus_unit_poll_disable[(bus_state & 0x7f)/8] | (1<<(bus_state%8));
              bus_unit_err_begin_poll_clk[(bus_state & 0x7f)]=Time_2048ms_Counter;
          }
      }    
      else
      {
          bus_cn_unit_tx_err[(bus_state & 0x7f)]=0x04;
      }
      bus_img_cnt = 0;
      bus_state &= 0x7f; 
      port_recv_dl[BUS_PORT] = port_recv_pt[BUS_PORT];
      bus_err_rec[bus_err_rec_pt] = bus_state;
      bus_err_rec[bus_err_rec_pt+1] = bus_img;
      bus_err_rec_pt +=2;
      if(bus_err_rec_pt>222)
         bus_err_rec_pt = 0;
    }
  }
  else
    bus_img_cnt=0;

  if((bus_img & 0x7f) != (bus_state & 0x7f))
  {
    (*((far BYTE *)(SOFT_ERR_FLAG+0x0083+BUS_PORT*4)))++;
    bus_para_init();  //bus_init();
  }  

  if(bus_reply != 0x55)
  {
    bus_reply_cnt++;
//    if(bus_reply_cnt>200)
    if(bus_reply_cnt>20)
    { 
      if((bh_event_alarm_warn==0x02)||(bh_event_alarm_warn==0x05))
      {
          bh_event_alarm_warn=0;
          bh_event_alarm_now_action_warn=NO;
      }
      
      if((bh_event_alarm_action==0x02)||(bh_event_alarm_action==0x05))
      {
          bh_event_alarm_action=0;
          bh_event_alarm_now_action_warn=NO;
      }
      
      (*((far BYTE *)(SOFT_ERR_FLAG+0x0082+BUS_PORT*4)))++; 
      bus_reply_cnt = 0;
      bus_reply = 0x55;
      port_recv_dl[BUS_PORT] = port_recv_pt[BUS_PORT];
    }
  }
  else
    bus_reply_cnt = 0;
}

