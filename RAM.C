
;;;总线-->RXBUF(SJA1000)-->CAN0_RecePort_Buf-->CAN_Port_ReceAsm(void)-->CAN_MSG_Proc  ...接收
;;;                                                             -->CAN0_Port_Transmit ...发送







/***********************************************/
/* Definition of global variables      [NEAR]  */
/***********************************************/

//    for CPU use
//    -----------------------------------------------------------
near  WORD                             RAM_CPU_RST_TAG1;
near  WORD                             RAM_CPU_RST_TAG2;


near  TYPE_CPU_INT_MONI                RAM_CPU_Int_Moni;
near  WORD                             RAM_CPU_INT_Rst_Cn[8];


near  BYTE                             Rcd_Info_Myself_Tmp[8];  // for data trans from function call
near  BYTE                             Rcd_Info_System_Tmp[8];  // for data trans from function call

//    for CPU use end


//    for CAN use
//    -----------------------------------------------------------
near  TYPE_CAN_RAM_PARA                RAM_CAN0_Para;        ;;;近  变量类型    变量名
#pragma locate (RAM_CAN0_Para          = 0x002040)            ;;;给这个变量人为指定地址

near  BYTE                             CAN0_CurrentSta_Buf[MAX_CAN_NUMBER];   //-是CAN单元的地址 ,用于记录该单元现在的通信状态,记录单元状态,是自己上送的
near  BYTE                             CAN0_IED_StaChg[MAX_CAN_NUMBER];		//-记录单元变化的状态,这种值应该没有什么实际意义,是供查询用的,但是对应单元号是固定的
#pragma locate (CAN0_CurrentSta_Buf    = 0x002080)         ;;;这款芯片不再是从这里开始执行了


near  BYTE                             CAN0_ReceCOS_Buf[3*512];
near  BYTE                             CAN0_COSACK_Buf[16];         ;;;这个数组中可以记录16个需要发送确认报文的源地址
near  BYTE                             CAN0_ReceYCCHG_Buf[64];






near  BYTE                             CAN0_RecePort_Buf[12*256];    ;;;把从总线上接收到的内容先保存在这儿,释放接收缓冲器的空间,让这些内容在这等待进一步处理
;;;上面这个是接收报文的缓冲区,内容来自SJA1000接收缓冲器,等待进一步处理
near  BYTE                             CAN0_ReceProc_Buf[512];       ;;;接收到的内容保存区

near  BYTE                             CAN0_TranProc_Buf[512];       ;;;每个缓冲区都给足了512字节的空间















near  BYTE                             CN_CAN0_Comm_Error[MAX_CAN_NUMBER];
near  BYTE                             CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER];    ;;;难道是每台装置对应一字节
near  BYTE                             CAN0_Comm_CurrentSta[MAX_CAN_NUMBER];


;;;可能这里的char就相当于short
near  BYTE                             CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr;    ;;;已经保存到发送区得数目
near  BYTE                             CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr;    ;;;已经发送走的数据,要保证能发送必须把已经保存的发送即大于1


near  WORD                             CAN_Time2048ms_Last_Value;
near  WORD                             CAN_Not_Idle_Cn;

near  BYTE                             CAN_NOW_POL_YCALL_FLAG;


//    for CAN use end


//    for PORT (P554 & CAN & other) COMUSE
//    -----------------------------------------------------------
//    internal program: 0-11  , 16c554
//                      12    , CAN0
//                      13    , CAN1
//                      14    , CPU_COM
//                      15    , reserved
near  WORD                             Cn_SIO_CAN_Reset[16];


near  WORD                             SIO_CAN_Need_Reset;

//    for PORT (P554 & CAN & other) COMUSE end


//    for P554 use
//    -----------------------------------------------------------
near  WORD                             port_recv_pt[12];
near  WORD                             port_send_pt[12];   ;;;记录哪个口处于发送状态吗
near  WORD                             port_recv_dl[12];
near  WORD                             port_send_len[12];


near  WORD                             P554_Port_Tran_Close_RTS_Cn[12];
near  BYTE                             P554_Port_Transing_Lastbyte[12];

//    for P554 use end


//    for Protocol use
//    -----------------------------------------------------------
near  WORD                             YC_State[2048];  // 2048 words
near  WORD                             YX_State[256];   //  256 words 256*16 bit ;;;这个地方定义了一个数据用于存放数据
near  UL                               YM_State[256];   //  256 dwords



near  BYTE                             BCH_Array[6];    // used only in main_loop


near  BYTE                             Portx_Poll_First[14];
near  BYTE                             HOST_ZF_enable;
near  BYTE                             HOST_YK_Doing;
near  BYTE                             HOST_YK_Port_No;
near  WORD                             HOST_YK_Doing_Begin_Time;


near  WORD                             BH_To_YX_Delay_Begin_Time[512];

near  BYTE       YX_FG_FLAG;

near  BYTE       YX_FG_WAIT_FLAG;

near  WORD       YX_FG_WAIT_TIME;


near  WORD                             BH_Report_Bank_Sav_Ptr;    
near  WORD                             BH_Report_Bank_Tak_Ptr[12];    

near  BYTE                      IEC_last_frame_buf[12][256];


near  BYTE                             temp_buf[13];         ;;;这个区域的内容是不是保存为从发做准备的


/***********************************************/
/* Definition of global variables      [FAR]   */
/***********************************************/
// before all below is from rambank.c
BYTE            Register_554[12][512]; 





WORD            main_loop;

BYTE            p2_img;

BYTE            Dsa208_Work_State;

// here reserved 0003H-06FFH

/********** Set Port ********/
BYTE            set_port_rece_send_buf[256];

BYTE            set_port_rs_buf_sav_pt;

BYTE            set_port_rs_buf_tak_pt;

// here reserved 0802H-087FH
BYTE            set_rpt_recv_finish;
BYTE            set_transmit_flag;
WORD            set_send_pt;
WORD            set_send_len;


BYTE            set_len;
BYTE            set_type;
WORD            set_offset;
WORD            set_locate;
 

BYTE            set_syn_char_no;
BYTE            set_rpt_char_no;  
  

BYTE            set_port_mon_status;  
BYTE            set_port_mon_need_reply;  
WORD            set_mon_begin_wait_ied_clk;  
WORD            set_mon_begin_wait_host_clk;  
   
// here reserved 0894H-08FFH

WORD            set_buffer[256];


BYTE            set_transmit[256];       ;;;有转发的意思吗
BYTE            set_send_buf[512];
BYTE            set_recv_buf[512];




// move from '0x010a00' to '0x011000'
WORD            port_flag[12][128];

// here reserved 1C00H-1FFFH
BYTE            port_recv[12][512];

// here reserved 3800H-3FFFH
BYTE            port_send[12][512];

// here reserved 5800H-5FFFH                     ;;;这个地方 保留 这么大空间没用
BYTE            port_transmit[16][512];    // can include         ;;;这个地方给CAN这样安排了这么大的空间让他使用
      
BYTE            port_efficacy[12][512];

// here reserved 9800H-9FFFH

BYTE            port_deal_buf[12][512];


BYTE            port_report[512]; //-这个里面的内容应该是这样处理的,把接收到的每帧信息进行了处理,并提取出完整的消息组合在这里
									//-仅仅是一个中间单元而以
WORD            port_mon[16];              // can include

BYTE            port_transmit_flag[16];    // can include

// here reserved BA40H-BFFFH


// Exchange
WORD            Ex_YK_start[14];

WORD            Ex_YK_end[14];


WORD            exchange_unit;

WORD            exchange_src_port;

WORD            exchange_target_port;

WORD            exchange_type;

WORD            exchange_Len;

BYTE           *exchange_src_pt;

BYTE            exchange_buf[512];

// here reserved C300H-C3FFH


struct YX_CHANGE {unsigned short offset_no;  //确定变位YX的偏移量
                  unsigned short chg_cont0;  //变位YX帧的YX值，每帧2个字，WORD0
                  unsigned short chg_cont1;  //变位YX帧的YX值，每帧2个字，WORD1
                  unsigned short chg_bit0;   //如chg_bit0对应的位值为1，则表示该位YX产生变位
                  unsigned short chg_bit1;   //如chg_bit1对应的位值为1，则表示该位YX产生变位
                 }yx_change[256];    


struct YX_EVENT { unsigned short soe_ms;
                  unsigned short soe_sec_min;
                  unsigned short soe_hor_date;
                  unsigned short soe_chn_sta; //SOE对象号和性质（合/分） 
                 }yx_event[1024];    
#pragma locate (yx_event            = 0x01ce00)

/*!!!!!!!!!!!!!!!!!!!!! data transmite buffer !!!!!!!!!!!!!!!!!!!!!!!!*/
unsigned short YX_state_tr[4][64];  // 64*16=1024
#pragma locate (YX_state_tr         = 0x01ee00)

struct YX_CHANGE_TR {unsigned short offset_no;
                     unsigned short chg_cont0;
                     unsigned short chg_cont1;
                     unsigned short chg_bit0;
                     unsigned short chg_bit1;
                    }yx_change_tr[4][256];    
#pragma locate (yx_change_tr        = 0x01f000)

struct YX_EVENT yx_event_tr[4][1024];    
#pragma locate (yx_event_tr         = 0x021800)


WORD            yx_chg_out_pt[14]; //变位YX帧已发送出去的帧数
#pragma locate (yx_chg_out_pt       = 0x029800)

WORD            yx_soe_out_pt[14]; //SOE帧已发送出去的帧数
#pragma locate (yx_soe_out_pt       = 0x029820)

WORD            yx_chg_ex_pt[14]; //统计每个端口中变位YX个数
#pragma locate (yx_chg_ex_pt        = 0x029840)

WORD            yx_soe_ex_pt[14];
#pragma locate (yx_soe_ex_pt        = 0x029860)

WORD            yx_chg_tr_ex_pt[4]; //统计变位YX帧的个数
#pragma locate (yx_chg_tr_ex_pt     = 0x029880)

WORD            yx_soe_tr_ex_pt[4];
#pragma locate (yx_soe_tr_ex_pt     = 0x029888)

WORD            yx_chg_tr_in_pt[4]; //统计每个yx_transmite_table中变位YX个数
#pragma locate (yx_chg_tr_in_pt     = 0x029890)

WORD            yx_soe_tr_in_pt[4];
#pragma locate (yx_soe_tr_in_pt     = 0x029898)

WORD            yx_chg_in_pt;   //统计变位YX帧的个数


WORD            yx_soe_in_pt;  //统计SOE帧的个数
#pragma locate (yx_soe_in_pt        = 0x0298a2)





// below are for each port self use area:
BYTE            bus_unit_err[64];
#pragma locate (bus_unit_err        = 0x0298c0)

BYTE            bus_err_rec[224];
#pragma locate (bus_err_rec         = 0x029900)

BYTE            bus_err_rec_pt;
#pragma locate (bus_err_rec_pt      = 0x0299e0)
// here reserved 189E1H-189FFH

BYTE           unit_set_flag[256];                       // for what use by LHY?
#pragma locate (unit_set_flag       = 0x029a00)





BYTE           BH_Saved_Report_Bank[256][32];  // include CAN
#pragma locate (BH_Saved_Report_Bank= 0x029b00)  // +0x2000
// here reserved 2bb00H-2daffH     for BH_Saved_Report_Bank expand to [512][32]

BYTE           BH_Bank_Report[512];
#pragma locate (BH_Bank_Report      = 0x02db00)  

// [0] Trans_state     [1] CAN246_ADDR   [2] Port_No         [3] Unit_No
// [4] Action_Type     [5] Switch_No     [6] CAN243_ADDR     [7] CAN_YK_No
BYTE           CAN_246_Trans_YK_Buf[8];
#pragma locate (CAN_246_Trans_YK_Buf          = 0x02dd00)  
WORD           CAN_246_YK_Begin_WAIT_VERIFY;
#pragma locate (CAN_246_YK_Begin_WAIT_VERIFY  = 0x02dd08)  
WORD           CAN_246_YK_Begin_WAIT_CONFIRM;
#pragma locate (CAN_246_YK_Begin_WAIT_CONFIRM = 0x02dd0a)  

// reserved 0x02dd10-0x02dd9f
BYTE           bh_event_alarm_warn;    
BYTE           bh_event_alarm_action;    
BYTE           bh_event_alarm_now_action_warn;    
BYTE           bh_event_alarm_Reserved;    
WORD           bh_event_alarm_warn_Begin_Clk;    
WORD           bh_event_alarm_action_Begin_Clk;    



//   xxxx'yyyy
//   bit 4 =1        trans receive
//   bit 5 =1        trans send
//   bit 6, bit 7    no use
//   bit 3-0         destin port_no   0-11 P554; 12 CAN0; 13 CAN1; 14 cpucom;
BYTE            port_mirror[16];             ;;;每个端口的地址情况



//   bit 7 = 1      Now may send on CAN1
//   bit 6 = 1      Now may send on CAN0
//   bit 5 = 1      Now may rece on CAN1
//   bit 4 = 1      Now may rece on CAN0
BYTE            Now_CANx_Sw_Channel_Sta[256];  // include addr 64-255  ;;;记录保护单元的信息,一个单元给一个B的大小



BYTE           CAN0_CPUASKCMU_Trans_Buf[256*12];
#pragma locate (CAN0_CPUASKCMU_Trans_Buf = 0x02e000)   ;;;这个单元中保存的数据可能就是最终要发走的数据,已经满足帧的要求了

WORD           SC1801_YC_Last_State[192];    
#pragma locate (SC1801_YC_Last_State     = 0x02ec00)

WORD           SC1801_YC_Current_State[192];    
#pragma locate (SC1801_YC_Current_State   = 0x02ed80)

BYTE           SC1801_YX_Last_State[64];    
#pragma locate (SC1801_YX_Last_State     = 0x02ef00)

BYTE           SC1801_YX_Current_State[64];    
#pragma locate (SC1801_YX_Current_State  = 0x02ef40)
BYTE    moni_yx_chg_sav_ptr;
#pragma locate (moni_yx_chg_sav_ptr=0x2ef80)  // 
BYTE    moni_yx_chg_tak_ptr[12];
#pragma locate (moni_yx_chg_tak_ptr=0x2ef90)  //  

WORD    moni_yx_chg[256];
#pragma locate (moni_yx_chg        = 0x2efa0)  // 
WORD    moni_yx_last_state[32];
#pragma locate (moni_yx_last_state = 0x2f1a0)  //
BYTE            LC_WBL_Set_Flag;   
WORD            LC_WBL_Set_New;   
WORD            LC_WBL_Set_Current;   

WORD            yc_chg_in_count[16];
#pragma locate (yc_chg_in_count     = 0x2f1e4)
WORD            yc_chg_out_count[16];
#pragma locate (yc_chg_out_count     = 0x2f204)
WORD            last_yc[16][512];
#pragma locate (last_yc     = 0x2f224)

struct YC_CHANGE_TR {unsigned short chg_yc;
                     unsigned short yc_inf;
                    }yc_change_tr[16][512];    
#pragma locate (yc_change_tr        = 0x33224)
BYTE    mon_ok_flag[16];
#pragma locate (mon_ok_flag=0x3b224)  //
WORD    mon_err_start_time[16];
#pragma locate (mon_err_start_time=0x3b234)  // 
struct PRINTF_INFO{unsigned char bh_pannel;
                   unsigned char bh_unit;
                   unsigned char bhdz_printf_flag;
                   unsigned char bh_printf_flag;
                   unsigned char soe_printf_flag;
                  }printf_info;    
#pragma locate (printf_info        = 0x3b254)

