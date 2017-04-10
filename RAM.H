#ifndef _ram_h
#define _ram_h



;;;用C语言编程时量 不需要自己指定绝对地址,只要声明好就行,编译器可以自动安排,这就是它的能力
;;;但也多了分配原则,near 可能的意思就是近 在64KB之内
;;;而Far的意思是远 在64之外 是在RAM中


//     -----------------------------------------------------------
extern volatile near  WORD                             RAM_CPU_RST_TAG1;
extern volatile near  WORD                             RAM_CPU_RST_TAG2;

extern volatile near  TYPE_CPU_INT_MONI                RAM_CPU_Int_Moni;
extern volatile near  WORD                             RAM_CPU_INT_Rst_Cn[8];

extern volatile near  BYTE                             Rcd_Info_Myself_Tmp[8];  // for data trans from function call
extern volatile near  BYTE                             Rcd_Info_System_Tmp[8];  // for data trans from function call

//     -----------------------------------------------------------
extern volatile near  TYPE_CAN_RAM_PARA                RAM_CAN0_Para;      ;;;前面的是类型名,后面的是变量名

extern volatile near  BYTE                             CAN0_CurrentSta_Buf[MAX_CAN_NUMBER];
extern volatile near  BYTE                             CAN0_IED_StaChg[MAX_CAN_NUMBER];

extern volatile near  BYTE                             CAN0_ReceCOS_Buf[3*512];
#define                                                AND_CAN_RECECOS_QUELEN      0x1ff

extern volatile near  BYTE                             CAN0_COSACK_Buf[16];
#define                                                AND_CAN_COSACK_QUELEN       0x0f    ;;;就是一个常数仅仅保证一个周期是16

extern volatile near  BYTE                             CAN0_ReceYCCHG_Buf[64];
#define                                                AND_CAN_YCCHG_QUELEN        0x3f
          ;;;用来告诉编译程序他是一个I/O口
extern volatile near  BYTE                             CAN0_RecePort_Buf[12*256];
extern volatile near  BYTE                             CAN0_ReceProc_Buf[512];          ;;;接收到帧内容的保留区
extern volatile near  BYTE                             CAN0_TranProc_Buf[512];


extern volatile near  BYTE                             CN_CAN0_Comm_Error[MAX_CAN_NUMBER];
extern volatile near  BYTE                             CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER];
extern volatile near  BYTE                             CAN0_Comm_CurrentSta[MAX_CAN_NUMBER];

extern volatile near  BYTE                             CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr;
extern volatile near  BYTE                             CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr;

extern volatile near  WORD                             CAN_Time2048ms_Last_Value;
extern volatile near  WORD                             CAN_Not_Idle_Cn;
extern volatile near  BYTE                             CAN_NOW_POL_YCALL_FLAG;

//     -----------------------------------------------------------
extern volatile near  WORD                             Cn_SIO_CAN_Reset[16];

extern volatile near  WORD                             SIO_CAN_Need_Reset;


//     -----------------------------------------------------------
extern volatile near  WORD                             port_recv_pt[12];
extern volatile near  WORD                             port_send_pt[12];
extern volatile near  WORD                             port_send_len[12];
extern volatile near  WORD                             port_recv_dl[12];

extern volatile near  WORD                             P554_Port_Tran_Close_RTS_Cn[12];
extern volatile near  BYTE                             P554_Port_Transing_Lastbyte[12];


//     -----------------------------------------------------------
extern volatile near  WORD                             YC_State[2048];   ;;;这些空间应该是通用的
extern volatile near  WORD                             YX_State[256];    ;;;这个就限制了所有装置的遥信最多有256个,以字为单位
extern volatile near  UL                               YM_State[256];

extern volatile near  BYTE                             BCH_Array[6];


extern volatile near  BYTE                             Portx_Poll_First[14];
extern volatile near  BYTE                             HOST_ZF_enable;
extern volatile near  BYTE                             HOST_YK_Doing;
extern volatile near  BYTE                             HOST_YK_Port_No;
extern volatile near  WORD                             HOST_YK_Doing_Begin_Time;

extern volatile near  WORD                             BH_To_YX_Delay_Begin_Time[512];

extern volatile near  WORD                             BH_Report_Bank_Sav_Ptr;    
extern volatile near  WORD                             BH_Report_Bank_Tak_Ptr[12]; 
extern volatile near  BYTE                             IEC_last_frame_buf[12][256];
extern volatile near  BYTE                             temp_buf[13];   
extern volatile near  BYTE                             YX_FG_FLAG;
extern volatile near  BYTE                             YX_FG_WAIT_FLAG;
extern volatile near  WORD                             YX_FG_WAIT_TIME;
// from rambank.h







;;;控制信息 状态信息 数据信息
extern unsigned char Register_554[12][512];       ;;;原理不变但个个芯片应该有区别
struct WR_REG_16554{unsigned char THR; unsigned char IER;       ;;;发送器保持寄存器   中断使能寄存器  
                    unsigned char FCR; unsigned char LCR;       ;;;FIF0控制寄存器     线路控制寄存器
                    unsigned char MCR; unsigned char NC;        ;;;调制解调控制寄存器  空的
                    unsigned char NC1; unsigned char SR;
                    };

struct RD_REG_16554{unsigned char RHR; unsigned char NC;
                    unsigned char ISR; unsigned char NC1;
                    unsigned char NC2; unsigned char LSR;        ;;;                     线路状态寄存器
                    unsigned char MSR; unsigned char SR;
                    };       ;;;调制解调器状态寄存器  ?状态寄存器
  
struct BD_REG_16554{unsigned short BDR;
					};








extern WORD           main_loop;                         ;;;就是一个单元名称而已
extern BYTE           p2_img;
extern BYTE           Dsa208_Work_State;

//Set port
extern BYTE           set_port_rece_send_buf[256];


extern WORD           set_buffer[256];


extern BYTE           set_transmit[256];
extern BYTE           set_send_buf[512];
extern BYTE           set_recv_buf[512];


//Port_x
extern WORD           port_flag[12][128];        
extern BYTE           port_recv[12][512]; 
extern BYTE           port_send[12][512];
extern BYTE           port_transmit[16][512];   // can include        ;;;他应该有一个很大的特点就功能不复杂但容量很大实时性很好.
extern BYTE           port_efficacy[12][512];
extern BYTE           port_deal_buf[12][512];

extern BYTE           port_report[512];        ;;;暂存遥信内容 ,这个表示上送到后台的吗   以字节为单位

extern WORD           port_mon[16];             // can include
extern BYTE           port_transmit_flag[16];   // can include


//Exchange buffer
extern WORD           Ex_YK_start[14];
extern WORD           Ex_YK_end[14];

extern WORD           exchange_unit;
extern WORD           exchange_src_port;
extern WORD           exchange_target_port;
extern WORD           exchange_type;
extern WORD           exchange_Len;
extern BYTE          *exchange_src_pt;
extern BYTE           exchange_buf[512];



extern struct YX_CHANGE {unsigned short offset_no;
                         unsigned short chg_cont0;
                         unsigned short chg_cont1;
                         unsigned short chg_bit0;
                         unsigned short chg_bit1;
                        }yx_change[256];    

extern struct YX_EVENT {unsigned short soe_ms;
                        unsigned short soe_sec_min;
                        unsigned short soe_hor_date;
                        unsigned short soe_chn_sta;  
                       }yx_event[1024],yx_event_tr[4][1024];    

/*!!!!!!!!!!!!!!!!!!!!! data transmite buffer !!!!!!!!!!!!!!!!!!!!!!!!*/
extern WORD            YX_state_tr[4][64];
extern struct YX_CHANGE_TR  {unsigned short offset_no;
                             unsigned short chg_cont0;
                             unsigned short chg_cont1;
                             unsigned short chg_bit0;
                             unsigned short chg_bit1;
                            } yx_change_tr[4][256];    

extern WORD           yx_chg_out_pt[14];
extern WORD           yx_soe_out_pt[14];
extern WORD           yx_chg_ex_pt[14];
extern WORD           yx_soe_ex_pt[14];
extern WORD           yx_chg_tr_ex_pt[4];
extern WORD           yx_soe_tr_ex_pt[4];
extern WORD           yx_chg_tr_in_pt[4];
extern WORD           yx_soe_tr_in_pt[4];

extern WORD           yx_chg_in_pt;
extern WORD           yx_soe_in_pt;





extern BYTE           bus_unit_err[64];
extern BYTE           bus_err_rec[224];
extern BYTE           bus_err_rec_pt;

//public define

extern BYTE           unit_set_flag[256];



extern BYTE           BH_Saved_Report_Bank[256][32];        // 256 may -> 512; 32 -> 64
#define               BH_REPORT_BANK_SIZE                   256

extern BYTE           BH_Bank_Report[512];

extern BYTE           CAN_246_Trans_YK_Buf[8];
#define               CAN_246_TRANS_YK_STA_IDLE             0x00
#define               CAN_246_TRANS_YK_STA_WAIT_VERIFY      0x01
#define               CAN_246_TRANS_YK_STA_WAIT_CONFIRM     0x02
#define               CAN_246_TRANS_YK_STA_TRANSING_TO_CAN  0x03

extern WORD           CAN_246_YK_Begin_WAIT_VERIFY;
extern WORD           CAN_246_YK_Begin_WAIT_CONFIRM;

extern BYTE           bh_event_alarm_warn;    
extern BYTE           bh_event_alarm_action;    
extern BYTE           bh_event_alarm_now_action_warn;    
extern BYTE           bh_event_alarm_Reserved;    
extern WORD           bh_event_alarm_warn_Begin_Clk;    
extern WORD           bh_event_alarm_action_Begin_Clk;    

extern BYTE           port_mirror[16];   

extern BYTE           Now_CANx_Sw_Channel_Sta[256];  // include addr 64-255
extern BYTE           CAN0_CPUASKCMU_Trans_Buf[256*12];

extern WORD           SC1801_YC_Last_State[192];    
extern WORD           SC1801_YC_Current_State[192];    
extern BYTE           SC1801_YX_Last_State[64];    
extern BYTE           SC1801_YX_Current_State[64];
extern BYTE           moni_yx_chg_sav_ptr;
extern BYTE           moni_yx_chg_tak_ptr[12];
extern WORD           moni_yx_chg[256]; 
extern WORD           moni_yx_last_state[32];   
extern BYTE            LC_WBL_Set_Flag;   
extern WORD            LC_WBL_Set_New;   
extern WORD            LC_WBL_Set_Current;   
extern WORD            yc_chg_in_count[16];
extern WORD            yc_chg_out_count[16];
extern WORD            last_yc[16][512];
extern struct YC_CHANGE_TR {unsigned short chg_yc;
                     unsigned short yc_inf;
                    }yc_change_tr[16][512];
extern BYTE    mon_ok_flag[16];
extern WORD    mon_err_start_time[16];
extern struct PRINTF_INFO{unsigned char bh_pannel;
                          unsigned char bh_unit;
                          unsigned char bhdz_printf_flag;
                          unsigned char bh_printf_flag;
                          unsigned char soe_printf_flag;
                         }printf_info;    
    
#endif /* _ram_h */
