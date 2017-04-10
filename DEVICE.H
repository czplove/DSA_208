




/*___________________________ P554_0    Port_Addr ___________________________*/   

#define P554_BaseAddr            0x0F61000              ;;;这个是16554芯片在实际系统中的地址
#define P554_AddrDistance        0x200                  ;;;每块地址给这么大的地址空间
            ;;;通过对这些内部寄存器的控制就可以完成
#define P554_RBR                 0         // R           
#define P554_THR                 0         // W
#define P554_DLL                 0
#define P554_DLM                 1
#define P554_IER                 1
#define P554_FCR                 2         // W 
#define P554_IIR                 2         // R 
#define P554_LCR                 3
#define P554_MCR                 4
#define P554_LSR                 5
#define P554_MSR                 6
#define P554_SLF                 7


#define P554_0_A_BaseAddr        P554_BaseAddr+P554_AddrDistance*0  // 0x0F61000
#define P554_0_A_RBR             P554_0_A_BaseAddr+P554_RBR         // R
#define P554_0_A_THR             P554_0_A_BaseAddr+P554_THR         // W
#define P554_0_A_DLL             P554_0_A_BaseAddr+P554_DLL
#define P554_0_A_DLM             P554_0_A_BaseAddr+P554_DLM
#define P554_0_A_IER             P554_0_A_BaseAddr+P554_IER
#define P554_0_A_FCR             P554_0_A_BaseAddr+P554_FCR         // W 
#define P554_0_A_IIR             P554_0_A_BaseAddr+P554_IIR         // R 
#define P554_0_A_LCR             P554_0_A_BaseAddr+P554_LCR
#define P554_0_A_MCR             P554_0_A_BaseAddr+P554_MCR
#define P554_0_A_LSR             P554_0_A_BaseAddr+P554_LSR
#define P554_0_A_MSR             P554_0_A_BaseAddr+P554_MSR
#define P554_0_A_SLF             P554_0_A_BaseAddr+P554_SLF
     

#define P554_0_B_BaseAddr        P554_BaseAddr+P554_AddrDistance*1  // 0x0F61200
#define P554_0_B_RBR             P554_0_B_BaseAddr+P554_RBR         // R
#define P554_0_B_THR             P554_0_B_BaseAddr+P554_THR         // W
#define P554_0_B_DLL             P554_0_B_BaseAddr+P554_DLL
#define P554_0_B_DLM             P554_0_B_BaseAddr+P554_DLM
#define P554_0_B_IER             P554_0_B_BaseAddr+P554_IER
#define P554_0_B_FCR             P554_0_B_BaseAddr+P554_FCR         // W 
#define P554_0_B_IIR             P554_0_B_BaseAddr+P554_IIR         // R 
#define P554_0_B_LCR             P554_0_B_BaseAddr+P554_LCR
#define P554_0_B_MCR             P554_0_B_BaseAddr+P554_MCR
#define P554_0_B_LSR             P554_0_B_BaseAddr+P554_LSR
#define P554_0_B_MSR             P554_0_B_BaseAddr+P554_MSR
#define P554_0_B_SLF             P554_0_B_BaseAddr+P554_SLF
     

#define P554_0_C_BaseAddr        P554_BaseAddr+P554_AddrDistance*2  // 0x0F61400
#define P554_0_C_RBR             P554_0_C_BaseAddr+P554_RBR         // R
#define P554_0_C_THR             P554_0_C_BaseAddr+P554_THR         // W
#define P554_0_C_DLL             P554_0_C_BaseAddr+P554_DLL
#define P554_0_C_DLM             P554_0_C_BaseAddr+P554_DLM
#define P554_0_C_IER             P554_0_C_BaseAddr+P554_IER
#define P554_0_C_FCR             P554_0_C_BaseAddr+P554_FCR         // W 
#define P554_0_C_IIR             P554_0_C_BaseAddr+P554_IIR         // R 
#define P554_0_C_LCR             P554_0_C_BaseAddr+P554_LCR
#define P554_0_C_MCR             P554_0_C_BaseAddr+P554_MCR
#define P554_0_C_LSR             P554_0_C_BaseAddr+P554_LSR
#define P554_0_C_MSR             P554_0_C_BaseAddr+P554_MSR
#define P554_0_C_SLF             P554_0_C_BaseAddr+P554_SLF
     

#define P554_0_D_BaseAddr        P554_BaseAddr+P554_AddrDistance*3  // 0x0F61600
#define P554_0_D_RBR             P554_0_D_BaseAddr+P554_RBR         // R
#define P554_0_D_THR             P554_0_D_BaseAddr+P554_THR         // W
#define P554_0_D_DLL             P554_0_D_BaseAddr+P554_DLL
#define P554_0_D_DLM             P554_0_D_BaseAddr+P554_DLM
#define P554_0_D_IER             P554_0_D_BaseAddr+P554_IER
#define P554_0_D_FCR             P554_0_D_BaseAddr+P554_FCR         // W 
#define P554_0_D_IIR             P554_0_D_BaseAddr+P554_IIR         // R 
#define P554_0_D_LCR             P554_0_D_BaseAddr+P554_LCR
#define P554_0_D_MCR             P554_0_D_BaseAddr+P554_MCR
#define P554_0_D_LSR             P554_0_D_BaseAddr+P554_LSR
#define P554_0_D_MSR             P554_0_D_BaseAddr+P554_MSR
#define P554_0_D_SLF             P554_0_D_BaseAddr+P554_SLF
     
/*___________________________ P554_1    Port_Addr ___________________________*/   

#define P554_1_A_BaseAddr        P554_BaseAddr+P554_AddrDistance*4  // 0x0F61800
#define P554_1_A_RBR             P554_1_A_BaseAddr+P554_RBR         // R
#define P554_1_A_THR             P554_1_A_BaseAddr+P554_THR         // W
#define P554_1_A_DLL             P554_1_A_BaseAddr+P554_DLL
#define P554_1_A_DLM             P554_1_A_BaseAddr+P554_DLM
#define P554_1_A_IER             P554_1_A_BaseAddr+P554_IER
#define P554_1_A_FCR             P554_1_A_BaseAddr+P554_FCR         // W 
#define P554_1_A_IIR             P554_1_A_BaseAddr+P554_IIR         // R 
#define P554_1_A_LCR             P554_1_A_BaseAddr+P554_LCR
#define P554_1_A_MCR             P554_1_A_BaseAddr+P554_MCR
#define P554_1_A_LSR             P554_1_A_BaseAddr+P554_LSR
#define P554_1_A_MSR             P554_1_A_BaseAddr+P554_MSR
#define P554_1_A_SLF             P554_1_A_BaseAddr+P554_SLF
     

#define P554_1_B_BaseAddr        P554_BaseAddr+P554_AddrDistance*5  // 0x0F61a00
#define P554_1_B_RBR             P554_1_B_BaseAddr+P554_RBR         // R
#define P554_1_B_THR             P554_1_B_BaseAddr+P554_THR         // W
#define P554_1_B_DLL             P554_1_B_BaseAddr+P554_DLL
#define P554_1_B_DLM             P554_1_B_BaseAddr+P554_DLM
#define P554_1_B_IER             P554_1_B_BaseAddr+P554_IER
#define P554_1_B_FCR             P554_1_B_BaseAddr+P554_FCR         // W 
#define P554_1_B_IIR             P554_1_B_BaseAddr+P554_IIR         // R 
#define P554_1_B_LCR             P554_1_B_BaseAddr+P554_LCR
#define P554_1_B_MCR             P554_1_B_BaseAddr+P554_MCR
#define P554_1_B_LSR             P554_1_B_BaseAddr+P554_LSR
#define P554_1_B_MSR             P554_1_B_BaseAddr+P554_MSR
#define P554_1_B_SLF             P554_1_B_BaseAddr+P554_SLF
     

#define P554_1_C_BaseAddr        P554_BaseAddr+P554_AddrDistance*6  // 0x0F61c00
#define P554_1_C_RBR             P554_1_C_BaseAddr+P554_RBR         // R
#define P554_1_C_THR             P554_1_C_BaseAddr+P554_THR         // W
#define P554_1_C_DLL             P554_1_C_BaseAddr+P554_DLL
#define P554_1_C_DLM             P554_1_C_BaseAddr+P554_DLM
#define P554_1_C_IER             P554_1_C_BaseAddr+P554_IER
#define P554_1_C_FCR             P554_1_C_BaseAddr+P554_FCR         // W 
#define P554_1_C_IIR             P554_1_C_BaseAddr+P554_IIR         // R 
#define P554_1_C_LCR             P554_1_C_BaseAddr+P554_LCR
#define P554_1_C_MCR             P554_1_C_BaseAddr+P554_MCR
#define P554_1_C_LSR             P554_1_C_BaseAddr+P554_LSR
#define P554_1_C_MSR             P554_1_C_BaseAddr+P554_MSR
#define P554_1_C_SLF             P554_1_C_BaseAddr+P554_SLF
     

#define P554_1_D_BaseAddr        P554_BaseAddr+P554_AddrDistance*7  // 0x0F61e00
#define P554_1_D_RBR             P554_1_D_BaseAddr+P554_RBR         // R
#define P554_1_D_THR             P554_1_D_BaseAddr+P554_THR         // W
#define P554_1_D_DLL             P554_1_D_BaseAddr+P554_DLL
#define P554_1_D_DLM             P554_1_D_BaseAddr+P554_DLM
#define P554_1_D_IER             P554_1_D_BaseAddr+P554_IER
#define P554_1_D_FCR             P554_1_D_BaseAddr+P554_FCR         // W 
#define P554_1_D_IIR             P554_1_D_BaseAddr+P554_IIR         // R 
#define P554_1_D_LCR             P554_1_D_BaseAddr+P554_LCR
#define P554_1_D_MCR             P554_1_D_BaseAddr+P554_MCR
#define P554_1_D_LSR             P554_1_D_BaseAddr+P554_LSR
#define P554_1_D_MSR             P554_1_D_BaseAddr+P554_MSR
#define P554_1_D_SLF             P554_1_D_BaseAddr+P554_SLF
     
/*___________________________ P554_2    Port_Addr ___________________________*/   

#define P554_2_A_BaseAddr        P554_BaseAddr+P554_AddrDistance*8  // 0x0F62000
#define P554_2_A_RBR             P554_2_A_BaseAddr+P554_RBR         // R
#define P554_2_A_THR             P554_2_A_BaseAddr+P554_THR         // W
#define P554_2_A_DLL             P554_2_A_BaseAddr+P554_DLL
#define P554_2_A_DLM             P554_2_A_BaseAddr+P554_DLM
#define P554_2_A_IER             P554_2_A_BaseAddr+P554_IER
#define P554_2_A_FCR             P554_2_A_BaseAddr+P554_FCR         // W 
#define P554_2_A_IIR             P554_2_A_BaseAddr+P554_IIR         // R 
#define P554_2_A_LCR             P554_2_A_BaseAddr+P554_LCR
#define P554_2_A_MCR             P554_2_A_BaseAddr+P554_MCR
#define P554_2_A_LSR             P554_2_A_BaseAddr+P554_LSR
#define P554_2_A_MSR             P554_2_A_BaseAddr+P554_MSR
#define P554_2_A_SLF             P554_2_A_BaseAddr+P554_SLF
     

#define P554_2_B_BaseAddr        P554_BaseAddr+P554_AddrDistance*9  // 0x0F62200
#define P554_2_B_RBR             P554_2_B_BaseAddr+P554_RBR         // R
#define P554_2_B_THR             P554_2_B_BaseAddr+P554_THR         // W
#define P554_2_B_DLL             P554_2_B_BaseAddr+P554_DLL
#define P554_2_B_DLM             P554_2_B_BaseAddr+P554_DLM
#define P554_2_B_IER             P554_2_B_BaseAddr+P554_IER
#define P554_2_B_FCR             P554_2_B_BaseAddr+P554_FCR         // W 
#define P554_2_B_IIR             P554_2_B_BaseAddr+P554_IIR         // R 
#define P554_2_B_LCR             P554_2_B_BaseAddr+P554_LCR
#define P554_2_B_MCR             P554_2_B_BaseAddr+P554_MCR
#define P554_2_B_LSR             P554_2_B_BaseAddr+P554_LSR
#define P554_2_B_MSR             P554_2_B_BaseAddr+P554_MSR
#define P554_2_B_SLF             P554_2_B_BaseAddr+P554_SLF
     

#define P554_2_C_BaseAddr        P554_BaseAddr+P554_AddrDistance*10  // 0x0F62400
#define P554_2_C_RBR             P554_2_C_BaseAddr+P554_RBR         // R
#define P554_2_C_THR             P554_2_C_BaseAddr+P554_THR         // W
#define P554_2_C_DLL             P554_2_C_BaseAddr+P554_DLL
#define P554_2_C_DLM             P554_2_C_BaseAddr+P554_DLM
#define P554_2_C_IER             P554_2_C_BaseAddr+P554_IER
#define P554_2_C_FCR             P554_2_C_BaseAddr+P554_FCR         // W 
#define P554_2_C_IIR             P554_2_C_BaseAddr+P554_IIR         // R 
#define P554_2_C_LCR             P554_2_C_BaseAddr+P554_LCR
#define P554_2_C_MCR             P554_2_C_BaseAddr+P554_MCR
#define P554_2_C_LSR             P554_2_C_BaseAddr+P554_LSR
#define P554_2_C_MSR             P554_2_C_BaseAddr+P554_MSR
#define P554_2_C_SLF             P554_2_C_BaseAddr+P554_SLF
     

#define P554_2_D_BaseAddr        P554_BaseAddr+P554_AddrDistance*11  // 0x0F62600
#define P554_2_D_RBR             P554_2_D_BaseAddr+P554_RBR         // R
#define P554_2_D_THR             P554_2_D_BaseAddr+P554_THR         // W
#define P554_2_D_DLL             P554_2_D_BaseAddr+P554_DLL
#define P554_2_D_DLM             P554_2_D_BaseAddr+P554_DLM
#define P554_2_D_IER             P554_2_D_BaseAddr+P554_IER
#define P554_2_D_FCR             P554_2_D_BaseAddr+P554_FCR         // W 
#define P554_2_D_IIR             P554_2_D_BaseAddr+P554_IIR         // R 
#define P554_2_D_LCR             P554_2_D_BaseAddr+P554_LCR
#define P554_2_D_MCR             P554_2_D_BaseAddr+P554_MCR
#define P554_2_D_LSR             P554_2_D_BaseAddr+P554_LSR
#define P554_2_D_MSR             P554_2_D_BaseAddr+P554_MSR
#define P554_2_D_SLF             P554_2_D_BaseAddr+P554_SLF
     






















;;;CAN网配置的应该是单缓冲器,根据标志表示不同的内容.
/*___________________________ CAN_0     Port_Addr ___________________________*/   

#define CAN_BaseAddr             0x0F51000                           ;;;这个地方就是安排的在实际硬件中的地址
#define CAN_AddrDistance         0x1000                              ;;;一段CAN的地址长度

#define CAN_MODE                 0x00          ;;;控制
#define CAN_COMMAND              0x01          ;;;命令
#define CAN_STATUS               0x02          ;;;状态
#define CAN_INTSTA               0x03          ;;;中断
#define CAN_INTEN                0x04          ;;;验收代码
#define CAN_BUST0                0x06          ;;;总线定时0
#define CAN_BUST1                0x07          ;;;总线定时1
#define CAN_OUTCONT              0x08          ;;;输出控制
#define CAN_ERRCAP               0x0c          ;;;错误代码捕捉
#define CAN_ERRWARLIM            0x0d          ;;;错误报警限制
#define CAN_ACCCODE0             0x10          ;;;接收缓冲器和发送缓冲器是合用的,在不同的模式下数据表示的内容不同
#define CAN_ACCCODE1             0x11
#define CAN_ACCCODE2             0x12
#define CAN_ACCCODE3             0x13
#define CAN_ACCMASK0             0x14          
#define CAN_ACCMASK1             0x15
#define CAN_ACCMASK2             0x16
#define CAN_ACCMASK3             0x17
#define CAN_CLKDIVIDE            0x1f          ;;;时钟分频器
;;;虽然CAN内部还自带了RAM可以使用,但本系统并没有利用









#define CAN0_BaseAddr            CAN_BaseAddr+CAN_AddrDistance*0  // 0x0f21000          ;;;这个地址就是实际硬件的绝对地址
#define CAN0_MODE                CAN0_BaseAddr+CAN_MODE
#define CAN0_COMMAND             CAN0_BaseAddr+CAN_COMMAND
#define CAN0_STATUS              CAN0_BaseAddr+CAN_STATUS
#define CAN0_INTSTA              CAN0_BaseAddr+CAN_INTSTA     
#define CAN0_INTEN               CAN0_BaseAddr+CAN_INTEN
#define CAN0_BUST0               CAN0_BaseAddr+CAN_BUST0
#define CAN0_BUST1               CAN0_BaseAddr+CAN_BUST1
#define CAN0_OUTCONT             CAN0_BaseAddr+CAN_OUTCONT
#define CAN0_ERRCAP              CAN0_BaseAddr+CAN_ERRCAP
#define CAN0_ERRWARLIM           CAN0_BaseAddr+CAN_ERRWARLIM
#define CAN0_ACCCODE0            CAN0_BaseAddr+CAN_ACCCODE0           ;;;最终缓冲器的地址,总线与内部CPU的接口
#define CAN0_ACCCODE1            CAN0_BaseAddr+CAN_ACCCODE1           ;;;到了这个上面的内容就是帧的形式了
#define CAN0_ACCCODE2            CAN0_BaseAddr+CAN_ACCCODE2
#define CAN0_ACCCODE3            CAN0_BaseAddr+CAN_ACCCODE3
#define CAN0_ACCMASK0            CAN0_BaseAddr+CAN_ACCMASK0
#define CAN0_ACCMASK1            CAN0_BaseAddr+CAN_ACCMASK1
#define CAN0_ACCMASK2            CAN0_BaseAddr+CAN_ACCMASK2
#define CAN0_ACCMASK3            CAN0_BaseAddr+CAN_ACCMASK3
#define CAN0_CLKDIVIDE           CAN0_BaseAddr+CAN_CLKDIVIDE

#define CAN0_TRANS_BUF           CAN0_ACCCODE0        ;;;大小都是13B
#define CAN0_RECE_BUF            CAN0_ACCCODE0

/*___________________________ CAN_1     Port_Addr ___________________________*/   

#define CAN1_BaseAddr            CAN_BaseAddr+CAN_AddrDistance*1  // 0x0f22000       ;;;他们增强了程序的可移植性
#define CAN1_MODE                CAN1_BaseAddr+CAN_MODE
#define CAN1_COMMAND             CAN1_BaseAddr+CAN_COMMAND
#define CAN1_STATUS              CAN1_BaseAddr+CAN_STATUS
#define CAN1_INTSTA              CAN1_BaseAddr+CAN_INTSTA
#define CAN1_INTEN               CAN1_BaseAddr+CAN_INTEN
#define CAN1_BUST0               CAN1_BaseAddr+CAN_BUST0
#define CAN1_BUST1               CAN1_BaseAddr+CAN_BUST1
#define CAN1_OUTCONT             CAN1_BaseAddr+CAN_OUTCONT
#define CAN1_ERRCAP              CAN1_BaseAddr+CAN_ERRCAP
#define CAN1_ERRWARLIM           CAN1_BaseAddr+CAN_ERRWARLIM
#define CAN1_ACCCODE0            CAN1_BaseAddr+CAN_ACCCODE0
#define CAN1_ACCCODE1            CAN1_BaseAddr+CAN_ACCCODE1
#define CAN1_ACCCODE2            CAN1_BaseAddr+CAN_ACCCODE2
#define CAN1_ACCCODE3            CAN1_BaseAddr+CAN_ACCCODE3
#define CAN1_ACCMASK0            CAN1_BaseAddr+CAN_ACCMASK0
#define CAN1_ACCMASK1            CAN1_BaseAddr+CAN_ACCMASK1
#define CAN1_ACCMASK2            CAN1_BaseAddr+CAN_ACCMASK2
#define CAN1_ACCMASK3            CAN1_BaseAddr+CAN_ACCMASK3
#define CAN1_CLKDIVIDE           CAN1_BaseAddr+CAN_CLKDIVIDE

#define CAN1_TRANS_BUF           CAN1_ACCCODE0
#define CAN1_RECE_BUF            CAN1_ACCCODE0



















/*___________________________ DALLAS    Port_Addr ___________________________*/   

#define DALLAS_MEM_BASE                 0x0F40000

#define DALLAS_RCD_INFO_MYSELF_ADDR     DALLAS_MEM_BASE+0x0000    // 0x0f0000-0x0f07ff =128*16
#define DALLAS_RCD_INFO_SYSTEM_ADDR     DALLAS_MEM_BASE+0x0800    // 0x0f0800-0x0f0fff =128*16

// here 0x0f1000-0x0f1dff is idle.   ;;;省电
#define DALLAS_YM_SAV_PTR_ADDR  DALLAS_MEM_BASE+0x1000 //1024

#define DALLAS_RCD_MYSELF_SAV_PTR_ADDR  DALLAS_MEM_BASE+0x1e00    // 0-127
#define DALLAS_RCD_MYSELF_TAK_PTR_ADDR  DALLAS_MEM_BASE+0x1e01

#define DALLAS_XOR_MYSELF_SAV_PTR_ADDR  DALLAS_MEM_BASE+0x1e04    
#define DALLAS_XOR_MYSELF_TAK_PTR_ADDR  DALLAS_MEM_BASE+0x1e05

#define DALLAS_RCD_SYSTEM_SAV_PTR_ADDR  DALLAS_MEM_BASE+0x1e02
#define DALLAS_RCD_SYSTEM_TAK_PTR_ADDR  DALLAS_MEM_BASE+0x1e03

#define DALLAS_XOR_SYSTEM_SAV_PTR_ADDR  DALLAS_MEM_BASE+0x1e06
#define DALLAS_XOR_SYSTEM_TAK_PTR_ADDR  DALLAS_MEM_BASE+0x1e07

#define CN_CPU_RST_COLD                 DALLAS_MEM_BASE+0x1e08
#define CN_CPU_RST_HOT                  DALLAS_MEM_BASE+0x1e0a
// 0xf41e10--0xf41e7f  for later use
// 0xf41e80--0xf41eff  used by ram.c

#define SOFT_ERR_FLAG                   DALLAS_MEM_BASE+0x1f00  ;;;刚上电的时候就对这些标志位赋0

#define DALLAS_YEAR                     DALLAS_MEM_BASE+0x1fff
#define DALLAS_MONTH                    DALLAS_MEM_BASE+0x1ffe
#define DALLAS_DATE                     DALLAS_MEM_BASE+0x1ffd
#define DALLAS_DAY                      DALLAS_MEM_BASE+0x1ffc
#define DALLAS_HOUR                     DALLAS_MEM_BASE+0x1ffb
#define DALLAS_MINUTE                   DALLAS_MEM_BASE+0x1ffa
#define DALLAS_SECOND                   DALLAS_MEM_BASE+0x1ff9
#define DALLAS_CONTROL                  DALLAS_MEM_BASE+0x1ff8   ;;;都是一回事,只是说明的形式不同而已


















