/*****************************************************************************/
/*       FileName  :   MODBUS.C                                              */
/*       Content   :   DSA-208 MODBUS Module                                 */
/*       Date      :   Wed  07-11-2002                                       */
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




    
    
    
/*---------------------------------------------------------------------------*/
/*                    Definition  of  global  variables                      */
/*---------------------------------------------------------------------------*/

//    none


/*---------------------------------------------------------------------------*/
/*                    Definition  of  local  variables                       */
/*---------------------------------------------------------------------------*/
register  far  WORD                         *MODBUS_PARA_BASE_ADDR;

#define   MODBUS_PORT_NO                     port_no

#define   MODBUS_YC_FRAME                   0x01
#define   MODBUS_YX_FRAME                   0x02
#define   MODBUS_TS_FRAME                   0x04 
#define   MODBUS_YK_FRAME                   0x08

#define   TIME_MODBUS_VERIFY_TIME_VALUE     150    // 2048ms*150=307s
#define   MODBUS_WAIT_TIME_VALUE            2000    //等待接收时间 1ms*500
#define   MODBUS_YK_FX_WAIT_TIME_VALUE      10    //遥控等待返校时间 2048ms*15

#define   MODBUS_begin_veriiedclk_time           (*(MODBUS_PARA_BASE_ADDR+0x0000))
#define   MODBUS_rxd_wait_time                   (*(MODBUS_PARA_BASE_ADDR+0x0004))
#define   MODBUS_transmit_disable_begin_time     (*(MODBUS_PARA_BASE_ADDR+0x0005))
#define   MODBUS_rxd_head_flag              byte0(*(MODBUS_PARA_BASE_ADDR+0x0006))
#define   MODBUS_transmit_control           byte1(*(MODBUS_PARA_BASE_ADDR+0x0006)) 
#define   MODBUS_comm_err_counter                 (MODBUS_PARA_BASE_ADDR+0x0028) //0x0008-0x0027 
//receive
#define   MODBUS_transmit_yk_host_port      byte0(*(MODBUS_PARA_BASE_ADDR+0x0048))
#define   MODBUS_transmit_task_busy         byte1(*(MODBUS_PARA_BASE_ADDR+0x0048))
#define   MODBUS_now_poll_addr              byte0(*(MODBUS_PARA_BASE_ADDR+0x0049))
#define   MODBUS_transmit_flag              byte1(*(MODBUS_PARA_BASE_ADDR+0x0049))
#define   MODBUS_rec_OK                     byte0(*(MODBUS_PARA_BASE_ADDR+0x004a))
#define   MODBUS_begin_addr                 byte0(*(MODBUS_PARA_BASE_ADDR+0x004b))
#define   MODBUS_end_addr                   byte1(*(MODBUS_PARA_BASE_ADDR+0x004b))
#define   MODBUS_YK_fx_wait_time                 (*(MODBUS_PARA_BASE_ADDR+0x004c))
#define   MODBUS_YK_fx_flag                 byte0(*(MODBUS_PARA_BASE_ADDR+0x004d))
#define   MODBUS_YK_poll_addr               byte1(*(MODBUS_PARA_BASE_ADDR+0x004d))
#define   MODBUS_transmit_wait_time              (*(MODBUS_PARA_BASE_ADDR+0x004e))
#define   MODBUS_begin_zf_disable_time           (*(MODBUS_PARA_BASE_ADDR+0x004f))
#define   MODBUS_wait_replay                byte0(*(MODBUS_PARA_BASE_ADDR+0x0050))
#define   MODBUS_rec_frame_type             byte1(*(MODBUS_PARA_BASE_ADDR+0x0050))



/*---------------------------------------------------------------------------*/
/*                        IMPORT            functions                        */
/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/
/************************************************/
/* MODBUS_MODBUS_CRC16_cal         function              */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
unsigned int MODBUS_CRC16(unsigned char *MODBUS_CRC16_start,unsigned char MODBUS_CRC16_bytes)    //*x为指向每行前5个数据的指针
{
unsigned int bx, cx, i, j;

    bx = 0xffff;
    cx = 0xa001;
    for(i=0;i<MODBUS_CRC16_bytes;i++)
     {
      bx=bx^MODBUS_CRC16_start[i];
      for(j=0;j<8;j++)
       {
        if ((bx&0x0001)==1)
         {
          bx=bx>>1;
          bx=bx&0x7fff;
          bx=bx^cx;
         }
        else
         {
          bx=bx>>1;
          bx=bx&0x7fff;
         }
       }
     }
    return(bx);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MODBUS_fcb_check         function              */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static BYTE MODBUS_not_comm_count(void)
{
    BYTE  the_ram_axl;
    BYTE  the_ram_axh;
    the_ram_axl=port_report[0];
    the_ram_axh=port_send[MODBUS_PORT_NO][0];
    if(the_ram_axl!=the_ram_axh)
      return YES;
    else 
      return NO;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MODBUS_transmit_yk_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MODBUS_transmit_yk_cmd(void)  
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MODBUS_PORT_NO]);
    
    temp_ptS_B[0] =port_transmit[MODBUS_PORT_NO][5]+1;
    temp_ptS_B[1] =0x05;
    if(port_transmit[MODBUS_PORT_NO][8]==CORE_CODE_YK_CHOOSE)
     {
      if(port_transmit[MODBUS_PORT_NO][16]==CORE_CODE2_YK_CLOSE) 
       {
        the_ram_ax=MODBUS_info[port_transmit[MODBUS_PORT_NO][5]+1].yk_address+2*port_transmit[MODBUS_PORT_NO][17]+1;
        temp_ptS_B[2] =byte1(the_ram_ax); //遥控合闸位地址
        temp_ptS_B[3] =byte0(the_ram_ax);      	
       }
      else
       if(port_transmit[MODBUS_PORT_NO][16]==CORE_CODE2_YK_TRIP)
      	{
         the_ram_ax=MODBUS_info[port_transmit[MODBUS_PORT_NO][5]+1].yk_address+2*port_transmit[MODBUS_PORT_NO][17];
         temp_ptS_B[2] =byte1(the_ram_ax); //遥控分闸位地址
         temp_ptS_B[3] =byte0(the_ram_ax);      	 
      	}
     }	
    temp_ptS_B[4] =0xff;
    temp_ptS_B[5] =0x00;
    the_ram_ax=MODBUS_CRC16(temp_ptS_B,6);
    temp_ptS_B[6] =byte0(the_ram_ax);
    temp_ptS_B[7] =byte1(the_ram_ax);    
    MODBUS_YK_poll_addr=temp_ptS_B[0];
    port_send_len[MODBUS_PORT_NO]=8;    
    port_send_begin();
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MODBUS_YC_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MODBUS_YC_cmd(BYTE slave_adr)  
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MODBUS_PORT_NO]);
    temp_ptS_B[0] =slave_adr;
    temp_ptS_B[1] =0x03;
    temp_ptS_B[2] =byte1(MODBUS_info[slave_adr].yc_address); //YC起始地址
    temp_ptS_B[3] =byte0(MODBUS_info[slave_adr].yc_address);
    temp_ptS_B[4] =byte1(MODBUS_info[slave_adr].yc_num);  //访问YC区字数
    temp_ptS_B[5] =byte0(MODBUS_info[slave_adr].yc_num);     
    the_ram_ax=MODBUS_CRC16(temp_ptS_B,6);
    temp_ptS_B[6] =byte0(the_ram_ax);
    temp_ptS_B[7] =byte1(the_ram_ax);
    port_send_len[MODBUS_PORT_NO]=8;    
    port_send_begin();
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MODBUS_YX_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MODBUS_YX_cmd(BYTE slave_adr)  
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MODBUS_PORT_NO]);
    temp_ptS_B[0] =slave_adr;
    temp_ptS_B[1] =0x03;
    temp_ptS_B[2] =byte1(MODBUS_info[slave_adr].yx_address); //YX起始地址
    temp_ptS_B[3] =byte0(MODBUS_info[slave_adr].yx_address);
    temp_ptS_B[4] =byte1(MODBUS_info[slave_adr].yx_num);  //访问YX区字数
    temp_ptS_B[5] =byte0(MODBUS_info[slave_adr].yx_num);     
    the_ram_ax=MODBUS_CRC16(temp_ptS_B,6);
    temp_ptS_B[6] =byte0(the_ram_ax);
    temp_ptS_B[7] =byte1(the_ram_ax);
    port_send_len[MODBUS_PORT_NO]=8;    
    port_send_begin();
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MODBUS_TS_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MODBUS_TS_cmd(BYTE slave_adr)  
{
    WORD the_ram_ax;
    
    temp_ptS_B=(far BYTE *)&(port_send[MODBUS_PORT_NO]);
    temp_ptS_B[0] =slave_adr;
    temp_ptS_B[1] =0x03;
    temp_ptS_B[2] =byte1(MODBUS_info[slave_adr].ts_address); //TS起始地址
    temp_ptS_B[3] =byte0(MODBUS_info[slave_adr].ts_address);
    temp_ptS_B[4] =byte1(MODBUS_info[slave_adr].ts_num);  //访问TS区字数
    temp_ptS_B[5] =byte0(MODBUS_info[slave_adr].ts_num);     
    the_ram_ax=MODBUS_CRC16(temp_ptS_B,6);
    temp_ptS_B[6] =byte0(the_ram_ax);
    temp_ptS_B[7] =byte1(the_ram_ax);
    port_send_len[MODBUS_PORT_NO]=8;    
    port_send_begin();
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MODBUS_transmit_time_cmd    function           */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MODBUS_transmit_time_cmd(void)  
{
    WORD the_ram_ax;
    temp_ptS_B=(far BYTE *)&(port_send[MODBUS_PORT_NO]);
    temp_ptS_B[0] =0x00;
    temp_ptS_B[1] =0x10;
    temp_ptS_B[2] =0x00;
    temp_ptS_B[3] =0x02;
    temp_ptS_B[4] =0x00;
    temp_ptS_B[5] =0x04;
    temp_ptS_B[6] =0x08;
    temp_ptS_B[7] =0x00;
    the_ram_ax=REG_Year % 100;
    temp_ptS_B[8] =byte0(the_ram_ax);
    temp_ptS_B[9] =REG_Month;
    temp_ptS_B[10]=REG_Date;
    temp_ptS_B[11]=REG_Hour;            
    temp_ptS_B[12]=REG_Minute;
    the_ram_ax=REG_Second*1000+REG_1Msecond;                   
    temp_ptS_B[13]=byte1(the_ram_ax);
    temp_ptS_B[14]=byte0(the_ram_ax);
    the_ram_ax=MODBUS_CRC16(temp_ptS_B,15);
    temp_ptS_B[15] =byte0(the_ram_ax);
    temp_ptS_B[16] =byte1(the_ram_ax);
    port_send_len[MODBUS_PORT_NO]=17;    
    port_send_begin();
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/************************************************/
/* MODBUS_scheduler      function               */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void MODBUS_scheduler(void)
{
    #define    delta_len            temp_lp_int
    #define    bv_yx_no             port_check

    far  BYTE *temp_fptS;
         WORD  the_ram_ax;
         WORD  the_ram_bx;
         BYTE  temp_axl;
    
    temp_fptS=(far BYTE *)&(port_send[MODBUS_PORT_NO]);
      
    if((port_recv_pt[MODBUS_PORT_NO]!=port_recv_dl[MODBUS_PORT_NO])&&(MODBUS_wait_replay==YES))
     {
      if(MODBUS_rxd_head_flag==NO)
       {
       	disable();
       	if(port_recv_dl[MODBUS_PORT_NO]>port_recv_pt[MODBUS_PORT_NO])
         delta_len=(port_recv_pt[MODBUS_PORT_NO]+512)-port_recv_dl[MODBUS_PORT_NO];
        else
         delta_len=port_recv_pt[MODBUS_PORT_NO]-port_recv_dl[MODBUS_PORT_NO];
       	enable();
        for(temp_loop=port_recv_dl[MODBUS_PORT_NO];temp_loop<(delta_len+port_recv_dl[MODBUS_PORT_NO]);temp_loop++)
         {
       	  if(port_recv[MODBUS_PORT_NO][temp_loop]==port_send[MODBUS_PORT_NO][0])
       	   {
       	    the_ram_ax=(port_recv_dl[temp_loop]+1)&0x1ff;
       	    if((port_recv[MODBUS_PORT_NO][the_ram_ax]==3)||(port_recv[MODBUS_PORT_NO][the_ram_ax]==5))
       	     {
       	      MODBUS_rxd_head_flag=YES;
       	      break;
       	     }
       	   }  
       	  port_recv_dl[MODBUS_PORT_NO]++;
       	  port_recv_dl[MODBUS_PORT_NO]&=0x1ff;       	        	 
         }
       }
      if(MODBUS_rxd_head_flag==YES)
       {       	
       	disable();
       	if(port_recv_dl[MODBUS_PORT_NO]>port_recv_pt[MODBUS_PORT_NO])
         delta_len=(port_recv_pt[MODBUS_PORT_NO]+512)-port_recv_dl[MODBUS_PORT_NO];
        else
         delta_len=port_recv_pt[MODBUS_PORT_NO]-port_recv_dl[MODBUS_PORT_NO];
       	enable();
        if(delta_len>=3)
         {
          the_ram_ax=(port_recv_dl[MODBUS_PORT_NO]+1)&0x1ff;
          if(port_recv[MODBUS_PORT_NO][the_ram_ax]==3)
           {
           temp_int=(port_recv_dl[MODBUS_PORT_NO]+2)&0x1ff;
           if(delta_len>=(unsigned short)(port_recv[MODBUS_PORT_NO][temp_int]+5))
            {
             MODBUS_rxd_head_flag=NO;
             MODBUS_wait_replay=NO;
             MODBUS_rec_OK=YES;             
             goto rec_ok_deal;
            }
           }
          else
           if(delta_len>=8)
            {
             MODBUS_rxd_head_flag=NO;
             MODBUS_wait_replay=NO;
             MODBUS_rec_OK=YES;             
             goto rec_ok_deal;
            }            
         }
       }                      
     }         
     goto rxd_out_time;
rec_ok_deal: 
    if(MODBUS_rec_OK==YES)
     {
      MODBUS_rec_OK=NO;
      MODBUS_transmit_flag=YES;
      MODBUS_transmit_wait_time=Time_1ms_Counter;
      the_ram_bx=(port_recv_dl[MODBUS_PORT_NO]+1)&0x1ff;;
      if(port_recv[MODBUS_PORT_NO][the_ram_bx]==0x03)
       {
        the_ram_ax=(port_recv_dl[MODBUS_PORT_NO]+2)&0x1ff;
        temp_int=port_recv[MODBUS_PORT_NO][the_ram_ax]+5+port_recv_dl[MODBUS_PORT_NO];        
        for(temp_loop=port_recv_dl[MODBUS_PORT_NO];temp_loop<temp_int;temp_loop++)
         {
          if(temp_loop<=511)
            port_report[temp_loop-port_recv_dl[MODBUS_PORT_NO]]=port_recv[MODBUS_PORT_NO][temp_loop];
          else
            port_report[temp_loop-port_recv_dl[MODBUS_PORT_NO]]=port_recv[MODBUS_PORT_NO][temp_loop-512];
         }
       port_recv_dl[MODBUS_PORT_NO]+=delta_len;
       port_recv_dl[MODBUS_PORT_NO]&=0x1ff;
       temp_int=MODBUS_CRC16(&port_report[0],port_report[2]+3);
       if((byte0(temp_int)!=port_report[port_report[2]+3])||(byte1(temp_int)!=port_report[port_report[2]+4]))
        goto inrda;
       }
      else
       {
       	temp_int=port_recv_dl[MODBUS_PORT_NO]+8;
       	for(temp_loop=port_recv_dl[MODBUS_PORT_NO];temp_loop<temp_int;temp_loop++)
         {
          if(temp_loop<=511)
            port_report[temp_loop-port_recv_dl[MODBUS_PORT_NO]]=port_recv[MODBUS_PORT_NO][temp_loop];
          else
            port_report[temp_loop-port_recv_dl[MODBUS_PORT_NO]]=port_recv[MODBUS_PORT_NO][temp_loop-512];
         }
        port_recv_dl[MODBUS_PORT_NO]+=delta_len;
        port_recv_dl[MODBUS_PORT_NO]&=0x1ff;
        temp_int=MODBUS_CRC16(&port_report[0],6);
        if((byte0(temp_int)!=port_report[6])||(byte1(temp_int)!=port_report[7]))
         goto inrda;        
       }
      if(MODBUS_comm_err_counter[port_report[0]]!=0)
       {
        MODBUS_comm_err_counter[port_report[0]]=0;
        temp_int=port_report[0]+MODBUS_begin_addr-1;
        YX_State[IED_TX_STATE_START_WORD+temp_int/16]&=(0xffff-(0x0001<<(temp_int%16)));
       }    
     }
    switch(MODBUS_rec_frame_type)
     {
      case MODBUS_YC_FRAME:       //yc
         Core_Src_Unit =port_report[0]+MODBUS_begin_addr-1;
         Core_Det_Pt=(WORD *)&(YC_State[unit_info[Core_Src_Unit].yc_val_start]);
         temp_int=port_report[2]/2;
         if(temp_int>unit_info[Core_Src_Unit].yc_val_num)
          for(temp_loop=0;temp_loop<unit_info[Core_Src_Unit].yc_val_num;temp_loop++)
           {
            Core_Det_Pt[temp_loop]=port_report[2*temp_loop+3]*0x100+port_report[2*temp_loop+4];
            the_ram_bx=Core_Det_Pt[temp_loop];
            Core_Det_Pt[temp_loop]=(the_ram_bx&0xfff);
           }
         else
          for(temp_loop=0;temp_loop<temp_int;temp_loop++)
           {
            Core_Det_Pt[temp_loop]=port_report[2*temp_loop+3]*0x100+port_report[2*temp_loop+4];
            the_ram_bx=Core_Det_Pt[temp_loop];
            Core_Det_Pt[temp_loop]=(the_ram_bx&0xfff);
           }         
         break;
      case MODBUS_YX_FRAME:       //yx
         Core_Src_Unit=port_report[0]+MODBUS_begin_addr-1;
         Core_Src_Pt_B=(BYTE *)&port_deal_buf[MODBUS_PORT_NO][0];
         Core_Src_Len=2*unit_info[Core_Src_Unit].yx_num;
         for(temp_loop=0;temp_loop<unit_info[Core_Src_Unit].yx_num;temp_loop++)
          {
           Core_Src_Pt_B[2*temp_loop]=byte0(YX_State[unit_info[Core_Src_Unit].yx_start+temp_loop]);
           Core_Src_Pt_B[2*temp_loop+1]=byte1(YX_State[unit_info[Core_Src_Unit].yx_start+temp_loop]);
          }
         Core_Src_Pt_B=(BYTE *)&port_deal_buf[MODBUS_PORT_NO][0];
         for(temp_loop=0;temp_loop<MODBUS_info[port_report[0]].yx_num;temp_loop++)
          {
           Core_Src_Pt_B[2*temp_loop]=port_report[2*temp_loop+4];
           Core_Src_Pt_B[2*temp_loop+1]=port_report[2*temp_loop+3];
          }
         core_update_YX();    
         break;
      case MODBUS_TS_FRAME:       //ts
         Core_Src_Unit=port_report[0]+MODBUS_begin_addr-1;
         Core_Src_Pt_B=(BYTE *)&port_deal_buf[MODBUS_PORT_NO][0];
         Core_Src_Len=2*unit_info[Core_Src_Unit].yx_num;
         for(temp_loop=0;temp_loop<unit_info[Core_Src_Unit].yx_num;temp_loop++)
          {
           Core_Src_Pt_B[2*temp_loop]=byte0(YX_State[unit_info[Core_Src_Unit].yx_start+temp_loop]);
           Core_Src_Pt_B[2*temp_loop+1]=byte1(YX_State[unit_info[Core_Src_Unit].yx_start+temp_loop]);
          }
         Core_Src_Pt_B=(BYTE *)&port_deal_buf[MODBUS_PORT_NO][0];
         for(temp_loop=0;temp_loop<MODBUS_info[port_report[0]].ts_num;temp_loop++)
          {
           Core_Src_Pt_B[2*(temp_loop+MODBUS_info[port_report[0]].yx_num)]=port_report[2*temp_loop+4];
           Core_Src_Pt_B[2*(temp_loop+MODBUS_info[port_report[0]].yx_num)+1]=port_report[2*temp_loop+3];
          }
         core_update_YX();    
         break;
      case MODBUS_YK_FRAME:       //yk
         MODBUS_YK_fx_flag=NO; 
         if((port_report[0]==MODBUS_YK_poll_addr)&&(port_transmit[MODBUS_PORT_NO][8]==CORE_CODE_YK_CHOOSE))
          {
           exchange_target_port=MODBUS_transmit_yk_host_port;
           exchange_buf[1] = MODBUS_PORT_NO;
           exchange_buf[2] = PROTOCOL_MODBUS%0x100;
           exchange_buf[3] = PROTOCOL_MODBUS/0x100;
           exchange_buf[5] =MODBUS_YK_poll_addr+MODBUS_begin_addr-1;
           exchange_buf[8] = CORE_CODE_YK_VERIFY;
           exchange_buf[9] = 0x02;
           exchange_buf[10]= 0x00;
           if((port_report[3]%2)==0) 
            exchange_buf[16]=CORE_CODE2_YK_TRIP; 
           else
            exchange_buf[16]=CORE_CODE2_YK_CLOSE;
           if(exchange_target_port<14)
            {
             the_ram_bx=port_report[2]*0x100+port_report[3];
             the_ram_bx=(the_ram_bx-MODBUS_info[MODBUS_YK_poll_addr].yk_address)/2; 
             exchange_buf[17]=the_ram_bx+unit_info[port_report[0]+MODBUS_begin_addr-1].yk_start;
            } 
           else
            exchange_buf[17]=the_ram_bx;
           Ex_Produce_Transmit_Info();
          } 
         break;
      default:
         break;
     }
          
rxd_out_time:
    if(Judge_Time_In_MainLoop(MODBUS_rxd_wait_time,MODBUS_WAIT_TIME_VALUE)==YES)
     {
      MODBUS_rec_OK=NO;
      MODBUS_rxd_wait_time=Time_1ms_Counter; 
      MODBUS_transmit_flag=YES;
      MODBUS_wait_replay=NO;
      MODBUS_transmit_wait_time=Time_1ms_Counter;
//      if(MODBUS_not_comm_count()==YES)
//       {
       	MODBUS_comm_err_counter[port_send[MODBUS_PORT_NO][0]]++;
        if(MODBUS_comm_err_counter[port_send[MODBUS_PORT_NO][0]]>3)
         {
          MODBUS_comm_err_counter[port_send[MODBUS_PORT_NO][0]]=0;
          temp_int=port_send[MODBUS_PORT_NO][0]+MODBUS_begin_addr-1;
          YX_State[IED_TX_STATE_START_WORD+temp_int/16]|=(0x0001<<(temp_int%16));      	
       	 }
//       }
     }
inrda:
    if(port_transmit_flag[MODBUS_PORT_NO]==YES)
    {
     if(Judge_Time_In_MainLoop(MODBUS_transmit_wait_time,200)==YES)
     {      
      if((MODBUS_transmit_task_busy!=YES)||(MODBUS_transmit_yk_host_port==port_transmit[MODBUS_PORT_NO][1])) 
      {
      	port_transmit_flag[MODBUS_PORT_NO]=NO;                          
        switch(port_transmit[MODBUS_PORT_NO][8])
        {
            case CORE_CODE_YK_CHOOSE:
                 MODBUS_transmit_task_busy=YES;
                 MODBUS_wait_replay=YES;
                 MODBUS_transmit_disable_begin_time=Time_2048ms_Counter;
                 MODBUS_transmit_yk_host_port=port_transmit[MODBUS_PORT_NO][1];
                 MODBUS_transmit_yk_cmd();
                 MODBUS_rec_frame_type=MODBUS_YK_FRAME;
                 MODBUS_transmit_flag=NO;
                 MODBUS_rxd_head_flag=NO;
                 MODBUS_rxd_wait_time=Time_1ms_Counter;
                 MODBUS_YK_fx_flag=YES;
                 MODBUS_YK_fx_wait_time=Time_2048ms_Counter;
                 break;
                 
            case CORE_CODE_YK_EXECUTE:
                 MODBUS_transmit_task_busy=NO;
                 break;
                 
            case CORE_CODE_YK_CANCEL:
                 MODBUS_transmit_task_busy=NO;
                 break;
            
            default:
                 MODBUS_transmit_task_busy=NO;
                 break;
        }
      }
     }
    }  
    else
     {
      if(Judge_LongTime_In_MainLoop(MODBUS_begin_veriiedclk_time,TIME_MODBUS_VERIFY_TIME_VALUE)==YES)//校时
       {
        MODBUS_begin_veriiedclk_time=Time_2048ms_Counter;
        MODBUS_transmit_time_cmd();
        MODBUS_rec_frame_type=NO;
        MODBUS_transmit_wait_time=Time_1ms_Counter;
        MODBUS_transmit_flag=YES;
       }
      else 
      if(MODBUS_transmit_flag==YES)
       {
        if(MODBUS_YK_fx_flag==YES)
         {
          MODBUS_transmit_task_busy=YES;
          MODBUS_wait_replay=YES;
          MODBUS_transmit_disable_begin_time=Time_2048ms_Counter;
          MODBUS_transmit_yk_host_port=port_transmit[MODBUS_PORT_NO][1];
          MODBUS_transmit_yk_cmd();
          MODBUS_rec_frame_type=MODBUS_YK_FRAME;
          MODBUS_transmit_flag=NO;
          MODBUS_rxd_head_flag=NO;
          MODBUS_rxd_wait_time=Time_1ms_Counter;
          MODBUS_YK_fx_flag=YES;
          if(Judge_LongTime_In_MainLoop(MODBUS_YK_fx_wait_time,MODBUS_YK_FX_WAIT_TIME_VALUE)==YES)
           {
       	    MODBUS_YK_fx_flag=NO;
            exchange_target_port=MODBUS_transmit_yk_host_port;
            exchange_buf[1] = MODBUS_PORT_NO;
            exchange_buf[2] = PROTOCOL_MODBUS%0x100;
            exchange_buf[3] = PROTOCOL_MODBUS/0x100;
            exchange_buf[5] = 0;
            exchange_buf[8] = CORE_CODE_YK_VERIFY;
            exchange_buf[9] = 0x02;
            exchange_buf[10]= 0x00;
            exchange_buf[16]=CORE_CODE2_YK_ERROR; 
            exchange_buf[17]=port_transmit[MODBUS_PORT_NO][17];
            Ex_Produce_Transmit_Info();        	
           }
         }
        else       	
       	if(Judge_Time_In_MainLoop(MODBUS_transmit_wait_time,80)==YES)
        switch(MODBUS_transmit_control)
         {
         case 1:
              MODBUS_rec_frame_type=MODBUS_YC_FRAME;
              MODBUS_YC_cmd(MODBUS_now_poll_addr);
              MODBUS_now_poll_addr++;
              MODBUS_transmit_flag=NO;
              MODBUS_wait_replay=YES;
              MODBUS_rxd_head_flag=NO;
              MODBUS_rxd_wait_time=Time_1ms_Counter;
              if((MODBUS_now_poll_addr+MODBUS_begin_addr-1)>MODBUS_end_addr)
               {
                MODBUS_now_poll_addr=0x01;
                MODBUS_transmit_control=2;
               }              
              break;
         case 2:
              MODBUS_rec_frame_type=MODBUS_YX_FRAME;
              MODBUS_YX_cmd(MODBUS_now_poll_addr);
              MODBUS_now_poll_addr++;
              MODBUS_transmit_flag=NO;
              MODBUS_rxd_head_flag=NO;
              MODBUS_wait_replay=YES;
              MODBUS_rxd_wait_time=Time_1ms_Counter;
              if((MODBUS_now_poll_addr+MODBUS_begin_addr-1)>MODBUS_end_addr)
               {
                MODBUS_now_poll_addr=0x01;
                MODBUS_transmit_control=3;
               }              
              break;
         case 3:
              MODBUS_rec_frame_type=MODBUS_TS_FRAME;
              MODBUS_TS_cmd(MODBUS_now_poll_addr);
              MODBUS_now_poll_addr++;
              MODBUS_transmit_flag=NO;
              MODBUS_rxd_head_flag=NO;
              MODBUS_wait_replay=YES;
              MODBUS_rxd_wait_time=Time_1ms_Counter;
              if((MODBUS_now_poll_addr+MODBUS_begin_addr-1)>MODBUS_end_addr)
               {
                MODBUS_now_poll_addr=0x01;
                MODBUS_transmit_control=1;
               }              
              break;
         default:              
              break;
         }
       } 
     }
}


/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/
/************************************************/
/* MODBUS_Init       function                   */
/************************************************/
/*===========================================================================*/
void MODBUS_Init()
{
    BYTE temp_axl;        
    MODBUS_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];

    MODBUS_begin_veriiedclk_time=Time_2048ms_Counter;
    MODBUS_rxd_wait_time=Time_1ms_Counter;
    MODBUS_transmit_task_busy=NO;
    MODBUS_wait_replay=NO;
    MODBUS_begin_addr=byte0(port_info[MODBUS_PORT_NO].mirror_unit); 
    MODBUS_end_addr=byte1(port_info[MODBUS_PORT_NO].mirror_unit); 
    MODBUS_now_poll_addr=0x01;
    MODBUS_rxd_head_flag=NO;
    MODBUS_transmit_flag=YES;
    MODBUS_transmit_wait_time=Time_1ms_Counter;
    MODBUS_rec_OK=NO;
    MODBUS_rec_frame_type=0;
    MODBUS_YK_fx_flag=NO;
    MODBUS_begin_zf_disable_time=Time_2048ms_Counter;
    disable();
    port_recv_pt[MODBUS_PORT_NO]=0;
    port_recv_dl[MODBUS_PORT_NO]=0;
    enable();
    MODBUS_transmit_control=1;
    for(temp_axl=0;temp_axl<32;temp_axl++)
     MODBUS_comm_err_counter[temp_axl]=0;   
}

/************************************************/
/* MODBUS_Main     function                     */
/************************************************/
/*===========================================================================*/
void MODBUS_Main()
{
    MODBUS_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];

    MODBUS_scheduler();  
    
    if(Judge_LongTime_In_MainLoop(MODBUS_begin_zf_disable_time,15)==YES) // after initialed 1s, let ZF be enable.  
    {
        Portx_Poll_First[MODBUS_PORT_NO]=YES;
    }
    if(Judge_LongTime_In_MainLoop(MODBUS_transmit_disable_begin_time,30)==YES)
    {
        MODBUS_transmit_task_busy=NO;
    }

}


/************************************************/
/* MODBUS_Monitor     function                  */
/************************************************/
/*===========================================================================*/
void MODBUS_Monitor()
{
  MODBUS_PARA_BASE_ADDR=(far WORD *)&port_flag[port_no];
  
}

