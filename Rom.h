#ifndef _rom_h
#define _rom_h

#include "common.h"


extern const unsigned short BaudRate[8];

extern const WORD CAN_BUST_XTAL16M[8];

extern const unsigned char BCH_table[256];

extern const WORD CPU_SIO_BAUD_XTAL48M[8];

extern const BYTE P554_XTAL16M_CLOSE_RTS_DELAY[8];  // 300-39400:  0-7

extern const BYTE SYN_WORD_CDT_EB90[6];
extern const BYTE SYN_WORD_CDT_D709[6];
extern const BYTE SYN_WORD_BUS[6];
extern const BYTE SYN_WORD_LSA[6];
extern const BYTE SYN_WORD_SET[6];
extern const BYTE PROTOCOL_CDTBB_SOURCE_ADDR[12];
extern const BYTE PROTOCOL_CDTBB_DESTIN_ADDR[12];
extern const BYTE PROTOCOL_CDTBB_CONTROL_BYTE[12];
extern const BYTE PROTOCOL_CDT9702_SOURCE_ADDR[12];
extern const BYTE PROTOCOL_CDT9702_DESTIN_ADDR[12];
extern const BYTE PROTOCOL_HOSTZF_SOURCE_ADDR[12];
extern const BYTE PROTOCOL_HOSTZF_DESTIN_ADDR[12];
extern const BYTE PROTOCOL_C9302_SOURCE_ADDR[12];
extern const BYTE PROTOCOL_C9302_DESTIN_ADDR[12];
extern const BYTE PROTOCOL_ZLPCDT_SOURCE_ADDR[12];
extern const BYTE PROTOCOL_ZLPCDT_DESTIN_ADDR[12];
extern const BYTE CAN0_AS_BH_MON_ADDR_SET;
extern const BYTE HOSTZF_PROTOCOL_YK_TYPE[12];
//extern const BYTE TIME_VERIFY_PORT;

extern const BYTE LSA_INTERN_POLLING_TURN[16];
extern const WORD LSA_WAIT_TIME_VALUE[32];
extern const WORD YC_f_value_selected[8];    
extern const BYTE  IED_TX_STATE_START_WORD;
extern const BYTE  IED_TX_STATE_DWORD_NUM;
extern const BYTE  PROTOCOL_CDT_YKFG;
extern const WORD  BH_EVENT_TO_SOE_YX_RETURN_TIME;
extern const BYTE  IED_SOURCE_ADDRESS[12];
extern const BYTE MON_FUN_NO[12][5];
extern struct  UNIT_INFO {unsigned short yc_line_start; unsigned short yc_line_num;
                          unsigned short yc_val_start ; unsigned short yc_val_num;   
                          unsigned short yx_start;      unsigned short yx_num;
                          unsigned short yk_start;      unsigned short yk_num;
                          unsigned short dc_start;      unsigned short dc_num;
                          unsigned short ym_start;      unsigned short ym_num;
                          unsigned short protect_addr;  unsigned short protect_unit;
                           //H: pannel L:unit
                          unsigned short unit_type;     unsigned short channel_no;
                         } const unit_info[256];

extern struct  PORT_INFO {unsigned short protocal_type;  
                          unsigned short mirror_unit; // start - end
                          unsigned short bits;
                          unsigned short stopbit;
                          unsigned short bauds;
                          unsigned short semiduplex_parity;
                          unsigned short scan_panel;  // start - end
                          unsigned short scan_unit;   // start - end
                          unsigned short YK_enable;
                          unsigned short protect_enable;
                          unsigned short time_enable;
                          unsigned short table_no;    // YC - YX
                          unsigned short rebound;     //flag  char
                         } const port_info[14];

extern const WORD YX_property[256];
extern const WORD YX_YM[256];
extern const WORD YX_double[256];
extern const WORD YX_delay[512];
extern const WORD YC_transmite_table[4][512];
extern const WORD YX_transmite_table[4][1024];
extern const BYTE YM_transmite_table[4][256];


extern const BYTE YC_coef_table[1024];

extern struct BYQ_DW { WORD yc_val_no;    
                       WORD yx_no[32];
                       BYTE yx_weight[32];
                     }const Byq_Dw[8];

extern struct BH_EVENT_TO_SOE { WORD YX_State_No;
	                            BYTE GroupAddr;    
                                BYTE RLYAddr;
                                BYTE ActionType;
                                BYTE DeviceType;
                              }const BH_Event_To_SOE[512];

extern struct VQC_INFO { BYTE DSA_243_Addr;            ;;;无功补偿装置
                         BYTE Reserved;
	                     BYTE YK0_208_YK_BOARD_Addr;
	                     BYTE YK0_208_YK_BOARD_YKNO;
	                     BYTE YK1_208_YK_BOARD_Addr;
	                     BYTE YK1_208_YK_BOARD_YKNO;
	                     WORD YC_NO_HighSide[16];          ;;;是高压侧和低压侧吗
	                     WORD YC_NO_LowSide[16];
	                     WORD YX_NO[32];             ;;;可能是最大决定原则
                       }const Vqc_Info[2];       ;;;结构体数组

extern struct  DDB_ADDR { BYTE DDB_PORT_ADDR[6];
                        }const DDB_Addr[128];

extern const BYTE  BH_Act_Alarm_Type[256];  

extern const WORD  YX_OR_Table[64][9];
	                              
extern struct  MONI_SY_YX_TABLE{ BYTE   YX_Box_Def;  // d7: if YX led exist; d6:if is double YX; d3-d0:YX_NO in a box
                                 BYTE   YX_Box_Addr;
                                 WORD   YX0_Source_No;  // if single YX only YX0 valid
                                 WORD   YX1_Source_No;
                               }const MONI_SY_YX_Table[256];
                               
extern struct  MONI_SY_YC{ WORD      YC_BOX_ADDR;
						   WORD      YC_SELECT_NO;
	                       short     YC_MUL_XS;    
                           short     YC_DIV_XS;
                           short     YC_ADD_OFFSET;
                         }const Moni_SY_YC[256];

extern struct  YC_ADD{ WORD      YC_SELECT_NO[8];
	                   short     YC_MUL_XS[8];    
                       short     YC_DIV_XS[8];
                       WORD      YC_DESTIN_NO;
                     }const YC_Add[8];

extern struct  SC1801_YK_TABLE {unsigned char  sc1801_no;
                                unsigned char  sys_yk_no;
                               }const SC1801_YK_Table[128];
extern  struct MODBUS_INFO 
              {
               unsigned short yc_address; unsigned short yc_num;
               unsigned short yx_address; unsigned short yx_num;
               unsigned short ts_address; unsigned short ts_num;
               unsigned short yk_address; unsigned short yk_num;
               unsigned short yk_h_addr[4]; unsigned short yk_f_addr[4];
              }const MODBUS_info[32];
              
extern struct BH_EVENT_TO_SOE_RET
              {
               WORD YX_State_No;
               BYTE GroupAddr;
               BYTE RLYAddr;
               BYTE ActionType;
               BYTE DeviceType;
              }const BH_Event_To_SOE_Ret[512];
extern const  WORD  YC_LIMIT_VALUE[512];

extern struct SOE_ZF_INFO
{
     BYTE SOE_START_YX_NO;    //SOE 在本单元中存放的起始序号，在后台填库时对应序号为本单元YX起始号+SOE_START_YX_NO[]+偏移量
     BYTE SOE_SOURCE_NO[48];  //IED 单元厂家提供的SOE信息序号
}const Soe_zf_info[12][32];

extern const WORD  XH_SHSY_LINE_NO[6][32];

extern struct ZLP_SEND_SET
{
     BYTE length;         //0为主动上送型直流屏
     BYTE addr_no;        //地址在发送报文中的点号 >32为无地址
     BYTE check_type;     //校验的类型  0为无校验 1和校验 2异或校验 3 bch校验 4 CRC16校验
     BYTE check_start;    //算校验 报文中的起始点号
     BYTE check_length;   //算校验 报文中的校验长度
     BYTE set;            //备用
     WORD check_no;       //校验在报文中的点号,发送报文中校验为一个字节填0xff**(高位对应点号、低位对应点号)
     			  //		（发送报文中无校验，接收报文有校验,填0xfff0——一个字节校验
     			  //          0xf1f0——两个字节低前高后,0xf0f1——两个字节高前低后）			
     BYTE value[32]; 	  //发送的内容
}const Zlp_send_set[8];
extern struct ZLP_REC_SET
{
     BYTE addr_no;        //地址在发送报文中的点号
     BYTE num_length_no;  //数据的长度所在点号    
     BYTE check_start;    //算校验 报文中的起始点号
     BYTE check_length;   //算校验 报文中的校验长度减去数据长度的差值(除去数据，还有几位算入入校验)
     BYTE head[10];       //固定报文在报文中的点号(0xff结束)
     BYTE yc_type;        //遥测的类型 00--16进制  01--10进制
     BYTE yc_div;	  //缩小的系数
     BYTE length;	  //数据外其余字节的长度（总报文的长度减去报文内长度值）
     BYTE set1;	  	  //备用
     BYTE set2;	  	  //备用
     BYTE set3;	  	  //备用
     BYTE value[48];      //接收的内容	遥信内容为f0,f1,f2....fe
     			  //		遥测内容为00,01,02.....其中00为第一个遥测的高位01第一个遥测的低位
			  //            02为第二个遥测高位03为第二个遥测低位。
			  //不需要处理的内容填ff。	

}const Zlp_rec_set[8];
extern const WORD Print_BHDZ_Type[256][20];
extern const BYTE Print_BHDZ_Property[256];
extern const WORD Print_BHDZ_Unit[256][2];
extern const WORD Print_BH_Action_Type[256][20];
extern const WORD  Print_BHDYH[7];
extern const WORD  Print_YXH[8]; 
extern const WORD Print_BH_selfcheck_Type[48][20];
extern const WORD m_crc16_table[256];
extern const BYTE CAN_important_yc_unit[128];
extern  struct YC_OFFSET
{
     WORD yc_no;         //上送主站对应遥测点号
     WORD offset;        //对应遥测偏移量,主站遥测值＝YC_State[]-偏移量
}const yc_offset[4][32];

extern struct BH_EVENT_ALARM
{
     BYTE warn_yk_add;    //dsa208 yk address:0x30~0x3f
     BYTE warn_yk_no;     //dsa208 yk hao:0~4
     BYTE warn_yk_hf;     //dsa208 yk  H:0xcc  F:0x33
     BYTE warn_FG_yk_add; //dsa208 yk address:0x30~0x3f
     BYTE warn_FG_yk_no;  //dsa208 yk hao:0~4
     BYTE warn_FG_yk_hf;  //dsa208 yk  H:0xcc  F:0x33     
     BYTE action_yk_add;  //dsa208 yk address:0x30~0x3f
     BYTE action_yk_no;   //dsa208 yk hao:0~4
     BYTE action_yk_hf;   //dsa208 yk  H:0xcc  F:0x33
     BYTE action_FG_yk_add; //dsa208 yk address:0x30~0x3f
     BYTE action_FG_yk_no;  //dsa208 yk hao:0~4
     BYTE action_FG_yk_hf;  //dsa208 yk  H:0xcc  F:0x33      
}const bh_event_alarm ;

#endif /* _rom_h */
