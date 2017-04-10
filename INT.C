


      ;;;定义了中断   但不是每个都使用的 肯定有限 绝没有那么复杂
                  ;;;int_mask1=0x61;
#pragma  interrupt(NMI_int        =0x1f)    /*  NMI        for None     int  */
#pragma  interrupt(port_IL_intr   =0x1e)    /*  EXINT3     for 554_2    int  */  ;有
#pragma  interrupt(port_EH_intr   =0x1d)    /*  EXINT2     for 554_1    int  */  ;有
#pragma  interrupt(EPAOvr2_3_int  =0x1c)    /*  EPAOvr2_3  for None     int  */
#pragma  interrupt(EPAOvr0_1_int  =0x1b)    /*  EPAOvr0_1  for None     int  */
#pragma  interrupt(EPA3_int       =0x1a)    /*  EPA3       for None     int  */
#pragma  interrupt(EPA2_int       =0x19)    /*  EPA2       for None     int  */
#pragma  interrupt(soft1ms_int    =0x18)    /*  EPA1       for soft_1ms int  */  ;有   ;;;有CAN网的发送过程
                  ;;;int_mask =0xf8;
#pragma  interrupt(port_AD_intr   =0x07)    /*  EPA0       for 554_0    int  */  ;有
#pragma  interrupt(SIO_int        =0x06)    /*  SIORec     for SIORec   int  */  ;有;;;SIO串口接收
#pragma  interrupt(SIO_int        =0x05)    /*  SIOTran    for SIOTran  int  */  ;有;;;SIO串口发送
#pragma  interrupt(exint1_int     =0x04)    /*  EXINT1     for CAN_1    int  */  ;有
#pragma  interrupt(exint0_int     =0x03)    /*  EXINT0     for CAN_0    int  */  ;有       有CAN接收处理
#pragma  interrupt(reserved_int   =0x02)    /*  reserve    for None     int  */
#pragma  interrupt(T2OverFlow_int =0x01)    /*  T2Ovr      for None     int  */
#pragma  interrupt(T1OverFlow_int =0x00)    /*  T1Ovr      for None     int  */



;;;EPA 事件处理阵列 哎哎其实没什么说白了就是类似于高速输入输出口功能,再白一点就是 定时等,只不过实现的方法 有点挫
;;;输入捕捉   输出比较
















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
extern void CAN0_Port_Receive(void);
extern void CAN1_Port_Receive(void);
extern void CAN0_Port_Transmit(void);            ;;;CAN网的发送就在这儿
extern void CAN1_Port_Transmit(void);

extern void set_receive();
extern void set_send();







/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/

/***********************************************/
/* P554_int_proc      procedure                */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void P554_int_proc(BYTE the_port_no)
{
         BYTE  ram_axl;
         BYTE  ram_axh;
         BYTE  the_port_no_mirr;
         BYTE  the_port_rece_temp;
    far  BYTE *p554_base_addr;
    far  BYTE *the_ram_base_addr;
    

    p554_base_addr = (far  BYTE *)Register_554[the_port_no];	//-现在的问题出现了,他是怎么向554中写数据的,其他的软件都有映射之类的,编译之后
    ram_axh = p554_base_addr[P554_IIR];			//-就可以

    if((ram_axh & 0x07)==0x04)     // receive int
    {
        ram_axh=0;
        ram_axl=p554_base_addr[P554_LSR];
        if((port_mirror[the_port_no] & 0x10)!=0)
        {
            the_port_no_mirr=port_mirror[the_port_no] & 0x0f;
        }
        else
        {
            the_port_no_mirr=0xff;
        }

        while((ram_axl & 0x01)!=0)
        {
            the_port_rece_temp=p554_base_addr[P554_RBR];
            
            port_recv[the_port_no][port_recv_pt[the_port_no]] = the_port_rece_temp;
            port_recv_pt[the_port_no]++;
            port_recv_pt[the_port_no] &= 0x1ff;
        
            if(the_port_no_mirr<0x0c)
            {
                port_send[the_port_no_mirr][ram_axh]=the_port_rece_temp;
                ram_axh++;
            }
            else
            {
            	if(the_port_no_mirr==0x0e)
            	{
            		set_port_rece_send_buf[set_port_rs_buf_sav_pt]=the_port_rece_temp;
            		set_port_rs_buf_sav_pt++;
            		if(set_port_rs_buf_sav_pt==set_port_rs_buf_tak_pt) set_port_rs_buf_tak_pt++;
            	}
            }
            
            if((ram_axl & 0x0c)!=0)  // not no parity error, frame error;
            {
                //save to rcd_info_myself ???
                SIO_CAN_Need_Reset=SIO_CAN_Need_Reset | (0x0001<<the_port_no);   // bit x
            }
    
            ram_axl=p554_base_addr[P554_LSR];
        }
            
        if(ram_axh!=0)
        {
            port_send_len[the_port_no_mirr]=ram_axh;
            port_send_begin_no_monitor(the_port_no_mirr);
        }
    }
    else
    {
        if((ram_axh & 0x07)==0x02) // transmit int
        {
            if(port_send_pt[the_port_no]<port_send_len[the_port_no])
            {
                the_ram_base_addr=&port_send[the_port_no][port_send_pt[the_port_no]];
                if( (port_send_pt[the_port_no]+8)<port_send_len[the_port_no] )
                {
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[0];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[1];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[2];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[3];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[4];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[5];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[6];
                    the_ram_base_addr++;
                    p554_base_addr[P554_THR]=*the_ram_base_addr;//[7];
           
                    port_send_pt[the_port_no]=port_send_pt[the_port_no]+8;	//-记录已经发送的字节个数吗
                }
                else
                {
                    do
                    {
                        p554_base_addr[P554_THR]=the_ram_base_addr[0];	//-把数据写到这个单元内就是发送出去了吗
                        the_ram_base_addr++;
                        port_send_pt[the_port_no]++;
                    }
                    while(port_send_pt[the_port_no]<port_send_len[the_port_no]);
                }
            }
            else
            {
                if(the_port_no==0x0b)
                {
                    p554_base_addr[P554_MCR]=0x08;//0x0a;
                }
                else
                {
                    P554_Port_Tran_Close_RTS_Cn[the_port_no]=Time_1ms_Counter
                                                            +P554_XTAL16M_CLOSE_RTS_DELAY[port_info[the_port_no].bauds];
                    P554_Port_Transing_Lastbyte[the_port_no]=YES;
                }    
                port_mon[the_port_no]=0;
            }
        }
    }
    
}


/*---------------------------------------------------------------------------*/
/*                       PUBLIC             functions                        */
/*---------------------------------------------------------------------------*/

/***********************************************/
/* interrupt NMI     for none   svr procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void NMI_int(void)
{
    // do nothing
}



/***********************************************/
/* interrupt EXINT3  for 554_2  svr procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void port_IL_intr(void)
{
    BYTE  port_noiii;

    RAM_CPU_Int_Moni.EXINT3_INT_DEAD_COUNTER=0;
    
re_check_int_com2:
    for(port_noiii=8;port_noiii<12;port_noiii++)
    {
        P554_int_proc(port_noiii);
    }

    port_noiii=p3_pin;
    if((port_noiii & 0x80)==0x00) goto re_check_int_com2;
    port_noiii=p3_pin;
    if((port_noiii & 0x80)==0x00) goto re_check_int_com2;

}



/***********************************************/
/* interrupt EXINT2  for 554_1  svr procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void port_EH_intr(void)
{
    BYTE  port_noii;

    RAM_CPU_Int_Moni.EXINT2_INT_DEAD_COUNTER=0;
    
re_check_int_com1:
    for(port_noii=4;port_noii<8;port_noii++)
    {
        P554_int_proc(port_noii);
    }

    port_noii=p3_pin;
    if((port_noii & 0x40)==0x00) goto re_check_int_com1;
    port_noii=p3_pin;
    if((port_noii & 0x40)==0x00) goto re_check_int_com1;

}



/***********************************************/
/* interrupt EPAOvr2_3 for none svr procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void EPAOvr2_3_int(void)
{
    // do nothing
}

/***********************************************/
/* interrupt EPAOvr0_1 for none svr procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void EPAOvr0_1_int(void)
{
    // do nothing
}

/***********************************************/
/* interrupt EPA3  for none    svr  procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void EPA3_int(void)
{
    // do nothing
}

/***********************************************/
/* interrupt EPA2  for none    svr  procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void EPA2_int(void)
{
    // do nothing
}




















/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void soft1ms_int(void)   // EPA1               ;;;中断处理程序
{
    register WORD  reg_ibx;
    far BYTE *ram_base_addr;


//  process 1ms run
    reg_ibx=REG_TimeTemp+EPA1_PERIOD;

    REG_TimeTemp=timer1;

    epa1_time=timer1+EPA1_PERIOD;           ;;;每次中断后虽然不需要再设计,但时间比较基准是必须再给的 ,就在这儿

    REG_Surplus_Time=REG_Surplus_Time+REG_TimeTemp-reg_ibx;  ;;;不得不说句脏话 这他妈做的太精确了 连这么小的时间也进行记录分辨

    while(REG_Surplus_Time>=EPA1_PERIOD)
    {
        REG_Surplus_Time=REG_Surplus_Time-EPA1_PERIOD;      ;;;寄存器_盈余_时间
        REG_1Msecond++;          ;;;难道这个里面的值是根据晶振频率自动加的吗,也就是说他是时间基准吗
    }
  
    REG_1Msecond++;        ;;;这可能就是这个系统的时间基准

    Clock_Process();       ;;;系统计时
    
    Time_1ms_Counter++;
    
    if((Time_1ms_Counter & 0x7ff)==0) Time_2048ms_Counter++;     ;;;这个地方 也太他妈好了 进行了级计数 计数分辨率变为了2048MS
    
//  process CAN transmit
    CAN0_Port_Transmit();                   ;;;CAN网发送过程,相当于CPU中 软件中断中的发送程序段,唯一的发送过程
    
//  process 554 transmit
    for(reg_ibx=0;reg_ibx<12;reg_ibx++)       ;;;切忌通讯管理机的端口不包括两个CAN网,加CAN网之后共14个,是分两类处理的
    {
        //if(Port_Info[reg_ibx].PORT_SEMIDUPLEX==YES)
        {
            if(P554_Port_Transing_Lastbyte[reg_ibx]==YES)
            {
                if(Time_1ms_Counter==P554_Port_Tran_Close_RTS_Cn[reg_ibx])
                {
                    port_send_pt[reg_ibx]++;
                    ram_base_addr=(far BYTE *)Register_554[reg_ibx];
                   
                    if(reg_ibx!=10)
                    	ram_base_addr[P554_MCR]=0x08;        ;;;4
                    P554_Port_Transing_Lastbyte[reg_ibx]=NO;
                }
            }
        }
    }
    
    
//  process wachdog 
    RAM_CPU_Int_Moni.EPA1_INT_DEAD_COUNTER=0;
    
    RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER++;
    if(RAM_CPU_Int_Moni.MAINLOOP_DEAD_COUNTER>50000)  // 50s
    {
        Rcd_Info_Myself_Tmp[0]=0;
        Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CPU;
        Rcd_Info_Myself_Tmp[2]=RCD_INFO_MYSELF_AREA1_CPU_MAINLOOP;
        Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD;
        Rcd_Info_Myself_Tmp[4]=0;
        Rcd_Info_Myself_Tmp[5]=0;
        Rcd_Info_Myself_Tmp[6]=0;
        Rcd_Info_Myself_Tmp[7]=0;
        Store_Rcd_Info_Myself();
        
        RAM_CPU_INT_Rst_Cn[6]++;
        while(1) {};                ;;;永久等待
    }
}

























/***********************************************/
/* interrupt EPA0  for 554_0   svr  procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void port_AD_intr(void)
{
    BYTE  port_noi;
    WORD  ram_ax;


    ram_ax=epa_time0;
    ram_ax=epa_time0;

    RAM_CPU_Int_Moni.EPA0_INT_DEAD_COUNTER=0;
    
re_check_int_com0:
    for(port_noi=0;port_noi<4;port_noi++)
    {
        P554_int_proc(port_noi);
    }

    port_noi=p1_pin;
    if((port_noi & 0x01)==0x00) goto re_check_int_com0;
    port_noi=p1_pin;
    if((port_noi & 0x01)==0x00) goto re_check_int_com0;

}



/***********************************************/
/* interrupt SIOReceive   svr       procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//void SIOReceive_int(void)
//{
    
//}



/***********************************************/
/* interrupt SIOTransmit  svr       procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//void SIOTransmit_int(void)
//{

//}



/***********************************************/
/* interrupt SIO          svr       procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void SIO_int(void)
{
    unsigned char sp_sta_img;

    sp_sta_img = sp_status;
    if((sp_sta_img & 0x40)!=0)     //RI
    {
        set_receive();
    }
    //else
    if((sp_sta_img & 0x20)!=0)  //TI
    {
        set_send(); 
    }
}



/***********************************************/
/* interrupt EXINT1  for CAN_1  svr procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void exint1_int(void)
{
    BYTE dumy;


    RAM_CPU_Int_Moni.EXINT1_INT_DEAD_COUNTER=0;

    dumy=*(far BYTE *)(CAN1_INTSTA+0);
    while(dumy!=0)         // run time exceed 1.6s, may reset CPU
    {
        if((dumy & 0x08)!=0)   *(far BYTE *)(CAN1_COMMAND+0)=0x08;     // data overrun int

        if( ((dumy & 0x04)!=0) && ((*(far BYTE *)(CAN1_STATUS+0) & 0x40)!=0) )  // err int
        {
            SIO_CAN_Need_Reset=SIO_CAN_Need_Reset | 0x2000;   // bit 13
        }

        while((*(far BYTE *)(CAN1_STATUS+0) & 0x01)!=0)
        {
            CAN1_Port_Receive();
            *(far BYTE *)(CAN1_COMMAND+0)=0x04;
        }

        dumy=*(far BYTE *)(CAN1_INTSTA+0);
    }
}









;;;以下这个中断是如何触发的??现在不管

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void exint0_int(void)
{
    BYTE dumy;      ;;;局部变量


    RAM_CPU_Int_Moni.EXINT0_INT_DEAD_COUNTER=0;        ;;;这个的作用非常简单就是监视程序执行的大体过程

    dumy=*(far BYTE *)(CAN0_INTSTA+0);          ;;;首先通过加求得地址再通过强制类型转换求得适合的地址最后在通过指针取得内容
    while(dumy!=0)     // run time exceed 1.6s, may reset CPU
    {
        if( (dumy & 0x08)!=0 )                                                ;;;检查数据溢出中断是否发生
        { 
            *(far BYTE *)(CAN0_COMMAND+0)=0x08;      // data overrun int       ;;;数据溢出状态位被清除
        }

        if( ((dumy & 0x04)!=0) && ((*(far BYTE *)(CAN0_STATUS+0) & 0x40)!=0) )  // err int  ;;;错误报警中断  出错状态
        {
            SIO_CAN_Need_Reset=SIO_CAN_Need_Reset | 0x1000;   // bit 12      ;;;符合出错条件 需要复归CAN网
        }

        while((*(far BYTE *)(CAN0_STATUS+0) & 0x01)!=0)     ;;;接收缓冲器中有可用信息
        {
            CAN0_Port_Receive();                          ;;;通讯管理机 CAN接收是在这个中断中实现的
            *(far BYTE *)(CAN0_COMMAND+0)=0x04;           ;;;释放接收缓冲器
        }

        dumy=*(far BYTE *)(CAN0_INTSTA+0);                ;;;再一次读入中断寄存器的内容,以便决定是否还有内容需要处理
    }
}














/***********************************************/
/* interrupt reserved     svr       procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void reserved_int(void)
{
    // do nothing
}




/***********************************************/
/* interrupt T2OverFlow   svr       procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void T2OverFlow_int(void)
{
    // do nothing
}



/***********************************************/
/* interrupt T1OverFlow   svr       procedure  */
/***********************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void T1OverFlow_int(void)
{
    // do nothing
}









/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/

//    none

