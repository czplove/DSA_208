/*****************************************************************************/
/*       FileName  :   SET.C                                                 */
/*       Content   :   DSA-208 SET Module                                    */
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



#define   SET_MON_ADDRESS                 0x80
#define   BUS_485_PORT_NO                 0x0b
#define   SET_PORT_NO                     0xf0
#define   NORMAL_LONG_FRAME               0x38

#define   SET_WAIT_IED_TIME_OVER_VALUE    5000  // * 1 ms
#define   SET_WAIT_HOST_TIME_OVER_VALUE     15  // * 2 S

#define   SET_MON_IDLE                    0x00
#define   SET_MON_TRANSING_TO_IED         0x01
#define   SET_MON_TRANSING_TO_HOST        0x02  // no use
#define   SET_MON_WAIT_HOST_REPLY         0x03
#define   SET_MON_WAIT_IED_REPLY          0x04
                  


/************************************************/
/* Initial_CPUCOM function                      */
/************************************************/
/*===========================================================================*/
void Initial_CPUCOM(void)
{
    sp_con = 0x09;
  
    sp_baud = CPU_SIO_BAUD_TO_PC;   //0x8000 + 0xYY 
    //38400:0x19 9600:0x67 4800:0xcf 2400:19E 1200:340
    //48M
    //38400:0x4D 9600:0x137 4800:0x26F 1200:9C3

    if(Cn_SIO_CAN_Reset[PORT_NO_CPU_SIO]<0xffff)   Cn_SIO_CAN_Reset[PORT_NO_CPU_SIO]++;
}
















/************************************************/
/* CPUCOM_Send function used in short             */
/************************************************/
/*===========================================================================*/
void set_send()
{
    BYTE the_port_no;

    if(set_send_pt>=set_send_len)
    {
        if(set_port_mon_need_reply==YES) 
        {
            set_port_mon_status=SET_MON_WAIT_HOST_REPLY;       ;;;表示通讯管理机_等待_主机_回答吗
            set_rpt_recv_finish=NO;
        }    
        else
            set_port_mon_status=SET_MON_IDLE;
            
        //set_send_doing = 0x55;
        goto monitor_msg;
    }
    sbuf_tx = set_send_buf[set_send_pt];

////// for monitor port msg:  
monitor_msg:
    set_send_pt++;

    if((port_mirror[14] & 0x20)!=0)
    {
        the_port_no=port_mirror[14] & 0x0f;
        if(the_port_no<0x0c)
        {
            port_send[the_port_no][0]=set_send_buf[set_send_pt-2];
            port_send_len[the_port_no]=1;
            disable();
            port_send_begin_no_monitor(the_port_no);
            enable();
        }
    }
}












/************************************************/
/* set_check_sum function used in short           */
/************************************************/
/*===========================================================================*/
BYTE set_check_sum()
{
  WORD   sum=0;
  WORD   temp_loop;
  WORD   the_ram_bx;

  byte0(the_ram_bx)=set_recv_buf[7];
  byte1(the_ram_bx)=set_recv_buf[8];
  the_ram_bx+=9;
  for(temp_loop=0;temp_loop<the_ram_bx;temp_loop++)
  {
      sum += set_recv_buf[temp_loop];
  }
  
  if((set_recv_buf[temp_loop] == byte0(sum))&&(set_recv_buf[temp_loop+1] == byte1(sum)))
      return YES;
  else
      return NO;
}

/************************************************/
/* CPUCOM_Rece function used in short             */
/************************************************/
/*===========================================================================*/
void set_receive()
{
    BYTE temp_suf;
    BYTE the_port_no;
    WORD the_ram_ax;
    WORD the_ram_bx;

    temp_suf = sbuf_rx;

////// for monitor port msg:  
    if((port_mirror[14] & 0x10)!=0)
    {
        the_port_no=port_mirror[14] & 0x0f;
        if(the_port_no<0x0c)
        {
            port_send[the_port_no][0]=temp_suf;
            port_send_len[the_port_no]=1;
            disable();
            port_send_begin_no_monitor(the_port_no);
            enable();
        }
    }

    if((set_port_mon_status==SET_MON_WAIT_HOST_REPLY)||(set_port_mon_status==SET_MON_IDLE))
    {
        if(set_syn_char_no<4)
        {
            set_rpt_char_no = 0;
            if(temp_suf != SYN_WORD_SET[set_syn_char_no])
                set_syn_char_no = 0;
            else
                set_syn_char_no ++;
        }
        else
        { 
            set_recv_buf[set_rpt_char_no] = temp_suf;
            set_rpt_char_no ++;
            if(set_rpt_char_no>11)
            {
                the_ram_bx=set_recv_buf[8]*0x100+set_recv_buf[7];
                if(the_ram_bx>500)
                    set_syn_char_no = 0;
                else
                {    
                    if(set_rpt_char_no>(the_ram_bx+11)) 
                    {
                        //if(set_recv_buf[0]==0x80)
                        {
                            if((set_check_sum()==YES)&&(set_recv_buf[the_ram_bx+11]==SYN_WORD_SET[5]))
                                set_rpt_recv_finish = YES;
                        }
                        
                        set_syn_char_no  = 0;
                    }
                }    
            }
        }
    }
}









void set_add_sum_send_begin()
{
  WORD   sum=0;
  WORD   temp_loop;
  WORD   the_ram_bx;

  byte0(the_ram_bx)=set_send_buf[11];
  byte1(the_ram_bx)=set_send_buf[12];

  set_send_len=the_ram_bx+16;

  for(temp_loop=4;temp_loop<(the_ram_bx+9+4);temp_loop++)
  {
    sum += set_send_buf[temp_loop];            ;;;累加和
  }

  set_send_buf[temp_loop+0x00] = byte0(sum);
  set_send_buf[temp_loop+0x01] = byte1(sum);
  set_send_buf[temp_loop+0x02] = SYN_WORD_SET[5];

  set_send_pt=1;                  ;;;表示现在可以进行发送了吗
  //set_send_doing = 0xAA;
  sbuf_tx = set_send_buf[0];
}








void set_assemble_rpt_head()
{
    set_send_buf[0] = SYN_WORD_SET[0];	//-设置口同步字
    set_send_buf[1] = SYN_WORD_SET[1];	//-也是事先配置好的
    set_send_buf[2] = SYN_WORD_SET[2];
    set_send_buf[3] = SYN_WORD_SET[3];
}

void set_assemble_reply_head()
{
    set_assemble_rpt_head();
    set_send_buf[4] = set_recv_buf[1];
    set_send_buf[5] = SET_MON_ADDRESS;
    set_send_buf[6] = NORMAL_LONG_FRAME;
}


// baowen proc
// mem operation
void set_modify_mem_report()
{
  unsigned char  *mem_pt_char;
  unsigned short   temp_short;
  unsigned long  temp_long;
  unsigned short   the_ram_bx;

  byte0(temp_long)=set_recv_buf[11];
  byte1(temp_long)=set_recv_buf[12];
  byte2(temp_long)=set_recv_buf[13];
  byte3(temp_long)=set_recv_buf[14];
  
  mem_pt_char = (unsigned char  *)temp_long;

  byte0(temp_short)=set_recv_buf[7];
  byte1(temp_short)=set_recv_buf[8];
  
  temp_short-=6;
  
  for(the_ram_bx=0;the_ram_bx<temp_short;the_ram_bx++)
  {
      *mem_pt_char=set_recv_buf[the_ram_bx+15];
      mem_pt_char++;
  }
  
  set_port_mon_status=SET_MON_IDLE;
}

void set_assemble_mem_report()
{
  unsigned char  *mem_pt_char;
  unsigned short   temp_short;
  unsigned long  temp_long;
  unsigned short   the_ram_bx;
  
  byte0(temp_long)=set_recv_buf[11];
  byte1(temp_long)=set_recv_buf[12];
  byte2(temp_long)=set_recv_buf[13];
  byte3(temp_long)=set_recv_buf[14];
  
  mem_pt_char = (unsigned char  *)temp_long;

  byte0(temp_short)=set_recv_buf[15];
  byte1(temp_short)=set_recv_buf[16];

  if(temp_short>480) temp_short=480;
  set_assemble_reply_head();
  set_send_buf[7] = 0x00;    // current frame_no
  set_send_buf[8] = 0x01;    // frames num
  set_send_buf[9] = 0x70;    // code0
  set_send_buf[10] = 0x00;   // code1
  the_ram_bx=temp_short+6;
  set_send_buf[11] = byte0(the_ram_bx);   // lenL
  set_send_buf[12] = byte1(the_ram_bx);   // lenH
  set_send_buf[13] = 0xf0;   // PORT NO
  set_send_buf[14] = 0x00;   // UNIT NO
  
  set_send_buf[15] = byte0(temp_long);    
  set_send_buf[16] = byte1(temp_long);    
  set_send_buf[17] = byte2(temp_long);    
  set_send_buf[18] = byte3(temp_long);    

  for(the_ram_bx=0;the_ram_bx<temp_short;the_ram_bx++)
  {
      set_send_buf[the_ram_bx+19] = *mem_pt_char;
      mem_pt_char++;
  }    

  set_port_mon_status=SET_MON_TRANSING_TO_HOST;
  set_port_mon_need_reply=NO;
  set_add_sum_send_begin();
}


void set_reply_clk_report()
{
  BYTE   the_ram_axl;
  
  set_assemble_reply_head();
  set_send_buf[7] = 0x00;    // current frame_no
  set_send_buf[8] = 0x01;    // frames num
  set_send_buf[9] = 0x72;    // code0
  set_send_buf[10] = 0x00;   // code1
  set_send_buf[11] = 0x0b;   // lenL
  set_send_buf[12] = 0x00;   // lenH
  set_send_buf[13] = 0xf0;   // PORT NO
  set_send_buf[14] = 0x00;   // UNIT NO
  
  disable();
  the_ram_axl= REG_Year % 100;
  set_send_buf[15] = the_ram_axl/10*0x10+(the_ram_axl%10);
  the_ram_axl= REG_Year / 100;
  set_send_buf[16] = the_ram_axl/10*0x10+(the_ram_axl%10);
  set_send_buf[17] = REG_Month/10*0x10+(REG_Month % 10);    
  set_send_buf[18] = REG_Date/10*0x10+(REG_Date % 10);    
  set_send_buf[19] = REG_Hour/10*0x10+(REG_Hour % 10);    
  set_send_buf[20] = REG_Minute/10*0x10+(REG_Minute % 10);    
  set_send_buf[21] = REG_Second/10*0x10+(REG_Second % 10);    
  the_ram_axl= REG_1Msecond % 100;
  set_send_buf[22] = the_ram_axl/10*0x10+(the_ram_axl%10);
  the_ram_axl= REG_1Msecond / 100;
  set_send_buf[23] = the_ram_axl/10*0x10+(the_ram_axl%10);
  enable();

  set_port_mon_status=SET_MON_TRANSING_TO_HOST;
  set_port_mon_need_reply=NO;
  set_add_sum_send_begin();
}

void set_modify_clk_report()
{
    far BYTE *ram_base_addr;
        BYTE  ram_axl;

    disable();
    ram_base_addr=(far BYTE *)DALLAS_CONTROL;
    ram_base_addr[0]=0x80;

    ram_base_addr[7]=set_recv_buf[11];
    ram_base_addr[6]=set_recv_buf[13];
    ram_base_addr[5]=set_recv_buf[14];
    ram_base_addr[3]=set_recv_buf[15];
    ram_base_addr[2]=set_recv_buf[16];
    ram_base_addr[1]=set_recv_buf[17];
    ram_base_addr[0]=0x00;
    
    Read_Time_From_Dallas();
    
    set_port_mon_status=SET_MON_IDLE;
}

void set_announce_rpt()
{
  set_assemble_rpt_head();
  set_send_buf[4] = 0x10;
  set_send_buf[5] = SET_MON_ADDRESS;	//-0x80
  set_send_buf[6] = NORMAL_LONG_FRAME;	//-0x38

  set_send_buf[7] = 0x00;    // current frame_no
  set_send_buf[8] = 0x01;    // frames num
  set_send_buf[9] = 0xa5;    // code0
  set_send_buf[10] = 0x00;   // code1
  set_send_buf[11] = 0x04;   // lenL
  set_send_buf[12] = 0x00;   // lenH
  set_send_buf[13] = 0xf0;   // PORT NO
  set_send_buf[14] = 0x00;   // UNIT NO
  
  set_send_buf[15] = 0x01;   
  set_send_buf[16] = 0x44;   

  set_port_mon_status=SET_MON_TRANSING_TO_HOST;		//-0x02  // no use 表示MON发送到HOST吗
  set_port_mon_need_reply=NO;
  set_add_sum_send_begin();
}

void set_assemble_error_report()
{
  unsigned char  *mem_pt_char;
  unsigned short   temp_short;
  unsigned long  temp_long;
  unsigned short   the_ram_bx;
  
  set_assemble_reply_head();
  set_send_buf[7] = 0x00;     // current frame_no
  set_send_buf[8] = 0x01;     // frames num
  set_send_buf[9] = 0xaf;     // code0
  set_send_buf[10] = 0x00;    // code1
  set_send_buf[11] = 0x02;    // LENL
  set_send_buf[12] = 0x00;    // LENH
  set_send_buf[13] = set_recv_buf[5];    // code0
  set_send_buf[14] = set_recv_buf[6];    // code1

  set_port_mon_status=SET_MON_TRANSING_TO_HOST;
  set_port_mon_need_reply=NO;
  set_add_sum_send_begin();
}



// YC operation
void set_assemble_yc_port_unit()
{
  BYTE    *mem_pt_char;
  WORD     the_ram_ax;
  WORD     the_ram_bx;
  BYTE     port_no;
  BYTE     unit_no;
  
  port_no=set_recv_buf[9];
  unit_no=set_recv_buf[10];
  
  if(port_no>0x0d) 
  {
      set_port_mon_status=SET_MON_IDLE;
      return;
  }
  
  the_ram_ax=unit_info[byte0(port_info[port_no].mirror_unit)+unit_no].yc_val_num;
  if(the_ram_ax>240) the_ram_ax=240;
  mem_pt_char = (BYTE *)&YC_State[unit_info[byte0(port_info[port_no].mirror_unit)+unit_no].yc_val_start];

  set_assemble_reply_head();
  set_send_buf[7] = 0x00;    // current frame_no
  set_send_buf[8] = 0x01;    // frames num
  set_send_buf[9] = 0x01;    // code0
  set_send_buf[10] = 0x00;   // code1
  the_ram_bx=the_ram_ax*2+2;
  set_send_buf[11] = byte0(the_ram_bx);   // lenL
  set_send_buf[12] = byte1(the_ram_bx);   // lenH
  set_send_buf[13] = port_no;   // PORT NO
  set_send_buf[14] = unit_no;   // UNIT NO
  
  for(the_ram_bx=0;the_ram_bx<the_ram_ax;the_ram_bx++)
  {
      set_send_buf[the_ram_bx*2+15] = *mem_pt_char;
      mem_pt_char++;
      set_send_buf[the_ram_bx*2+16] = *mem_pt_char;
      mem_pt_char++;
  }    

  set_port_mon_status=SET_MON_TRANSING_TO_HOST;
  set_port_mon_need_reply=NO;
  set_add_sum_send_begin();
}


void set_assemble_yc_dbase_block()
{
    WORD     yc_start_no;
    WORD     yc_trans_num;
    WORD     the_ram_ax;
    WORD     the_ram_bx;
    BYTE     current_frame;
    
    current_frame=set_recv_buf[15];
    byte0(yc_start_no)=set_recv_buf[11];
    byte1(yc_start_no)=set_recv_buf[12];
    byte0(yc_trans_num)=set_recv_buf[13];
    byte1(yc_trans_num)=set_recv_buf[14];
    if(current_frame<(2048/128))
    {
        if(current_frame<(((short)yc_trans_num-1)/128+1))
        {
            set_assemble_reply_head();
            set_send_buf[7] = current_frame;                   // current frame_no
            set_send_buf[8] = (((short)yc_trans_num-1)/128)+1;    // frames num
            set_send_buf[9] = 0x01;    // code0
            set_send_buf[10] = 0x77;   // code1

            set_send_buf[13] = set_recv_buf[9];     // PORT NO
            set_send_buf[14] = set_recv_buf[10];    // UNIT NO
            if(current_frame==(((short)yc_trans_num-1)/128))
            {
                the_ram_ax=yc_trans_num-current_frame*128;
                
                yc_start_no=yc_start_no+current_frame*128;
                if(current_frame==0)
                {
                    the_ram_bx=the_ram_ax*2+2;
                    set_send_buf[11] = byte0(the_ram_bx);   // lenL
                    set_send_buf[12] = byte1(the_ram_bx);   // lenH
                    for(the_ram_bx=0;the_ram_bx<the_ram_ax;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*2+15]=byte0(YC_State[yc_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*2+16]=byte1(YC_State[yc_start_no+the_ram_bx]);
                    }    
                }
                else
                {
                    the_ram_bx=the_ram_ax*2;
                    set_send_buf[11] = byte0(the_ram_bx);   // lenL
                    set_send_buf[12] = byte1(the_ram_bx);   // lenH
                    for(the_ram_bx=0;the_ram_bx<the_ram_ax;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*2+13]=byte0(YC_State[yc_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*2+14]=byte1(YC_State[yc_start_no+the_ram_bx]);
                    }    
                }
            }
            else
            {
                yc_start_no=yc_start_no+current_frame*128;
                if(current_frame==0)
                {
                    set_send_buf[11] = 0x02;   // lenL
                    set_send_buf[12] = 0x01;   // lenH
                    for(the_ram_bx=0;the_ram_bx<128;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*2+15]=byte0(YC_State[yc_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*2+16]=byte1(YC_State[yc_start_no+the_ram_bx]);
                    }
                }
                else
                {
                    set_send_buf[11] = 0x00;   // lenL
                    set_send_buf[12] = 0x01;   // lenH
                    for(the_ram_bx=0;the_ram_bx<128;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*2+13]=byte0(YC_State[yc_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*2+14]=byte1(YC_State[yc_start_no+the_ram_bx]);
                    }    
                }
            }
            set_port_mon_status=SET_MON_TRANSING_TO_HOST;
            set_port_mon_need_reply=NO;
            set_add_sum_send_begin();
        }
        else
        {
            set_port_mon_status=SET_MON_IDLE;
        }
    }
    else
    {
        set_port_mon_status=SET_MON_IDLE;
    }
}

void set_assemble_yx_dbase_block()
{
    WORD     yx_start_no;
    WORD     yx_trans_num;
    WORD     the_ram_ax;
    WORD     the_ram_bx;
    BYTE     current_frame;
    
    current_frame=set_recv_buf[15];
    byte0(yx_start_no)=set_recv_buf[11];
    byte1(yx_start_no)=set_recv_buf[12];
    byte0(yx_trans_num)=set_recv_buf[13];
    byte1(yx_trans_num)=set_recv_buf[14];
    if(current_frame<(256/128))  // MAX YX WORDS   /   EVERY FRAME YX WORDS
    {
        if(current_frame<(((short)yx_trans_num-1)/128+1))
        {
            set_assemble_reply_head();
            set_send_buf[7] = current_frame;                   // current frame_no
            set_send_buf[8] = (((short)yx_trans_num-1)/128)+1;    // frames num
            set_send_buf[9] = 0x11;    // code0
            set_send_buf[10] = 0x77;   // code1
            set_send_buf[13] = set_recv_buf[9];     // PORT NO
            set_send_buf[14] = set_recv_buf[10];    // UNIT NO
            if(current_frame==(((short)yx_trans_num-1)/128))
            {
                the_ram_ax=yx_trans_num-current_frame*128;
                
                yx_start_no=yx_start_no+current_frame*128;
                if(current_frame==0)
                {
                    the_ram_bx=the_ram_ax*2+2;
                    set_send_buf[11] = byte0(the_ram_bx);   // lenL
                    set_send_buf[12] = byte1(the_ram_bx);   // lenH
                    for(the_ram_bx=0;the_ram_bx<the_ram_ax;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*2+15]=byte0(YX_State[yx_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*2+16]=byte1(YX_State[yx_start_no+the_ram_bx]);
                    }   
                }
                else
                {
                    the_ram_bx=the_ram_ax*2;
                    set_send_buf[11] = byte0(the_ram_bx);   // lenL
                    set_send_buf[12] = byte1(the_ram_bx);   // lenH
                    for(the_ram_bx=0;the_ram_bx<the_ram_ax;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*2+13]=byte0(YX_State[yx_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*2+14]=byte1(YX_State[yx_start_no+the_ram_bx]);
                    }   
                }
            }
            else
            {
                yx_start_no=yx_start_no+current_frame*128;
                if(current_frame==0)
                {
                    set_send_buf[11] = 0x02;   // lenL
                    set_send_buf[12] = 0x01;   // lenH
                    for(the_ram_bx=0;the_ram_bx<128;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*2+15]=byte0(YX_State[yx_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*2+16]=byte1(YX_State[yx_start_no+the_ram_bx]);
                    }   
                }
                else
                {
                    set_send_buf[11] = 0x00;   // lenL
                    set_send_buf[12] = 0x01;   // lenH
                    for(the_ram_bx=0;the_ram_bx<128;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*2+13]=byte0(YX_State[yx_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*2+14]=byte1(YX_State[yx_start_no+the_ram_bx]);
                    }   
                }
            }
            set_port_mon_status=SET_MON_TRANSING_TO_HOST;
            set_port_mon_need_reply=NO;
            set_add_sum_send_begin();
        }
        else
        {
            set_port_mon_status=SET_MON_IDLE;
        }
    }
    else
    {
        set_port_mon_status=SET_MON_IDLE;
    }
}

void set_assemble_ym_dbase_block()
{
    WORD     ym_start_no;
    WORD     ym_trans_num;
    WORD     the_ram_ax;
    WORD     the_ram_bx;
    BYTE     current_frame;
    
    current_frame=set_recv_buf[15];
    byte0(ym_start_no)=set_recv_buf[11];
    byte1(ym_start_no)=set_recv_buf[12];
    byte0(ym_trans_num)=set_recv_buf[13];
    byte1(ym_trans_num)=set_recv_buf[14];
    if(current_frame<(256/64))
    {
        if(current_frame<(((short)ym_trans_num-1)/64+1))
        {
            set_assemble_reply_head();
            set_send_buf[7] = current_frame;                   // current frame_no
            set_send_buf[8] = (((short)ym_trans_num-1)/64)+1;    // frames num
            set_send_buf[9] = 0x21;    // code0
            set_send_buf[10] = 0x77;   // code1
            set_send_buf[13] = set_recv_buf[9];     // PORT NO
            set_send_buf[14] = set_recv_buf[10];    // UNIT NO
            if(current_frame==(((short)ym_trans_num-1)/64))
            {
                the_ram_ax=ym_trans_num-current_frame*64;
                
                ym_start_no=ym_start_no+current_frame*64;
                if(current_frame==0)
                {
                    the_ram_bx=the_ram_ax*4+2;
                    set_send_buf[11] = byte0(the_ram_bx);   // lenL
                    set_send_buf[12] = byte1(the_ram_bx);   // lenH
                    for(the_ram_bx=0;the_ram_bx<the_ram_ax;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*4+15]=byte0(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+16]=byte1(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+17]=byte2(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+18]=byte3(YM_State[ym_start_no+the_ram_bx]);
                    }    
                }
                else
                {
                    the_ram_bx=the_ram_ax*4;
                    set_send_buf[11] = byte0(the_ram_bx);   // lenL
                    set_send_buf[12] = byte1(the_ram_bx);   // lenH
                    for(the_ram_bx=0;the_ram_bx<the_ram_ax;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*4+13]=byte0(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+14]=byte1(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+15]=byte2(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+16]=byte3(YM_State[ym_start_no+the_ram_bx]);
                    }    
                }
            }
            else
            {
                ym_start_no=ym_start_no+current_frame*64;
                if(current_frame==0)
                {
                    set_send_buf[11] = 0x02;   // lenL
                    set_send_buf[12] = 0x01;   // lenH
                    for(the_ram_bx=0;the_ram_bx<64;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*4+15]=byte0(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+16]=byte1(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+17]=byte2(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+18]=byte3(YM_State[ym_start_no+the_ram_bx]);
                    }    
                }
                else
                {
                    set_send_buf[11] = 0x00;   // lenL
                    set_send_buf[12] = 0x01;   // lenH
                    for(the_ram_bx=0;the_ram_bx<64;the_ram_bx++)
                    {
                        set_send_buf[the_ram_bx*4+13]=byte0(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+14]=byte1(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+15]=byte2(YM_State[ym_start_no+the_ram_bx]);
                        set_send_buf[the_ram_bx*4+16]=byte3(YM_State[ym_start_no+the_ram_bx]);
                    }    
                }
            }
            set_port_mon_status=SET_MON_TRANSING_TO_HOST;
            set_port_mon_need_reply=NO;
            set_add_sum_send_begin();
        }
        else
        {
            set_port_mon_status=SET_MON_IDLE;
        }
    }
    else
    {
        set_port_mon_status=SET_MON_IDLE;
    }
}

void set_transmit_dc_init_port_unit()
{
  BYTE    *mem_pt_char;
  BYTE     port_no;
  BYTE     unit_no;
  
  port_no=set_recv_buf[9];
  unit_no=set_recv_buf[10];
  
  if((port_no!=BUS_485_PORT_NO)||(port_transmit_flag[BUS_485_PORT_NO]==0xaa))
  {
      set_port_mon_status=SET_MON_IDLE;
      return;
  }
  
  port_transmit_flag[BUS_485_PORT_NO]=0xaa;
  mem_pt_char=&(port_transmit[BUS_485_PORT_NO][0]);
  mem_pt_char[0]=WHOLE_PACKAGE;
  mem_pt_char[1]=unit_no;
  mem_pt_char[2]=0x07;
  mem_pt_char[3]=0x00;
  
  set_port_mon_status=SET_MON_IDLE;;
}

void set_transmit_ied_yc_unit_all()
{
  BYTE    *mem_pt_char;
  BYTE     port_no;
  BYTE     unit_no;
  
  port_no=set_recv_buf[9];
  unit_no=(set_recv_buf[10] & 0x0f)+0x10;
  
  if((port_no!=BUS_485_PORT_NO)||(port_transmit_flag[BUS_485_PORT_NO]==0xaa))
  {
      set_port_mon_status=SET_MON_IDLE;
      return;
  }
  
  port_transmit_flag[BUS_485_PORT_NO]=0xaa;
  mem_pt_char=&(port_transmit[BUS_485_PORT_NO][0]);
  mem_pt_char[0]=WHOLE_PACKAGE;
  mem_pt_char[1]=unit_no;
  mem_pt_char[2]=0x02;
  mem_pt_char[3]=0x00;
  
  set_mon_begin_wait_ied_clk=Time_1ms_Counter;
  set_port_mon_status=SET_MON_WAIT_IED_REPLY;
}

void set_transmit_ied_yc_adjust()
{
  BYTE    *mem_pt_char;
  BYTE     port_no;
  BYTE     unit_no;
  BYTE     the_ram_axl;
  BYTE     the_ram_axh;
  
  port_no=set_recv_buf[9];
  unit_no=set_recv_buf[10];
  
  if((port_no!=BUS_485_PORT_NO)||(port_transmit_flag[BUS_485_PORT_NO]==0xaa))
  {
      set_port_mon_status=SET_MON_IDLE;
      return;
  }
  
  port_transmit_flag[BUS_485_PORT_NO]=0xaa;
  mem_pt_char=&(port_transmit[BUS_485_PORT_NO][0]);
  mem_pt_char[0]=WHOLE_PACKAGE;
  mem_pt_char[1]=unit_no;
  mem_pt_char[2]=0x04;
  the_ram_axh=set_recv_buf[7]-2;
  mem_pt_char[3]=the_ram_axh;
  for(the_ram_axl=0;the_ram_axl<the_ram_axh;the_ram_axl++)
      mem_pt_char[the_ram_axl+4]=set_recv_buf[the_ram_axl+11];
  
  set_mon_begin_wait_ied_clk=Time_1ms_Counter;
  set_port_mon_status=SET_MON_WAIT_IED_REPLY;
}

void set_transmit_ied_ack()
{
  BYTE    *mem_pt_char;
  BYTE     port_no;
  BYTE     unit_no;
  WORD     rece_code;
  
  port_no           =set_recv_buf[9];
  unit_no           =set_recv_buf[10];
  
  if((port_no!=BUS_485_PORT_NO)||(port_transmit_flag[BUS_485_PORT_NO]==0xaa))
  {
      set_port_mon_status=SET_MON_IDLE;
      return;
  }
  
  port_transmit_flag[BUS_485_PORT_NO]=0xaa;
  mem_pt_char=&(port_transmit[BUS_485_PORT_NO][0]);
  mem_pt_char[0]=WHOLE_PACKAGE;
  mem_pt_char[1]=unit_no;
  mem_pt_char[2]=0x06;
  mem_pt_char[3]=0x00;
  
  set_port_mon_status=SET_MON_IDLE;
}

void set_transmit_ied_nak()
{
  BYTE    *mem_pt_char;
  BYTE     port_no;
  BYTE     unit_no;
  WORD     rece_code;
  
  port_no           =set_recv_buf[9];
  unit_no           =set_recv_buf[10];
  byte1(rece_code)  =set_recv_buf[11];
  byte0(rece_code)  =set_recv_buf[12];
  
  if((port_no!=BUS_485_PORT_NO)||(port_transmit_flag[BUS_485_PORT_NO]==0xaa))
  {
      set_port_mon_status=SET_MON_IDLE;
      return;
  }
  
  port_transmit_flag[BUS_485_PORT_NO]=0xaa;
  mem_pt_char=&(port_transmit[BUS_485_PORT_NO][0]);
  mem_pt_char[0]=WHOLE_PACKAGE;
  mem_pt_char[1]=unit_no;
  if(rece_code==0x0400)
      mem_pt_char[2]=0x15;
  else
      mem_pt_char[2]=0x15;
  mem_pt_char[3]=0x00;
  
  set_port_mon_status=SET_MON_IDLE;
}

void set_assemble_dc_port_unit()
{
  BYTE    *mem_pt_char;
  WORD     the_ram_ax;
  WORD     the_ram_bx;
  BYTE     port_no;
  BYTE     unit_no;
  
  port_no=set_recv_buf[9];
  unit_no=set_recv_buf[10];
  
  if(port_no>0x0d) 
  {
      set_port_mon_status=SET_MON_IDLE;
      return;
  }
  
  the_ram_ax=unit_info[byte0(port_info[port_no].mirror_unit)+unit_no].dc_num;
  if(the_ram_ax>240) the_ram_ax=240;
  mem_pt_char = (BYTE *)&YC_State[unit_info[byte0(port_info[port_no].mirror_unit)+unit_no].dc_start];

  set_assemble_reply_head();
  set_send_buf[7] = 0x00;    // current frame_no
  set_send_buf[8] = 0x01;    // frames num
  set_send_buf[9] = 0x05;    // code0
  set_send_buf[10] = 0x00;   // code1
  the_ram_bx=the_ram_ax*2+2;
  set_send_buf[11] = byte0(the_ram_bx);   // lenL
  set_send_buf[12] = byte1(the_ram_bx);   // lenH
  set_send_buf[13] = port_no;   // PORT NO
  set_send_buf[14] = unit_no;   // UNIT NO
  
  for(the_ram_bx=0;the_ram_bx<the_ram_ax;the_ram_bx++)
  {
      set_send_buf[the_ram_bx*2+15] = *mem_pt_char;
      mem_pt_char++;
      set_send_buf[the_ram_bx*2+16] = *mem_pt_char;
      mem_pt_char++;
  }    

  set_port_mon_status=SET_MON_TRANSING_TO_HOST;
  set_port_mon_need_reply=NO;
  set_add_sum_send_begin();
}


// YX operation
void set_assemble_yx_port_unit()
{
  BYTE    *mem_pt_char;
  WORD     the_ram_ax;
  WORD     the_ram_bx;
  BYTE     port_no;
  BYTE     unit_no;
  
  port_no=set_recv_buf[9];
  unit_no=set_recv_buf[10];
  
  if(port_no>0x0d) 
  {
      set_port_mon_status=SET_MON_IDLE;
      return;
  }
  
  the_ram_ax=unit_info[byte0(port_info[port_no].mirror_unit)+unit_no].yx_num;
  if(the_ram_ax>240) the_ram_ax=240;
  mem_pt_char = (BYTE *)&YX_State[unit_info[byte0(port_info[port_no].mirror_unit)+unit_no].yx_start];

  set_assemble_reply_head();
  set_send_buf[7] = 0x00;    // current frame_no
  set_send_buf[8] = 0x01;    // frames num
  set_send_buf[9] = 0x11;    // code0
  set_send_buf[10] = 0x00;   // code1
  the_ram_bx=the_ram_ax*2+2;
  set_send_buf[11] = byte0(the_ram_bx);   // lenL
  set_send_buf[12] = byte1(the_ram_bx);   // lenH
  set_send_buf[13] = port_no;   // PORT NO
  set_send_buf[14] = unit_no;   // UNIT NO
  
  for(the_ram_bx=0;the_ram_bx<the_ram_ax;the_ram_bx++)
  {
      set_send_buf[the_ram_bx*2+15] = *mem_pt_char;
      mem_pt_char++;
      set_send_buf[the_ram_bx*2+16] = *mem_pt_char;
      mem_pt_char++;
  }    

  set_port_mon_status=SET_MON_TRANSING_TO_HOST;
  set_port_mon_need_reply=NO;
  set_add_sum_send_begin();
}


// YM operation
void set_assemble_ym_port_unit()
{
  BYTE    *mem_pt_char;
  WORD     the_ram_ax;
  WORD     the_ram_bx;
  BYTE     port_no;
  BYTE     unit_no;
  
  port_no=set_recv_buf[9];
  unit_no=set_recv_buf[10];
  
  if(port_no>0x0d) 
  {
      set_port_mon_status=SET_MON_IDLE;
      return;
  }
  
  the_ram_ax=unit_info[byte0(port_info[port_no].mirror_unit)+unit_no].ym_num;
  if(the_ram_ax>120) the_ram_ax=120;
  mem_pt_char = (BYTE *)&YM_State[unit_info[byte0(port_info[port_no].mirror_unit)+unit_no].ym_start];

  set_assemble_reply_head();
  set_send_buf[7] = 0x00;    // current frame_no
  set_send_buf[8] = 0x01;    // frames num
  set_send_buf[9] = 0x21;    // code0
  set_send_buf[10] = 0x00;   // code1
  the_ram_bx=the_ram_ax*4+2;
  set_send_buf[11] = byte0(the_ram_bx);   // lenL
  set_send_buf[12] = byte1(the_ram_bx);   // lenH
  set_send_buf[13] = port_no;   // PORT NO
  set_send_buf[14] = unit_no;   // UNIT NO
  
  for(the_ram_bx=0;the_ram_bx<the_ram_ax;the_ram_bx++)
  {
      set_send_buf[the_ram_bx*4+15] = *mem_pt_char;
      mem_pt_char++;
      set_send_buf[the_ram_bx*4+16] = *mem_pt_char;
      mem_pt_char++;
      set_send_buf[the_ram_bx*4+17] = *mem_pt_char;
      mem_pt_char++;
      set_send_buf[the_ram_bx*4+18] = *mem_pt_char;
      mem_pt_char++;
  }    

  set_port_mon_status=SET_MON_TRANSING_TO_HOST;
  set_port_mon_need_reply=NO;
  set_add_sum_send_begin();
}

void set_assemble_yk_select_port_unit()
{
  BYTE     port_no;
  BYTE     unit_no;
  
  port_no=set_recv_buf[9];
  unit_no=set_recv_buf[10];
  
  if((port_no>0x0d)||(port_transmit_flag[port_no]!=0x55))
  {
      set_port_mon_status=SET_MON_IDLE;
      return;
  }

  temp_ptS_B=&(port_transmit[port_no][0]);
  temp_ptS_B[0]=PORT_EXCHANGE_STA_START;
  temp_ptS_B[1]=SETPORT;  
  temp_ptS_B[5]=unit_no;
  temp_ptS_B[8]=CORE_CODE_YK_CHOOSE;
  temp_ptS_B[9]=0x02;
  temp_ptS_B[10]=0x00;
  temp_ptS_B[16]=set_recv_buf[11];
  temp_ptS_B[17]=set_recv_buf[12];
  
  port_transmit_flag[port_no]=0xaa;
  
  set_mon_begin_wait_ied_clk=Time_1ms_Counter;
  set_port_mon_status=SET_MON_WAIT_IED_REPLY;
}

void set_assemble_yk_select_all_port()
{
  
  exchange_buf[1] = SETPORT;              // source port
  exchange_buf[8] = CORE_CODE_YK_CHOOSE;  // CMD    operate type
  exchange_buf[9] = 0x02;                 // LEN_L
  exchange_buf[10]= 0x00;                 // LEN_H
      
  exchange_buf[16]= set_recv_buf[11];
  
  exchange_buf[5] = set_recv_buf[12]; // YK   object No.
  exchange_buf[17]= 0xff;  

  if(Ex_YK_CDTBB_ObjectNo_To_UnitAddr()==YES)  // if  YK object No. not exist do not transmit_info
  {
      port_transmit[exchange_target_port][0]=PORT_EXCHANGE_STA_START;
      Ex_Produce_Transmit_Info();
      
      set_mon_begin_wait_ied_clk=Time_1ms_Counter;
      set_port_mon_status=SET_MON_WAIT_IED_REPLY;
  }
  else
  {
      set_port_mon_status=SET_MON_IDLE;
  }
}

void set_assemble_yk_execute_port_unit()
{
  BYTE     port_no;
  BYTE     unit_no;
  
  port_no=set_recv_buf[9];
  unit_no=set_recv_buf[10];
  
  if((port_no>0x0d)||(port_transmit_flag[port_no]!=0x55))
  {
      set_port_mon_status=SET_MON_IDLE;
      return;
  }

  temp_ptS_B=&(port_transmit[port_no][0]);
  temp_ptS_B[0]=PORT_EXCHANGE_STA_START;
  temp_ptS_B[1]=SETPORT;  
  temp_ptS_B[5]=unit_no;
  if(set_recv_buf[5]==0x32)
      temp_ptS_B[8]=CORE_CODE_YK_EXECUTE;
  else
      temp_ptS_B[8]=CORE_CODE_YK_CANCEL; 
  temp_ptS_B[9]=0x02;
  temp_ptS_B[10]=0x00;
  temp_ptS_B[16]=set_recv_buf[11];
  temp_ptS_B[17]=set_recv_buf[12];
  
  port_transmit_flag[port_no]=0xaa;
  
  set_port_mon_status=SET_MON_IDLE;
}

void set_assemble_yk_execute_all_port()
{
  exchange_buf[1] = SETPORT;              // source port
  if(set_recv_buf[5]==0x32)
      exchange_buf[8] = CORE_CODE_YK_EXECUTE;  // CMD    operate type
  else
      exchange_buf[8] = CORE_CODE_YK_CANCEL;
  exchange_buf[9] = 0x02;                 // LEN_L
  exchange_buf[10]= 0x00;                 // LEN_H
      
  exchange_buf[16]= set_recv_buf[11];
  
  exchange_buf[5] = set_recv_buf[12]; // YK   object No.
  exchange_buf[17]= 0xff;  

  if(Ex_YK_CDTBB_ObjectNo_To_UnitAddr()==YES)  // if  YK object No. not exist do not transmit_info
  {
      port_transmit[exchange_target_port][0]=PORT_EXCHANGE_STA_START;
      Ex_Produce_Transmit_Info();
  }

  set_port_mon_status=SET_MON_IDLE;
}

void set_assemble_rcd_report()
{
    BYTE           rcd_type;
    BYTE           rcd_start_no;
    BYTE           rcd_num;

    BYTE           the_ram_axl;
    BYTE           the_ram_axh;
    WORD           the_ram_bx;
    BYTE          *mem_pt_char;
    
    rcd_type    =set_recv_buf[11];
    rcd_start_no=set_recv_buf[12];
    rcd_num     =set_recv_buf[13];
    if(rcd_num>16) rcd_num=16;

    set_assemble_reply_head();
    set_send_buf[7] = 0x00;    // current frame_no
    set_send_buf[8] = 0x01;    // frames num
    set_send_buf[9] = 0x78;    // code0
    set_send_buf[10] = 0x00;   // code1
    the_ram_bx=rcd_num*16+5;
    set_send_buf[11] = byte0(the_ram_bx);   // lenL
    set_send_buf[12] = byte1(the_ram_bx);   // lenH
    set_send_buf[13] = 0xf0;   // PORT NO
    set_send_buf[14] = 0x00;   // UNIT NO
    set_send_buf[15] = rcd_type;
    set_send_buf[16] = rcd_start_no;
    set_send_buf[17] = rcd_num;
    if(rcd_type==0x01)  // soe
    {
        if(yx_soe_in_pt==0)  the_ram_bx=1023;
        else                 the_ram_bx=yx_soe_in_pt-1;
        if(the_ram_bx<rcd_start_no) 
            the_ram_bx=the_ram_bx+1024-rcd_start_no;
        else
            the_ram_bx=the_ram_bx-rcd_start_no;
        the_ram_axh=0;
        while(the_ram_axh<rcd_num)
        {
            mem_pt_char=(BYTE *)(&(yx_event[the_ram_bx]));
            set_send_buf[the_ram_axh*16+18]=0x00;
            set_send_buf[the_ram_axh*16+19]=0x00;
            set_send_buf[the_ram_axh*16+20]=0x00;
            set_send_buf[the_ram_axh*16+21]=mem_pt_char[5];
            set_send_buf[the_ram_axh*16+22]=mem_pt_char[4];
            set_send_buf[the_ram_axh*16+23]=mem_pt_char[3];
            set_send_buf[the_ram_axh*16+24]=mem_pt_char[2];
            set_send_buf[the_ram_axh*16+25]=mem_pt_char[1];
            set_send_buf[the_ram_axh*16+26]=mem_pt_char[0];
            set_send_buf[the_ram_axh*16+27]=0xf0;
            set_send_buf[the_ram_axh*16+28]=0x00;
            if(mem_pt_char[7]>0x7f)
                set_send_buf[the_ram_axh*16+29]=0x01;
            else
                set_send_buf[the_ram_axh*16+29]=0x10;
            set_send_buf[the_ram_axh*16+30]=mem_pt_char[7] & 0x7f;
            set_send_buf[the_ram_axh*16+31]=mem_pt_char[6];
            set_send_buf[the_ram_axh*16+32]=0x00;
            set_send_buf[the_ram_axh*16+33]=0x00;
            
            the_ram_axh++;
            if(the_ram_bx==0) the_ram_bx=1023;
            else              the_ram_bx--;
        }    
    }
    else
    {
        if(rcd_type==0x02)  // myself
        {
            the_ram_bx =*((far BYTE *)DALLAS_RCD_MYSELF_SAV_PTR_ADDR);
            if(the_ram_bx==0)  the_ram_bx=RCD_INFO_PTR_LENGTH-1;  // 127
            else               the_ram_bx--;
            if(the_ram_bx<rcd_start_no) 
                the_ram_bx=the_ram_bx+RCD_INFO_PTR_LENGTH-rcd_start_no;
            else
                the_ram_bx=the_ram_bx-rcd_start_no;
            the_ram_axh=0;
            while(the_ram_axh<rcd_num)
            {
                mem_pt_char=(BYTE *)(DALLAS_RCD_INFO_MYSELF_ADDR+the_ram_bx*16);
                set_send_buf[the_ram_axh*16+18]=mem_pt_char[0];
                set_send_buf[the_ram_axh*16+19]=mem_pt_char[1];
                set_send_buf[the_ram_axh*16+20]=mem_pt_char[2];
                set_send_buf[the_ram_axh*16+21]=mem_pt_char[3];
                set_send_buf[the_ram_axh*16+22]=mem_pt_char[4];
                set_send_buf[the_ram_axh*16+23]=mem_pt_char[5];
                set_send_buf[the_ram_axh*16+24]=mem_pt_char[6];
                set_send_buf[the_ram_axh*16+25]=mem_pt_char[7];
                set_send_buf[the_ram_axh*16+26]=mem_pt_char[8];
                set_send_buf[the_ram_axh*16+27]=mem_pt_char[9];
                set_send_buf[the_ram_axh*16+28]=mem_pt_char[10];
                set_send_buf[the_ram_axh*16+29]=mem_pt_char[11];
                set_send_buf[the_ram_axh*16+30]=mem_pt_char[12];
                set_send_buf[the_ram_axh*16+31]=mem_pt_char[13];
                set_send_buf[the_ram_axh*16+32]=mem_pt_char[14];
                set_send_buf[the_ram_axh*16+33]=mem_pt_char[15];
            
                the_ram_axh++;
                if(the_ram_bx==0) the_ram_bx=RCD_INFO_PTR_LENGTH-1;  // 127
                else              the_ram_bx--;
            }    
        }
        else                // system
        {
            the_ram_bx =*((far BYTE *)DALLAS_RCD_SYSTEM_SAV_PTR_ADDR);
            if(the_ram_bx==0)  the_ram_bx=RCD_INFO_PTR_LENGTH-1;  // 127
            else               the_ram_bx--;
            if(the_ram_bx<rcd_start_no) 
                the_ram_bx=the_ram_bx+RCD_INFO_PTR_LENGTH-rcd_start_no;
            else
                the_ram_bx=the_ram_bx-rcd_start_no;
            the_ram_axh=0;
            while(the_ram_axh<rcd_num)
            {
                mem_pt_char=(BYTE *)(DALLAS_RCD_INFO_SYSTEM_ADDR+the_ram_bx*16);
                set_send_buf[the_ram_axh*16+18]=mem_pt_char[0];
                set_send_buf[the_ram_axh*16+19]=mem_pt_char[1];
                set_send_buf[the_ram_axh*16+20]=mem_pt_char[2];
                set_send_buf[the_ram_axh*16+21]=mem_pt_char[3];
                set_send_buf[the_ram_axh*16+22]=mem_pt_char[4];
                set_send_buf[the_ram_axh*16+23]=mem_pt_char[5];
                set_send_buf[the_ram_axh*16+24]=mem_pt_char[6];
                set_send_buf[the_ram_axh*16+25]=mem_pt_char[7];
                set_send_buf[the_ram_axh*16+26]=mem_pt_char[8];
                set_send_buf[the_ram_axh*16+27]=mem_pt_char[9];
                set_send_buf[the_ram_axh*16+28]=mem_pt_char[10];
                set_send_buf[the_ram_axh*16+29]=mem_pt_char[11];
                set_send_buf[the_ram_axh*16+30]=mem_pt_char[12];
                set_send_buf[the_ram_axh*16+31]=mem_pt_char[13];
                set_send_buf[the_ram_axh*16+32]=mem_pt_char[14];
                set_send_buf[the_ram_axh*16+33]=mem_pt_char[15];
            
                the_ram_axh++;
                if(the_ram_bx==0) the_ram_bx=RCD_INFO_PTR_LENGTH-1;  // 127
                else              the_ram_bx--;
            }    
        }
    }
  
    set_port_mon_status=SET_MON_TRANSING_TO_HOST;
    set_port_mon_need_reply=NO;
    set_add_sum_send_begin();
}

void set_assemble_channal_msg_report()
{
    BYTE           the_ram_axl;
    BYTE           the_ram_axh;
    WORD           the_ram_bx;
    BYTE           the_port_no;
    BYTE           the_unit_no;
    
    the_port_no=set_recv_buf[9];   // port_no
    the_unit_no=set_recv_buf[10];  // unit_no
	if(the_port_no>0x0f) 
	{
		set_assemble_error_report();
		return;
	}	
	for(the_ram_axl=0;the_ram_axl<16;the_ram_axl++)
	{
		port_mirror[the_ram_axl]=0x0f;
	}
	
	//port_mirror[0x0e]=0x3a;
	if(the_unit_no==0x01)
	    port_mirror[the_port_no]=0x1e;
	if(the_unit_no==0x02)
	    port_mirror[the_port_no]=0x2e;
	if(the_unit_no==0x03)
	    port_mirror[the_port_no]=0x3e;
	
    set_assemble_reply_head();
    set_send_buf[7] = 0x00;    // current frame_no
    set_send_buf[8] = 0x01;    // frames num
    set_send_buf[9] = 0x79;    // code0
    set_send_buf[10] = 0x00;   // code1
    the_ram_axh=set_port_rs_buf_sav_pt;
    the_ram_axl=set_port_rs_buf_tak_pt+(BYTE)(0x100-the_ram_axh);
    the_ram_bx=the_ram_axl+2;
    set_send_buf[11] = byte0(the_ram_bx);   // lenL
    set_send_buf[12] = byte1(the_ram_bx);   // lenH
    set_send_buf[13] = the_port_no;   // PORT NO
    set_send_buf[14] = the_unit_no;   // UNIT NO
    
    for(the_ram_axh=0;the_ram_axh<the_ram_axl;the_ram_axh++)
    {
    	set_send_buf[the_ram_axh+15]=set_port_rece_send_buf[set_port_rs_buf_tak_pt];
    	set_port_rs_buf_tak_pt++;
    }
    
    set_port_mon_status=SET_MON_TRANSING_TO_HOST;
    set_port_mon_need_reply=NO;
    set_add_sum_send_begin();
}


void set_deal_rpt(void)         ;;;口处理从发 还是报表?
{
  unsigned register short  tempx; 
  WORD     rece_code;
  
  byte1(rece_code)=set_recv_buf[5];
  byte0(rece_code)=set_recv_buf[6];        ;;;组合成了字
  
  switch(rece_code)
  {
      case 0x0100:
           set_assemble_yc_port_unit();         ;;;这个难道就是向后台发送数据的准备口吗????
           break;
      case 0x0177:
           set_assemble_yc_dbase_block();
           break;
      case 0x0200:
           set_transmit_ied_yc_unit_all();
           break;
      case 0x0400:
           set_transmit_ied_yc_adjust();
           break;
      case 0x0604:
           set_transmit_ied_ack();
           break;
      case 0x1500:
           set_transmit_ied_nak();
           break;
      case 0x0500:
           set_assemble_dc_port_unit();
           break;
      case 0x0700:
           set_transmit_dc_init_port_unit();
           break;
      case 0x1100:
           set_assemble_yx_port_unit();
           break;
      case 0x1177:
           set_assemble_yx_dbase_block();
           break;
      case 0x2100:
           set_assemble_ym_port_unit();
           break;
      case 0x2177:
           set_assemble_ym_dbase_block();
           break;
      case 0x3000:
           set_assemble_yk_select_port_unit();
           break;
      case 0x307F:
           set_assemble_yk_select_all_port();
           break;
      case 0x3200:
      case 0x3300:
           set_assemble_yk_execute_port_unit();
           break;
      case 0x327f:
      case 0x337f:
           set_assemble_yk_execute_all_port();
           break;
      case 0x7000:
           set_assemble_mem_report();
           break;
      case 0x7100:
           set_modify_mem_report();
           break;
      case 0x7200:
           set_reply_clk_report();
           break;
      case 0x7300:
           set_modify_clk_report();
           break;
      case 0x7800:
           set_assemble_rcd_report();
           break;
      case 0x7900:
           set_assemble_channal_msg_report();
           break;
      case 0xce00:
           set_assemble_reply_head();
           set_send_buf[7] = 0x00;    // current frame_no
           set_send_buf[8] = 0x01;    // frames num
           set_send_buf[9] = 0xce;    // code0
           set_send_buf[10] = 0x00;   // code1
           set_send_buf[11] = 0x03;   // lenL
           set_send_buf[12] = 0x00;   // lenH
           set_send_buf[13] = printf_info.bh_pannel;   
           set_send_buf[14] = printf_info.bh_unit;
           set_send_buf[15] = printf_info.bhdz_printf_flag;     
           set_port_mon_status=SET_MON_TRANSING_TO_HOST;
           set_port_mon_need_reply=NO;
           set_add_sum_send_begin();
           break;
      case 0xce01:
           printf_info.bh_pannel=set_recv_buf[9];
           printf_info.bh_unit=set_recv_buf[10];
           printf_info.bhdz_printf_flag=set_recv_buf[11];
           set_assemble_reply_head();
           set_send_buf[7] = 0x00;    // current frame_no
           set_send_buf[8] = 0x01;    // frames num
           set_send_buf[9] = 0xce;    // code0
           set_send_buf[10] = 0x00;   // code1
           set_send_buf[11] = 0x03;   // lenL
           set_send_buf[12] = 0x00;   // lenH
           set_send_buf[13] = printf_info.bh_pannel;   
           set_send_buf[14] = printf_info.bh_unit;
           set_send_buf[15] = printf_info.bhdz_printf_flag;     
           set_port_mon_status=SET_MON_TRANSING_TO_HOST;
           set_port_mon_need_reply=NO;
           set_add_sum_send_begin();
           break;
      case 0xcf00:
           set_assemble_reply_head();
           set_send_buf[7] = 0x00;    // current frame_no
           set_send_buf[8] = 0x01;    // frames num
           set_send_buf[9] = 0xcf;    // code0
           set_send_buf[10] = 0x00;   // code1
           set_send_buf[11] = 0x02;   // lenL
           set_send_buf[12] = 0x00;   // lenH
           set_send_buf[13] = printf_info.bh_printf_flag;   
           set_send_buf[14] = printf_info.soe_printf_flag;
           set_port_mon_status=SET_MON_TRANSING_TO_HOST;
           set_port_mon_need_reply=NO;
           set_add_sum_send_begin();
           break;
      case 0xcf01:
           printf_info.bh_printf_flag=set_recv_buf[9];
           printf_info.soe_printf_flag=set_recv_buf[10];
           set_assemble_reply_head();
           set_send_buf[7] = 0x00;    // current frame_no
           set_send_buf[8] = 0x01;    // frames num
           set_send_buf[9] = 0xcf;    // code0
           set_send_buf[10] = 0x01;   // code1
           set_send_buf[11] = 0x02;   // lenL
           set_send_buf[12] = 0x00;   // lenH
           set_send_buf[13] = printf_info.bh_printf_flag;   
           set_send_buf[14] = printf_info.soe_printf_flag;
           set_port_mon_status=SET_MON_TRANSING_TO_HOST;
           set_port_mon_need_reply=NO;
           set_add_sum_send_begin();
           break;
      case 0xa500:
           if((set_recv_buf[11]==0x01)&&(set_recv_buf[12]==0x55))
           {
               disable();
               while(1)
               {
               }
           }
           else
           {
           	   if((set_recv_buf[11]==0x01)&&(set_recv_buf[12]==0x00))
           	   	   set_announce_rpt();
           }
           break;

      default:
           set_assemble_error_report();
           break;
  }

}










void set_main()                       ;;;设置口
{
    BYTE   temp_rnd,*temp_pt,*temp_pt1;    ;;;临时
    

    if(set_port_mon_status==SET_MON_IDLE)             //0	最初的时候值为SET_MON_TRANSING_TO_HOST
    {
        if(set_rpt_recv_finish==YES)
        {   
            set_deal_rpt();
            set_rpt_recv_finish = NO;       ;;;是YES处理好之后就给NO
        }
    }
    else
    {
        if(set_port_mon_status==SET_MON_WAIT_IED_REPLY)             ;;;4   设置定值现在等待智能装置的回答,难道是那个讲的反复切换端口模式
        {
            if(set_transmit_flag==0xaa)        ;;;判断现在的发送标志,通用AA表示通,55表示断吗
            {
                set_transmit_flag = 0x55;
                
                set_assemble_reply_head();      ;;;组和回答头
                set_send_buf[7]  = 0x00;    // current frame_no
                set_send_buf[8]  = 0x01;    // frames num

                if(set_transmit[8]==CORE_CODE_YK_VERIFY)
                {
                    set_send_buf[9] =0x38;  // code0
                    set_send_buf[10]=0x00;  // code1
                    set_send_buf[11]=0x04;
                    set_send_buf[12]=0x00;
                    if(set_send_buf[10]==0x7f)
                    {
                        set_send_buf[13]=0xf0; // port_no
                        set_send_buf[14]=0x00; // unit_no         ;;;这个单元号有柜号的意思吗
                        set_send_buf[15]=set_transmit[16];
                        set_send_buf[16]=set_transmit[17]
                            +unit_info[byte0(port_info[set_transmit[1]].mirror_unit) + set_transmit[5]].yk_start;   ;;;选择了单元的绝对地址
                    }
                    else
                    {
                        set_send_buf[13]=set_transmit[1];
                        set_send_buf[14]=set_transmit[5];
                        set_send_buf[15]=set_transmit[16];
                        set_send_buf[16]=set_transmit[17];
                    }

                    set_port_mon_need_reply=NO;       ;;;表示YK已经确认了,不需要了吗
                }
                else
                {
                    if((set_transmit[2]==(PROTOCOL_INTERNAL_BUS%0x100))
                     &&(set_transmit[3]==(PROTOCOL_INTERNAL_BUS/0x100)))  ;;;判断规约号是否对,即进行规约匹配
                    {                                                     ;;;是这个规约就进行如下处理,不是的就结束
                        if(set_transmit[8]==0x02)
                        {
                            set_send_buf[9] =0x02;  // code0
                            set_send_buf[10]=0x00;  // code1
                        }
                        else
                        {
                            if(set_transmit[8]==0x04)
                            {
                                set_send_buf[9] =0x04;  // code0
                                set_send_buf[10]=0x00;  // code1
                            }
                            else
                            {
                                set_send_buf[9] =0xaf;  // code0  err code      ;;;错误
                                set_send_buf[10]=0x00;  // code1  err cause     ;;;原因
                                
                                set_transmit[9] =2;
                                set_transmit[10]=0;
                                set_transmit[16]=set_transmit[8];
                                set_transmit[17]=0;
                            }
                        }
                        
                        byte0(temp_loop1)= set_transmit[9];
                        byte1(temp_loop1)= set_transmit[10];
                        if(temp_loop1>220) temp_loop1=220;
                        temp_loop=temp_loop1+2;
                        set_send_buf[11]=byte0(temp_loop);
                        set_send_buf[12]=byte1(temp_loop);
                        
                        set_send_buf[13]=set_transmit[1];
                        set_send_buf[14]=set_transmit[5];
                        
                        for(temp_loop=0;temp_loop<temp_loop1;temp_loop++)
                        {
                            set_send_buf[temp_loop+15]=set_transmit[temp_loop+16];
                        }
                    }
                    set_port_mon_need_reply=NO;
                }
                
                set_port_mon_status=SET_MON_TRANSING_TO_HOST;
                set_add_sum_send_begin();
            }
            else          ;;;猜测不等于AA就是55,那么还没有发送就判断在超时之前有没有
            {
                if(Judge_Time_In_MainLoop(set_mon_begin_wait_ied_clk,SET_WAIT_IED_TIME_OVER_VALUE)==YES)
                {
                    set_port_mon_status=SET_MON_IDLE;           ;;;超过了就给0
                }
            }
        }
        else
        {
            if(set_port_mon_status==SET_MON_WAIT_HOST_REPLY)        ;;;3
            {
                set_port_mon_status=SET_MON_IDLE;              ;;;满足这个条件就给他
            }
        }
    }
    
}











/************************************************/
/* set_init       function                      */
/************************************************/
/*===========================================================================*/
void set_init(void)                             ;;;可能就是一个供调用的子程序集
{
    /*IO port init in main*/
    unsigned short tempset;

    set_port_mon_status=SET_MON_IDLE;	//-整定口MON状态
    set_port_mon_need_reply=NO;		//-初始状态不需要回复
    
    set_syn_char_no = 0;
    set_rpt_char_no = 0;
    set_rpt_recv_finish = NO;
    
    for(tempset=0;tempset<512;tempset++)
        set_recv_buf[tempset] = 0;		//-接收缓冲区初始为0
        
    set_send_len = 0;
    set_send_pt  = 0;
    set_transmit_flag = 0x55;	//-最初为什么为55呢
    
    set_port_rs_buf_tak_pt=0;
    set_port_rs_buf_sav_pt=0;
    printf_info.bhdz_printf_flag=0x55;
    printf_info.bh_printf_flag=0x55;
    printf_info.soe_printf_flag=0x55;
    printf_info.bh_pannel=0;
    printf_info.bh_unit=0;
}

