;;;这个口的功能很有可能就是向后台发送数据的端口




//  for later running in practise only need rename 'port_send_begin_no_monitor' as 'port_send_begin'
;;;难道这个程序考虑到了调试和实际运行两方面的作用吗
void   port_send_begin_no_monitor(BYTE the_port_no)
{
  struct WR_REG_16554 *m_wr_554;         ;;;结构体指针 

  m_wr_554 = (struct WR_REG_16554 *)Register_554[the_port_no];
  {
      m_wr_554->MCR = 0x0a;         ;;;位3被置位时,外部串行通道中断被使能,还有些是控制引脚输出电平的
  }
  if(the_port_no==10)  m_wr_554->MCR=0x0b;         ;;;为了获得正确的输入极性
  port_send_pt[the_port_no] = 1; 
  m_wr_554->THR = port_send[the_port_no][0];        //-发送器保持寄存器,这儿应该就是最终发送了

}










//-一但开始发送一帧信息,就不允许中断了
void port_send_begin()
{
    BYTE the_port_no;
    WORD the_ram_ax;
    WORD the_ram_bx;
    
    disable();            ;;;关中断
    port_send_begin_no_monitor(port_no);       ;;;口发送开始不监测,port_no表示的是哪个端口

////// for monitor port msg:  
    if((port_mirror[port_no] & 0x20)!=0)
    {
        the_port_no=port_mirror[port_no] & 0x0f;         ;;;取出哪个口处于发送状态或可以用,不可能时同时进行的
        if(the_port_no<0x0c)             ;;;小于12就是用16554进行串口变并口或并口变串口的运用
        {
            the_ram_bx=port_send_len[port_no];  //-表示一个完整信息流发送几个字节的数据
            for(the_ram_ax=0;the_ram_ax<the_ram_bx;the_ram_ax++)
            {
                port_send[the_port_no][the_ram_ax]=port_send[port_no][the_ram_ax];   //-难道为了满足速度要求进行双击缓冲,呵呵,而且还在一个空间内
            }
            port_send_len[the_port_no]=port_send_len[port_no]; //-还是记录需要发送几个字节
            port_send_begin_no_monitor(the_port_no);
        }
        else
        {
        	if(the_port_no==0x0e)        ;;;14不能人为配置,它是固定的CPU端口
        	{
        		the_ram_bx=port_send_len[port_no];
        		the_ram_ax=0;
        		while(the_ram_bx>0)
        		{
        			set_port_rece_send_buf[set_port_rs_buf_sav_pt]=port_send[port_no][the_ram_ax];
        			the_ram_ax++;
        			the_ram_bx--;
        			set_port_rs_buf_sav_pt++;   ;;;我猜的不错的话这个是发送的判断依据之一,他的上级因该已经准备好了数据,下级检测到这些标志自动发送
        			if(set_port_rs_buf_sav_pt==set_port_rs_buf_tak_pt) set_port_rs_buf_tak_pt++;
        		}
        	}
        }
    }
    enable();
}














/*===========================================================================*/
void init_port_not_clr_ptr()
{
    far  BYTE *p554_base_addr;
         BYTE  ram_axl;
         BYTE  ram_axh;


    if(port_no>11) return;     ;;;即只能初始化12个口,即只有前12个可以这么干

    p554_base_addr=(far  BYTE *)Register_554[port_no];         ;;;确定是前12个端口中的哪个端口

//     initial   FCR
    p554_base_addr[P554_FCR]=0x41;        // receive trigger at 4 bytes 
                                          // enable FIFO
//     initial   LCR
    ram_axh=(byte0(port_info[port_no].bits)-5) & 0x03;
    if(byte0(port_info[port_no].stopbit)==1)
    {
        //ram_axl=ram_axl & 0xfb;
    }
    else
    {
        ram_axh=ram_axh+0x04;
    }

    if(byte0(port_info[port_no].semiduplex_parity)==PARITY_ODD)
    {
        ram_axh=ram_axh+0x08; 
    }
    else
    {
        if(byte0(port_info[port_no].semiduplex_parity)==PARITY_EVEN)
        {
            ram_axh=ram_axh+0x18; 
        }
    }
    
    p554_base_addr[P554_LCR]=ram_axh | 0x80;
    
//     initial   BAUD
    p554_base_addr[P554_DLL]=byte0(BaudRate[port_info[port_no].bauds]);
    p554_base_addr[P554_DLM]=byte1(BaudRate[port_info[port_no].bauds]);
    
//     reset    LCR
    p554_base_addr[P554_LCR]=ram_axh;
    ram_axh=p554_base_addr[P554_RBR];
//     initial  IER
    p554_base_addr[P554_IER]=0x03;   // 
//     initial  MCR
//    if(port_no==0x0b)
//        p554_base_addr[P554_MCR]=0x08;//0x0a;   // disable trans
//    else
//        p554_base_addr[P554_MCR]=0x0a;//0x08;   // disable trans
    if(port_no==10)  p554_base_addr[P554_MCR]=0x0b;
    else             p554_base_addr[P554_MCR]=0x08;

    if(Cn_SIO_CAN_Reset[port_no]<0xffff)   Cn_SIO_CAN_Reset[port_no]++;
}











/************************************************/
/* init_port   function  Must DI before call it */
/************************************************/
/*===========================================================================*/
void init_port()
{
    init_port_not_clr_ptr();
    
    port_recv_pt[port_no] = 0;
    port_recv_dl[port_no] = 0;
    port_send_pt[port_no] = 0;
    port_send_len[port_no]= 0;
}















;;;从这段程序可以很清楚的知道C语言和汇编语言没有任何区别,仅仅是人为表达的习惯问题,最终的结果和过程都是一样的,仅仅在翻译的时候不太,但这是由伪指令指挥
;;;编译器干的这个模块不是你的任务

/*===========================================================================*/
void Initial_CAN(BYTE the_port_no)         ;;;这是形参
{
    far BYTE *can_base_addr;
        BYTE ram_iaxl;                  ;;;这个就是内部变量而已

    if(the_port_no>1) return;          ;;;大于1就不需要初始化
    
    can_base_addr=(far BYTE *)CAN_BaseAddr+(unsigned long)the_port_no*CAN_AddrDistance;
    *(far BYTE *)(can_base_addr+CAN_MODE     ) =0x01;     ;;;启动复位模式
    *(far BYTE *)(can_base_addr+CAN_CLKDIVIDE) =0x88;     ;;;选择增强CAN模式PELI

    *(far BYTE *)(can_base_addr+CAN_INTEN    ) =0x0d;   // open overrun_int, error_warn_int,
                                                        //      TX_int     , RX_int         

    ram_iaxl=port_info[PORT_NO_CAN_0+the_port_no].bauds;
    *(far BYTE *)(can_base_addr+CAN_BUST0    ) =byte0(CAN_BUST_XTAL16M[ram_iaxl]);         ;;;这个Byte0没有特殊含义就是一个带参数宏 ,为程序模块化而设计
    *(far BYTE *)(can_base_addr+CAN_BUST1    ) =byte1(CAN_BUST_XTAL16M[ram_iaxl]);         ;;;设置了波特率
    *(far BYTE *)(can_base_addr+CAN_OUTCONT  ) =0xda;       ;;;;;;输出控制决定输出方式

    *(far BYTE *)(can_base_addr+CAN_ACCCODE0 ) =0x00;       ;;;本单元地址
    *(far BYTE *)(can_base_addr+CAN_ACCMASK0 ) =0x00;

    *(far BYTE *)(can_base_addr+CAN_ACCCODE1 ) =0x00;
    *(far BYTE *)(can_base_addr+CAN_ACCMASK1 ) =0xff;

    *(far BYTE *)(can_base_addr+CAN_ACCCODE2 ) =0xff;
    *(far BYTE *)(can_base_addr+CAN_ACCMASK2 ) =0xff;    // to receive all, set it with 0xff ;仅仅这儿和CPU不同

    *(far BYTE *)(can_base_addr+CAN_ACCCODE3 ) =0x00;
    *(far BYTE *)(can_base_addr+CAN_ACCMASK3 ) =0xff;

    *(far BYTE *)(can_base_addr+CAN_MODE     ) =0x00;     ;;;这个地方启动了工作模式

    ram_iaxl =*(far BYTE *)(can_base_addr+CAN_STATUS);         ;;;这是指针读入的是值
    ram_iaxl =*(far BYTE *)(can_base_addr+CAN_INTSTA);         ;;;读入了状态
    
    if(Cn_SIO_CAN_Reset[the_port_no+PORT_NO_CAN_0]<0xffff)   Cn_SIO_CAN_Reset[the_port_no+PORT_NO_CAN_0]++;
}























/*===========================================================================*/
void Judge_P554_CAN_Reset(void)
{
//         BYTE the_port_no;
         BYTE the_ram_axl;
         WORD the_ram_bx;
    far  BYTE *p554_base_addr;
    
    for(port_no=0;port_no<12;port_no++)
    {
        p554_base_addr=(far BYTE *)Register_554[port_no];        ;;;是12的二维数组,是表示3个554可以带12个猫吗
        the_ram_axl=p554_base_addr[P554_LSR];
        the_ram_bx=0x0001;
        the_ram_bx=the_ram_bx<<port_no;
        if((the_ram_axl & 0x0c)!=0)
        {
            SIO_CAN_Need_Reset=SIO_CAN_Need_Reset | the_ram_bx;		//-运行到这里说明对应的接口需要初始化吗
//            if(port_no==7) (*((far BYTE *)(SOFT_ERR_FLAG+0x0000)))=the_ram_axl;
        }
        
        if((SIO_CAN_Need_Reset & the_ram_bx)!=0)
        {
            disable();
            //port_no=the_port_no;
            init_port_not_clr_ptr();
            enable();
            the_ram_bx=~the_ram_bx;
            SIO_CAN_Need_Reset=SIO_CAN_Need_Reset & the_ram_bx;
        }
        
    }


    p554_base_addr=(far BYTE *)CAN0_BaseAddr;
    if(((SIO_CAN_Need_Reset & 0x1000)!=0) || ((p554_base_addr[CAN_MODE] & 0x01)!=0) || ((p554_base_addr[CAN_STATUS] & 0x80)!=0))
    {                                          ;;;终止发送
        disable();
        Initial_CAN(0);
        enable();
        SIO_CAN_Need_Reset=SIO_CAN_Need_Reset & 0xefff;
    }
    
    p554_base_addr=(far BYTE *)CAN1_BaseAddr;
    if(((SIO_CAN_Need_Reset & 0x2000)!=0) || ((p554_base_addr[CAN_MODE] & 0x01)!=0) || ((p554_base_addr[CAN_STATUS] & 0x80)!=0))
    {
        disable();
        Initial_CAN(1);
        enable();
        SIO_CAN_Need_Reset=SIO_CAN_Need_Reset & 0xdfff;
    }
    
}

















/*===========================================================================*/
void init_all_port(void)
{
//缺硬件复位
  for(port_no=0;port_no<12;port_no++)
  {
     init_port();
     port_mon[port_no] = 0;
  }
}

    