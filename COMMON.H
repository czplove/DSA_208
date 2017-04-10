;;;此文件为208 程序中部分全局变量定义库文件


// 'compile condtionally' set here
//#define BH_EVENT_ALARM_ENABLE     


      ;;;难道下面的作用就是在字中取出字节内容来用吗
#define byte0(x)                                  (*((unsigned char  *)&x    +0))        ;;;这个是定义的带参数的宏
#define byte1(x)                                  (*((unsigned char  *)&x+1))        ;;;这个宏最终取出的是内容
#define byte2(x)                                  (*((unsigned char  *)&x+2))
#define byte3(x)                                  (*((unsigned char  *)&x+3))

#define low_word(x)                               (*((unsigned short *)&x+0))
#define high_word(x)                              (*((unsigned short *)&x+1))

#define YES                                       0xaa
#define NO                                        0x55
#define UNKNOW                                    0x00
#define GOOD                                      0xaa
#define ERR                                       0x55

#define ON                                        0xaa
#define OFF                                       0x55

#define PARITY_DISABLE                            0x00
#define PARITY_ODD                                0x01
#define PARITY_EVEN                               0x02

#define MAX_CAN_NUMBER                            0x40
#define MAX_COM_NUMBER                            0x10





typedef unsigned char                             BYTE;   ;;;类型定义
typedef unsigned short                            WORD;
typedef unsigned short                            UI;
typedef unsigned long                             UL;



           ;;;以下这些的作用应该是计数器用于保证,程序跑飞之后能够知道,因为正常的时候应该及时清零
typedef struct{ WORD EXINT0_INT_DEAD_COUNTER;    // moni EXINT0 hold triggle level,not reset, so can't come into short again
                WORD EXINT1_INT_DEAD_COUNTER;    // as above
                WORD EXINT2_INT_DEAD_COUNTER;    // as above
                WORD EXINT3_INT_DEAD_COUNTER;    // as above
                WORD   EPA0_INT_DEAD_COUNTER;    // as above
                WORD   EPA1_INT_DEAD_COUNTER;    // moni with some reason , only soft_time_1ms is disabled.
                WORD   MAINLOOP_DEAD_COUNTER;    // moni main_loop come into dead loop
              }TYPE_CPU_INT_MONI;
;;;类型定义    以后可以用TYPE_CPU_INT_MONI来说明结构体变量

typedef struct{ BYTE RECE_CPU_REPLYINFO_ENABLE   ;      ;;;决定是否现在可以接收帧内容
                BYTE RECE_CPU_REPLYINFO_ADDR     ;      ;;;接收_CPU_返回信息_地址 单元地址
                BYTE RECE_CPU_REPLYINFO_WISHPROC ;      ;;;希望处理吗
                BYTE RECE_CPU_LONGFRAME_FRAMESUM ;      ;;;总帧数
                BYTE RECE_CPU_LONGFRAME_FRAMECUR ;       ;;;现在接收帧的号码

                BYTE TRAN_CPU_INFO_ADDR          ;      ;;;记录哪台装置接收这个报文
                BYTE TRAN_CPU_LONGFRAME_FRAMESUM ;
                BYTE TRAN_CPU_LONGFRAME_FRAMECUR ;
                BYTE TRAN_CPU_IS_LAST_FRAME      ;
                BYTE F_TRANS_STATUS              ;
              }TYPE_CAN_PORT;







typedef struct{ BYTE CAN_PROHIBITPOLLADDR_FROM_COMx   ;
                BYTE CAN_PROHIBITPOLLADDR_COMP   ;
                BYTE CAN_MON_WAIT_STATYP         ;
                BYTE CAN_MON_STATUS              ;
                BYTE CAN_MON_NEED_REPLY          ;     
                BYTE CAN_NEEDTRANSTOHOST         ;      
              }TYPE_CAN_PROC; 











typedef struct{ WORD CAN_BEGINWAITCPUREPLY_TIME  ;    ;;;开始等待CPU回答时间
                WORD CAN_BEGINWAITRTUREPLY_TIME  ;    ;;;开始等待RTU回答时间
                WORD CAN_BEGIN_VERICPUCLK_TIME   ;    ;;;    核实CPU时钟时间
                WORD CAN_BEGIN_POL_YCALL_TIME    ;     
                WORD CAN_BEGIN_POL_YXALL_TIME    ;
                WORD CAN_BEGIN_POL_YMALL_TIME    ;
                WORD CAN_BEGIN_POL_DD_AC_TIME    ;
                BYTE CAN_NOW_POL_INFO_TYPE       ;     ;;;   信息
                BYTE CAN_NOW_POL_CPU_MAC_ADDR  ;       ;;;    物理地址 
                
                BYTE CAN_RECEYCCHG_BUF_SAV_PTR   ;
                BYTE CAN_RECEYCCHG_BUF_TAK_PTR   ;
                
                BYTE CAN_RECECOS_RETACK_SAV_PTR  ;      
                BYTE CAN_RECECOS_RETACK_TAK_PTR  ;
                
                WORD CAN_RECECOS_BUF_SAV_PTR     ;
                WORD CAN_RECECOS_BUF_TAK_PTR     ;

                BYTE CAN_PORT_RECE_BUF_SAV_PTR   ;     ;;;接收 缓冲保存指针
                BYTE CAN_PORT_RECE_BUF_TAK_PTR   ;     ;;;接收 缓存任务指针 
              }TYPE_CAN_RAM_PARA;                       ;;;定义了结构体的变量类型名,可以用他来定义结构体变量







// define types of Rcd info
// ------------ RCD_INFO_MYSELF_AREA0 ----------------  ;;;     自检_区域
// chip info    0 - undefined
#define RCD_INFO_MYSELF_AREA0_CPU                 0xe1
#define RCD_INFO_MYSELF_AREA0_ROM                 0xe2
#define RCD_INFO_MYSELF_AREA0_RAM                 0xe3
#define RCD_INFO_MYSELF_AREA0_FLASH               0xe4
#define RCD_INFO_MYSELF_AREA0_DALLAS              0xe5
#define RCD_INFO_MYSELF_AREA0_MAX706              0xe6
#define RCD_INFO_MYSELF_AREA0_SJA1000             0xe7
#define RCD_INFO_MYSELF_AREA0_16C554              0xe8
#define RCD_INFO_MYSELF_AREA0_ALTRA               0xe9

// port info
#define RCD_INFO_MYSELF_AREA0_554_0_A             0x00
#define RCD_INFO_MYSELF_AREA0_554_0_B             0x01
#define RCD_INFO_MYSELF_AREA0_554_0_C             0x02
#define RCD_INFO_MYSELF_AREA0_554_0_D             0x03
#define RCD_INFO_MYSELF_AREA0_554_1_A             0x04
#define RCD_INFO_MYSELF_AREA0_554_1_B             0x05
#define RCD_INFO_MYSELF_AREA0_554_1_C             0x06
#define RCD_INFO_MYSELF_AREA0_554_1_D             0x07
#define RCD_INFO_MYSELF_AREA0_554_2_A             0x08
#define RCD_INFO_MYSELF_AREA0_554_2_B             0x09
#define RCD_INFO_MYSELF_AREA0_554_2_C             0x0a
#define RCD_INFO_MYSELF_AREA0_554_2_D             0x0b
#define RCD_INFO_MYSELF_AREA0_CAN                 0x0c
#define RCD_INFO_MYSELF_AREA0_CAN1                0x0d
#define RCD_INFO_MYSELF_AREA0_CPUCOM              0x0e

// ------------ RCD_INFO_MYSELF_AREA1 ----------------
// CPU ACTION   varible  0 - undefined
#define RCD_INFO_MYSELF_AREA1_CPU_RST             0x01
#define RCD_INFO_MYSELF_AREA1_CPU_EXINT0_INT      0x02
#define RCD_INFO_MYSELF_AREA1_CPU_EXINT1_INT      0x03
#define RCD_INFO_MYSELF_AREA1_CPU_EXINT2_INT      0x04
#define RCD_INFO_MYSELF_AREA1_CPU_EXINT3_INT      0x05
#define RCD_INFO_MYSELF_AREA1_CPU_EPA0_INT        0x06
#define RCD_INFO_MYSELF_AREA1_CPU_EPA1_INT        0x07
#define RCD_INFO_MYSELF_AREA1_CPU_MAINLOOP        0x08

// Dallas Mem varible  0 - undefined
#define RCD_INFO_MYSELF_AREA1_RCD_MYSELF_PTR_ADDR 0x01
#define RCD_INFO_MYSELF_AREA1_RCD_SYSTEM_PTR_ADDR 0x02

#define RCD_INFO_MYSELF_AREA1_DALLAS_YEAR         0x03
#define RCD_INFO_MYSELF_AREA1_DALLAS_MONTH        0x04
#define RCD_INFO_MYSELF_AREA1_DALLAS_DATE         0x05
#define RCD_INFO_MYSELF_AREA1_DALLAS_DAY          0x06
#define RCD_INFO_MYSELF_AREA1_DALLAS_HOUR         0x07
#define RCD_INFO_MYSELF_AREA1_DALLAS_MINUTE       0x08
#define RCD_INFO_MYSELF_AREA1_DALLAS_SECOND       0x09


// ------------ RCD_INFO_MYSELF_AREA2 ----------------
// CPU ACTION   varible  0 - undefined
#define RCD_INFO_MYSELF_AREA2_CPURST_COLD         0x01
#define RCD_INFO_MYSELF_AREA2_CPURST_HOT          0x02
#define RCD_INFO_MYSELF_AREA2_CPU_RUN_DEAD        0x03

// Dallas Mem varible  0 - undefined
#define RCD_INFO_MYSELF_AREA2_PTR_ADDR_XOR_ERR    0x01
#define RCD_INFO_MYSELF_AREA2_BCD_DATA_ERR        0x02

// PORTx      varible  0 - undefined
#define RCD_INFO_MYSELF_AREA2_PORT_STA_RET_IDLE   0x01


// ---------------------------------------------------
// ------------ RCD_INFO_SYSTEM_AREA0 ----------------
// chip info    0 - undefined
#define RCD_INFO_SYSTEM_AREA0_CPU                 0xe1
#define RCD_INFO_SYSTEM_AREA0_ROM                 0xe2
#define RCD_INFO_SYSTEM_AREA0_RAM                 0xe3
#define RCD_INFO_SYSTEM_AREA0_FLASH               0xe4
#define RCD_INFO_SYSTEM_AREA0_DALLAS              0xe5
#define RCD_INFO_SYSTEM_AREA0_MAX706              0xe6
#define RCD_INFO_SYSTEM_AREA0_SJA1000             0xe7
#define RCD_INFO_SYSTEM_AREA0_16C554              0xe8
#define RCD_INFO_SYSTEM_AREA0_ALTRA               0xe9

// port info
#define RCD_INFO_SYSTEM_AREA0_554_0_A             0x00
#define RCD_INFO_SYSTEM_AREA0_554_0_B             0x01
#define RCD_INFO_SYSTEM_AREA0_554_0_C             0x02
#define RCD_INFO_SYSTEM_AREA0_554_0_D             0x03
#define RCD_INFO_SYSTEM_AREA0_554_1_A             0x04
#define RCD_INFO_SYSTEM_AREA0_554_1_B             0x05
#define RCD_INFO_SYSTEM_AREA0_554_1_C             0x06
#define RCD_INFO_SYSTEM_AREA0_554_1_D             0x07
#define RCD_INFO_SYSTEM_AREA0_554_2_A             0x08
#define RCD_INFO_SYSTEM_AREA0_554_2_B             0x09
#define RCD_INFO_SYSTEM_AREA0_554_2_C             0x0a
#define RCD_INFO_SYSTEM_AREA0_554_2_D             0x0b
#define RCD_INFO_SYSTEM_AREA0_CAN                 0x0c
#define RCD_INFO_SYSTEM_AREA0_CAN1                0x0d
#define RCD_INFO_SYSTEM_AREA0_CPUCOM              0x0e

// ------------ RCD_INFO_SYSTEM_AREA1 ----------------
// CPU ACTION   varible  0 - undefined
#define RCD_INFO_SYSTEM_AREA1_CPU_RST             0x01
#define RCD_INFO_SYSTEM_AREA1_CPU_EXINT0_INT      0x02
#define RCD_INFO_SYSTEM_AREA1_CPU_EXINT1_INT      0x03
#define RCD_INFO_SYSTEM_AREA1_CPU_EXINT2_INT      0x04
#define RCD_INFO_SYSTEM_AREA1_CPU_EXINT3_INT      0x05
#define RCD_INFO_SYSTEM_AREA1_CPU_EPA0_INT        0x06
#define RCD_INFO_SYSTEM_AREA1_CPU_EPA1_INT        0x07
#define RCD_INFO_SYSTEM_AREA1_CPU_MAINLOOP        0x08

// Dallas Mem varible  0 - undefined
#define RCD_INFO_SYSTEM_AREA1_DALLAS_YEAR         0x03
#define RCD_INFO_SYSTEM_AREA1_DALLAS_MONTH        0x04
#define RCD_INFO_SYSTEM_AREA1_DALLAS_DATE         0x05
#define RCD_INFO_SYSTEM_AREA1_DALLAS_DAY          0x06
#define RCD_INFO_SYSTEM_AREA1_DALLAS_HOUR         0x07
#define RCD_INFO_SYSTEM_AREA1_DALLAS_MINUTE       0x08
#define RCD_INFO_SYSTEM_AREA1_DALLAS_SECOND       0x09


// ------------ RCD_INFO_SYSTEM_AREA2 ----------------
// CPU        ACTION   varible  0 - undefined
#define RCD_INFO_SYSTEM_AREA2_CPURST_COLD         0x01
#define RCD_INFO_SYSTEM_AREA2_CPURST_HOT          0x02
#define RCD_INFO_SYSTEM_AREA2_CPU_RUN_DEAD        0x03

// Dallas     Mem      varible  0 - undefined
#define RCD_INFO_SYSTEM_AREA2_PTR_ADDR_XOR_ERR    0x01
#define RCD_INFO_SYSTEM_AREA2_BCD_DATA_ERR        0x02

// CAN        ACTION   varible  0 - undefined
#define RCD_INFO_SYSTEM_AREA2_YK_EXECUTE          0x01
#define RCD_INFO_SYSTEM_AREA2_SET_MODI            0x02
#define RCD_INFO_SYSTEM_AREA2_SETGROUP_MODI       0x03
#define RCD_INFO_SYSTEM_AREA2_246_YK_EXECUTE      0x04
#define RCD_INFO_SYSTEM_AREA2_246_YK_CANCEL       0x05







//-----------------  SER definition  ----------------------
#define PORT_NO_P554_00                           0x00
#define PORT_NO_P554_01                           0x01
#define PORT_NO_P554_02                           0x02
#define PORT_NO_P554_03                           0x03
#define PORT_NO_P554_04                           0x04
#define PORT_NO_P554_05                           0x05
#define PORT_NO_P554_06                           0x06
#define PORT_NO_P554_07                           0x07
#define PORT_NO_P554_08                           0x08
#define PORT_NO_P554_09                           0x09
#define PORT_NO_P554_10                           0x0a
#define PORT_NO_P554_11                           0x0b
#define PORT_NO_CAN_0                             0x0c        ;;;口_number_CAN_0  这个定义的是数组的行号,这类数组是512列的,但是不一定都用,只是有这样区域
#define PORT_NO_CAN_1                             0x0d
          ;;;以上14个是各个端口的偏移量










#define PORT_NO_CPU_SIO                           0x0e


#define RCD_INFO_PTR_LENGTH                       0x80  
#define AND_RCD_INFO_PTR_LENGTH                   0x7f

#define EPA1_PERIOD                              12000   // f=48M, for 1ms   ;;;由于采用48MHz的晶振频率 且时钟源是fosc/4所以有250/3 Ns的分辨率


#define DATABASE_YC_INFO_LEN                      0x02
#define DATABASE_YX_INFO_LEN                      0x02
#define DATABASE_YM_INFO_LEN                      0x04
#define DATABASE_DD_INFO_LEN                      0x04





// IDLE
#define PORT_EXCHANGE_STA_IDLE                    0x00
// PROCEEDING
#define PORT_EXCHANGE_STA_START                   0x41           ;;;继续处理吗
#define PORT_EXCHANGE_STA_PROCING                 0x42
// END
#define PORT_EXCHANGE_STA_END_OK                  0x81
#define PORT_EXCHANGE_STA_END_ERR                 0x82
#define PORT_EXCHANGE_STA_END_WAIT_TIME_OUT       0x83
// WAIT
#define PORT_EXCHANGE_STA_WAIT_EXCHANGE_INFO      0xc1



//----------------  CPU   definition  ---------------------
#define CBS_38400                               0x804D
#define CBS_9600                                0x8137
#define CBS_1200                                0x89C3

#define CPU_SIO_BAUD_TO_PC                   CBS_38400
#define CPU_SIO_BAUD_TO_LED                  CBS_38400

#define SETPORT                                   0xf0

#define WHOLE_PACKAGE                             0xff


//----------------  CORE  definition  ---------------------
#define CORE_CODE_YK_CHOOSE                       0xe0      ;;;难道这些是内部应用层得内容
#define CORE_CODE_YK_VERIFY                       0xe1      ;;;这个数据可能就是应用层得数据
#define CORE_CODE_YK_EXECUTE                      0xe2
#define CORE_CODE_YK_CANCEL                       0xe3


#define CORE_CODE2_YK_CLOSE                       0xcc        ;;;遥控 合
#define CORE_CODE2_YK_TRIP                        0x33        ;;;遥控 分
#define CORE_CODE2_YK_ERROR                       0xff        ;;;电路 出错
#define CORE_CODE2_YK_EXECUTE                     0xaa        ;;;遥控 执行
#define CORE_CODE2_YK_CANCEL                      0x55        ;;;遥控 取消 




// none
#define PROTOCOL_NONE                           0x0000        ;;;表示没有配规约
         ;;;以下是所有规约的定义的类型名称及类型号
// IED- BUS protocol for all info
#define PROTOCOL_INTERNAL_BUS                   0x0101  // JK

#define PROTOCOL_MODBUS                         0x0201  // JK
#define PROTOCOL_WD_SEU                         0x0202  // reserved 02xx-05xx for other type MODBUS
#define PROTOCOL_MOD_HT                         0x0203  // JK
#define PROTOCOL_MODBUS_ABB                     0x0204  // JK
#define PROTOCOL_MOD_HFHK                       0x0205  // JK
#define PROTOCOL_MODBUS_ST2                     0x0206  // JK
#define PROTOCOL_DS_XXQ                         0x0207  // JK

#define PROTOCOL_IEC103_RCS                     0x0801
#define PROTOCOL_IEC103_ISA                     0x0802
#define PROTOCOL_IEC103_MGT                     0x0803
#define PROTOCOL_IEC103_LSA                     0x0804
#define PROTOCOL_IEC103_NAZI                    0x0805
#define PROTOCOL_IEC103_BJSF                    0x0806

// IED- ZLP
#define PROTOCOL_ZLP_CDT                        0x1101  // JK
#define PROTOCOL_ZLP_CDT_PSMA10                 0x1102  // JK
#define PROTOCOL_ZLP_MK                         0x1103  // JK
#define PROTOCOL_ZLP_DH                         0x1104  // JK
#define PROTOCOL_ZLP_SHCY                       0x1105  // JK
#define PROTOCOL_ZLP_JRK                        0x1106  // JK
#define PROTOCOL_ZLP_DPMZ                       0x1107  // JK
#define PROTOCOL_ZLP_LNJC                       0x1108  // JK
#define PROTOCOL_ZLP_WCK3                       0x1109  // JK
#define PROTOCOL_ZLP_ZWK                        0x110a  // JK
#define PROTOCOL_ZLP_DH2                        0x110b  // JK
#define PROTOCOL_ZLP_WZJK                       0x110c  // JK
#define PROTOCOL_ZLP_SPC                        0x110d  // JK
#define PROTOCOL_ZLP_JZDQ                       0x110e  // JK
#define PROTOCOL_ZLP_JK2K1                      0x110f  // JK
#define PROTOCOL_ZLP_PSME10                     0x1601  // JK
#define PROTOCOL_ZLP_SHCY2                      0x1602  // JK
#define PROTOCOL_ZLP_SC                         0x1603  // JK
#define PROTOCOL_ZLP_NK                         0x1604  // JK
#define PROTOCOL_ZLP_ATX                        0x1605  // JK
#define PROTOCOL_ZLPDPMZ                        0x1606  // JK
#define PROTOCOL_ZLP_EASY			0x1111  // jk
#define PROTOCOL_ZLP_ZJHY			0x1112  // jk
#define PROTOCOL_ZLP_JZDQ1                      0x1113  // JK
#define PROTOCOL_ZLP_CDT1                       0x1114  // JK
#define PROTOCOL_ZLP_ZJSC                       0x1115  // JK
#define PROTOCOL_ZLPTG                          0x1116
#define PROTOCOL_ZLP_MK80                       0x1117  // JK
#define PROTOCOL_ZLP_POD                        0x1118  // JK
#define PROTOCOL_ZLP_STD                        0x1119  // JK
#define PROTOCOL_ZLP_BJSF                       0x111a  // JK
#define PROTOCOL_ZLP_JS                       0x111b  //         yjw
// IED- GPS
#define PROTOCOL_GPS_NARI                       0x1201  // JK
#define PROTOCOL_GPS12                          0x1202  // JK
#define PROTOCOL_GPS_ST2K                       0x1203  // JK
// IED- JZ
#define PROTOCOL_JZPLC                          0x1301  // JK
#define PROTOCOL_JZXJY                          0x1302  // JK

// IED- XJD
#define PROTOCOL_XJD_SDX                        0x1401  // JK
#define PROTOCOL_XDL_MLX                        0x1402  // JK
#define PROTOCOL_ZPJK                           0x1403  // JK
// IED- VQC
#define PROTOCOL_VQC_DWK                        0x1501  // JK
#define PROTOCOL_VQC_SDJX                       0x1502  // JK
#define PROTOCOL_VQC_DWK3                       0x1503  // JK
#define PROTOCOL_VQC_MCVQ1                      0x1504  // JK
#define PROTOCOL_YZTY_HMK                       0x1505  // JK
#define PROTOCOL_VQC_VQC100                     0x1506  // JK
// IED- XHXQ
#define PROTOCOL_XHXQ_KDXH                      0x1701  // JK
#define PROTOCOL_XHXQ_SHSY                      0x1702  // JK
#define PROTOCOL_XHXQ_ZGTD                      0x1703  // JK
#define PROTOCOL_XHXQ_HDXH                      0x1704  // JK

// IED- WD
#define PROTOCOL_WD_FJLD                        0x1801  // JK

// IED- JLP
#define PROTOCOL_JLP_PJD10                      0x1901  // JK
#define PROTOCOL_JLP_ATX                        0x1902  // JK
// IED- TQ
#define PROTOCOL_TQ_CM320                       0x1a01  // JK
//IED-Li ci xi tong
#define PROTOCOL_LC_ZYDL                        0x1b01  // JK
// IED- DDB
#define PROTOCOL_DDB_BB                         0x2101  // JK
#define PROTOCOL_DDB_WS                         0x2102  // JK
#define PROTOCOL_DDB_HT                         0x2103  // JK
#define PROTOCOL_DDB_HX                         0x2104  // JK
#define PROTOCOL_DDB_WSKZ                       0x2105  // JK
#define PROTOCOL_DDB_XMZ                        0x2106  // JK
#define PROTOCOL_DDB_NJKD                       0x2107  // JK
#define PROTOCOL_DDB_CD194E                     0x2108  // JK

// IED- rece CDT
#define PROTOCOL_C9302_CDT                      0x3101  // JK

// IED- BH
#define PROTOCOL_LFP                            0x4101  // BH 
#define PROTOCOL_DFP201                         0x4121
#define PROTOCOL_DFP500                         0x4201
#define PROTOCOL_LSA                            0x4202
#define PROTOCOL_XX_HYR12                       0x4202  
#define PROTOCOL_ISA                            0x4202  
#define PROTOCOL_REC_HOST                       0x4203  
#define PROTOCOL_LFP20                          0x4204
#define PROTOCOL_ISA300_CDT                     0x4205


// IED- JK
#define PROTOCOL_DSA301                         0x4901  // JK PORT 
#define PROTOCOL_PA100                          0x4902  // JK+BH
#define PROTOCOL_BJKR101                        0x4903  // JK+BH
#define PROTOCOL_FSA2000                        0x4904  // JK+BH
#define PROTOCOL_DISA1                          0x4905  // JK+BH
#define PROTOCOL_XJBH_FCK                       0x4906  // JK+BH
#define PROTOCOL_NAZI94                         0x4907  // JK+BH
#define PROTOCOL_BJHX                           0x4908  // JK+BH
#define PROTOCOL_NAZI94JK                       0x4909  // JK

// IED- BH+JK
#define PROTOCOL_CAN_DSA                        0x4C01  // JK+BH

// HOST- CDT
#define PROTOCOL_CDT_BB                         0x8101  // HOST
#define PROTOCOL_CDT_D709                       0x8102  
#define PROTOCOL_CDT_HOSTZF                     0x8103  
#define PROTOCOL_CDT_XT9702                     0x8104  
#define PROTOCOL_CDT_HOSTZF_VQC                 0x8108  

// HOST- OTHER
#define PROTOCOL_1801                           0x8301
#define PROTOCOL_1801_DF                        0x8302
#define PROTOCOL_U4F                            0x8401

#define PROTOCOL_IEC101                         0x8801
#define PROTOCOL_IEC101_YTDF                    0x8802
#define PROTOCOL_IEC101_LNJC                    0x8803
#define PROTOCOL_IEC101_NRDW                    0x8804

// HOST- MONIPIN
#define PROTOCOL_MONI_SX                        0x9101
#define PROTOCOL_MONI_SY                        0x9102
#define PROTOCOL_MONI_HT                        0x9103

#define PROTOCOL_PRINTF                         0xb001
// HOST- OTHER
#define PROTOCOL_SETPORT                        0xe001



#endif /* _common_h */
