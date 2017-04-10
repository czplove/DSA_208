/*****************************************************************************/
/*       FileName  :   CAN.C                                                 */
/*       Content   :   DSA-208 CAN  Module                                   */
/*       Date      :   Sat  08-16-2003                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/
             //-CMU����ڶ����豸
/**�ڶ����豸���ڶ�ʱ��ѯ��һ�������豸����ѯ���ʱ��T3(10s-30s)һ��Ӧ�ϳ����Կ���ʱ���������ͻ�����̡�
��ѯ�������ڶ����豸�Լ���RAM�У��Ա��ϼ�ͨ���豸��ѯʱ�������͡�
ͨ�ų�����о����ж���ͨ��������������£���Ӳ�������ͨ���жϡ��ڶ����豸Ӧ���ܷ��յ����ڶ�ʱ����Ϣ
��ѯ�����Ϊͨ�ų�����оݣ��������ڵ�ȡװ����Ϣ���趨װ�ã���Ӧ��Ϊͨ�ų�����оݡ�<ͨ�ų�����о�>
�ڶ����豸�붨ʱ���һ�������豸����Уʱ���ģ�Уʱ���������10���ӡ�
�ڶ����豸��ͨ�Ž����У����������������ʷ����𡱵������
a)	�յ�����æ�����ģ�һ�����Ӧ�������ν��̡�
b)	�յ�COS���ģ���������ȷ�ϱ��ġ�
c)	�յ�״̬�仯���ģ�����������У��Ժ�����������ȡ������ִ�С�
**/


;;;����CAN��Ӧ����򵥵ľ��ǽ���+���ͺ�һ���������,Ϊ�������˳��Ӧ�ÿ��ٵĽ��н���+����
;;;����Ĵ����ͻ�����������������.
;;;��1MS�ж��н��з���CAN0_Port_Transmit();���ж�Ҫ��Ҫ������ʵʱ��,
;;;�����������ж�exint0_int�н��е�,�������������ݽ��յ��ʹٷ��������жϽ��н���[CAN0_Port_Receive()]
;;;��CPU�Ĳ�֮ͬ��������,���Ƿ���ͬһ���̶��ж��е��ǲ�ѯ��ʽ,�������Ƿ���������ͬ��ʽ��
;;;�ж�֮��,һ���ǲ�ѯ,һ�����ж�.
;;;�����ݵĴ�������CPUҲ�ǲ�ͬ��,CPU�Ƕ�ʱ����,�����ﲻ���Ƕ�ʱ������,�������ڴ���CAN0_Main();
;;;��ʵ,��һ������Ϊ����ֻ��ʱ�䲻ȷ���Ķ�ʱ.�м��������Ͳ���ʲô�����뷢����,���ݱ�־λ��



/*---------------------------------------------------------------------------*/
/*                    Definition  of  global  variables                      */
/*---------------------------------------------------------------------------*/

//    none


/*---------------------------------------------------------------------------*/
/*                    Definition  of  local  variables                       */
/*---------------------------------------------------------------------------*/

//    none
                    ;;;���¶���Ķ��Ǻ�;;Frame��֡����˼
//-----------------  CAN definition  ----------------------
#define CAN_VERTIME_FRAME                         0x00       ;;;��ʱ����
#define CAN_SHORT_FRAME                           0x08       ;;;���ݶ̱���
#define CAN_LONG_FRAME                            0x38       ;;;���ݳ�����  ;;;��ʵͨ�ű��ľ������ָ�ʽ
#define CAN_CPUASKCMU_FRAME                       0x20

#define CAN_COMM_BEERR_NUM                           4   // must <200 
#define CAN_COMM_BEGOOD_NUM                          2

#define CAN_BROADCAST_ADDR                        0xff           ;;;�㲥��ַ

#define AND_MAX_CAN_NUMBER                        0x3f        ;;;���ݻ����������Χ?

#define BH_GUIHAO_IN_CAN_INTERNAL_PROTOCOL        0x01        ;;;����_���__�ڲ���_��Լ


//  CAN0 address: 0x00        ,MON 
//                0x01-0x3f   ,CPU_CAN0
//                0xff        ,Broadcast
#define MIN_CAN0_ADDR                             0x01             ;;;�����CAN0��Դ��ַ�ķ�Χ��1~63
#define MAX_CAN0_ADDR                             0x3f
//  CAN0 address: 0x00        ,MON   so CPU program needn't modify protocol 
//                0x40        ,as MON_CAN1 use
//                0x41-0x7f   ,CPU_CAN0
//                0xff        ,Broadcast
#define MIN_CAN1_ADDR                             0x41
#define MAX_CAN1_ADDR                             0x7f


    
//#define TIME_ASK_YC_VALUE                           30   //2004.09.05 HU HONGBING --yc faster 
//#define TIME_ASK_YX_VALUE                           61   
//#define TIME_ASK_YM_VALUE                           72   
//#define TIME_ASK_DD_VALUE                           80   
#define TIME_ASK_YC_VALUE                            5   
#define TIME_ASK_YX_VALUE                           10   
#define TIME_ASK_YM_VALUE                           15   
#define TIME_ASK_DD_VALUE                           15         ;;;ֱ���� ��ʱ��λ��2048MS

#define TIME_CAN_VERIFY_TIME_VALUE                  15    

#define CAN_WAIT_CPU_MSG_TIME                     800   // 800ms

#define CAN_NOW_POL_NONE                          0x00
#define CAN_NOW_POL_YCALL                         0x01
#define CAN_NOW_POL_YXALL                         0x02
#define CAN_NOW_POL_YMALL                         0x03
#define CAN_NOW_POL_DD_AC                         0x04


//  CAN_MON_Status       
#define CAN_MON_IDLE                              0xf0
#define CAN_MON_WAITCPUREPLY                      0xf1
#define CAN_MON_TRANSINGTOCPU                     0xf2


//  CAN_CODE
#define CODE_CAN_RD_CMU_MEM                       0x01
#define CODE_CAN_WR_CMU_MEM_BYTE                  0x02
#define CODE_CAN_WR_CMU_MEM_WORD                  0x03
#define CODE_CAN_RD_CMU_INFO_RCD_MYSELF           0x04
#define CODE_CAN_RD_CMU_INFO_RCD_SYSTEM           0x05
#define CODE_CAN_WR_CMU_INFO_RCD_MYSELF           0x06
#define CODE_CAN_WR_CMU_INFO_RCD_SYSTEM           0x07
#define CODE_CAN_RD_CMU_INFO_CN_RESET             0x08
#define CODE_CAN_WR_CMU_INFO_CN_RESET             0x09



#define CODE_CAN_STATUS_CHANGE                    0x01        ;;; CPU��CSU��������״̬�仯��CMU����ȷ��
#define CODE_CAN_COS_BEGIN                        0x10        ;;; CPU��������COS��CMU��ȷ��  ;;;10H��11H��12H��13H  ��Щ����ң�ŵ�����,���⻹����һ���Ǳ���14
#define CODE_CAN_COS_END                          0x14  //0x13
#define CODE_CAN_COS_ACK                          0x1f        ;;; CMUȷ�ϣ�  ��CPU��500ms��δ�յ�ȷ�ϣ�������������͡�
#define CODE_CAN_STH_RESET                        0x30        ;;; CMU��λCPU��CPU��ش������λ�����־λ��
#define CODE_CAN_STH_RESET_ACK                    0x31        ;;; CPUȷ��
#define CODE_CAN_BRDCST_RST_CPU                   0x32        ;;; CMU�㲥��λCPU���ݣ�CPU����ش�
#define CODE_CAN_BRDCST_RST_RLY                   0x33        ;;;CMU�㲥����CPU���������źţ�CPU����ش�
#define CODE_CAN_FREZSETDD                        0x34        ;;;CMU���õ����Ϣ
#define CODE_CAN_PAUSE_CPU_AUTO                   0x35        ;;; CMU�㲥��ǹ㲥��ͣCPU��������  ;;;Ҫѧϰ�˼���Ӣ�Ķ���,��������Ч
#define CODE_CAN_RST_CPU_COMM                     0x36        ;;;CMU�㲥��ǹ㲥��λͨ�Ų���
#define CODE_CAN_YK_CHOOSE                        0x40        ;;; CMUң��CPU
#define CODE_CAN_YK_CONFIRM                       0x41        ;;;ң��ִ��CMU->CPU
#define CODE_CAN_CONFIRMMSG                       0x80
#define CODE_CAN_CANT_REPLYINFO                   0xf0        ;;; CMU��CPU��CSUӦ��������Ԫ���޷�������������Ϣ��

#define CANT_RPLCPUBUSY                           0x01
#define CANT_RPLCPUNOINFO                         0x02
#define CANT_RPLCPUNOSUPT                         0x03
#define CANT_RPLWAITCPUTIMEOUT                    0x04
#define CANT_RPLMONTRANFAIL                       0x05
#define CANT_RPLRECERTUERRCODE                    0x06        // MON RECE RTU 
#define CANT_RPLRECECPUERRCODE                    0x07        // MON RECE CPU 
#define CANT_RPLWAITRTUTIMEOUT                    0x08

         ;;;�����ǳ����ĵ��������
#define CODE_CAN_YC_ALL                           0x60       ;;;��ѯȫң�� �ش�ȫYC
#define CODE_CAN_YC_CHG                           0x61       ;;; ��ѯ�仯YC              �ش�仯YC
#define CODE_CAN_YX_ALL                           0x62       ;;;��ѯȫYX                �ش�ȫYX
#define CODE_CAN_YX_ALL_CLRCOSSOE                 0x63       ;;;��ѯȫYX����COS��SOE   �ش�ȫYX
#define CODE_CAN_YM_ALL                           0x64       ;;;��ѯȫYM                �ش�ȫYM
#define CODE_CAN_DD_AC                            0x65       ;;;��ѯȫDD_AC             �ش�ȫDD_AC         ;;;�й��޹�
#define CODE_CAN_SOE                              0x66       ;;;��ѯSOE                 �ش�SOE
#define CODE_CAN_LIFE_NOTIME                      0x67       ;;;��ѯ��ʱ�깤��           �ش���ʱ�깤��
#define CODE_CAN_LIFE_TIME                        0x6c       ;;;��ѯ��ʱ�깤��           �ش��ʱ�깤��
#define CODE_CAN_EVENT                            0x68       ;;;��ѯ�����¼�             �ش𱣻��¼�
#define CODE_CAN_W_R_RTU_DATA                     0x69       ;;;��������                 �ش����
#define CODE_CAN_W_R_RLY_DATA                     0x6a       ;;;��������                 �ش𱣻�
#define CODE_CAN_REQUIRE_CPU                      0x6b       ;;;��ѯCPU                 �ش��ѯ
#define CODE_CAN_EQUIP_INFO                       0x6d       ;;;��ѯװ����Ϣ             �ش�װ����Ϣ
#define CODE_CAN_WAVRCDDATA                       0x6e       ;;;����¼������             �ش�¼������
#define CODE_CAN_EVENT_EXT                        0x6f  // 2004-07-10
#define CODE_CAN_ACK_LONG_FRAME                   0x80
#define CODE_CAN_SOE_ACK                          0x76       ;;;ȷ�Ͻ���SOE
#define CODE_CAN_LIFE_NOT_ACK                     0x77       ;;;ȷ�Ͻ��չ����仯(����ʱ��)
#define CODE_CAN_EVENT_ACK                        0x78       ;;;ȷ�Ͻ��ձ����¼�
#define CODE_CAN_LIFE_T_ACK                       0x7c       ;;;ȷ�Ͻ��չ����仯(��ʱ��)
#define CODE_CAN_EVENT_EXT_ACK                    0x7f  // 2004-07-10


#define CAN_NONEED_WAIT_CPU                       0xff
#define CAN_WAIT_CPU_RESETCPUACK                  0x00
#define CAN_WAIT_CPU_YKVERIFY                     0x01         ;;;��Щ������Ӧ�ò��ڲ��Ķ���,���ⲿû�й�ϵ��������·�㶼û�й�ϵ
#define CAN_WAIT_CPU_YKRESULT                     0x02
#define CAN_WAIT_CPU_YC                           0x03
#define CAN_WAIT_CPU_YCCHG                        0x04
#define CAN_WAIT_CPU_YX                           0x05
#define CAN_WAIT_CPU_YXCLR                        0x06
#define CAN_WAIT_CPU_YM                           0x07
#define CAN_WAIT_CPU_DDAC                         0x08
#define CAN_WAIT_CPU_SOE                          0x09
#define CAN_WAIT_CPU_LIFENOTM                     0x0a
#define CAN_WAIT_CPU_RLYEVENT                     0x0b
#define CAN_WAIT_CPU_EVENT_EXT                    0x0c  // 2004-07-10
#define CAN_WAIT_CPU_CPUPOL                       0x0c
#define CAN_WAIT_CPU_LIFETIME                     0x0d
#define CAN_WAIT_CPU_EQPINFO                      0x0e
#define CAN_WAIT_CPU_WAVRCD                       0x0f

#define CAN_WAIT_RTU_YKCONFIRM                    0x10

#define CAN_WAIT_CPU_RLYEVENTNO                   0x20
#define CAN_WAIT_CPU_RLYSETTING                   0x21
#define CAN_WAIT_CPU_RLYMEASURE                   0x22
#define CAN_WAIT_CPU_RSTRLYSIGN                   0x23
#define CAN_WAIT_CPU_RLYSETVERI                   0x24
#define CAN_WAIT_RTU_CHGSETACK                    0x25
#define CAN_WAIT_CPU_RLYOUTLIMT                   0x26
#define CAN_WAIT_CPU_RLYSETGRUP                   0x27
#define CAN_WAIT_CPU_SETGRUPVER                   0x28
#define CAN_WAIT_RTU_SETGRPCONF                   0x29
#define CAN_WAIT_CPU_RLYSETINFOH                  0x2a
#define CAN_WAIT_CPU_RLYSETINFOL                  0x2b


#define CAN_NONEEDTRANSTOHOST                     0xff

#define   LSA_CODE_ACK                    0x06
#define   LSA_CODE_NAK                    0x15
        ;;;?��ЩӦ����Ӧ�ò�Ķ�������,���ⲿû�й�ϵ
#define   LSA_CODE_C2_ASK_EVENT_RCD       0x40             ;;;ASK��ȡ����˼,,ȡ�����¼�
#define   LSA_CODE_C3_ASK_SETTING         0x53            ;;;ȡ������ֵ
#define   LSA_CODE_C4_ASK_MEASURE         0x4d            ;;;ȡ��������ֵ
#define   LSA_CODE_C5_BH_FUGUI            0x4f            ;;;�����źŸ���
#define   LSA_CODE_C6_SETTING_MODI        0x54            ;;;�Ķ�ֵ
#define   LSA_CODE_C7_VERIFY_TIME         0x04            
#define   LSA_CODE_C11_ASK_OUTLIMIT_RCD   0x81            ;;;ȡ����Խ�޼�¼
#define   LSA_CODE_C12_ASK_SETGRP         0x82            ;;;ȡ������ֵ���
#define   LSA_CODE_C13_SETGRP_MODI        0x83            ;;;���ñ�����ֵ���
#define   LSA_CODE_C14_SETGRP_MODI_ACK    0x84            ;;;ȷ���޸Ķ�ֵ���
#define   LSA_CODE_C14_RLY_OPERATE_ACT    0x8f  // 2004-07-10

#define   LSA_CODE_R1_REPLY_EVENT_RCD     0x40            ;;;���ͱ����¼�R1
#define   LSA_CODE_R2_REPLY_SETTING       0x53            ;;;���ͱ�����ֵ
#define   LSA_CODE_R3_REPLY_MEASURE       0x4d            ;;;���ͱ�������ֵ
#define   LSA_CODE_R4_REPLY_SELFCHK       0x51            ;;;
#define   LSA_CODE_R5_REPLY_SETTING_VERI  0x54            ;;;��ֵ��У
#define   LSA_CODE_R5_REPLY_SET_EXT_VERI  0x55  // 2004-07-10;;;
#define   LSA_CODE_R11_REPLY_OUTLIMIT_RCD 0x81            ;;;���ͱ���Խ�޼�¼
#define   LSA_CODE_R12_REPLY_SETGRP       0x82            ;;;���ͱ�����ֵ���
#define   LSA_CODE_R13_REPLY_SETGRP_VERI  0x83            ;;;��ֵ��ŷ�У

#define   CAN0_Comm_Yx_LastSta			( (unsigned  char *)&(port_flag[12][32]) )	//-�������˼���Ҹղ������˼��ƫ��
						//-ʵ����˼Ӧ����ȡ12��32�еĵ�ַ





/*---------------------------------------------------------------------------*/
/*                        IMPORT            functions                        */
/*---------------------------------------------------------------------------*/
















/*---------------------------------------------------------------------------*/
/*                        LOCAL             functions                        */
/*---------------------------------------------------------------------------*/


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BYTE CAN_MON_common_proc(void)
{
    if((CAN0_ReceProc_Buf[0]==CAN_SHORT_FRAME) && (CAN0_ReceProc_Buf[2]==CODE_CAN_CANT_REPLYINFO))
    {
        // inform other COM CAN can't reply
        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
        return YES;
    }
    else
    {
        return NO;
    }
}

//-˵����ô�������ݿ�����������,��֪��������ݿ��жิ����,ʵ������,������ô����










/************************************************/
/* CAN_judge_state_to_yx                        */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void CAN_judge_state_to_yx(void)
{
	
    	BYTE *the_ram_addr_byte;
	    BYTE  the_ram_axh;	
	    
	the_ram_addr_byte=(BYTE *)&YX_State[IED_TX_STATE_START_WORD+(byte0(port_info[12].mirror_unit))/16];	   ;;;���ȡ��������CAN0��ľ��Ե�ַ��YX�е�״̬,һλ��ʾһ̨װ��
	if(HOST_ZF_enable==YES)       ;;;��ʾ�����ٷ�����˼��
    {
    	for(temp_loop=0;temp_loop<8;temp_loop++)
     	{
    		if(CAN0_Comm_Yx_LastSta[temp_loop]!=the_ram_addr_byte[temp_loop])       ;;;����8̨һ�ȷ����в�ͬ�ľͼ���,����û�б仯�ͼ�������һ��
    		{				
    			for(temp_loop1=0;temp_loop1<8;temp_loop1++)
    			{
    				the_ram_axh=(1<<(temp_loop1%8));
    				if(((CAN0_Comm_Yx_LastSta[temp_loop])&(the_ram_axh))!=((the_ram_addr_byte[temp_loop])&(the_ram_axh)))   ;;;һ̨һ̨�ı�
    				{
    					Core_Temp_Loop=IED_TX_STATE_START_WORD+(byte0(port_info[12].mirror_unit))/16+temp_loop/2;    ;;;����ط��ְ�16����ϳ���һ��
    					Core_Det_Pt = (WORD *)&YX_State[Core_Temp_Loop];
					    Core_Src_Pt = &yx_change[yx_chg_in_pt].offset_no;             ;;;ȷ����λYX��ƫ����,����������������˵��
					   *Core_Src_Pt = Core_Temp_Loop/2;          Core_Src_Pt++; // offset
			    		if(((Core_Temp_Loop)& 0x01)==0)
					    {
					       *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX0      ;;;���Ӧ����8̨װ�õ����
			    		   *Core_Src_Pt = *(Core_Det_Pt+0x01);   Core_Src_Pt++; // YX1
			    		   if(((temp_loop)&0x01)==0)
					       *Core_Src_Pt = (WORD)(the_ram_axh);    
					       else
					       *Core_Src_Pt = (WORD)(the_ram_axh*0x100);
					       										  Core_Src_Pt++;   // CHG0
					       *Core_Src_Pt = 0x00;                                 // CHG1
			    		} 
					    else
 					    {
			    		   *Core_Src_Pt = *(Core_Det_Pt-0x01);   Core_Src_Pt++; // YX0
					       *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX1
					       *Core_Src_Pt = 0x00;                  Core_Src_Pt++; // CHG0
					       if(((temp_loop)&0x01)==0)
			    		   *Core_Src_Pt = (WORD)(the_ram_axh);   // CHG1             ;;;��ЩYX��֪��ʵ���������Բ�֪��Ŀ��
			    		   else
			    		   *Core_Src_Pt = (WORD)(the_ram_axh*0x100);
					    }
   
					    yx_chg_in_pt ++;
			    		yx_chg_in_pt &= 0xff;         ;;;ȷ��ѭ��
    					// insert SOE
					    Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;  ;;;����仯��ң���¼�ʱҲ��Ҫ��¼ʱ��
						//ms
					   *Core_Det_Pt = REG_1Msecond;       ;;;�����������ϵͳ�ļ�ʱ��λ,Ҳ���Բ²�REG������������ϵͳ��
					    Core_Det_Pt++;
						//sec min
					   *Core_Det_Pt = (WORD)REG_Minute*0x100 + REG_Second;
					    Core_Det_Pt++;
					    //hour day
					   *Core_Det_Pt = (WORD)REG_Date*256 + REG_Hour;
					    Core_Det_Pt++;
					    // channel state
					   
					   *Core_Det_Pt = Core_Temp_Loop*16+temp_loop1;      ;;;Ҳ��һ����Ϊ����BUG�Ŀ�����,Ϊ��ֻ���Լ�֪��
					   if(((temp_loop)&0x01)==1)
					   *Core_Det_Pt = Core_Temp_Loop*16+temp_loop1+8;
					   if(((the_ram_addr_byte[temp_loop])&(the_ram_axh))!=0)
					   {
					  	 *Core_Det_Pt|=0x8000;
					   }
						yx_soe_in_pt++;                ;;;�仯��ң������¼1024�����˾͸���ǰ�����
					    if(yx_soe_in_pt>1023)
						    yx_soe_in_pt = 0;
					}    
    		
    			}
    			CAN0_Comm_Yx_LastSta[temp_loop]=the_ram_addr_byte[temp_loop];
    		}
     	}
    }
    else
    {
    	for(temp_loop=0;temp_loop<8;temp_loop++)
     	{
    		CAN0_Comm_Yx_LastSta[temp_loop]=the_ram_addr_byte[temp_loop];        ;;;����64̨װ�õ�״̬
     	}	
    
    }	
	
}	













/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BYTE CAN_MON_stachg_task_proc(void)
{
    WORD  dumyword;

    byte1(dumyword)=0;
    for(byte0(dumyword)=MIN_CAN0_ADDR;byte0(dumyword)<=MAX_CAN0_ADDR;byte0(dumyword)++)     ;;;��1~63��鵥Ԫ
    {
        if(CAN0_IED_StaChg[byte0(dumyword)]!=0)          ;;;�ж�װ�õ�ǰ״̬,һ������Ϊ0
        {
            if((CAN0_IED_StaChg[byte0(dumyword)] & 0x01)!=0)
            {
                byte1(dumyword)=CODE_CAN_LIFE_TIME;  // life     ;;;��ѯ��ʱ�깤��           �ش��ʱ�깤��
                CAN0_IED_StaChg[byte0(dumyword)]=CAN0_IED_StaChg[byte0(dumyword)] & 0xfe;    ;;;������֮��Ͱ������־���
            }
            else
            {
                if((CAN0_IED_StaChg[byte0(dumyword)] & 0x04)!=0)
                {
                    byte1(dumyword)=CODE_CAN_EVENT;  // RLY Event       ;;;��ѯ�����¼�             �ش𱣻��¼�
                    CAN0_IED_StaChg[byte0(dumyword)]=CAN0_IED_StaChg[byte0(dumyword)] & 0xfb;   ;;;������ҿ��Ĺ�Լ�е㲻ͬ,����ÿλ���岻һ��
                }
                else
                {
                    if((CAN0_IED_StaChg[byte0(dumyword)] & 0x08)!=0)
                    {
                        byte1(dumyword)=CODE_CAN_SOE;  // SOE          ;;;��ѯSOE                 �ش�SOE
                        CAN0_IED_StaChg[byte0(dumyword)]=CAN0_IED_StaChg[byte0(dumyword)] & 0xf7;
                    }
                    else
                    {
                        if((CAN0_IED_StaChg[byte0(dumyword)] & 0x10)!=0)
                        {
                            byte1(dumyword)=CODE_CAN_YC_CHG;  // YC_CHG        ;;; ��ѯ�仯YC              �ش�仯YC
                            CAN0_IED_StaChg[byte0(dumyword)]=CAN0_IED_StaChg[byte0(dumyword)] & 0xef;
                        }
						            else
						            {// 2004-07-10         ;;;������Ǳ�����������ӵ�,˵����ֻҪ�ܽ�������
	                        if((CAN0_IED_StaChg[byte0(dumyword)] & 0x20)!=0)
    	                    {
        	                    byte1(dumyword)=CODE_CAN_EVENT_EXT;  // RLY_EVENT_EXT
            	                CAN0_IED_StaChg[byte0(dumyword)]=CAN0_IED_StaChg[byte0(dumyword)] & 0xdf;
                	        }
                	      }    
                    }
                }
            }
            if(byte1(dumyword)!=0)             ;;;�жϴ���Ľ��,����0˵��û���κ������Ҫ�����
            {
                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;      ;;;��ʾ֡������
                CAN0_TranProc_Buf[1]=0x00;              // S
                CAN0_TranProc_Buf[2]=0x00;              // Group
                CAN0_TranProc_Buf[3]=byte1(dumyword);   // CMD         ;;;������֡������,����˵������ݾ��Ǻ�̨������Ҫ����CPUȡ��Ϣ
                CAN0_TranProc_Buf[4]=0x00;              // LenL
                CAN0_TranProc_Buf[5]=0x00;              // LenH
                CAN0_TranProc_Buf[6]=byte1(dumyword);   // SumL    add from 'group'���ۼӺ�
                CAN0_TranProc_Buf[7]=0x00;              // SumH    ;;;��Щ�����ڵ����ݾͿ��Է��뷢�ͻ�������ȥ��
                
                if(byte1(dumyword)==CODE_CAN_LIFE_TIME)
                {
                    REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_LIFETIME;        ;;;��Ӧ�����ݸ���Ӧ��ֵ
                }
                else
                {
                    if(byte1(dumyword)==CODE_CAN_EVENT)
                    {
                        REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_RLYEVENT;
                    }
                    else
                    {
                        if(byte1(dumyword)==CODE_CAN_SOE)
                        {
                            REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_SOE;
                        }
                        else
                        {// 2004-07-10
                        	if(byte1(dumyword)==CODE_CAN_YC_CHG)
                        	{
                            	REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YCCHG;
                          }	
                          else
                            {
                            	REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_EVENT_EXT;
                            }
                        }
                    }
                }
                
                REG_CAN0_Port.TRAN_CPU_INFO_ADDR=byte0(dumyword);         ;;;����̨CPU����Ϣ
                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;              ;;;֡��������
                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;       ;;;����ڳ����Ķ��Ե�
                
                REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;          ;;;��Ҫ�ش�
                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;     ;;;ת��˼·��Ҳ��������֮���ͨ����,����һ��ϵͳ;��Ҫ���͵�������
                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;          ;;;�����־�Ϳ��Ծ����Ƿ�����,���ͷ�����CMU-->CPU
                
                return YES;
            }
        }
    }
    
    return NO;
}







/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BYTE CAN_MON_inquire_task_proc(void)        ;;;CAN_����_��Ҫ_����_����
{
    if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_VERICPUCLK_TIME,TIME_CAN_VERIFY_TIME_VALUE)==YES)
    {
        CAN0_TranProc_Buf[0]=CAN_VERTIME_FRAME;           ;;;�ѵ�����ط�����0��Ԫ��,֡����?
        
        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
        
        REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
        
        RAM_CAN0_Para.CAN_BEGIN_VERICPUCLK_TIME=Time_2048ms_Counter;
        return YES;         ;;;������ֵYES
    }
    
refind_addr:
    if(RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR>MAX_CAN0_ADDR)        ;;;          >63
    {
        if((Portx_Poll_First[12]!=YES)&&(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_DD_AC))        ;;;            4
        {
            Portx_Poll_First[12]=YES;
        }    
       	if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YCALL)            ;;;    1
       	{
		       if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YXALL_TIME,TIME_ASK_YX_VALUE)==YES)
		        {
           		RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YXALL;
           		RAM_CAN0_Para.CAN_BEGIN_POL_YXALL_TIME=Time_2048ms_Counter;
    		    }
    		   else
            {
			        if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME,TIME_ASK_YC_VALUE)==YES)
			         {
			            RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YCALL;
            		  RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME=Time_2048ms_Counter;
        		   }
        		   else  return NO;
    		    }
		    }
		else
        {	
	       	if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YXALL)
    	    {
			    if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YMALL_TIME,TIME_ASK_YM_VALUE)==YES)
    			{
			        RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YMALL;
			        RAM_CAN0_Para.CAN_BEGIN_POL_YMALL_TIME=Time_2048ms_Counter;
			    }
	    		else
    	        {
				    if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME,TIME_ASK_YC_VALUE)==YES)
				    {
			   	        RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YCALL;
            			RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME=Time_2048ms_Counter;
	        		}
	        		else  return NO;
    			}
		    }
			else
			{
		       	if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YMALL)
    		   	{
				    if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_DD_AC_TIME,TIME_ASK_DD_VALUE)==YES)
    				{
			            RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_DD_AC;
			           	RAM_CAN0_Para.CAN_BEGIN_POL_DD_AC_TIME=Time_2048ms_Counter;
				    }
		    		else
    		        {
					    if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME,TIME_ASK_YC_VALUE)==YES)
					    {
			    	        RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YCALL;
            				RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME=Time_2048ms_Counter;
		        		}
		        		else  return NO;
    				}
				}
	    		else
   		        {
					if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME,TIME_ASK_YC_VALUE)==YES)
					{
		    		    RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YCALL;
           				RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME=Time_2048ms_Counter;
	        		}
	        		else  return NO;
   				}
			}
        }
        RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR=MIN_CAN0_ADDR; // 1
    }
    else  // 1 type pol has not complete
    {
        if(RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR==REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx)
        {
            RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
            goto refind_addr;
        }
        // other ifs
            
        if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YCALL)
        {
            if(unit_info[RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit)].yc_val_num==0)
            {
                RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
                goto refind_addr;
            }
                
            CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
            CAN0_TranProc_Buf[1]=0x00;              // S
            CAN0_TranProc_Buf[2]=0x00;              // Group
            CAN0_TranProc_Buf[3]=CODE_CAN_YC_ALL;   // CMD
            CAN0_TranProc_Buf[4]=0x00;              // LenL
            CAN0_TranProc_Buf[5]=0x00;              // LenH
            CAN0_TranProc_Buf[6]=CODE_CAN_YC_ALL;   // SumL    add from 'group'
            CAN0_TranProc_Buf[7]=0x00;              // SumH
                
            REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
            REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YC;
        }
        else
        {
            if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YXALL) 
            {
                if(unit_info[RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit)].yx_num==0)
                {
                    RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
                    goto refind_addr;
                }
               
                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                CAN0_TranProc_Buf[1]=0x00;              // S
                CAN0_TranProc_Buf[2]=0x00;              // Group
                CAN0_TranProc_Buf[3]=CODE_CAN_YX_ALL;   // CMD
                CAN0_TranProc_Buf[4]=0x00;              // LenL
                CAN0_TranProc_Buf[5]=0x00;              // LenH
                CAN0_TranProc_Buf[6]=CODE_CAN_YX_ALL;   // SumL    add from 'group'
                CAN0_TranProc_Buf[7]=0x00;              // SumH
               
                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YX;
            }
            else
            {
                if(RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE==CAN_NOW_POL_YMALL) 
                {
                    if(unit_info[RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit)].ym_num==0)
                    {
                        RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
                        goto refind_addr;
                    }
                
                    CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                    CAN0_TranProc_Buf[1]=0x00;              // S
                    CAN0_TranProc_Buf[2]=0x00;              // Group
                    CAN0_TranProc_Buf[3]=CODE_CAN_YM_ALL;   // CMD
                    CAN0_TranProc_Buf[4]=0x00;              // LenL
                    CAN0_TranProc_Buf[5]=0x00;              // LenH
                    CAN0_TranProc_Buf[6]=CODE_CAN_YM_ALL;   // SumL    add from 'group'
                    CAN0_TranProc_Buf[7]=0x00;              // SumH
                
                    REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                    REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YM;
                }
                else  // CAN_Now_Pol_DD_AC
                {
                    if(unit_info[RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit)].dc_num==0)
                    {
                        RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
                        goto refind_addr;
                    }
                
                    CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                    CAN0_TranProc_Buf[1]=0x00;              // S
                    CAN0_TranProc_Buf[2]=0x00;              // Group
                    CAN0_TranProc_Buf[3]=CODE_CAN_DD_AC;    // CMD
                    CAN0_TranProc_Buf[4]=0x00;              // LenL
                    CAN0_TranProc_Buf[5]=0x00;              // LenH
                    CAN0_TranProc_Buf[6]=CODE_CAN_DD_AC;    // SumL    add from 'group'
                    CAN0_TranProc_Buf[7]=0x00;              // SumH
               
                    REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                    REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_DDAC;
                }
            }
        }
            
        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR;
        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                
        REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;
        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;

        RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR++;
            
        return YES;
    }
    
    return NO;
}









    
    
    
/*---------------------------------------------------------------------------*/
/*                        PUBLIC            functions                        */
/*---------------------------------------------------------------------------*/

//-����϶��������ж��ж�ʱ���մ�����,�ͱ����������һ���Ĵ���
/*===========================================================================*/
void CAN0_Port_Receive(void)    // in Exint0_int           ;;;������ж�����Ҫִ��
{
    register WORD  reg_ibx;                        ;;;�������������Ƭ�ڼĴ�����,����������Ҳ���ֻҪ������ھ���
        near BYTE *the_ram_base_addr;               ;;;��������𵽾������������


    //   store CAN_Frame to COM0

    byte0(reg_ibx)=*(far BYTE *)(CAN0_RECE_BUF+0x03);   // sourcer addr         ;;;Դ��ַ   �����������
    if( (byte0(reg_ibx)<MIN_CAN0_ADDR) || (byte0(reg_ibx)>MAX_CAN0_ADDR) )      ;;;Ҳ����˵����CAN0�ϵ�CPU ��ַֻ���������Χ�ڳ�������
    {
        return;       ;;;�����Ͳ�����
    }       
    if(((BYTE)(RAM_CAN0_Para.CAN_PORT_RECE_BUF_SAV_PTR+1))!=RAM_CAN0_Para.CAN_PORT_RECE_BUF_TAK_PTR)   ;;;�����ж����ݽ����Ƿ���,��Ȳ����������
    {   // Buf len 256.   if overflow no Rece it to CAN0_RecePort_Buf
        the_ram_base_addr=(near BYTE *)&CAN0_RecePort_Buf+RAM_CAN0_Para.CAN_PORT_RECE_BUF_SAV_PTR*12; ;;;��ȡ�������Ļ�ַ,�ټ���ƫ����(ÿ12������һ��)

        the_ram_base_addr[0x00]=*(far BYTE *)(CAN0_RECE_BUF+0x01);       ;;;�Դ������Ͻ��ܵ���֡���б���,���ǽ��յ�����Ϣ�ĵ�һ����Ψһһ�ν���
        the_ram_base_addr[0x01]=*(far BYTE *)(CAN0_RECE_BUF+0x02);       ;;;����ط����ǰѽ��յ�������ȡ���ڲ��洢���й�������,SJA1000�������Դ����
        the_ram_base_addr[0x02]=*(far BYTE *)(CAN0_RECE_BUF+0x03);       ;;;���ڽ��յ��������Ѿ�ȷ��,����û�м�¼"֡��Ϣ" 
        the_ram_base_addr[0x03]=*(far BYTE *)(CAN0_RECE_BUF+0x04);
        the_ram_base_addr[0x04]=*(far BYTE *)(CAN0_RECE_BUF+0x05);
        the_ram_base_addr[0x05]=*(far BYTE *)(CAN0_RECE_BUF+0x06);
        the_ram_base_addr[0x06]=*(far BYTE *)(CAN0_RECE_BUF+0x07);
        the_ram_base_addr[0x07]=*(far BYTE *)(CAN0_RECE_BUF+0x08);
        the_ram_base_addr[0x08]=*(far BYTE *)(CAN0_RECE_BUF+0x09);
        the_ram_base_addr[0x09]=*(far BYTE *)(CAN0_RECE_BUF+0x0a);
        the_ram_base_addr[0x0a]=*(far BYTE *)(CAN0_RECE_BUF+0x0b);
        the_ram_base_addr[0x0b]=*(far BYTE *)(CAN0_RECE_BUF+0x0c);  ;;;���ջ�������Ȼ�ɶ���Сֻ��13B����,��������һ��FIFO�Ŀռ�64B
                     ;;;��û��ʲô���ԵĶ�,�е�ֻ������

        byte0(reg_ibx)=0;       ;;;û��ʲô����ĺ���,��Ϊ��Ĵ�����������������
        if(the_ram_base_addr[0x00]==0x00)  // slave reply            ;;;���Ŀ���ַ�Ƿ���ȷ,�ӻ�_�ظ� ��ֻҪ�յ������ı�����������ȷ�ϱ���,�������Ǻ�
        {
	        if((Now_CANx_Sw_Channel_Sta[the_ram_base_addr[0x02]] & 0x10)!=0)  // may rece on CAN0 ;;;�Ǳ�ʾ���ڿ��Խ��������Ԫ�ı�����
    	    {
	    	    if( (*(far BYTE *)(CAN0_RECE_BUF+0x01)== 0x00) &&                          ;;;֡�ĸ�ʽ�ǹ̶���,��Ҫ��Ϊ��ϳ�����
    	    	   ((*(far BYTE *)(CAN0_RECE_BUF+0x04) & 0x38)==CAN_SHORT_FRAME) )
	        	{
    	        	byte1(reg_ibx)=*(far BYTE *)(CAN0_RECE_BUF+0x06);   // CMD
	    	        if((byte1(reg_ibx)>=CODE_CAN_COS_BEGIN) &&
    	    	       (byte1(reg_ibx)<=CODE_CAN_COS_END  ))        // COS auto   ;;;�ж���֡�������,����������Χ�ھ����������;����Ӧ����ȷ�ϱ���
        	    	{
            	    	if(((RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR+1) & AND_CAN_COSACK_QUELEN)    ;;;��Χ��0~15
                	    !=RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR)
	                	{
    	                	CAN0_COSACK_Buf[RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR]=*(far BYTE *)(CAN0_RECE_BUF+0x03);    ;;;����ط��Ǽ�¼����֡��Դ��ַ��
        	                	          ;;;��¼����ȫң�ŵ�Դ��ַ
	            	        RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR =(RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR+1) & AND_CAN_COSACK_QUELEN;
    	            	           ;;;��ЩҲ�����Ƿ�������
	    	            }
    	    	        else
        	    	    {
            	    	    byte0(reg_ibx)=1;    
	                	}
		            }
    		    }   
    	    }
        	else
	        {
   		        byte0(reg_ibx)=1;        ;;;û�гɹ����վ���1
        	}
        }
        else
        {
        	if((the_ram_base_addr[0x00]==Vqc_Info[0].DSA_243_Addr)||         ;;;�޹�����װ�õĵ�ַ�Ѿ��������й̶�,SJA1000�Ľ��������߲��ܵ�,��������������
        	   (the_ram_base_addr[0x00]==Vqc_Info[1].DSA_243_Addr))
        	{
	        	Now_CANx_Sw_Channel_Sta[the_ram_base_addr[0x02]] &= 0x3f;       ;;;����Ƿ��������޹�����װ�õı��Ĺ����Ҳ���մ���
		        Now_CANx_Sw_Channel_Sta[the_ram_base_addr[0x02]] += 0x40;    // may send on CAN0  ;;;����־λ��ֵ
        	}
        	else
	        {
   		        byte0(reg_ibx)=1;                ;;;�ǲ���ֻҪ���Ƿ��������ݾͼ�¼����,��Ŀ���ַ����
        	}
        }


         
      if(byte0(reg_ibx)==0)     RAM_CAN0_Para.CAN_PORT_RECE_BUF_SAV_PTR++;      ;;;ֻҪ�Ƿ��������Ԫ�ľͼ�¼����,���������ڶ˿�̫����,���ܼ�ʱ����,�����ȼ�¼,�ɹ�����һ���ͼ�1
    }
        
} 
















     ;;;Ҫ���Լ�ͬһ������ϰ��
//-����ط��Ĵ����ǶԽ��յ������ݿ鴦��,�ȶ�ʱ����Сһ���ȼ�
/*===========================================================================*/
void CAN_Port_ReceAsm(void)    // in Main_loop        ;;;���յĹ���Ӧ�úʹ���Ĺ����Ƿֿ����������ܱ�֤ʵʱ��
{
    register      WORD  reg_iax;                     ;;;����ط���ת�������������Ӧ�ò�ķ�����
    register near BYTE *reg_ibx;                 ;;;?��Ȼ�����ڲ��Ĵ����е�Ҳ���Ǿֲ�����
             BYTE  ram_the_save_ptr;
             BYTE  ram_the_take_ptr;
             WORD  ram_ax;
             WORD  ram_bx;
             WORD  ram_cx;
             WORD  ram_dx;
			 long  ram_ulx;	
             BYTE  ram_array[80];             ;;;������ٵ�����ʱ����
             
         far BYTE *ram_the_base_addr;
         far BYTE *ram_the_base_addr_b;
         far WORD *ram_the_base_addr_w;
         far WORD *ram_the_base_addr_w1;


    ram_the_save_ptr=RAM_CAN0_Para.CAN_PORT_RECE_BUF_SAV_PTR;	//-��¼���Ѿ����յ��ĸ���
    ram_the_take_ptr=RAM_CAN0_Para.CAN_PORT_RECE_BUF_TAK_PTR;        ;;;�����˿鴦��,�����˹�����,�ҿ����жϼ�������

    while(ram_the_take_ptr!=ram_the_save_ptr)    ;;;����˵�����µ�֡���յ�,����һ��ѭ��֪�����������еĽ�������֮��Ž���,һ�β�ֹ����һ֡��Ϣ
    {	//-����Լ����֡�����ǹ̶���12���ֽ�
        reg_ibx=(near BYTE *)&CAN0_RecePort_Buf+ram_the_take_ptr*12;    ;;;ȡ������Ҫ�������ݵ��׵�ַ,����13�ֽڵı���ֻȡ�˺�12��,Ȼ��������㿪ʼ��ַ
        
        if(*(near BYTE *)(reg_ibx+0x00)==CAN_BROADCAST_ADDR)         //-�ж��ǹ㲥��ַ��
        {
            // broadcast process                   ;;;���ǵĶ���CMU û�д���
        }
        else  // destin addr=0     ;;;����Ԫָ����ַΪ0
        {
            if(*(near BYTE *)(reg_ibx+0x00)==0x00)       //-�ж��Ƿ���Ҫ������CMU��֡,���˵���Ƿ�����ͨѶ�������
            {
                byte0(reg_iax)=*(near BYTE *)(reg_ibx+0x03) & 0x38;     //-ȡ�����յ�֡������
                if(byte0(reg_iax)==CAN_VERTIME_FRAME)                   //-�����ڿ�ʼӢ���洦����,��Ҳ����һ�����߶���,�ж��Ƿ��Ƕ�ʱ����
                {
                    // verify time process     ;;;��ʵʱ��Ĺ���,������CMU���¶�ʱ �������������Ŀ϶�����Ч������
                }
                else                   ;;;��ʵ,ͨ��֡������ֻ������
                {
                    if(byte0(reg_iax)==CAN_SHORT_FRAME)  // CAN_SHORT_FRAME
                    {
                        byte0(reg_iax)=*(near BYTE *)(reg_ibx+0x02);   // addr      ;;;ȡԴ��ַ���ĸ���Ԫ������
                        CAN0_CurrentSta_Buf[byte0(reg_iax)]=*(near BYTE *)(reg_ibx+0x04) & 0x7f;   //-ȡͨ��״̬S,������֡�ĵ�Ԫ
                    
                        byte1(reg_iax)=*(near BYTE *)(reg_ibx+0x05);        // CMD
                        if(byte1(reg_iax)==CODE_CAN_STATUS_CHANGE)          // status auto ;;;�ж��Ƿ���CPU��CSU��������״̬�仯��CMU����ȷ�ϵ�֡
                        {	//-��ô���ܸı䵥Ԫ�ŵ�ʱ��,����Ҫ���㴦��
                            CAN0_IED_StaChg[byte0(reg_iax)]=*(near BYTE *)(reg_ibx+0x06)	//-������ϢӦ����HOST������
                                                            | CAN0_IED_StaChg[byte0(reg_iax)];	//-��Ϊ�б仯ֵ����1,����ֱ�Ӱѱ仯�����Ͼ���
                        }
                        else
                        {
                            if((byte1(reg_iax)>=CODE_CAN_COS_BEGIN) &&      //-0x10~14
                               (byte1(reg_iax)<=CODE_CAN_COS_END  ))        // COS auto ����,CMU��ȷ��
                            {	//-�����嵥ԪΪ1ʱ������˵�����Ԫ���ڲ��ľ��Ե�ַ����1��Ҫ����,����ʱ��ƫ����
                                byte0(ram_ax)=byte0(port_info[PORT_NO_CAN_0].mirror_unit)+*(near BYTE *)(reg_ibx+0x02);  // Dbase addr ;;;�������ݿ��о��Ե�ַ
                                byte1(ram_ax)=*(near BYTE *)(reg_ibx+0x05)-CODE_CAN_COS_BEGIN;     //-��֡����������YX������

				               if(byte1(ram_ax) > 0)  // at least 1 YX COS byte ;;;�����Ƿ���YX����
                                {  //-���Ȱ����������Ԫ��YXֵ��������(����������),Ȼ��ѱ仯�˵��޸�,�������ݸ��µ�YX_state[]��,ֻ���µ����ļ���֤�仯�Ķ����˾���
                                    byte1(ram_bx)=unit_info[byte0(ram_ax)].yx_num;  // yx_num as words;;;ȡ�����õ�ң����
                                    ram_the_base_addr_w1=(far WORD *)&(YX_State[unit_info[byte0(ram_ax)].yx_start]);  //-ң�ŵ��׵�ַ,��ʼ����ʱ��ȫ��0,�����ú���֮��ÿ̨װ�ö�Ӧ�Ŀռ��ǹ̶���
                                    for(byte0(ram_bx)=0;byte0(ram_bx)<byte1(ram_bx);byte0(ram_bx)++)   //-�ӵ�0����ʼ���ֱ�����õ����һ��
                                    {     //-��װ�����YX��Ԫ����ֵ���ݶ����port_report[]��,���ڽ��յ���������Ϊʲô,��Ҫ���ϴε�ֵ����һ����,��ֱ�����
                                        port_report[byte0(ram_bx)*2+0x00]=byte0(*ram_the_base_addr_w1);     //-��������ֵ��ͷ��
                                        port_report[byte0(ram_bx)*2+0x01]=byte1(*ram_the_base_addr_w1);
                                        ram_the_base_addr_w1++;       ;;;����������Ѿ���¼��ң�ŵ�����,����ط���һ�α���
                                    }    
                                	//-���Ȱ����еľ�ֵ������ԭ������ǲ�һ�����е�ֵ�������,���ܽ���ֻ��һ���ָ���
                                    // fill new YX value
                                    byte1(ram_bx)=byte1(ram_bx)*2;     //-���õ�ʱ��ң���ǰ�����ķ����ʱ��ң���ǰ��ֽڸ���,���Գ���2
                                    
                                    
                                    if(byte1(ram_ax)>3)	//at least 4 yx Cos byte,add by kkk
                                	  {
                                		   byte0(reg_iax)=*( BYTE *)(reg_ibx+0x06);   ;;;�м����BYTE����byte�������Ͷ���Ĳ���,ȡ��ң�����
                                		   if(byte0(reg_iax)==0x03)  // yx_num
                                       {
                                        	port_report[0]=*( BYTE *)(reg_ibx+0x07); // YX_Value    ;;;�ɴ�������ֱ�Ӹ�ֵ
                                        	port_report[1]=*( BYTE *)(reg_ibx+0x08); // YX_Value
                                        	byte1(reg_iax)=byte0(reg_iax)+1;
                                   	   }
                                		
                                		   if(byte0(reg_iax)==0x0c)  // yx_num
                                    		{
                                        	port_report[2]=*( BYTE *)(reg_ibx+0x07); // YX_Value
                                        	port_report[3]=*( BYTE *)(reg_ibx+0x08); // YX_Value
                                        	byte1(reg_iax)=byte0(reg_iax)+1;
                                   		  }
                                	  }    //add by kkk         ;;;��������⹦�����ӵ�,����ԭ���ı���...
                                	  else
                                	  {
	                                    byte0(reg_iax)=*( BYTE *)(reg_ibx+0x06);  // COS:YX_No  ;;;ȡ��YX��ż��仯�����ĸ�YX
	                                    if(byte0(reg_iax)<byte1(ram_bx))  // yx_num        ;;;����ط��Ǳ���,�����,С��˵��û�����
	                                    {
	                                        port_report[byte0(reg_iax)]=*( BYTE *)(reg_ibx+0x07); // YX_Value  ;;;�仯�����ԭ����ֵ,��û�б仯��û�б�Ҫ�滻
	                                        byte1(reg_iax)=byte0(reg_iax);         //-��YX���,��¼����д������ֵ
	                                    }
	                                
	                                    if(byte1(ram_ax)>1)  // at least 2 YX COS byte   ;;;����������������
	                                    {
	                                        byte0(reg_iax)=*( BYTE *)(reg_ibx+0x08);  // COS:YX_No
	                                        if(byte0(reg_iax)<byte1(ram_bx))  // yx_num   ;;;�ڼ���YX����Ϊ���ŵ��������Ƚ�
	                                        {
	                                            port_report[byte0(reg_iax)]=*( BYTE *)(reg_ibx+0x09); // YX_Value  ;;;ֻҪʵ��YX������ķ�Χ�ھ������Ӧ��ֵ
	                                            if(byte1(reg_iax)<byte0(reg_iax)) byte1(reg_iax)=byte0(reg_iax); ;;;�����ң�ź�����εıȽ�������Ĵ�ӹ�������
	                                        }
	    
	                                        if(byte1(ram_ax)>2)  // at least 3 YX COS byte
	                                        {
	                                            byte0(reg_iax)=*( BYTE *)(reg_ibx+0x0a);  // COS:YX_No
	                                            if(byte0(reg_iax)<byte1(ram_bx))  // yx_num  ;;;������С�ں���Ϊ�Ǵ�0��ʼ������
	                                            {
	                                                port_report[byte0(reg_iax)]=*( BYTE *)(reg_ibx+0x0b); // YX_Value
	                                                if(byte1(reg_iax)<byte0(reg_iax)) 
	                                                byte1(reg_iax)=byte0(reg_iax);
	                                            }
	                                        }
	                                    }
                                	  }
                                
                                    Core_Src_Pt_B=&port_report[0];  //-Ϊ��ͨ�ÿռ��Ǵ��ĵ���һ����Ҫʹ��,�׵�ַ
                                    Core_Src_Unit=byte0(ram_ax);                    ;;;���ݿ��еľ��Ե�ַ(unit_info)
                                    Core_Src_Len =byte1(reg_iax)+1;                 //-��¼���м���ң��һ����1��B
                                    if((Core_Src_Len & 0x01)!=0) Core_Src_Len++;    ;;;��������,�����ֵ��B����Ϊ�˸��ֶ��ֲ���ʧ���ݾͽ�һλ

	                                //-�����Ǵ��ڳ���ģ�黯��ԭ��,��������ֱ�������ݿ�����ֵ,���ǲ�û��������,����������ʱ������֯,������ͳһ����
                                    core_update_YX();           ;;;�²���εĶ����ǰѴӱ���װ���н��յ���ң�����͵���̨?
                                }
                            }
                            else     // other short frame   ;;;�����������͵Ķ�֡
                            {
                                if((REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE==YES)   &&
                                   (REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR  ==*(near BYTE *)(reg_ibx+0x02)))    ;;;Դ��ַ,�ܹ�12�ֽ�
                                {
                                    CAN0_ReceProc_Buf[0]=CAN_SHORT_FRAME;
                                    CAN0_ReceProc_Buf[1]=*(near BYTE *)(reg_ibx+0x04);        ;;;Data0
                                    CAN0_ReceProc_Buf[2]=*(near BYTE *)(reg_ibx+0x05);
                                    CAN0_ReceProc_Buf[3]=*(near BYTE *)(reg_ibx+0x06);
                                    CAN0_ReceProc_Buf[4]=*(near BYTE *)(reg_ibx+0x07);
                                    CAN0_ReceProc_Buf[5]=*(near BYTE *)(reg_ibx+0x08);
                                    CAN0_ReceProc_Buf[6]=*(near BYTE *)(reg_ibx+0x09);
                                    CAN0_ReceProc_Buf[7]=*(near BYTE *)(reg_ibx+0x0a);
                                    CAN0_ReceProc_Buf[8]=*(near BYTE *)(reg_ibx+0x0b);
                                
                                    REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=YES;	//-��CPU����Ϣ��Ҫ����
                                    REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE  =NO;	//-����_CPU_�ش���Ϣ_�ܹ�,,ֵΪNO˵�������Ѿ����յ�����Ϣ��û�������ٽ�����
                                }
                            }
                        }
                    }   
                    else  // not CAN_SHORT_FRAME
                    {
                        if(byte0(reg_iax)==CAN_LONG_FRAME)     // long frame
                        {
                            if((REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE==YES)   &&
                               (REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR  ==*(near BYTE *)(reg_ibx+0x02)))
                            {
                                byte0(reg_iax)=*(near BYTE *)(reg_ibx+0x01) & 0x3f;   // frame No.
                                if((byte0(reg_iax)==0) || (byte0(reg_iax)==REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR))
                                {
                                    if(byte0(reg_iax)==0)    // 1st frame      ;;;��ʼ֡(��֡)
                                    {
		                                Core_Src_Unit=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);   ;;;�þ��Ե�ַ,��Щ�������������Ե�
		                                   if(unit_info[Core_Src_Unit].unit_type==0x0101)   ;;;�õ��ĵ�ַ������unit_info�еľ��Ե�ַ
    		                                {
			                                Core_Src_Pt_B=&port_report[0];         ;;;��ָ��ָ���������
			                                Core_Src_Len =0x02; // byte num
			                                
		                                	port_report[0]=byte0(YX_State[unit_info[Core_Src_Unit].yx_start]);
		                                	port_report[1]=byte1(YX_State[unit_info[Core_Src_Unit].yx_start]);
        		                        	if(((*(near BYTE *)(reg_ibx+0x05)) & 0x08)==0)       ;;;״̬����,��ѯ��ǰ�����¼��ź�δ����
        		                        		 port_report[1]&=0x7f;    ;;;C���������ִ�Сд��
            		                    	else                                           
            		                    		 port_report[1]|=0x80;
		                                    core_update_YX();
                		                    }
                                        CAN0_CurrentSta_Buf[*(near BYTE *)(reg_ibx+0x02)]         ;;;��¼�����Ԫ״̬����Ϣ
                                            =*(near BYTE *)(reg_ibx+0x05) & 0x7f;             
                                        REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMESUM=*(near BYTE *)(reg_ibx+0x04);  ;;;��¼�����ĵ���֡�� Num
                            
                                        if(REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMESUM<=64)     ;;;һ�������ĵĴ�С����С�ڵ���64
                                        {
                                            byte0(reg_iax)=*(near BYTE *)(reg_ibx+0x08);
                                            byte1(reg_iax)=*(near BYTE *)(reg_ibx+0x09);        ;;;ȡKֵ,ͨ�����ַ�����ϳ���
                                            if(reg_iax<=(WORD)(REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMESUM-1)*8)  ;;;�������Ӧ��С�ڵ���
                                            {
                                                CAN0_ReceProc_Buf[0]=CAN_LONG_FRAME;
                                                CAN0_ReceProc_Buf[1]=*(near BYTE *)(reg_ibx+0x05);   // S 
                                                CAN0_ReceProc_Buf[2]=*(near BYTE *)(reg_ibx+0x06);   // Group
                                                CAN0_ReceProc_Buf[3]=*(near BYTE *)(reg_ibx+0x07);   // CMD
                                                CAN0_ReceProc_Buf[4]=*(near BYTE *)(reg_ibx+0x08);   // LenL
                                                CAN0_ReceProc_Buf[5]=*(near BYTE *)(reg_ibx+0x09);   // LenH
                                    
                                                if(REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMESUM>1)
                                                {
                                                    CAN0_ReceProc_Buf[6]=*(near BYTE *)(reg_ibx+0x0a);
                                                    CAN0_ReceProc_Buf[7]=*(near BYTE *)(reg_ibx+0x0b);   ;;;�������1֡�Ͳ�����Щ����(���ڵ�һ����λ��)
                                                    REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR=1;   ;;;��ʾ��ǰ�Ѿ�������һ֡������
                                                }
                                                else  // only 1 frame,no need verify suml,sumh
                                                {
                                                    REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=YES;   ;;;��ʾϣ��������
                                                    REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE  =NO;    ;;;���Ѿ�������,�������ٽ�������,���Ǵ����?
                                                }
                                            }   
                                        }
                                    }
                                    else  // later frame         ;;;����0֡�Ĵ������
                                    {
                                        reg_iax=byte0(reg_iax)*8;
                                        CAN0_ReceProc_Buf[reg_iax+0x00]=*(near BYTE *)(reg_ibx+0x04);
                                        CAN0_ReceProc_Buf[reg_iax+0x01]=*(near BYTE *)(reg_ibx+0x05);
                                        CAN0_ReceProc_Buf[reg_iax+0x02]=*(near BYTE *)(reg_ibx+0x06);
                                        CAN0_ReceProc_Buf[reg_iax+0x03]=*(near BYTE *)(reg_ibx+0x07);
                                        CAN0_ReceProc_Buf[reg_iax+0x04]=*(near BYTE *)(reg_ibx+0x08);
                                        CAN0_ReceProc_Buf[reg_iax+0x05]=*(near BYTE *)(reg_ibx+0x09);
                                        CAN0_ReceProc_Buf[reg_iax+0x06]=*(near BYTE *)(reg_ibx+0x0a);
                                        CAN0_ReceProc_Buf[reg_iax+0x07]=*(near BYTE *)(reg_ibx+0x0b);
                                
                                        REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR
                                            =REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR+1;       ;;;�ֽ�����һ֡��ȻҲ��1��
                                    
                                        if(REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR>=
                                           REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMESUM)         ;;;�ж��Ƿ����е�֡�������������??
                                        {
                                            // here may add check Sum ,if error,restart receiving.
                                            // but now it do not check Sum.
                                            REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=YES;
                                            REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE  =NO;       ;;;������Ͼ�����û�д�����֮ǰ�������ٽ�����
                                        }
                                    }
                                }
                                else  // receive frame is not currnet frame wanted
                                {
                                    REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR=0;        ;;;���ܾ���һ�ֳ���Ĵ���
                                }
                            }    
                        }
                        else
                        {
                        	                         ;;;������������ʽ,ͨ��֡û��������ʽ��
                        }
                    }
                }
            }
            else              ;;;�����Ƿ�����CMU��֡�������´���
            {
                byte0(reg_iax)=*(near BYTE *)(reg_ibx+0x00);      ;;;ȡĿ���ַ
                byte0(ram_cx)=0xff;               ;;;�������־����
                if(byte0(reg_iax)==Vqc_Info[0].DSA_243_Addr) byte0(ram_cx)=0x00;   //-�ж��Ƿ��ǵ�ѹ�޹�����,,?��ѹ�޹������������ر��װ����
                if(byte0(reg_iax)==Vqc_Info[1].DSA_243_Addr) byte0(ram_cx)=0x01;   //-������̨�޹�����װ��
                if(byte0(ram_cx)<2)          ;;;���������޹�����֮һ,��ôֵ��С��2
                {
                    if((*(near BYTE *)(reg_ibx+0x03))==CAN_LONG_FRAME)  
                    {
                        if( ((*(near BYTE *)(reg_ibx+0x01))==0x00)  // frame no
                          &&((*(near BYTE *)(reg_ibx+0x04))==0x01)  // frame num
                          )          ;;;֡��Ϊ0��֡��Ϊ1�Ĵ���
                        {
                            if((*(near BYTE *)(reg_ibx+0x07))==CODE_CAN_YC_ALL)  // ask YC ;;;��ѯȫң��ش�ȫң��
                            {
                                ram_array[0]=0x0a;  // frame num    ;;;ǰ���ram_array���ܾ�����������,�����������ݲ�����ֱ�ӷ��͵Ļ���Ҫͳһ����
                                ram_array[1]=0x00;  // S
                                ram_array[2]=0x00;  // Group
                                ram_array[3]=CODE_CAN_YC_ALL;  // CMD
                                ram_array[4]=0x42;  // LENL         ;;;���������ల��32��ң����,���Է����д���,K�͵���66
                                ram_array[5]=0x00;  // LENH
                                ram_array[6]=0x00;  // Line_No

                                for(ram_bx=0;ram_bx<16;ram_bx++)
                                {
                                    ram_ax=YC_State[Vqc_Info[byte0(ram_cx)].YC_NO_HighSide[ram_bx]];
									                 if((ram_ax & 0x0800)!=0) ram_ax|=0xf000;     ;;;�ѵ�YC����λ11�Ƿ���λ,����ط��ͽ�����������
									                   ram_ulx=(short)ram_ax;
									                   
									                  if((ram_bx>0)&&(ram_bx<7))  // U    ;;;һ��һ��������������Ǵ�����,Ӧ�úͿ�������һ���ײ��ϵ�
									                    {                           ;;;��������ڷŵ��ǵ�ѹ����
									                     	ram_ax =ram_ulx*128/75;
										                   if(((ram_bx==4)||(ram_bx==5))&&((Vqc_Info[byte0(ram_cx)].Reserved & 0x01)!=0))  ;;;�������ߵ�ѹ�Ĵ���
											                     ram_ax =ram_ulx*739/250;                           ;;;Ԥ��λӰ���ߵ�ѹ�Ĵ�����
									                    }
									                  else    ;;;����ɫ����ʾ���ǻ���Ĭ�ϵ�����
									                    {
										                        if((ram_bx>6)&&(ram_bx<11))  // I
									                           {
										                    	      ram_ax =ram_ulx*256/75;
									                       	   }
										                        else
										                          {
										                      	    if((ram_bx>10)&&(ram_bx<13))  // P,Q
										                     	        {
										                    		        ram_ax =ram_ulx*616/375;
										                      	      }
										                       	    else
										                            {
											                      	     if(ram_bx==13)        // cosQ       ;;;���ݲ�ͬ���������͸�����Ӧ�Ĵ���
												                             {
												                    	         ram_ax =ram_ulx*256/125;
											                                }
										                            }
									                         	  }
									                    }
                                    ram_array[ram_bx*2+7]=byte0(ram_ax);       ;;;ȡһ���ֵĵ�λ
                                    ram_array[ram_bx*2+8]=byte1(ram_ax);       ;;;ȡͬһ���ֵĸ�λ
                                }    
                                
                                
                                ram_array[39]=0x01;  // Line_No         ;;;���к�
                                for(ram_bx=0;ram_bx<16;ram_bx++)
                                {
                                    ram_ax=YC_State[Vqc_Info[byte0(ram_cx)].YC_NO_LowSide[ram_bx]];
									if((ram_ax & 0x0800)!=0) ram_ax|=0xf000;
									ram_ulx=(short)ram_ax;
									if((ram_bx>0)&&(ram_bx<7))  // U
									{
										ram_ax =ram_ulx*128/75;
										if(((ram_bx==4)||(ram_bx==5))&&((Vqc_Info[byte0(ram_cx)].Reserved & 0x01)!=0))
											ram_ax =ram_ulx*739/250;
									}
									else
									{
										if((ram_bx>6)&&(ram_bx<11))  // I
										{
											ram_ax =ram_ulx*256/75;
										}
										else
										{
											if((ram_bx>10)&&(ram_bx<13))  // P,Q
											{
												ram_ax =ram_ulx*616/375;
											}
											else 
											{
												if(ram_bx==13)        // cosQ
												{
													ram_ax =ram_ulx*256/125;
												}
											}
										}
									}
                                    ram_array[ram_bx*2+40]=byte0(ram_ax);
                                    ram_array[ram_bx*2+41]=byte1(ram_ax);
                                }   
                                
                                
                                 
                                ram_ax=0;                           ;;;�����һ����ʱ�Ĵ���,�������ۼ���
                                for(ram_bx=2;ram_bx<72;ram_bx++)
                                {
                                    ram_ax+=ram_array[ram_bx];        ;;;����Ҫ���ۼӺ�
                                }
                                ram_array[72]=byte0(ram_ax);           ;;;����16λ�ۼӺ�
                                ram_array[73]=byte1(ram_ax);
                                
                                
                                for(ram_bx=0;ram_bx<10;ram_bx++)
                                {
                                    if((BYTE)(CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr+1)==   // Que_len=256
                                              CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr     )
                                    {
                                        CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr++;
                                    }
                                    ram_the_base_addr_b=(far BYTE *)&CAN0_CPUASKCMU_Trans_Buf[0]
                                                       +CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr*12;   ;;;��12�����ݾ�����˵��һ��
                                    ram_the_base_addr_b[0x00]=*(near BYTE *)(reg_ibx+0x02) ;  // destin addr  ;;;ԭ����������ĵ�ַ�����Ϊ��Ŀ�ĵ�ַ
                                    ram_the_base_addr_b[0x01]= ram_bx;                        // frame_no ;;;��ǰ֡��
                                    ram_the_base_addr_b[0x02]= byte0(reg_iax);                // source addr
                                    ram_the_base_addr_b[0x03]= CAN_LONG_FRAME;                // 
                                    ram_the_base_addr_b[0x04]= ram_array[ram_bx*8+0x00];      // ������
                                    ram_the_base_addr_b[0x05]= ram_array[ram_bx*8+0x01];      // 
                                    ram_the_base_addr_b[0x06]= ram_array[ram_bx*8+0x02];      // 
                                    ram_the_base_addr_b[0x07]= ram_array[ram_bx*8+0x03];      // 
                                    ram_the_base_addr_b[0x08]= ram_array[ram_bx*8+0x04];      // 
                                    ram_the_base_addr_b[0x09]= ram_array[ram_bx*8+0x05];      // 
                                    ram_the_base_addr_b[0x0a]= ram_array[ram_bx*8+0x06];      // 
                                    ram_the_base_addr_b[0x0b]= ram_array[ram_bx*8+0x07];      // 
                                    CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr++;   ;;;�ֱ�����һ���ȴ�����
                                }
                            }
                            else
                            {
                                if((*(near BYTE *)(reg_ibx+0x07))==CODE_CAN_YX_ALL)  // ask YX
                                {
                                    ram_array[0]=0x02;  // frame num
                                    ram_array[1]=0x00;  // S
                                    ram_array[2]=0x00;  // Group
                                    ram_array[3]=CODE_CAN_YX_ALL;  // CMD
                                    ram_array[4]=0x08;  // LENL
                                    ram_array[5]=0x00;  // LENH
                                    ram_dx=0;
                                    for(ram_bx=0;ram_bx<16;ram_bx++)
                                    {
                                        ram_ax=YX_State[(Vqc_Info[byte0(ram_cx)].YX_NO[ram_bx])/16];
                                        if((ram_ax & (0x0001<<((Vqc_Info[byte0(ram_cx)].YX_NO[ram_bx])%16)))!=0)
                                        {
                                            ram_dx=ram_dx | (0x0001<<ram_bx);
                                        }
                                    }    
                                    ram_array[6]=0x00;  // YX_No
                                    ram_array[7]=byte0(ram_dx);
                                    ram_array[8]=0x01;  // YX_No
                                    ram_array[9]=byte1(ram_dx);
                                    ram_dx=0;
                                    for(ram_bx=16;ram_bx<32;ram_bx++)
                                    {
                                        ram_ax=YX_State[(Vqc_Info[byte0(ram_cx)].YX_NO[ram_bx])/16];
                                        if((ram_ax & (0x0001<<((Vqc_Info[byte0(ram_cx)].YX_NO[ram_bx])%16)))!=0)
                                        {
                                            ram_dx=ram_dx | (0x0001<<(ram_bx & 0x0f));
                                        }
                                    }    
                                    ram_array[10] =0x02;  // YX_No
                                    ram_array[11]=byte0(ram_dx);
                                    ram_array[12]=0x03;  // YX_No
                                    ram_array[13]=byte1(ram_dx);

                                    ram_ax=0;
                                    for(ram_bx=2;ram_bx<14;ram_bx++)
                                    {
                                        ram_ax+=ram_array[ram_bx];
                                    }
                                    ram_array[14]=byte0(ram_ax);
                                    ram_array[15]=byte1(ram_ax);
                                
                                    for(ram_bx=0;ram_bx<2;ram_bx++)
                                    {
                                        if((BYTE)(CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr+1)==   // Que_len=256
                                                  CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr     )
                                        {
                                            CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr++;
                                        }
                                        ram_the_base_addr_b=(far BYTE *)&CAN0_CPUASKCMU_Trans_Buf[0]
                                                           +CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr*12;
                                        ram_the_base_addr_b[0x00]=*(near BYTE *)(reg_ibx+0x02) ;  // destin addr
                                        ram_the_base_addr_b[0x01]= ram_bx;                        // frame_no
                                        ram_the_base_addr_b[0x02]= byte0(reg_iax);                // source addr
                                        ram_the_base_addr_b[0x03]= CAN_LONG_FRAME;                // 
                                        ram_the_base_addr_b[0x04]= ram_array[ram_bx*8+0x00];      // 
                                        ram_the_base_addr_b[0x05]= ram_array[ram_bx*8+0x01];      // 
                                        ram_the_base_addr_b[0x06]= ram_array[ram_bx*8+0x02];      // 
                                        ram_the_base_addr_b[0x07]= ram_array[ram_bx*8+0x03];      // 
                                        ram_the_base_addr_b[0x08]= ram_array[ram_bx*8+0x04];      // 
                                        ram_the_base_addr_b[0x09]= ram_array[ram_bx*8+0x05];      // 
                                        ram_the_base_addr_b[0x0a]= ram_array[ram_bx*8+0x06];      // 
                                        ram_the_base_addr_b[0x0b]= ram_array[ram_bx*8+0x07];      // 
                                        CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr++;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if(((*(near BYTE *)(reg_ibx+0x03))==CAN_SHORT_FRAME)&&((*(near BYTE *)(reg_ibx+0x01))==0x00)) ;;;�Ƕ�֡�Ĵ������
                        {
                            if(  ( (*(near BYTE *)(reg_ibx+0x05))==CODE_CAN_YK_CHOOSE )        ;;;CMU���յ���Ӧ����ң��ѡ��У֡
                               &&( (*(near BYTE *)(reg_ibx+0x06))==(*(near BYTE *)(reg_ibx+0x08)) )  // action type 
                               &&( (*(near BYTE *)(reg_ibx+0x07))==(*(near BYTE *)(reg_ibx+0x09)) )  // YK_NO
                               &&(((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_CLOSE)||((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_TRIP)) 
                               &&( (*(near BYTE *)(reg_ibx+0x07))<0x02)    ;;;����ط��;�����Switch��ֵֻ��Ϊ0��1  @#@#$%
                              )   ;;;�߼�����������ȼ� ��<��<��
                            {
                            	if(port_transmit_flag[PORT_NO_P554_11]==0x55)           ;;;����55��ʾ��������Ҫ������,����Ҫ׼����������
                            	{
                            		port_transmit[PORT_NO_P554_11][0]=PORT_EXCHANGE_STA_START;   ;;;�мɹ�������ܷ����κ�����,��ֻ��ת������
                            		port_transmit[PORT_NO_P554_11][1]=PORT_NO_CAN_0;                 ;;;��¼�˿ں�
                            		port_transmit[PORT_NO_P554_11][2]=PROTOCOL_CAN_DSA%0x100;
                            		port_transmit[PORT_NO_P554_11][3]=PROTOCOL_CAN_DSA/0x100;        ;;;��Լ��
                            		
                            		port_transmit[PORT_NO_P554_11][8] =CORE_CODE_YK_CHOOSE;        ;;;��¼���յ����ĵľ�������
                            		port_transmit[PORT_NO_P554_11][9] =0x02;
                            		port_transmit[PORT_NO_P554_11][10]=0x00;
                            		port_transmit[PORT_NO_P554_11][16]=(*(near BYTE *)(reg_ibx+0x06));   ;;;��¼����Ĳ���,�ϻ��Ƿ�
                            		if((*(near BYTE *)(reg_ibx+0x07))==0x00)     ;;;������0�Ĵ���
                            		{
                            		    port_transmit[PORT_NO_P554_11][5] =Vqc_Info[byte0(ram_cx)].YK0_208_YK_BOARD_Addr;
                            		    port_transmit[PORT_NO_P554_11][17]=Vqc_Info[byte0(ram_cx)].YK0_208_YK_BOARD_YKNO;
                            		}
                            		else
                            		{
                            		    port_transmit[PORT_NO_P554_11][5] =Vqc_Info[byte0(ram_cx)].YK1_208_YK_BOARD_Addr;
                            		    port_transmit[PORT_NO_P554_11][17]=Vqc_Info[byte0(ram_cx)].YK1_208_YK_BOARD_YKNO;
                            		}
                            		
                            		port_transmit_flag[PORT_NO_P554_11]=0xaa;       ;;;׼����ת������֮��Ϳ���ת����������
                            		
                            		CAN_246_YK_Begin_WAIT_VERIFY=Time_2048ms_Counter;
                            		
                            		CAN_246_Trans_YK_Buf[0]=CAN_246_TRANS_YK_STA_WAIT_VERIFY;
                            		CAN_246_Trans_YK_Buf[1]=(*(near BYTE *)(reg_ibx+0x02));      // CAN246_ADDR ;;;��¼��̨װ�÷�����֡
                            		CAN_246_Trans_YK_Buf[2]=PORT_NO_P554_11;
                            		CAN_246_Trans_YK_Buf[3]=port_transmit[PORT_NO_P554_11][5];   // YK_BUS_ADDR
                            		CAN_246_Trans_YK_Buf[4]=(*(near BYTE *)(reg_ibx+0x06));      // Action_Type   ;;;��̨װ�÷����ĺϻ�ֵ�����
                            		CAN_246_Trans_YK_Buf[5]=port_transmit[PORT_NO_P554_11][17];  // YK_No
                            		CAN_246_Trans_YK_Buf[6]=byte0(reg_iax);                      // CAN243_ADDR
                            		CAN_246_Trans_YK_Buf[7]=(*(near BYTE *)(reg_ibx+0x09));      // CAN_YK_No
                            	}
                            }
                            else
                            {
                                if(  ( (*(near BYTE *)(reg_ibx+0x05))==CODE_CAN_YK_CONFIRM )
                                   &&( (*(near BYTE *)(reg_ibx+0x06))==(*(near BYTE *)(reg_ibx+0x08)) )  // action type 
                                   &&( (*(near BYTE *)(reg_ibx+0x07))==(*(near BYTE *)(reg_ibx+0x09)) )  // YK_No
                                   &&(((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_EXECUTE)||((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_CANCEL)) 
                                   &&( (*(near BYTE *)(reg_ibx+0x07))<0x02)
                                  )
                                {
                                	(*((far BYTE *)(SOFT_ERR_FLAG+0x0008)))++;
                                	if((port_transmit_flag[PORT_NO_P554_11]==0x55)
                                	 &&(CAN_246_Trans_YK_Buf[0]==CAN_246_TRANS_YK_STA_WAIT_CONFIRM)
                                	 &&(                byte0(reg_iax)==CAN_246_Trans_YK_Buf[6])    // CAN243_ADDR
                                	 &&((*(near BYTE *)(reg_ibx+0x07))==CAN_246_Trans_YK_Buf[7])    // YK_No
                                	 )
                            	    {
                            		    port_transmit[PORT_NO_P554_11][0]=PORT_EXCHANGE_STA_START;
                                		port_transmit[PORT_NO_P554_11][1]=PORT_NO_CAN_0;
                                		port_transmit[PORT_NO_P554_11][2]=PROTOCOL_CAN_DSA%0x100;
                                		port_transmit[PORT_NO_P554_11][3]=PROTOCOL_CAN_DSA/0x100;
                            		
                                		if((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_EXECUTE)
                                		    port_transmit[PORT_NO_P554_11][8]=CORE_CODE_YK_EXECUTE;
                                		else
                                		    port_transmit[PORT_NO_P554_11][8]=CORE_CODE_YK_CANCEL;
                                		      
                                		port_transmit[PORT_NO_P554_11][9] =0x02;
                    	        		port_transmit[PORT_NO_P554_11][10]=0x00;
                        	    		port_transmit[PORT_NO_P554_11][16]=(*(near BYTE *)(reg_ibx+0x06));
                            			if((*(near BYTE *)(reg_ibx+0x07))==0x00)
                            			{
                            			    port_transmit[PORT_NO_P554_11][5] =Vqc_Info[byte0(ram_cx)].YK0_208_YK_BOARD_Addr;
                            		    	port_transmit[PORT_NO_P554_11][17]=Vqc_Info[byte0(ram_cx)].YK0_208_YK_BOARD_YKNO;
	                            		}
    	                        		else
        	                    		{
            	                		    port_transmit[PORT_NO_P554_11][5] =Vqc_Info[byte0(ram_cx)].YK1_208_YK_BOARD_Addr;
                	            		    port_transmit[PORT_NO_P554_11][17]=Vqc_Info[byte0(ram_cx)].YK1_208_YK_BOARD_YKNO;
                    	        		}
                            		
                        	    		port_transmit_flag[PORT_NO_P554_11]=0xaa;
                        	    		
                                        Rcd_Info_System_Tmp[0]=0;                         // reserved
                                        Rcd_Info_System_Tmp[1]=RCD_INFO_SYSTEM_AREA0_CAN;
                                        Rcd_Info_System_Tmp[2]=CAN_246_Trans_YK_Buf[6];   // YK_NO
			                            if((*(near BYTE *)(reg_ibx+0x06))==CORE_CODE2_YK_EXECUTE)
			                                Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_246_YK_EXECUTE;
			                            else    
			                                Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_246_YK_CANCEL;
            			                Rcd_Info_System_Tmp[4]=CAN_246_Trans_YK_Buf[2];  // source port
                        			    Rcd_Info_System_Tmp[5]=CAN_246_Trans_YK_Buf[3];  // 485 YK_BUS_ADDR
			                            Rcd_Info_System_Tmp[6]=CAN_246_Trans_YK_Buf[4];  // 485 Action_Type
            			                Rcd_Info_System_Tmp[7]=CAN_246_Trans_YK_Buf[5];  // 485 YK_NO
                        
                        			    Store_Rcd_Info_System();
                        	    		
                        	    		CAN_246_Trans_YK_Buf[0]=CAN_246_TRANS_YK_STA_IDLE;
                        	    		CAN_246_Trans_YK_Buf[1]=0xff;
                        	    		CAN_246_Trans_YK_Buf[2]=0xff;
                        	    		CAN_246_Trans_YK_Buf[3]=0xff;
                        	    		CAN_246_Trans_YK_Buf[4]=0xff;
                        	    		CAN_246_Trans_YK_Buf[5]=0xff;
                        	    		CAN_246_Trans_YK_Buf[6]=0xff;
                        	    		CAN_246_Trans_YK_Buf[7]=0xff;
                            		  }
                            	  }
                            }
                            
                        }
                    }
                }
            }
        }
        
        ram_the_take_ptr++;         //-��ʾ�ִ�����һ������,��ʵӦ����������һ���ֽڵ����ݰ�
    }
    
    RAM_CAN0_Para.CAN_PORT_RECE_BUF_TAK_PTR=ram_the_save_ptr;	//-��������˵ĸ���

} 













/*===========================================================================*/
void CAN0_Port_Transmit(void)    //  in short 1ms process
{
    register WORD  reg_ibx;
             BYTE  ram_need_port_trans;           ;;;���ڼ�¼�Ƿ�������շ�����
             BYTE  ram_the_array[13];             ;;;֡��¼��
         far BYTE *ram_the_base_addr;
             
    ram_need_port_trans=0;
    
    if(REG_CAN0_Port.F_TRANS_STATUS!=0)     //  transing,not know complete  ;;;������ڷ��;ͽ���������жϹ���
    {
        if( ( (Now_CANx_Sw_Channel_Sta[0x00]<0x80)&&((*(far BYTE *)(CAN0_STATUS+0) & 0x0c)==0x0c) )     ;;;�жϷ��ͻ�������״̬���Ƿ����д��������
          ||( (Now_CANx_Sw_Channel_Sta[0x00]>0x7f)&&((*(far BYTE *)(CAN1_STATUS+0) & 0x0c)==0x0c) ) )
        {
            REG_CAN0_Port.F_TRANS_STATUS=0;   ;;;���´��˷�������֮���⵽���ͽ�����,��ô�͸�0˵�����ͽ���

        	if(Now_CANx_Sw_Channel_Sta[0x00]<0x80)      ;;;ѡ�񵽵���CAN0����1��
        	  {
	            CAN0_Comm_CurrentSta[0]=GOOD;        ;;;��ǰ�Ǻõ�
    	        if(Portx_Poll_First[12]!=YES)       ;;;Portx_Poll_First���ھ�����������HOST_ZF_enable��0xaa
        	    {
            	    CAN0_Comm_Error_LastSta[0]=GOOD;
	            }
            
    	        if(CAN0_Comm_Error_LastSta[0]==ERR)
        	    {
            	    if(CN_CAN0_Comm_Error[0]>1)
                	{
                    	CN_CAN0_Comm_Error[0]=CN_CAN0_Comm_Error[0]-2;
	                }
    	            else
        	        {
            	        CN_CAN0_Comm_Error[0]=0;
                	}
                
	                if(CN_CAN0_Comm_Error[0]==0)
    	            {
        	            CAN0_Comm_Error_LastSta[0]=GOOD;
            	        CAN0_CurrentSta_Buf[0]=CAN0_CurrentSta_Buf[0] & 0x7f;
                	    // store 0 -> communicate_right in    event_rcd RAM
	                }
	            }    
	            else  // CAN0_Comm_Error_LastSta[0]==GOOD
    	        {
        	        CN_CAN0_Comm_Error[0]=0;
            	}
            }
          else
            {
	            CAN0_Comm_CurrentSta[MAX_CAN_NUMBER-1]=GOOD;             ;;;64
    	        if(Portx_Poll_First[12]!=YES)
        	    {
            	    CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]=GOOD;
	            }
            
    	        if(CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]==ERR)
        	    {
            	    if(CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]>1)
                	{
                    	CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]-=2;
	                }
    	            else
        	        {
            	        CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]=0;
                	}
                
	                if(CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]==0)
    	            {
        	            CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]=GOOD;
            	        CAN0_CurrentSta_Buf[MAX_CAN_NUMBER-1]=CAN0_CurrentSta_Buf[MAX_CAN_NUMBER-1] & 0x7f;
                	    // store 0 -> communicate_right in    event_rcd RAM
	                }
	            }    
	            else  // CAN0_Comm_Error_LastSta[0]==GOOD
    	        {
        	        CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]=0;
            	}
            }
            
            if(REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_TRANSINGTOCPU)        ;;;F2 ����CPU����Ϣ
            {    // to be different from sending COSACK
                if(REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME==YES)        ;;;�ѵ���������������
                {
                    if(REG_CAN0_Proc.CAN_MON_NEED_REPLY==YES)      ;;;��������һ֡�˾�Ҫ��,��û�б�Ҫ�ش�
                    {
                        RAM_CAN0_Para.CAN_BEGINWAITCPUREPLY_TIME=Time_1ms_Counter;
                        
                        REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=NO;
                        REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR=REG_CAN0_Port.TRAN_CPU_INFO_ADDR;      ;;;��ʾ��Ҫ��̨CPU�Ļش�
                        REG_CAN0_Port.RECE_CPU_LONGFRAME_FRAMECUR=0;     ;;;����йر�־λ
                        
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_WAITCPUREPLY;       ;;;�ȴ�CPU�Ļش�
                        REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE=YES;             ;;;��ͨ����CPU�ش��ͨ�� 
                    }
                    else
                    {
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;     ;;;��ʾ����Ҫ�ȴ�CPU�ش���
                    }
                }
            }
        }
        else   // can_status indicate not trans complete  ;;;����д���ݵĴ���
        {
            REG_CAN0_Port.F_TRANS_STATUS++;       ;;;�ӿ�ʼ�´﷢�����ʼ����20MS��û�з��ͳ�ȥ�ľͷ���
            if(REG_CAN0_Port.F_TRANS_STATUS>20)    // 20ms
            {
                // may store info to indicate trans abort      ;;;��ʱ���Ͳ��ɹ��ľͷ����������
                if(Now_CANx_Sw_Channel_Sta[0x00]<0x80)          ;;;����������CAN0����1����Ҫ�´�����������
                	*(far BYTE *)(CAN0_COMMAND+0)=0xe2;  // yao zhe trans_CAN  ;;;ز�۷��� ;����������ڴ���ȴ��еķ��ͱ�ȡ��
                else
                	*(far BYTE *)(CAN1_COMMAND+0)=0xe2;  // yao zhe trans_CAN
                	
                REG_CAN0_Port.F_TRANS_STATUS=0;
                
                if(Now_CANx_Sw_Channel_Sta[0x00]<0x80)  // fail at CAN0
                {
	                CAN0_Comm_CurrentSta[0]=ERR;         ;;;��¼CAN0����֡�����˴���,û���ܹ����ͳ�ȥ
    	            if(Portx_Poll_First[12]!=YES)       ;;;�᲻���Ǳ�ֻ֤��¼һ����
        	        {
            	        CAN0_Comm_Error_LastSta[0]=ERR;
                	}
                
	                if(CAN0_Comm_Error_LastSta[0]==GOOD)
    	            {
        	            if(CN_CAN0_Comm_Error[0]<CAN_COMM_BEERR_NUM)            ;;;4
            	        {
                	        CN_CAN0_Comm_Error[0]++;
                    	}
                    
	                    if(CN_CAN0_Comm_Error[0]>=CAN_COMM_BEERR_NUM)
    	                {
        	                CAN0_Comm_Error_LastSta[0]=ERR;
	                       	if(CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]==ERR)
                        	{
    	        	            for(reg_ibx=0;reg_ibx<MAX_CAN_NUMBER;reg_ibx++)
        	        	        {
        	            	        CAN0_Comm_CurrentSta[reg_ibx]=CAN0_Comm_CurrentSta[reg_ibx] | 0x80;
        	            	    }    
                        	}
	                    }
    	            }
        	        else  // F_RAM_CAN.CAN_Comm_Error_LastSta=ERR 
            	    {
                	    CN_CAN0_Comm_Error[0]=CAN_COMM_BEERR_NUM;
                    	for(reg_ibx=1;reg_ibx<(MAX_CAN_NUMBER-1);reg_ibx++)
	                    {
       	            		Now_CANx_Sw_Channel_Sta[reg_ibx]=0xa0;
                    	}
                	}
                }
                else
                {
	                CAN0_Comm_CurrentSta[MAX_CAN_NUMBER-1]=ERR;
    	            if(Portx_Poll_First[12]!=YES)
        	        {
            	        CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]=ERR;
                	}
                
	                if(CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]==GOOD)
    	            {
        	            if(CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]<CAN_COMM_BEERR_NUM)
            	        {
                	        CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]++;
                    	}
                    
	                    if(CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]>=CAN_COMM_BEERR_NUM)
    	                {
        	                CAN0_Comm_Error_LastSta[MAX_CAN_NUMBER-1]=ERR;
                        
                        	if(CAN0_Comm_Error_LastSta[0]==ERR)
                        	{
	            	            for(reg_ibx=0;reg_ibx<MAX_CAN_NUMBER;reg_ibx++)
    	            	        {
           	        	        	CAN0_Comm_CurrentSta[reg_ibx]=CAN0_Comm_CurrentSta[reg_ibx] | 0x80;
                	        	}
                	        }	
	                    }
    	            }
        	        else  // F_RAM_CAN.CAN_Comm_Error_LastSta=ERR 
            	    {
                	    CN_CAN0_Comm_Error[MAX_CAN_NUMBER-1]=CAN_COMM_BEERR_NUM;
                    	for(reg_ibx=1;reg_ibx<(MAX_CAN_NUMBER-1);reg_ibx++)
	                    {
               	    		Now_CANx_Sw_Channel_Sta[reg_ibx]=0x50;
                    	}
                	}
                }
                
                CAN0_Comm_CurrentSta[REG_CAN0_Port.TRAN_CPU_INFO_ADDR]=UNKNOW;
                
                if(REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_TRANSINGTOCPU)
                {// to be different from sending COSACK
                    REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                }
            }
        }
    }
    else
    {//  must put into the_array        ;;;�����е���˼��
        if(RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR!=RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR)  // COS_ACK 1st
        {
            ram_the_array[0x00]=0x88;
            ram_the_array[0x01]=CAN0_COSACK_Buf[RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR];  ;;;Ŀ�ĵ�ַ
            ram_the_array[0x02]=0x00;                                                       ;;;��ǰ֡��
            ram_the_array[0x03]=0x00;             // MON Addr                               ;;;Դ��ַ
            ram_the_array[0x04]=CAN_SHORT_FRAME;  // short frame                            ;;;֡����
            ram_the_array[0x05]=0x00;             // S                                      ;;;װ�õ�ǰ״̬
            ram_the_array[0x06]=CODE_CAN_COS_ACK;                                           ;;;��CPU������COS��ʾ�յ� �·���ȷ�ϱ��� Ҫ��500MS֮�ڷ���
            ram_the_array[0x07]=0x00;
            ram_the_array[0x08]=0x00;
            ram_the_array[0x09]=0x00;
            ram_the_array[0x0a]=0x00;
            ram_the_array[0x0b]=0x00;
            ram_the_array[0x0c]=0x00;
            
            ram_need_port_trans=1;                ;;;׼������֡������1��ʾ���ڿ��Է���
            RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR                                         ;;;���һ�η���ָ��������ƶ�һλ
                =(RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR+1) & AND_CAN_COSACK_QUELEN;       ;;;��Χ��0~15
        }
        else
        {
            if(REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_TRANSINGTOCPU)       // normal MSG 2nd ;;;��������о������ⲽ�Ĺ���
            {
                ram_the_array[0x00]=0x88;
                if(CAN0_TranProc_Buf[0]==CAN_SHORT_FRAME)              ;;;�������еĶ̱��ľ���������ط���֯��,��֯�ɱ�׼֡����ʽ,���з���
                {
                    ram_the_array[0x01]=REG_CAN0_Port.TRAN_CPU_INFO_ADDR;   ;;;Ŀ���ַ
                    ram_the_array[0x02]=0;                                  ;;;��ǰ֡��
                    ram_the_array[0x03]=0;                                  ;;;Դ��ַ
                    ram_the_array[0x04]=CAN_SHORT_FRAME;                    ;;;֡����
                    
                    ram_the_array[0x05]=CAN0_TranProc_Buf[1];               ;;;ʵ�ʵ����ݵ�Ԫ8�ֽ�
                    ram_the_array[0x06]=CAN0_TranProc_Buf[2];
                    ram_the_array[0x07]=CAN0_TranProc_Buf[3];
                    ram_the_array[0x08]=CAN0_TranProc_Buf[4];
                    ram_the_array[0x09]=CAN0_TranProc_Buf[5];
                    ram_the_array[0x0a]=CAN0_TranProc_Buf[6];
                    ram_the_array[0x0b]=CAN0_TranProc_Buf[7];
                    ram_the_array[0x0c]=CAN0_TranProc_Buf[8];
                    
                    ram_need_port_trans=1;                                ;;;���ո�SJA1000�´﷢��������������?
                    REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=YES;             ;;;�����־��ʾ��YES����
                }
                else
                {
                    if((CAN0_TranProc_Buf[0]==CAN_VERTIME_FRAME) && (REG_Year%100 < 70))//fjh 2006 07 20
                    {
                        //Clock_Process();
                        ram_the_array[0x01]=CAN_BROADCAST_ADDR;       ;;;�㲥��ַ
                        ram_the_array[0x02]=0;
                        ram_the_array[0x03]=0;
                        ram_the_array[0x04]=CAN_VERTIME_FRAME;        ;;;��ʱ����
                    
                        ram_the_array[0x05]=REG_Year % 100;
                        ram_the_array[0x06]=REG_Month;
                        ram_the_array[0x07]=REG_Date;
                        ram_the_array[0x08]=REG_Hour;
                        ram_the_array[0x09]=REG_Minute;
                        ram_the_array[0x0a]=REG_Second;
                        ram_the_array[0x0b]=byte1(REG_1Msecond);
                        ram_the_array[0x0c]=byte0(REG_1Msecond);       ;;;ûʲô�ر��ǰ����Ǻ��б����ĺ�
                    
                        ram_need_port_trans=1;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=YES;      ;;;�ѵ���Щ�ط���ʾ����д�����ݿ��Է�������
                        
			        	if(Now_CANx_Sw_Channel_Sta[ram_the_array[0x01]]>0x7f) 
			        	{
			        		Now_CANx_Sw_Channel_Sta[ram_the_array[0x01]]=0x50;
			        	}
			        	else
			        	{
			        		Now_CANx_Sw_Channel_Sta[ram_the_array[0x01]]=0xa0;
			        	}
                    }
                    else  // CAN_LONG_FRAME        ;;;���������ĵ���֯�ط�
                    {
                        ram_the_array[0x01]=REG_CAN0_Port.TRAN_CPU_INFO_ADDR;               ;;;Ŀ�ĵ�ַ
                        ram_the_array[0x02]=REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR;      ;;;��ǰ֡�� 
                        ram_the_array[0x03]=0;                                              ;;;Դ��ַ  
                        ram_the_array[0x04]=CAN_LONG_FRAME;                                 ;;;֡����
                    
                        if(REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR==0)     ;;;һ����֯�����ĵķ����ж�
                        {
                            ram_the_array[0x05]=REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM;    ;;;��֡��
                            ram_the_array[0x06]=CAN0_TranProc_Buf[1];
                            ram_the_array[0x07]=CAN0_TranProc_Buf[2];
                            ram_the_array[0x08]=CAN0_TranProc_Buf[3];
                            ram_the_array[0x09]=CAN0_TranProc_Buf[4];
                            ram_the_array[0x0a]=CAN0_TranProc_Buf[5];
                            ram_the_array[0x0b]=CAN0_TranProc_Buf[6];
                            ram_the_array[0x0c]=CAN0_TranProc_Buf[7];               ;;;ʣ�µ�֡����,֮ǰ���жϴ����ʱ��׼������,��ֻ����֯����
                        }
                        else
                        {
                            reg_ibx=REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR*8;
                            ram_the_array[0x05]=CAN0_TranProc_Buf[reg_ibx+0x00];
                            ram_the_array[0x06]=CAN0_TranProc_Buf[reg_ibx+0x01];
                            ram_the_array[0x07]=CAN0_TranProc_Buf[reg_ibx+0x02];
                            ram_the_array[0x08]=CAN0_TranProc_Buf[reg_ibx+0x03];
                            ram_the_array[0x09]=CAN0_TranProc_Buf[reg_ibx+0x04];
                            ram_the_array[0x0a]=CAN0_TranProc_Buf[reg_ibx+0x05];
                            ram_the_array[0x0b]=CAN0_TranProc_Buf[reg_ibx+0x06];
                            ram_the_array[0x0c]=CAN0_TranProc_Buf[reg_ibx+0x07];
                        }
                        
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR++;      ;;;8������һ�����������ͳһ��ͷ
                        if(REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR
                            >=REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM)  ;;;�ﵽ��֡��
                        {
                            REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=YES;     ;;;��ʾ���������һ�������ĵ����һ֡��
                        }
                        ram_need_port_trans=1;    
                    }
                }
            }
            else
            {
                if(CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr!=CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr)         // CPU ask CMU 3rd   ;;;��Щ������CPU ��CMU,CMU�Ļش�
                {
                    ram_the_base_addr=(far BYTE *)&CAN0_CPUASKCMU_Trans_Buf[0]+CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr*12;
                    
                    ram_the_array[0x00]=0x88;
                    ram_the_array[0x01]=ram_the_base_addr[0x00];
                    ram_the_array[0x02]=ram_the_base_addr[0x01];
                    ram_the_array[0x03]=ram_the_base_addr[0x02];             
                    ram_the_array[0x04]=ram_the_base_addr[0x03];  
                    ram_the_array[0x05]=ram_the_base_addr[0x04];             
                    ram_the_array[0x06]=ram_the_base_addr[0x05];
                    ram_the_array[0x07]=ram_the_base_addr[0x06];
                    ram_the_array[0x08]=ram_the_base_addr[0x07];
                    ram_the_array[0x09]=ram_the_base_addr[0x08];
                    ram_the_array[0x0a]=ram_the_base_addr[0x09];
                    ram_the_array[0x0b]=ram_the_base_addr[0x0a];
                    ram_the_array[0x0c]=ram_the_base_addr[0x0b];
            
                    ram_need_port_trans=1;
                    CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr++;
                }
            }
        }
                                                                
           ;;;CPUд��,BSP����,����֮��ͽ�����CAN����
        if(ram_need_port_trans!=0)                                  ;;;���տ�ʼ������������ط���,�������0�;�ֱ�ӽ�����
        {
        	if(Now_CANx_Sw_Channel_Sta[ram_the_array[0x01]]>0x7f)     ;;;�ж���CAN0����1�ڷ���
        	 {
	            *(far BYTE *)(CAN1_TRANS_BUF+0x00)=ram_the_array[0x00];       ;;;����ط��Ͱ�Ҫ���͵�����,д�뵽�����յ�Ԫ,һ������Ϳ����Զ�����
	            temp_buf[0]=ram_the_array[0x00];
    	        *(far BYTE *)(CAN1_TRANS_BUF+0x01)=ram_the_array[0x01];
    	        temp_buf[1]=ram_the_array[0x01];
        	    *(far BYTE *)(CAN1_TRANS_BUF+0x02)=ram_the_array[0x02];
        	 temp_buf[2]=ram_the_array[0x02];
            	*(far BYTE *)(CAN1_TRANS_BUF+0x03)=ram_the_array[0x03];
            	temp_buf[3]=ram_the_array[0x03];
	            *(far BYTE *)(CAN1_TRANS_BUF+0x04)=ram_the_array[0x04];
	            temp_buf[4]=ram_the_array[0x04];
    	        *(far BYTE *)(CAN1_TRANS_BUF+0x05)=ram_the_array[0x05];
    	        temp_buf[5]=ram_the_array[0x05];
        	    *(far BYTE *)(CAN1_TRANS_BUF+0x06)=ram_the_array[0x06];
        	    temp_buf[6]=ram_the_array[0x06];
            	*(far BYTE *)(CAN1_TRANS_BUF+0x07)=ram_the_array[0x07];
            	temp_buf[7]=ram_the_array[0x07];
	            *(far BYTE *)(CAN1_TRANS_BUF+0x08)=ram_the_array[0x08];
	            temp_buf[8]=ram_the_array[0x08];
    	        *(far BYTE *)(CAN1_TRANS_BUF+0x09)=ram_the_array[0x09];
    	        temp_buf[9]=ram_the_array[0x09];
        	    *(far BYTE *)(CAN1_TRANS_BUF+0x0a)=ram_the_array[0x0a];
        	    temp_buf[10]=ram_the_array[0x0a];
            	*(far BYTE *)(CAN1_TRANS_BUF+0x0b)=ram_the_array[0x0b];
            	temp_buf[11]=ram_the_array[0x0b];
	            *(far BYTE *)(CAN1_TRANS_BUF+0x0c)=ram_the_array[0x0c];
	            temp_buf[12]=ram_the_array[0x0c];
            
    	        *(far BYTE *)(CAN1_COMMAND+0)=0xe1;                             ;;;��SJA1000�´﷢����������
    	        Now_CANx_Sw_Channel_Sta[0x00]=0x80;  // now sending on CAN1
        	 }
        	else
        	{
	            *(far BYTE *)(CAN0_TRANS_BUF+0x00)=ram_the_array[0x00];       ;;;д�뷢�ͻ������ȴ�����,���ĵط���,����֮ǰ
	            temp_buf[0]=ram_the_array[0x00];
    	        *(far BYTE *)(CAN0_TRANS_BUF+0x01)=ram_the_array[0x01];
    	        temp_buf[1]=ram_the_array[0x01];
        	    *(far BYTE *)(CAN0_TRANS_BUF+0x02)=ram_the_array[0x02];
        	    temp_buf[2]=ram_the_array[0x02];
            	*(far BYTE *)(CAN0_TRANS_BUF+0x03)=ram_the_array[0x03];
            	temp_buf[3]=ram_the_array[0x03];
	            *(far BYTE *)(CAN0_TRANS_BUF+0x04)=ram_the_array[0x04];
	            temp_buf[4]=ram_the_array[0x04];
    	        *(far BYTE *)(CAN0_TRANS_BUF+0x05)=ram_the_array[0x05];
    	        temp_buf[5]=ram_the_array[0x05];
        	    *(far BYTE *)(CAN0_TRANS_BUF+0x06)=ram_the_array[0x06];
        	    temp_buf[6]=ram_the_array[0x06];
            	*(far BYTE *)(CAN0_TRANS_BUF+0x07)=ram_the_array[0x07];
            	temp_buf[7]=ram_the_array[0x07];
	            *(far BYTE *)(CAN0_TRANS_BUF+0x08)=ram_the_array[0x08];
	            temp_buf[8]=ram_the_array[0x08];
    	        *(far BYTE *)(CAN0_TRANS_BUF+0x09)=ram_the_array[0x09];
    	        temp_buf[9]=ram_the_array[0x09];
        	    *(far BYTE *)(CAN0_TRANS_BUF+0x0a)=ram_the_array[0x0a];
        	    temp_buf[10]=ram_the_array[0x0a];
            	*(far BYTE *)(CAN0_TRANS_BUF+0x0b)=ram_the_array[0x0b];
            	temp_buf[11]=ram_the_array[0x0b];
	            *(far BYTE *)(CAN0_TRANS_BUF+0x0c)=ram_the_array[0x0c];
	            temp_buf[12]=ram_the_array[0x0c];
            
    	        *(far BYTE *)(CAN0_COMMAND+0)=0xe1;          ;;;�����汣���Ķ�Ĭ����1 ;;;��SJA1000�´﷢������,�����Զ����з���
    	        Now_CANx_Sw_Channel_Sta[0x00]=0x40;  // now sending on CAN0       ;;;��ʾ��������ͨ���ĸ�CAN�ڷ���
            }
            REG_CAN0_Port.F_TRANS_STATUS=1;
        }
    }
} 













//-��ʵ�����Ѿ�������ԭ����һ����,ֻ����������Щ��ͬ����
/*===========================================================================*/
void CAN_MSG_Proc(void)            //-�Խ��յ��ı�������һ����������,�Ѿ���֡�ĸ�ʽ�����������
{
         WORD  dumyword;
         WORD  sumword ;
         WORD  sumword1;
    near BYTE *the_ram_address;
    far  BYTE *the_ram_addr;
    far  WORD *the_ram_address_w1;

    if(REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_IDLE)  // if=CAN_MON_TransingToCPU,do nothing ;;;�����жϵ�������״̬ ʹ�����ϵͳ����������,��ʲôҲ����
    {
        if(port_transmit_flag[PORT_NO_CAN_0]==0xaa)        ;;;����AA��ʾ��������ֵ,��Ҫ��һ������
        {
            port_transmit_flag[PORT_NO_CAN_0]=0x55;        ;;;��ʾ��������Ҫ����
            
            switch(port_transmit[PORT_NO_CAN_0][8])     ;;;�ѵ�Ϊ�����и�ʽͳһ,���ڲ�ʹ�������е�Ӧ�ò�
            {
                case CORE_CODE_YK_CHOOSE:
                    if(port_transmit[PORT_NO_CAN_0][5]==MAX_CAN0_ADDR)       ;;;ǰ���ʾ�����õ���CAN0��,������
                    {
                        exchange_target_port=port_transmit[PORT_NO_CAN_0][1];

                        exchange_buf[1] = PORT_NO_CAN_0;           ;;;��ʾ��CAN0��
                        exchange_buf[2] = PROTOCOL_CAN_DSA%0x100;
                        exchange_buf[3] = PROTOCOL_CAN_DSA/0x100;  ;;;��¼Э������
                        exchange_buf[5] = MAX_CAN0_ADDR;           ;;;����ַ��,����̨װ�÷�����
                        exchange_buf[8] = CORE_CODE_YK_VERIFY;     ;;;�Ա��ĵĽ�һ���������ݷ���������
                        exchange_buf[9] = 0x02;
                        exchange_buf[10]= 0x00;
                        exchange_buf[16]= port_transmit[PORT_NO_CAN_0][16];  // the same as CORE_CODE2_YK_CLOSE/TRIP/ERROR
                        exchange_buf[17]= port_transmit[PORT_NO_CAN_0][17];  // YK No.
                        if(exchange_target_port<14)
                            exchange_buf[17] += unit_info[byte0(port_info[12].mirror_unit) + exchange_buf[5]].yk_start;
                        Ex_Produce_Transmit_Info();        ;;;����һ����������׼������
                        if(port_transmit[PORT_NO_CAN_0][17]==1)  //VQC208 ON/OFF 2004.09.02   ;;;������Щ��û�������߼�����һ�����ӵ�����
                         {
                          dumyword=unit_info[MAX_CAN1_ADDR].channel_no;
                          sumword=dumyword/16;
                          sumword1=dumyword%16;
                          if(port_transmit[PORT_NO_CAN_0][16]==CORE_CODE2_YK_CLOSE)      ;;;ң��
                           YX_State[sumword]|=0x0001<<sumword1;         ;;;����λ�ٻ�
                          else
                           if(port_transmit[PORT_NO_CAN_0][16]==CORE_CODE2_YK_TRIP)      ;;;ң��
                            YX_State[sumword]&=(~(0x0001<<sumword1));
                         }
                    }
                    else
                    {
	                    if((port_transmit[PORT_NO_CAN_0][5]>=MIN_CAN0_ADDR)&&(port_transmit[PORT_NO_CAN_0][5]<=MAX_CAN0_ADDR))
    	                {
        	                CAN0_TranProc_Buf[0]=CAN_SHORT_FRAME;
            	            CAN0_TranProc_Buf[1]=0x00;              // S
                	        CAN0_TranProc_Buf[2]=0x40;              // CMD           ;;;�Ƿ���ң��ѡ��ı�����(CMU->CPU)
                    	    CAN0_TranProc_Buf[3]=port_transmit[PORT_NO_CAN_0][16];    // action type
                        	CAN0_TranProc_Buf[4]=port_transmit[PORT_NO_CAN_0][17];    // YK no.
	                        CAN0_TranProc_Buf[5]=CAN0_TranProc_Buf[3];                ;;;������һ����
    	                    CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[4];    
        	                CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][5];        ;;;װ�õĵ�ַ      
            	            CAN0_TranProc_Buf[8]=~(CAN0_TranProc_Buf[7]);             ;;;ȡ�������м���
                
                	        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=port_transmit[PORT_NO_CAN_0][5];    ;;;�����ŵ��ǵ�ַ��Ϣ
                    	    REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;                     ;;;��ʾ��Щ������Ϊ��֡׼������
                        	REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
	                        REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;                           ;;;��ЩӦ�ö���Щ����λ,
    	                    REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YKVERIFY;        ;;;��ʾ��ҪCPU����ң��ȷ�ϱ���
        	                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=port_transmit[PORT_NO_CAN_0][1];  // source port;;;�й���,�϶�Ҳ������
            	            REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                	    }
                	  }    
                    break;       ;;;���������ж����������
                    
                case CODE_CAN_WAVRCDDATA:
                    if((port_transmit[PORT_NO_CAN_0][5]>=MIN_CAN0_ADDR)&&(port_transmit[PORT_NO_CAN_0][5]<=MAX_CAN0_ADDR))
                    {
                        CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                        CAN0_TranProc_Buf[1]=0x00;              // S
                        CAN0_TranProc_Buf[2]=0x00;              // Group
                        CAN0_TranProc_Buf[3]=CODE_CAN_WAVRCDDATA;              // CMD
                        CAN0_TranProc_Buf[4]=0x02;              // LenL
                        CAN0_TranProc_Buf[5]=0x00;              // LenH
                        CAN0_TranProc_Buf[6]=port_transmit[PORT_NO_CAN_0][20]; 
                        CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][21];    
                        sumword=CAN0_TranProc_Buf[2]+CAN0_TranProc_Buf[3]+CAN0_TranProc_Buf[4]
                               +CAN0_TranProc_Buf[6]+CAN0_TranProc_Buf[7];
                        CAN0_TranProc_Buf[8]=byte0(sumword);
                        CAN0_TranProc_Buf[9]=byte1(sumword);              
                
                        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=port_transmit[PORT_NO_CAN_0][5];
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=2;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
                        REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;
                        REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_WAVRCD;
                        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=port_transmit[PORT_NO_CAN_0][1];  // source port
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                    }
                    break;
                    
                case CORE_CODE_YK_EXECUTE:
                    if(port_transmit[PORT_NO_CAN_0][5]==MAX_CAN0_ADDR)
                    {
                     if(port_transmit[PORT_NO_CAN_0][17]==0)
                      {
           	            CAN0_TranProc_Buf[0]=CAN_SHORT_FRAME;              ;;;�����źű��Ļ�����,�����Ӧ֡����,����˸�������Data0~Data7,��Щ����Ӧ�ò�
               	        CAN0_TranProc_Buf[1]=0x00;              // S
                   	    CAN0_TranProc_Buf[2]=CODE_CAN_BRDCST_RST_RLY;  // CMD    ;;;CMU�㲥����CPU���������źţ�CPU����ش�
                       	CAN0_TranProc_Buf[3]=0x00;
                        CAN0_TranProc_Buf[4]=0x00;
   	                    CAN0_TranProc_Buf[5]=0x00;
       	                CAN0_TranProc_Buf[6]=0x00;
           	            CAN0_TranProc_Buf[7]=0x00;
               	        CAN0_TranProc_Buf[8]=0x00;
                	
                   	    REG_CAN0_Port.TRAN_CPU_INFO_ADDR=CAN_BROADCAST_ADDR;        ;;;��¼��֡�ĵ�ַ
                       	REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                   
   	                    REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;          ;;;��ʾ�費��ҪCPU�ش�
       	                //REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YKRESULT;
           	            REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port
               	        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;

			for(byte0(sumword)=0;byte0(sumword)<12;byte0(sumword)++) // 2004-07-29
 			 {
      			  if((port_info[byte0(sumword)].protocal_type==PROTOCOL_NAZI94)&&(port_transmit_flag[byte0(sumword)] != YES))
      			   {
			    port_transmit_flag[byte0(sumword)] = 0xaa;
			    the_ram_addr=&port_transmit[byte0(sumword)][0];
			    the_ram_addr[1]=port_transmit[PORT_NO_CAN_0][1];
			    the_ram_addr[8]=0x80;
			    the_ram_addr[9]=0x04;
			    the_ram_addr[10]=0x00;
			    the_ram_addr[16]=0xff;
			    the_ram_addr[17]=0x80;
			    the_ram_addr[18]=0x01;
			    the_ram_addr[19]=0x00;
      			   }
      			  if((port_info[byte0(sumword)].protocal_type==PROTOCOL_IEC103_RCS)&&(port_transmit_flag[byte0(sumword)] != YES))
      			   {
			    port_transmit_flag[byte0(sumword)] = 0xaa;
			    the_ram_addr=&port_transmit[byte0(sumword)][0];
			    the_ram_addr[1]=port_transmit[PORT_NO_CAN_0][1];
			    the_ram_addr[8]=0x14;
			    the_ram_addr[9]=0x04;
			    the_ram_addr[10]=0x00;
			    the_ram_addr[16]=0xff;
			    the_ram_addr[17]=0xff;
			    the_ram_addr[18]=0x00;
			    the_ram_addr[19]=0x00;
      			   }         
  			 }
  		     }
                    }
                    else
                    {
	                    if((port_transmit[PORT_NO_CAN_0][5]==REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx)&&
    	                   (REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx!=0xff))
        	            {
            	            CAN0_TranProc_Buf[0]=CAN_SHORT_FRAME;
                	        CAN0_TranProc_Buf[1]=0x00;              // S
                    	    CAN0_TranProc_Buf[2]=0x41;              // CMD
                        	CAN0_TranProc_Buf[3]=0xaa;              // action type
	                        CAN0_TranProc_Buf[4]=port_transmit[PORT_NO_CAN_0][17];    // YK no.
    	                    CAN0_TranProc_Buf[5]=CAN0_TranProc_Buf[3];              
        	                CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[4];    
            	            CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][5];              
                	        CAN0_TranProc_Buf[8]=~(CAN0_TranProc_Buf[7]);              
                	
                    	    REG_CAN0_Port.TRAN_CPU_INFO_ADDR=port_transmit[PORT_NO_CAN_0][5];
                        	REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
	                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
    	                    REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;
        	                REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YKRESULT;
            	            REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port
                	        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                        
                    	    Rcd_Info_System_Tmp[0]=0;  // reserved
                        	Rcd_Info_System_Tmp[1]=RCD_INFO_SYSTEM_AREA0_CAN;
	                        Rcd_Info_System_Tmp[2]=CAN0_TranProc_Buf[7];
    	                    Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_YK_EXECUTE;
        	                Rcd_Info_System_Tmp[4]=port_transmit[PORT_NO_CAN_0][1];  // source port
            	            Rcd_Info_System_Tmp[5]=0;  
                	        Rcd_Info_System_Tmp[6]=0;  
                    	    Rcd_Info_System_Tmp[7]=0; 
                        
                        	Store_Rcd_Info_System();
                        }	
                    }
                    break;
                    
                case CORE_CODE_YK_CANCEL:
                    if((port_transmit[PORT_NO_CAN_0][5]>=MIN_CAN0_ADDR)&&(port_transmit[PORT_NO_CAN_0][5]<=MAX_CAN0_ADDR))
                    {
                        CAN0_TranProc_Buf[0]=CAN_SHORT_FRAME;
                        CAN0_TranProc_Buf[1]=0x00;              // S
                        CAN0_TranProc_Buf[2]=0x41;              // CMD
                        CAN0_TranProc_Buf[3]=0x55;              // action type
                        CAN0_TranProc_Buf[4]=port_transmit[PORT_NO_CAN_0][17];    // YK no.
                        CAN0_TranProc_Buf[5]=CAN0_TranProc_Buf[3];              
                        CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[4];    
                        CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][5];              
                        CAN0_TranProc_Buf[8]=~(CAN0_TranProc_Buf[7]);              
                
                        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=port_transmit[PORT_NO_CAN_0][5];
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
                        REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;
                        REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YKRESULT;
                        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                    }
                    break;
                    
                case LSA_CODE_ACK: 
                    Rcd_Info_System_Tmp[0]=0;  // reserved
                    Rcd_Info_System_Tmp[1]=RCD_INFO_SYSTEM_AREA0_CAN;            // CAN PORT
                    Rcd_Info_System_Tmp[2]=REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx;     // CAN ADDR
                    Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_SET_MODI;       // do  what
                    Rcd_Info_System_Tmp[4]=port_transmit[PORT_NO_CAN_0][1];      // source port
                    Rcd_Info_System_Tmp[5]=0;
                    Rcd_Info_System_Tmp[6]=0;  
                    Rcd_Info_System_Tmp[7]=0; 
                        
                    Store_Rcd_Info_System();
                case LSA_CODE_NAK:
                    if(REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx<=MAX_CAN0_ADDR)
                    {
                        CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                        CAN0_TranProc_Buf[1]=0x00;              // S
                        CAN0_TranProc_Buf[2]=0x00;              // Group
                        CAN0_TranProc_Buf[3]=CODE_CAN_W_R_RLY_DATA;              // CMD
                        CAN0_TranProc_Buf[4]=0x02;              // LenL
                        CAN0_TranProc_Buf[5]=0x00;              // LenH
                        CAN0_TranProc_Buf[6]=BH_GUIHAO_IN_CAN_INTERNAL_PROTOCOL; // replace port_transmit[PORT_NO_CAN_0][16]
                        CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][17];    
                        sumword=CAN0_TranProc_Buf[2]+CAN0_TranProc_Buf[3]+CAN0_TranProc_Buf[4]
                               +CAN0_TranProc_Buf[6]+CAN0_TranProc_Buf[7];
                        CAN0_TranProc_Buf[8]=byte0(sumword);
                        CAN0_TranProc_Buf[9]=byte1(sumword);              
                
                        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx;
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=2;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
                        REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                        //REG_CAN0_Proc.CAN_MON_WAIT_STATYP=CAN_WAIT_CPU_YKRESULT;
                        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                    }
                    break;

                case LSA_CODE_C14_SETGRP_MODI_ACK:
                    if((port_transmit[PORT_NO_CAN_0][19]==REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx)&&
                       (REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx!=0xff))
                    {
                        CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                        CAN0_TranProc_Buf[1]=0x00;              // S
                        CAN0_TranProc_Buf[2]=0x00;              // Group
                        CAN0_TranProc_Buf[3]=CODE_CAN_W_R_RLY_DATA;              // CMD
                        CAN0_TranProc_Buf[4]=0x05;              // LenL
                        CAN0_TranProc_Buf[5]=0x00;              // LenH
                        CAN0_TranProc_Buf[6]=BH_GUIHAO_IN_CAN_INTERNAL_PROTOCOL; // replace port_transmit[PORT_NO_CAN_0][16]
                        CAN0_TranProc_Buf[7]=port_transmit[PORT_NO_CAN_0][17];    // CMD
                        CAN0_TranProc_Buf[8]=port_transmit[PORT_NO_CAN_0][18];    // LEN
                        CAN0_TranProc_Buf[9]=port_transmit[PORT_NO_CAN_0][19];    
                        CAN0_TranProc_Buf[10]=port_transmit[PORT_NO_CAN_0][20];    
                        byte0(sumword)=port_transmit[PORT_NO_CAN_0][21];
                        CAN0_TranProc_Buf[11]=byte0(sumword);
                        byte1(sumword)=port_transmit[PORT_NO_CAN_0][22];
                        CAN0_TranProc_Buf[12]=byte1(sumword);
                        sumword=sumword+CAN0_TranProc_Buf[3]+CAN0_TranProc_Buf[4]+CAN0_TranProc_Buf[5]+CAN0_TranProc_Buf[6]; // group=0
                        CAN0_TranProc_Buf[13]=byte0(sumword);
                        CAN0_TranProc_Buf[14]=byte1(sumword);
                
                        REG_CAN0_Port.TRAN_CPU_INFO_ADDR=CAN0_TranProc_Buf[9];
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                        REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=2;
                        REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                    
                        REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                        REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;

                        Rcd_Info_System_Tmp[0]=0;  // reserved
                        Rcd_Info_System_Tmp[1]=RCD_INFO_SYSTEM_AREA0_CAN;
                        Rcd_Info_System_Tmp[2]=CAN0_TranProc_Buf[9];
                        Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_SETGROUP_MODI;
                        Rcd_Info_System_Tmp[4]=port_transmit[PORT_NO_CAN_0][1];   // source port
                        Rcd_Info_System_Tmp[5]=0;
                        Rcd_Info_System_Tmp[6]=0;  
                        Rcd_Info_System_Tmp[7]=0; 
                        
                        Store_Rcd_Info_System();
                    }
                    break;

                default:  // other all as BH msg
                    CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                    CAN0_TranProc_Buf[1]=0x00;              // S
                    CAN0_TranProc_Buf[2]=0x00;              // Group
                    CAN0_TranProc_Buf[3]=CODE_CAN_W_R_RLY_DATA;              // CMD
                    byte0(sumword)=port_transmit[PORT_NO_CAN_0][9] ;
                    byte1(sumword)=port_transmit[PORT_NO_CAN_0][10];
                    if(sumword>504) sumword=504;
                    CAN0_TranProc_Buf[4]=byte0(sumword);    // LenL
                    CAN0_TranProc_Buf[5]=byte1(sumword);    // LenH
                    CAN0_TranProc_Buf[6]=BH_GUIHAO_IN_CAN_INTERNAL_PROTOCOL; // replace port_transmit[PORT_NO_CAN_0][16]          
                    for(dumyword=7;dumyword<(sumword+6);dumyword++)
                    {
                        CAN0_TranProc_Buf[dumyword]=port_transmit[PORT_NO_CAN_0][dumyword+10];
                    }
                    byte0(sumword1)=CAN0_TranProc_Buf[dumyword-2];
                    byte1(sumword1)=CAN0_TranProc_Buf[dumyword-1];
                    sumword1=sumword1+CAN0_TranProc_Buf[3]+CAN0_TranProc_Buf[4]
                                     +CAN0_TranProc_Buf[5]+CAN0_TranProc_Buf[6]; // group=0
                    CAN0_TranProc_Buf[dumyword+0]=byte0(sumword1);
                    CAN0_TranProc_Buf[dumyword+1]=byte1(sumword1);
                    
                    //2004-07-10
                    if(port_transmit[PORT_NO_CAN_0][8]==LSA_CODE_C14_RLY_OPERATE_ACT)
                    {
	                    if((port_transmit[PORT_NO_CAN_0][19]==REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx)&&
    	                   (REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx!=0xff))
	                    {
		                    REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
    		                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;  // source port

		                    Rcd_Info_System_Tmp[0]=0;  // reserved
        		            Rcd_Info_System_Tmp[1]=RCD_INFO_SYSTEM_AREA0_CAN;            // CAN PORT
                		    Rcd_Info_System_Tmp[2]=REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx;     // CAN ADDR
		                    Rcd_Info_System_Tmp[3]=RCD_INFO_SYSTEM_AREA2_SET_MODI;       // do  what
        		            Rcd_Info_System_Tmp[4]=port_transmit[PORT_NO_CAN_0][1];      // source port
                		    Rcd_Info_System_Tmp[5]=0;
		                    Rcd_Info_System_Tmp[6]=0;  
        		            Rcd_Info_System_Tmp[7]=0; 
                        
                		    Store_Rcd_Info_System();
                    	}
                    	else
                    		break;
                    }
                    else
                    {
	                    REG_CAN0_Proc.CAN_MON_NEED_REPLY=YES;
    	                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=port_transmit[PORT_NO_CAN_0][1];  // source port
                    }
                    REG_CAN0_Port.TRAN_CPU_INFO_ADDR=CAN0_TranProc_Buf[9];
                    REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                    REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=(sumword+8+7)/8;
                    REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;

       	            REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                    break;
            }
            REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx=0xff;
            
        }
        else
        {
            if(CAN_MON_stachg_task_proc()!=YES)      ;;;����YES˵����������Ҫ����,�������Զ���ʱ�жϵ�
            {
                if(CAN_MON_inquire_task_proc()!=YES)       ;;;�������Ҳ��һ������
                {
                    // other task
                }
            }    
        }
    }
    else // not CAN_MON_IDLE	CAN_MON_����
    {
        if(REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_WAITCPUREPLY)
        {
            if(REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC==YES)
            {
                if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YC) goto rcd_ser_good;      ;;;�ж�CPU��Ҫ�ش������
                if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YX) goto rcd_ser_good;
                if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YM) goto rcd_ser_good;
                goto no_rcd_ser_good;
rcd_ser_good:
                CAN0_Comm_CurrentSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=GOOD;
                if(CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]!=ERR)
                    CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=GOOD;
                if(Portx_Poll_First[12]!=YES)
                {
                    CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=0;
                    CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=GOOD;
                }
                else
                {
                    if(CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]==ERR)
                    {
                        if(CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]>1)
                        {
                            CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]--;
                            CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]--;
                        }
                        else
                        {
                            CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=0;
                        }
                    
                        if(CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]==0)
                        {
                            CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=GOOD;
                            if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                            the_ram_addr=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
                            the_ram_addr[0]=PROTOCOL_LSA%0x100;     // protocol type
                            the_ram_addr[1]=0x51;                   // CMD      code
                            the_ram_addr[2]=0x00;                   // frame    no
                            the_ram_addr[3]=0x12;                   // valid    bytes  in this frame
                            the_ram_addr[4]=the_ram_addr[0]
                                           +the_ram_addr[1]
                                           +the_ram_addr[2]
                                           +the_ram_addr[3];        // verify   byte
                            the_ram_addr[5]=PROTOCOL_LSA/0x100;     // Reserved
            
                            the_ram_addr[6]=0x01;                   // Frame    Num
                            the_ram_addr[7]=0x00;                   // Reserved
                            the_ram_addr[8]=CAN0_AS_BH_MON_ADDR_SET;// CAN      GuiHao
                            the_ram_addr[9]=0x51;
                            the_ram_addr[10]=0x0d;
                            byte0(sumword)=REG_Year%100;
                            the_ram_addr[11]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                            byte0(sumword)=(REG_Year%10000)/100;
                            the_ram_addr[12]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                            the_ram_addr[13]=(REG_Month/10)*0x10+REG_Month%10;
                            the_ram_addr[14]=(REG_Date/10)*0x10+REG_Date%10;
                            the_ram_addr[15]=(REG_Hour/10)*0x10+REG_Hour%10;
                            the_ram_addr[16]=(REG_Minute/10)*0x10+REG_Minute%10;
                            the_ram_addr[17]=(REG_Second/10)*0x10+REG_Second%10;
                            byte0(sumword)=REG_1Msecond%100;
                            the_ram_addr[18]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                            byte0(sumword)=(REG_1Msecond%1000)/100;
                            the_ram_addr[19]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                            the_ram_addr[20]=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                            the_ram_addr[21]=0x05;
                            sumword=0;
                            for(byte0(dumyword)=9;byte0(dumyword)<22;byte0(dumyword)++)
                            {
                             	sumword+=the_ram_addr[byte0(dumyword)];
                            }
                            the_ram_addr[22]=byte0(sumword);
                            the_ram_addr[23]=byte1(sumword);
            
                            BH_Report_Bank_Sav_Ptr++;
                            if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
                            
                             // BH to SOE
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE[dumyword].ActionType==the_ram_addr[21]))
                                        	{
                                            	BH_To_YX_Delay_Begin_Time[dumyword]=Time_2048ms_Counter;
                    
	                                            dumyword =BH_Event_To_SOE[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] | (1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]=(byte1(dumyword) & 0x0f)+0x80; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                 
                                        dumyword++;
                                    }
                                    
                                    // BH to SOE Ret
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE_Ret[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE_Ret[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE_Ret[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE_Ret[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE_Ret[dumyword].ActionType==the_ram_addr[21]))
                                        	{
	                                            dumyword =BH_Event_To_SOE_Ret[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] & (0xffff-1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]= byte1(dumyword) & 0x0f; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                                        dumyword++;
                                    }
                            
                            
                            
                            
                            
                            
                            
                        }
                    }
                    else
                    {
                        CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=0;
                    }    
                }

no_rcd_ser_good:
                // different status process 
                if(CAN_MON_common_proc()!=YES)         ;;;�������Ϊʲôû��Դ����?????����
                {
                    if(REG_CAN0_Proc.CAN_NEEDTRANSTOHOST==CAN_NONEEDTRANSTOHOST)        ;;;����ط��������йؼ���̨
                    {
                        byte0(dumyword)=0;
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_LIFETIME) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_LIFE_TIME)       ;;;����Խ��յ������ݽ��д���
                            {
                                // save Life_Time
//                                if(port_info[12].protect_enable==0xaa)
                                {
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                                    the_ram_addr=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
            
                                    the_ram_addr[0]=PROTOCOL_LSA%0x100;     // protocol type
                                    the_ram_addr[1]=0x51;                   // CMD      code
                                    the_ram_addr[2]=0x00;                   // frame    no
                                    the_ram_addr[3]=0x12;                   // valid    bytes  in this frame
                                    the_ram_addr[4]=the_ram_addr[0]
                                                   +the_ram_addr[1]
                                                   +the_ram_addr[2]
                                                   +the_ram_addr[3];        // verify   byte
                                    the_ram_addr[5]=PROTOCOL_LSA/0x100;     // Reserved
            
                                    the_ram_addr[6]=0x01;                   // Frame    Num
                                    the_ram_addr[7]=0x00;                   // Reserved
                                    the_ram_addr[8]=CAN0_AS_BH_MON_ADDR_SET;// CAN      GuiHao
                                    for(byte1(dumyword)=9;byte1(dumyword)<(9+13+2);byte1(dumyword)++)
                                    {
                                        the_ram_addr[byte1(dumyword)]=CAN0_ReceProc_Buf[byte1(dumyword)-2];
                                    }
            
                                    BH_Report_Bank_Sav_Ptr++;
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
                            	
                            	 // BH to SOE
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE[dumyword].ActionType==the_ram_addr[21]))
                                        	{
                                            	BH_To_YX_Delay_Begin_Time[dumyword]=Time_2048ms_Counter;
                    
	                                            dumyword =BH_Event_To_SOE[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] | (1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]=(byte1(dumyword) & 0x0f)+0x80; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                 
                                        dumyword++;
                                    }
                                    
                                    // BH to SOE Ret
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE_Ret[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE_Ret[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE_Ret[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE_Ret[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE_Ret[dumyword].ActionType==the_ram_addr[21]))
                                        	{
	                                            dumyword =BH_Event_To_SOE_Ret[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] & (0xffff-1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]= byte1(dumyword) & 0x0f; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                                        dumyword++;
                                    }
                            	
                            	
                            
                            
                                }
                            
                                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                                CAN0_TranProc_Buf[1]=0x00;                  // S
                                CAN0_TranProc_Buf[2]=0x00;                  // Group
                                CAN0_TranProc_Buf[3]=CODE_CAN_LIFE_T_ACK;   // CMD
                                CAN0_TranProc_Buf[4]=0x00;   
                                CAN0_TranProc_Buf[5]=0x00;   
                                CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[3];  // SumL 
                                CAN0_TranProc_Buf[7]=0x00;   
                                
                                REG_CAN0_Port.TRAN_CPU_INFO_ADDR=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                                REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                                
                                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
                                REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                                
                                byte0(dumyword)=1;
                            }
                        }

                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_RLYEVENT) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_EVENT)
                            {
                                // save EVENT
//                                if(port_info[12].protect_enable==0xaa)
                                {
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                                    the_ram_addr=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
            
                                    the_ram_addr[0]=PROTOCOL_LSA%0x100;     // protocol type
                                    the_ram_addr[1]=0x40;                    // CMD      code
                                    the_ram_addr[2]=0x00;                    // frame    no
                                    the_ram_addr[3]=0x18;                    // valid    bytes  in this frame
                                    the_ram_addr[4]=the_ram_addr[0]
                                                   +the_ram_addr[1]
                                                   +the_ram_addr[2]
                                                   +the_ram_addr[3];         // verify   byte
                                    the_ram_addr[5]=PROTOCOL_LSA/0x100;     // Reserved
            
                                    the_ram_addr[6]=0x01;             // Frame    Num
                                    the_ram_addr[7]=0x00;             // Reserved
                                    the_ram_addr[8]=CAN0_AS_BH_MON_ADDR_SET;    // CAN      GuiHao
                                    for(byte1(dumyword)=9;byte1(dumyword)<(9+19+2);byte1(dumyword)++)
                                    {
                                        the_ram_addr[byte1(dumyword)]=CAN0_ReceProc_Buf[byte1(dumyword)-2];
                                    }
            
                                    BH_Report_Bank_Sav_Ptr++;
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                        			if((BH_Act_Alarm_Type[the_ram_addr[24]]==0x02)&&(bh_event_alarm_now_action_warn==NO))
                        			    bh_event_alarm_warn  =1;
                        			if((BH_Act_Alarm_Type[the_ram_addr[24]]==0x01)&&(bh_event_alarm_now_action_warn==NO))
                        			    bh_event_alarm_action=1;

                                    // BH to SOE
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE[dumyword].RLYAddr   ==the_ram_addr[20])
                                    	     &&(BH_Event_To_SOE[dumyword].ActionType==the_ram_addr[24]))
                                        	{
                                            	BH_To_YX_Delay_Begin_Time[dumyword]=Time_2048ms_Counter;
                    
	                                            dumyword =BH_Event_To_SOE[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] | (1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]=(byte1(dumyword) & 0x0f)+0x80; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                 
                                        dumyword++;
                                    }
                                    
                                    // BH to SOE Ret
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE_Ret[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE_Ret[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE_Ret[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE_Ret[dumyword].RLYAddr   ==the_ram_addr[20])
                                    	     &&(BH_Event_To_SOE_Ret[dumyword].ActionType==the_ram_addr[24]))
                                        	{
	                                            dumyword =BH_Event_To_SOE_Ret[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] & (0xffff-1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]= byte1(dumyword) & 0x0f; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                                        dumyword++;
                                    }
                                }
                                
                                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                                CAN0_TranProc_Buf[1]=0x00;                  // S
                                CAN0_TranProc_Buf[2]=0x00;                  // Group
                                CAN0_TranProc_Buf[3]=CODE_CAN_EVENT_ACK;    // CMD
                                CAN0_TranProc_Buf[4]=0x00;   
                                CAN0_TranProc_Buf[5]=0x00;   
                                CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[3];  // SumL 
                                CAN0_TranProc_Buf[7]=0x00;   
                                
                                REG_CAN0_Port.TRAN_CPU_INFO_ADDR=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                                REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                                
                                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
                                REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                                
                                byte0(dumyword)=1;
                            }
                        }
						// 2004-07-10                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_EVENT_EXT) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_EVENT_EXT)
                            {
                                // save EVENT
//                                if(port_info[12].protect_enable==0xaa)
                                {
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                                    the_ram_addr=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
            
                                    the_ram_addr[0]=PROTOCOL_LSA%0x100;      // protocol type
                                    the_ram_addr[1]=0x41;                    // CMD      code
                                    the_ram_addr[2]=0x00;                    // frame    no
                                    the_ram_addr[3]=0x19;                    // valid    bytes  in this frame
                                    the_ram_addr[4]=the_ram_addr[0]
                                                   +the_ram_addr[1]
                                                   +the_ram_addr[2]
                                                   +the_ram_addr[3];         // verify   byte
                                    the_ram_addr[5]=PROTOCOL_LSA/0x100;     // Reserved
            
                                    the_ram_addr[6]=0x01;             // Frame    Num
                                    the_ram_addr[7]=0x00;             // Reserved
                                    the_ram_addr[8]=CAN0_AS_BH_MON_ADDR_SET;    // CAN      GuiHao
                                    for(byte1(dumyword)=9;byte1(dumyword)<(9+20+2);byte1(dumyword)++)
                                    {
                                        the_ram_addr[byte1(dumyword)]=CAN0_ReceProc_Buf[byte1(dumyword)-2];
                                    }
            
                                    BH_Report_Bank_Sav_Ptr++;
                                    if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
            
                        			//if((BH_Act_Alarm_Type[the_ram_addr[24]]==0x02)&&(bh_event_alarm_now_action_warn==NO))
                        			//    bh_event_alarm_warn  =1;
                        			//if((BH_Act_Alarm_Type[the_ram_addr[24]]==0x01)&&(bh_event_alarm_now_action_warn==NO))
                        			//    bh_event_alarm_action=1;

                                    // BH to SOE
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE[dumyword].DeviceType==0x80+the_ram_addr[25])  // DeviceType=LSA_EXTxx
					    	            {
                            	            if((BH_Event_To_SOE[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE[dumyword].RLYAddr   ==the_ram_addr[20])
                                    	     &&(BH_Event_To_SOE[dumyword].ActionType==the_ram_addr[24]))
                                        	{
                                            	BH_To_YX_Delay_Begin_Time[dumyword]=Time_2048ms_Counter;
                    
	                                            dumyword =BH_Event_To_SOE[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] | (1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]=(byte1(dumyword) & 0x0f)+0x80; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                 
                                        dumyword++;
                                    }
                                    
                                    // BH to SOE Ret
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE_Ret[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE_Ret[dumyword].DeviceType==0x80+the_ram_addr[25])  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE_Ret[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE_Ret[dumyword].RLYAddr   ==the_ram_addr[20])
                                    	     &&(BH_Event_To_SOE_Ret[dumyword].ActionType==the_ram_addr[24]))
                                        	{
	                                            dumyword =BH_Event_To_SOE_Ret[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] & (0xffff-1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]= byte1(dumyword) & 0x0f; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                                        dumyword++;
                                    }
                                }
                                
                                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                                CAN0_TranProc_Buf[1]=0x00;                  // S
                                CAN0_TranProc_Buf[2]=0x00;                  // Group
                                CAN0_TranProc_Buf[3]=CODE_CAN_EVENT_EXT_ACK;    // CMD
                                CAN0_TranProc_Buf[4]=0x00;   
                                CAN0_TranProc_Buf[5]=0x00;   
                                CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[3];  // SumL 
                                CAN0_TranProc_Buf[7]=0x00;   
                                
                                REG_CAN0_Port.TRAN_CPU_INFO_ADDR=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                                REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                                
                                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
                                REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                                
                                byte0(dumyword)=1;
                            }
                        }// 2004-07-10
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_SOE) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_SOE)
                            {
                                byte1(dumyword)=(CAN0_ReceProc_Buf[5]*256+CAN0_ReceProc_Buf[4])/8;
                                if(byte1(dumyword)>32) byte1(dumyword)=32;

                                for(byte0(dumyword)=0;byte0(dumyword)<byte1(dumyword);byte0(dumyword)++)
                                {
                                    the_ram_address=(near BYTE *)&CAN0_ReceProc_Buf[6+byte0(dumyword)*8];
                                    Core_Src_Unit=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);  // Dbase addr
                                    if( the_ram_address[6]< ((unit_info[byte0(Core_Src_Unit)].yx_num)*16) )
                                    {
                                        port_report[0]=the_ram_address[0];
                                        port_report[1]=the_ram_address[1];
                                        port_report[2]=the_ram_address[2];
                                        port_report[3]=the_ram_address[3];
                                        port_report[4]=the_ram_address[4];
                                        port_report[5]=the_ram_address[5];
                                        port_report[6]=the_ram_address[6];
                                        port_report[7]=the_ram_address[7];
                                        Core_Src_Pt_B=&(port_report[0]);
                                        core_insert_SOE();
                                    }
                                }


                                CAN0_TranProc_Buf[0]=CAN_LONG_FRAME;
                                CAN0_TranProc_Buf[1]=0x00;                  // S
                                CAN0_TranProc_Buf[2]=0x00;                  // Group
                                CAN0_TranProc_Buf[3]=CODE_CAN_SOE_ACK;      // CMD
                                CAN0_TranProc_Buf[4]=0x00;   
                                CAN0_TranProc_Buf[5]=0x00;   
                                CAN0_TranProc_Buf[6]=CAN0_TranProc_Buf[3];  // SumL 
                                CAN0_TranProc_Buf[7]=0x00;   
                                
                                REG_CAN0_Port.TRAN_CPU_INFO_ADDR=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMESUM=1;
                                REG_CAN0_Port.TRAN_CPU_LONGFRAME_FRAMECUR=0;
                                REG_CAN0_Port.TRAN_CPU_IS_LAST_FRAME=NO;
                                
                                REG_CAN0_Proc.CAN_NEEDTRANSTOHOST=CAN_NONEEDTRANSTOHOST;
                                REG_CAN0_Proc.CAN_MON_NEED_REPLY=NO;
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_TRANSINGTOCPU;
                                
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YCCHG) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_YC_CHG)
                            {
                                byte0(dumyword)=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);
                                the_ram_address=(near BYTE *)&YC_State[unit_info[byte0(dumyword)].yc_val_start];
                                byte1(dumyword)=CAN0_ReceProc_Buf[4]/33*16;
                                byte0(dumyword)=unit_info[byte0(dumyword)].yc_val_num;
                                
                                if(byte1(dumyword)>byte0(dumyword))  // need save yc_num
                                {
                                    byte1(dumyword)=byte0(dumyword);
                                }
                                
                                byte0(sumword)=6;
                                for(byte0(dumyword)=0;byte0(dumyword)<byte1(dumyword);byte0(dumyword)++)
                                {
                                    if(byte0(dumyword)&0x0f==0)  byte0(sumword)++;
                                    byte1(sumword)=byte0(dumyword)*2+byte0(sumword);
                                    the_ram_address[0]=CAN0_ReceProc_Buf[byte1(sumword)];
                                    the_ram_address[1]=CAN0_ReceProc_Buf[byte1(sumword)+1];
                                    the_ram_address=the_ram_address+2;
                                }
                                
                                //if((RAM_CAN0_Para.CAN_RECEYCCHG_BUF_SAV_PTR+1) & AND_CAN_YCCHG_QUELEN
                                //    ==RAM_CAN0_Para.CAN_RECEYCCHG_BUF_TAK_PTR)
                                //{
                                //  RAM_CAN0_Para.CAN_RECEYCCHG_BUF_TAK_PTR
                                //  =(RAM_CAN0_Para.CAN_RECEYCCHG_BUF_TAK_PTR+1) & AND_CAN_YCCHG_QUELEN;
                                //}
                                   
                                //CAN0_ReceYCCHG_Buf[RAM_CAN0_Para.CAN_RECEYCCHG_BUF_SAV_PTR]
                                //    =REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                    
                                //RAM_CAN0_Para.CAN_RECEYCCHG_BUF_SAV_PTR
                                //=(RAM_CAN0_Para.CAN_RECEYCCHG_BUF_SAV_PTR+1) & AND_CAN_YCCHG_QUELEN;
                                
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YC) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_YC_ALL)
                            {
                                byte0(dumyword)=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);
                                the_ram_address=(near BYTE *)&YC_State+(unit_info[byte0(dumyword)].yc_val_start*DATABASE_YC_INFO_LEN);
                                byte1(dumyword)=CAN0_ReceProc_Buf[4]/33*16;
                                byte0(dumyword)=unit_info[byte0(dumyword)].yc_val_num;
                                
                                if(byte1(dumyword)>byte0(dumyword))  // need save yc_num
                                {
                                    byte1(dumyword)=byte0(dumyword);
                                }
                                
                                byte0(sumword)=6;
                                for(byte0(dumyword)=0;byte0(dumyword)<byte1(dumyword);byte0(dumyword)++)
                                {
                                    if((byte0(dumyword)&0x0f)==0)  byte0(sumword)++;
                                    byte1(sumword)=byte0(dumyword)*2+byte0(sumword);
                                    the_ram_address[0]=CAN0_ReceProc_Buf[byte1(sumword)];
                                    the_ram_address[1]=CAN0_ReceProc_Buf[byte1(sumword)+1];
                                    the_ram_address=the_ram_address+2;
                                }
                                
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YX) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_YX_ALL)
                            {
                                Core_Src_Unit=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);  // Dbase addr
                                Core_Src_Len =CAN0_ReceProc_Buf[4]/2;  // bytes num
                                if(Core_Src_Len>(unit_info[byte0(Core_Src_Unit)].yx_num)*2)
                                    Core_Src_Len=(unit_info[byte0(Core_Src_Unit)].yx_num)*2;
                                
                                for(byte0(dumyword)=0;byte0(dumyword)<Core_Src_Len;byte0(dumyword)++)
                                {
                                    port_report[byte0(dumyword)]=CAN0_ReceProc_Buf[byte0(dumyword)*2+7];
                                }

                                Core_Src_Pt_B=&port_report[0];
                                if((Core_Src_Len & 0x01)!=0) 
                                {
                                	port_report[Core_Src_Len]=0;
                                	Core_Src_Len++;
                                }	

                                if(unit_info[Core_Src_Unit].unit_type==0x0101)
                                {
                                	if((CAN0_ReceProc_Buf[1] & 0x08)==0)  port_report[1]&=0x7f;
                                	else                                  port_report[1]|=0x80;
                                }
                                core_update_YX();
                                
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YM) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_YM_ALL)
                            {
                                byte0(dumyword)=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);
                                the_ram_address=(near BYTE *)&YM_State[unit_info[byte0(dumyword)].ym_start];
                                byte1(dumyword)=CAN0_ReceProc_Buf[4]/17*4;
                                byte0(dumyword)=unit_info[byte0(dumyword)].ym_num;
                                
                                if(byte1(dumyword)>byte0(dumyword))  // need save ym_num
                                {
                                    byte1(dumyword)=byte0(dumyword);
                                }
                                
                                byte0(sumword)=6;
                                for(byte0(dumyword)=0;byte0(dumyword)<byte1(dumyword);byte0(dumyword)++)
                                {
                                    if((byte0(dumyword)&0x03)==0)  byte0(sumword)++;
                                    byte1(sumword)=byte0(dumyword)*4+byte0(sumword);
                                    the_ram_address[0]=CAN0_ReceProc_Buf[byte1(sumword)];
                                    the_ram_address[1]=CAN0_ReceProc_Buf[byte1(sumword)+1];
                                    the_ram_address[2]=CAN0_ReceProc_Buf[byte1(sumword)+2];
                                    the_ram_address[3]=CAN0_ReceProc_Buf[byte1(sumword)+3];
                                    the_ram_address=the_ram_address+4;
                                }
                                
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if((REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_DDAC) &&
                           (CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME))
                        {
                            if(CAN0_ReceProc_Buf[3]==CODE_CAN_DD_AC)
                            {
                                byte0(dumyword)=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR+byte0(port_info[PORT_NO_CAN_0].mirror_unit);
                                the_ram_address=(near BYTE *)&YM_State[unit_info[byte0(dumyword)].dc_start];
                                byte1(dumyword)=CAN0_ReceProc_Buf[4]/17*4;
                                byte0(dumyword)=unit_info[byte0(dumyword)].dc_num;
                                
                                if(byte1(dumyword)>byte0(dumyword))  // need save ym_num
                                {
                                    byte1(dumyword)=byte0(dumyword);
                                }
                               
                                byte0(sumword)=6;
                                for(byte0(dumyword)=0;byte0(dumyword)<byte1(dumyword);byte0(dumyword)++)
                                {
                                    if((byte0(dumyword)&0x03)==0)  byte0(sumword)++;
                                    byte1(sumword)=byte0(dumyword)*4+byte0(sumword);
                                    the_ram_address[0]=CAN0_ReceProc_Buf[byte1(sumword)];
                                    the_ram_address[1]=CAN0_ReceProc_Buf[byte1(sumword)+1];
                                    the_ram_address[2]=CAN0_ReceProc_Buf[byte1(sumword)+2];
                                    the_ram_address[3]=CAN0_ReceProc_Buf[byte1(sumword)+3];
                                    the_ram_address=the_ram_address+4;
                                }
                                
                                REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                                byte0(dumyword)=1;
                            }
                        }
                        
                        if(byte0(dumyword)==0)
                        {
                            REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                            // mean receive sth,but is not needed,if restart receive
                            // time arrived and not received will add err 1 times.
                        }
                    }
                    else  //  not CAN_NoNeedTransToRTU
                    {
                        if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YKVERIFY)
                        {
                            if((CAN0_ReceProc_Buf[0]==CAN_SHORT_FRAME) && (CAN0_ReceProc_Buf[2]==CODE_CAN_YK_CHOOSE))
                            {
                                REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                RAM_CAN0_Para.CAN_BEGINWAITRTUREPLY_TIME=Time_2048ms_Counter;
                                
                                exchange_target_port=REG_CAN0_Proc.CAN_NEEDTRANSTOHOST;

                                exchange_buf[1] = PORT_NO_CAN_0;
                                exchange_buf[2] = PROTOCOL_CAN_DSA%0x100;
                                exchange_buf[3] = PROTOCOL_CAN_DSA/0x100;
                                exchange_buf[5] = REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                exchange_buf[8] = CORE_CODE_YK_VERIFY;
                                exchange_buf[9] = 0x02;
                                exchange_buf[10]= 0x00;
                                exchange_buf[16]= CAN0_ReceProc_Buf[3];  // the same as CORE_CODE2_YK_CLOSE/TRIP/ERROR
                                exchange_buf[17]= CAN0_ReceProc_Buf[4];  // YK No.
                                if(exchange_target_port<14)
                                    exchange_buf[17] += unit_info[byte0(port_info[12].mirror_unit) + exchange_buf[5]].yk_start;
                                Ex_Produce_Transmit_Info();
                            }
                            
                        }
                        else
                        {
                            if((CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME) && (CAN0_ReceProc_Buf[3]==CODE_CAN_W_R_RLY_DATA))
                            {// 2004-07-10
                                if((CAN0_ReceProc_Buf[7]==LSA_CODE_R5_REPLY_SETTING_VERI)||
                                   (CAN0_ReceProc_Buf[7]==LSA_CODE_R13_REPLY_SETGRP_VERI)||
                                   (CAN0_ReceProc_Buf[7]==LSA_CODE_R5_REPLY_SET_EXT_VERI))
                                {
                                    REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                    RAM_CAN0_Para.CAN_BEGINWAITRTUREPLY_TIME=Time_2048ms_Counter;
                                }    
                                exchange_target_port=REG_CAN0_Proc.CAN_NEEDTRANSTOHOST;

                                exchange_buf[1] = PORT_NO_CAN_0;
                                exchange_buf[2] = PROTOCOL_LSA%0x100;
                                exchange_buf[3] = PROTOCOL_LSA/0x100;         // in CDT,CDT9702,HOSTZF these exange baowen be looked as LSA port
                                exchange_buf[4] = CAN0_AS_BH_MON_ADDR_SET;
                                exchange_buf[5] = REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                exchange_buf[8] = CAN0_ReceProc_Buf[7]; // CMD
                                
                                if((exchange_buf[8]!=LSA_CODE_ACK) && (exchange_buf[8]!=LSA_CODE_NAK))
                                {
                                    dumyword = CAN0_ReceProc_Buf[8]+1+2; // LEN
                                    exchange_buf[9] = byte0(dumyword);
                                    exchange_buf[10]= byte1(dumyword);
                                    for(sumword=16;sumword<(dumyword+16);sumword++)
                                        exchange_buf[sumword] = CAN0_ReceProc_Buf[sumword-10];
                                    exchange_buf[16]=exchange_buf[4];    //hhb 2004.10.21    BH GUIHAO
                                }
                                else
                                {
                                    exchange_buf[9]  = 0x02; // LEN
                                    exchange_buf[10] = 0x00;
                                    exchange_buf[16] = exchange_buf[4];
                                    exchange_buf[17] = exchange_buf[8];
                                }
                                
                                Ex_Produce_Transmit_Info();
                            }
                            else
                            {
                            	if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_WAVRCD)
                            	{
                            		if((CAN0_ReceProc_Buf[0]==CAN_LONG_FRAME) && (CAN0_ReceProc_Buf[3]==CODE_CAN_WAVRCDDATA))
                            		{
                            			exchange_target_port=REG_CAN0_Proc.CAN_NEEDTRANSTOHOST;
                            			
                            			if(port_transmit_flag[exchange_target_port]!=0xaa)
                            			{
                            				port_transmit_flag[exchange_target_port]=0xaa;
                            				
                            				the_ram_addr=(BYTE *)&(port_transmit[exchange_target_port][0]);
                            				the_ram_addr[1]=PORT_NO_CAN_0;
                            				the_ram_addr[2]=PROTOCOL_CAN_DSA%0x100;
                            				the_ram_addr[3]=PROTOCOL_CAN_DSA/0x100;
                            				the_ram_addr[4]=CAN0_AS_BH_MON_ADDR_SET;
                            				the_ram_addr[5]=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                            				the_ram_addr[8]=CODE_CAN_WAVRCDDATA;
                            				
                            				byte0(sumword)=CAN0_ReceProc_Buf[4];
                            				byte1(sumword)=CAN0_ReceProc_Buf[5];
                            				sumword+=2;
                            				
                            				for(dumyword=0;dumyword<sumword;dumyword++)
                            				{
                            					the_ram_addr[dumyword+21]=CAN0_ReceProc_Buf[dumyword+4];
                            				}

                            				sumword+=5;
                            				the_ram_addr[16]=byte0(sumword);
                            				the_ram_addr[17]=byte1(sumword);
                            				the_ram_addr[18]=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
//                            				the_ram_addr[19]=0x01;
                                                        the_ram_addr[19]=CAN0_AS_BH_MON_ADDR_SET;  //hhb 2004.10.21
                            				the_ram_addr[20]=0x6e;

                            				the_ram_addr[9] =byte0(sumword);
                            				the_ram_addr[10]=byte1(sumword);
                            			}
                            		}
                            	}
                            }
                        }
                        REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                    }
                    
                }
            }
            else
            {
                if(Judge_Time_In_MainLoop(RAM_CAN0_Para.CAN_BEGINWAITCPUREPLY_TIME,CAN_WAIT_CPU_MSG_TIME)==YES)   ;;;�ȴ�CPU�ش��ʱ����800MS
                {
                    if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YC) goto rcd_ser_err;
                    if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YX) goto rcd_ser_err;
                    if(REG_CAN0_Proc.CAN_MON_WAIT_STATYP==CAN_WAIT_CPU_YM) goto rcd_ser_err;
                    goto no_rcd_ser_err;
rcd_ser_err:
                    CAN0_Comm_CurrentSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=ERR;
                    // may let ser err times and ser err last time to judge ser err 
	                if(CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]!=GOOD)
    	                CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=ERR;
                    if(Portx_Poll_First[12]!=YES)
                    {
                        CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=CAN_COMM_BEERR_NUM;
                        CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=ERR;
                        CAN0_CurrentSta_Buf[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]
                            =CAN0_CurrentSta_Buf[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR] | 0x80;
                    }
                    else
                    {
                        if(CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]==GOOD)
                        {
                            if(CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]<CAN_COMM_BEERR_NUM)
                            {
                                CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]++;
                            }
                        
                            if(CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]>=CAN_COMM_BEERR_NUM)
                            {
                                CAN0_Comm_Error_LastSta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=ERR;
                                CAN0_CurrentSta_Buf[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]
                                    =CAN0_CurrentSta_Buf[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR] | 0x80;
                                
                                if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
                                the_ram_addr=(far BYTE *)&(BH_Saved_Report_Bank[BH_Report_Bank_Sav_Ptr][0]);
                                the_ram_addr[0]=PROTOCOL_LSA%0x100;     // protocol type
                                the_ram_addr[1]=0x51;                   // CMD      code
                                the_ram_addr[2]=0x00;                   // frame    no
                                the_ram_addr[3]=0x12;                   // valid    bytes  in this frame
                                the_ram_addr[4]=the_ram_addr[0]
                                               +the_ram_addr[1]
                                               +the_ram_addr[2]
                                               +the_ram_addr[3];        // verify   byte
                                the_ram_addr[5]=PROTOCOL_LSA/0x100;     // Reserved
            
                                the_ram_addr[6]=0x01;                   // Frame    Num
                                the_ram_addr[7]=0x00;                   // Reserved
                                the_ram_addr[8]=CAN0_AS_BH_MON_ADDR_SET;// CAN      GuiHao
                                the_ram_addr[9]=0x51;
                                the_ram_addr[10]=0x0d;
                                byte0(sumword)=REG_Year%100;
                                the_ram_addr[11]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                                byte0(sumword)=(REG_Year%10000)/100;
                                the_ram_addr[12]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                                the_ram_addr[13]=(REG_Month/10)*0x10+REG_Month%10;
                                the_ram_addr[14]=(REG_Date/10)*0x10+REG_Date%10;
                                the_ram_addr[15]=(REG_Hour/10)*0x10+REG_Hour%10;
                                the_ram_addr[16]=(REG_Minute/10)*0x10+REG_Minute%10;
                                the_ram_addr[17]=(REG_Second/10)*0x10+REG_Second%10;
                                byte0(sumword)=REG_1Msecond%100;
                                the_ram_addr[18]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                                byte0(sumword)=(REG_1Msecond%1000)/100;
                                the_ram_addr[19]=(byte0(sumword)/10)*0x10+byte0(sumword)%10;
                                the_ram_addr[20]=REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR;
                                the_ram_addr[21]=0x04;
                                sumword=0;
                                for(byte0(dumyword)=9;byte0(dumyword)<22;byte0(dumyword)++)
                                {
                                	sumword+=the_ram_addr[byte0(dumyword)];
                                }
                                the_ram_addr[22]=byte0(sumword);
                                the_ram_addr[23]=byte1(sumword);
            
                                BH_Report_Bank_Sav_Ptr++;
                                if(BH_Report_Bank_Sav_Ptr>=BH_REPORT_BANK_SIZE)  BH_Report_Bank_Sav_Ptr=0;
                                
                                 // BH to SOE
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE[dumyword].ActionType==the_ram_addr[21]))
                                        	{
                                            	BH_To_YX_Delay_Begin_Time[dumyword]=Time_2048ms_Counter;
                    
	                                            dumyword =BH_Event_To_SOE[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] | (1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]=(byte1(dumyword) & 0x0f)+0x80; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                 
                                        dumyword++;
                                    }
                                    
                                    // BH to SOE Ret
                                    dumyword=0;
                                    while(dumyword<512)
                                    {
                                        if(BH_Event_To_SOE_Ret[dumyword].YX_State_No>0x0fff) break;
                
						                if(BH_Event_To_SOE_Ret[dumyword].DeviceType==0x0a)  // DeviceType=LSA
					    	            {
                            	            if((BH_Event_To_SOE_Ret[dumyword].GroupAddr ==the_ram_addr[8] )
                                	         &&(BH_Event_To_SOE_Ret[dumyword].RLYAddr   ==(the_ram_addr[20]+0x80))
                                    	     &&(BH_Event_To_SOE_Ret[dumyword].ActionType==the_ram_addr[21]))
                                        	{
	                                            dumyword =BH_Event_To_SOE_Ret[dumyword].YX_State_No;
    	                                        YX_State[dumyword/16]=YX_State[dumyword/16] & (0xffff-1<<(dumyword % 16));
                    
        	                                    sumword =(the_ram_addr[19]%16)*100+(the_ram_addr[18]/16*10+the_ram_addr[18]%16);
            	                                port_report[0]= byte0(sumword);
                	                            port_report[1]= byte1(sumword);
                    	                        port_report[2]= the_ram_addr[17]/16*10+the_ram_addr[17]%16;  // second
                        	                    port_report[3]= the_ram_addr[16]/16*10+the_ram_addr[16]%16;  // minute
                            	                port_report[4]= the_ram_addr[15]/16*10+the_ram_addr[15]%16;  // hour
                                	            port_report[5]= the_ram_addr[14]/16*10+the_ram_addr[14]%16;  // date
                                    	        port_report[6]= byte0(dumyword);
                                        	    port_report[7]= byte1(dumyword) & 0x0f; 
                                            	Core_Src_Unit = 0x00;
                                            	Core_Src_Pt_B = &port_report[0];
	                                            core_insert_SOECOS();
                    
    	                                        break;
    	                                    }    
                                        }
                                        dumyword++;
                                    }
                                
                                
                            }
                        }
                        else  // CAN0_Comm_Error_LastSta=ERR
                        {
                            CN_CAN0_Comm_Error[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=CAN_COMM_BEERR_NUM;
                            if((Now_CANx_Sw_Channel_Sta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR] & 0x10)==0)
                            	Now_CANx_Sw_Channel_Sta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=0x50;
                            else
                            	Now_CANx_Sw_Channel_Sta[REG_CAN0_Port.RECE_CPU_REPLYINFO_ADDR]=0xa0;	
                        }
                    }    
no_rcd_ser_err:
                    //if(F_REG_CAN_PROC.CAN_NeedTransToRTU=CAN_NeedTransToCOMp)then
                    //{
                    //}
                    
                    REG_CAN0_Proc.CAN_MON_STATUS=CAN_MON_IDLE;
                }
            }
        }
        else  // REG_CAN0_Proc.CAN_MON_STATUS==CAN_MON_TransingToCPU
        {
            // do nothing
        }
    }
}


















/************************************************/
/* CAN0_init     function                       */
/************************************************/
//-��ô�����������оͽ��г�ʼ����,�ǲ���˵�����������һ���еĲ��������õ�
/*===========================================================================*/
void CAN0_Init(void)
{
        WORD  ram_bx;
    far BYTE *ram_base_addr;

    ram_base_addr=(far BYTE *)(&CAN0_CurrentSta_Buf);	//-��ʼ״̬��0
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(CAN0_CurrentSta_Buf)/sizeof(CAN0_CurrentSta_Buf[0]);
        ram_bx++)
    {
        ram_base_addr[ram_bx]=0;
    }
    
    ram_base_addr=(far BYTE *)(&CAN0_IED_StaChg);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(CAN0_IED_StaChg)/sizeof(CAN0_IED_StaChg[0]);
        ram_bx++)
    {
        ram_base_addr[ram_bx]=0;	//-��ʼ�����ǶԴ�����־���鸳0,��������һ���˵ķ��
    }
    
    ram_base_addr=(far BYTE *)(&CAN0_Comm_Error_LastSta);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(CAN0_Comm_Error_LastSta)/sizeof(CAN0_Comm_Error_LastSta[0]);
        ram_bx++)
    {
        ram_base_addr[ram_bx]=UNKNOW;
    }
    
    ram_base_addr=(far BYTE *)(&CAN0_Comm_CurrentSta);
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(CAN0_Comm_CurrentSta)/sizeof(CAN0_Comm_CurrentSta[0]);
        ram_bx++)
    {
        ram_base_addr[ram_bx]=UNKNOW;
    }
    
    ram_base_addr=(far BYTE *)(&Now_CANx_Sw_Channel_Sta);	//-��ʼ��50���Ǻ�������
    for(ram_bx=0;
        ram_bx<(WORD)sizeof(Now_CANx_Sw_Channel_Sta)/sizeof(Now_CANx_Sw_Channel_Sta[0x00]);
        ram_bx++)
    {
        ram_base_addr[ram_bx]=0x50;
    }
    if((Vqc_Info[0].DSA_243_Addr!=0x00)&&(Vqc_Info[0].DSA_243_Addr!=0xff))	//-����ѹ�޹����� ��ַ
        Now_CANx_Sw_Channel_Sta[Vqc_Info[0].DSA_243_Addr]|=0x30;
    if((Vqc_Info[1].DSA_243_Addr!=0x00)&&(Vqc_Info[1].DSA_243_Addr!=0xff))
        Now_CANx_Sw_Channel_Sta[Vqc_Info[1].DSA_243_Addr]|=0x30;	//-��ʼ״̬�� ���õļ���ѹ�޹����Ƶ�ַ���Խ���
    
    CAN_Not_Idle_Cn=0;
    
    REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE  =NO;
    REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=NO;
    REG_CAN0_Port.F_TRANS_STATUS             =0;

    REG_CAN0_Proc.CAN_MON_STATUS             =CAN_MON_IDLE;
    
    CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr=0;
    CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr=0;
    
    RAM_CAN0_Para.CAN_RECECOS_RETACK_SAV_PTR=0;	//-���ṹ�帳��ֵ
    RAM_CAN0_Para.CAN_RECECOS_RETACK_TAK_PTR=0;
    
    RAM_CAN0_Para.CAN_RECEYCCHG_BUF_SAV_PTR=0;
    RAM_CAN0_Para.CAN_RECEYCCHG_BUF_TAK_PTR=0;
    
    RAM_CAN0_Para.CAN_PORT_RECE_BUF_SAV_PTR=0;
    RAM_CAN0_Para.CAN_PORT_RECE_BUF_TAK_PTR=0;
    
    
    RAM_CAN0_Para.CAN_BEGIN_VERICPUCLK_TIME=Time_2048ms_Counter+TIME_CAN_VERIFY_TIME_VALUE+1;
    RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME =Time_2048ms_Counter+TIME_ASK_YC_VALUE+1;           ;;; ������Ч��ʱ��5+1
    RAM_CAN0_Para.CAN_BEGIN_POL_YXALL_TIME =Time_2048ms_Counter+TIME_ASK_YX_VALUE+1;           ;;;  10
    RAM_CAN0_Para.CAN_BEGIN_POL_YMALL_TIME =Time_2048ms_Counter+TIME_ASK_YM_VALUE+1;           ;;;   15
    RAM_CAN0_Para.CAN_BEGIN_POL_DD_AC_TIME =Time_2048ms_Counter+TIME_ASK_DD_VALUE+1;           ;;;   15 //-��Щ��Щʱ������ж���Чʱ��ε�
    
    RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_YCALL;
    CAN_NOW_POL_YCALL_FLAG=YES;
    RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR=MIN_CAN0_ADDR;
    REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx =0xff;        ;;;����Ҫ�ж�ʲô
    
    CAN_246_Trans_YK_Buf[0]=CAN_246_TRANS_YK_STA_IDLE;         ;;; 0
    CAN_246_Trans_YK_Buf[1]=0xff;
	CAN_246_Trans_YK_Buf[2]=0xff;
	CAN_246_Trans_YK_Buf[3]=0xff;
	CAN_246_Trans_YK_Buf[4]=0xff;
	CAN_246_Trans_YK_Buf[5]=0xff;
	CAN_246_Trans_YK_Buf[6]=0xff;
	CAN_246_Trans_YK_Buf[7]=0xff;
}










;;;��γ���Ҳ����ǶԽ��յ��ı��ĵĴ��������ʹ������,������ͨѶ�����ͨѶ�ĸ�����
;;;���������Ψһ�Ĵ������,�����Ӧ���Ƕ�һ�Ĺ���.
;;;����Ҫ�������ۺ�ʵ��Ҫ��,�ͱ����������͸ɻ�.
/*===========================================================================*/
void CAN0_Main(void)                           ;;;������ֻ��Ҫ�����ⲿ��Ҫ�ⲿ����,������Ҫ�Ѷ���ı���˵���˿����ⲿ����
{
    BYTE  the_ram_axl;
    BYTE *the_ram_addr_byte;

    CAN_Port_ReceAsm();                 ;;;�ǰѽ��յ�����Ϣת�����Լ��ڲ���Ϣ,���������Ϣ�Ѿ����Ǵ�������������,���ǵ�һ��������,������൱��CPU�еĽ��չ���  ��������
    CAN_MSG_Proc();                     ;;;�Ծ�������������ȡ����Ϣ����һ������,�����CPU��T1�жϵĴ������,CMU�ŵ���ͬһ��ʱ����
    
    if(REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx!=0xff)     ;;;��ֹ�����ַ_����12���ڵ��ĸ���
    {
        if(Judge_LongTime_In_MainLoop(RAM_CAN0_Para.CAN_BEGINWAITRTUREPLY_TIME,30)==YES)    ;;;�ж��Ƿ񳬹� 2048MS*30 �����Χ61.44 S
        {
            REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx=0xff;   ;;;����FF��û�б�Ҫ���ж���
        }
    }
    
    the_ram_addr_byte=(BYTE *)&YX_State[IED_TX_STATE_START_WORD+(byte0(port_info[12].mirror_unit))/16];  // so BUS mirror start addr must be x*16;;;ΪʲôҪȥ��ǰ32��
    for(the_ram_axl=0;the_ram_axl<64;the_ram_axl++)
    {
        if(CAN0_Comm_Error_LastSta[the_ram_axl]==GOOD)
            the_ram_addr_byte[the_ram_axl/8]=the_ram_addr_byte[the_ram_axl/8] & (0xff-(1<<(the_ram_axl%8)));  ;;;��ͬһ���ֽ����������һλһλ����
        if(CAN0_Comm_Error_LastSta[the_ram_axl]==ERR)   ;;;����0��ʾ,������1��ʾ
            the_ram_addr_byte[the_ram_axl/8]=the_ram_addr_byte[the_ram_axl/8] | (1<<(the_ram_axl%8)); ;;;����8���ֽڱ�ʾ64̨װ�õ�״̬
    }
    CAN_judge_state_to_yx();
    temp_int=Time_2048ms_Counter;       ;;;����2048�ļ���
    if(REG_CAN0_Proc.CAN_MON_STATUS!=CAN_MON_IDLE)
    {
        if(CAN_Time2048ms_Last_Value!=temp_int)
            CAN_Not_Idle_Cn++;
        if(CAN_Not_Idle_Cn>300) // 10 min
        {
            REG_CAN0_Port.RECE_CPU_REPLYINFO_ENABLE  =NO;
            REG_CAN0_Port.RECE_CPU_REPLYINFO_WISHPROC=NO;
            REG_CAN0_Port.F_TRANS_STATUS             =0;

            REG_CAN0_Proc.CAN_MON_STATUS             =CAN_MON_IDLE;
            port_transmit_flag[PORT_NO_CAN_0]=0x55;         ;;;����10���Ӿ͸�55˵��������

            RAM_CAN0_Para.CAN_BEGIN_VERICPUCLK_TIME=Time_2048ms_Counter+TIME_CAN_VERIFY_TIME_VALUE+1;     ;;; +15+1
            RAM_CAN0_Para.CAN_BEGIN_POL_YCALL_TIME =Time_2048ms_Counter+TIME_ASK_YC_VALUE+1;
            RAM_CAN0_Para.CAN_BEGIN_POL_YXALL_TIME =Time_2048ms_Counter+TIME_ASK_YX_VALUE+1;
            RAM_CAN0_Para.CAN_BEGIN_POL_YMALL_TIME =Time_2048ms_Counter+TIME_ASK_YM_VALUE+1;
            RAM_CAN0_Para.CAN_BEGIN_POL_DD_AC_TIME =Time_2048ms_Counter+TIME_ASK_DD_VALUE+1;
    
            RAM_CAN0_Para.CAN_NOW_POL_INFO_TYPE=CAN_NOW_POL_NONE;
            RAM_CAN0_Para.CAN_NOW_POL_CPU_MAC_ADDR=MIN_CAN0_ADDR;
            REG_CAN0_Proc.CAN_PROHIBITPOLLADDR_FROM_COMx =0xff;
            
            CAN_Not_Idle_Cn=0;          ;;;��ͷ��ʼ��ʲô��

            Rcd_Info_Myself_Tmp[0]=0;                                         // reserved
            Rcd_Info_Myself_Tmp[1]=RCD_INFO_MYSELF_AREA0_CAN;                 // PORT NO.        ;;;��¼�ĸ��˿ں�  Ϊ 12 CAN0
            Rcd_Info_Myself_Tmp[2]=0;                                         // UNIT ADDR
            Rcd_Info_Myself_Tmp[3]=RCD_INFO_MYSELF_AREA2_PORT_STA_RET_IDLE;   // do   what     ;;;01
            Rcd_Info_Myself_Tmp[4]=0;
            Rcd_Info_Myself_Tmp[5]=0;    
            Rcd_Info_Myself_Tmp[6]=0;  
            Rcd_Info_Myself_Tmp[7]=0; 
                        
            Store_Rcd_Info_Myself();
        }
    }
    
    CAN_Time2048ms_Last_Value=temp_int;        ;;;��¼ԭʼֵ�жϱ仯
    
    if(CAN_246_Trans_YK_Buf[0]==CAN_246_TRANS_YK_STA_WAIT_VERIFY)      ;;;01
    {
        if(Judge_LongTime_In_MainLoop(CAN_246_YK_Begin_WAIT_VERIFY,15)==YES)        ;;;�ж��Ƿ񳬹���15*2048��ʱ�� 30S
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
    }
    
    if(CAN_246_Trans_YK_Buf[0]==CAN_246_TRANS_YK_STA_WAIT_CONFIRM)
    {
        if(Judge_LongTime_In_MainLoop(CAN_246_YK_Begin_WAIT_CONFIRM,30)==YES)      ;;;60S
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
    }
    
    if(CAN_246_Trans_YK_Buf[0]==CAN_246_TRANS_YK_STA_TRANSING_TO_CAN)
    {
        if((BYTE)(CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr+1)==   // Que_len=256
            CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr     )
        {
            CAN0_CPUASKCMU_Trans_Buf_Tak_Ptr++;
        }
                                    
        the_ram_addr_byte=(far BYTE *)&CAN0_CPUASKCMU_Trans_Buf[0]
                         +CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr*12;
        the_ram_addr_byte[0x00]=CAN_246_Trans_YK_Buf[1]      ;  // CAN246_ADDR
        the_ram_addr_byte[0x01]=0                            ;  // frame No
        the_ram_addr_byte[0x02]=CAN_246_Trans_YK_Buf[6]      ;  // CAN243_ADDR
        the_ram_addr_byte[0x03]=CAN_SHORT_FRAME              ;
        the_ram_addr_byte[0x04]=0                            ;  // S
        the_ram_addr_byte[0x05]=CODE_CAN_YK_CHOOSE           ;  // CMD
        if((CAN_246_Trans_YK_Buf[4]==CORE_CODE2_YK_CLOSE)||(CAN_246_Trans_YK_Buf[4]==CORE_CODE2_YK_TRIP))
        {
        	the_ram_addr_byte[0x06]=CAN_246_Trans_YK_Buf[4];
        	the_ram_addr_byte[0x0a]=0x06;
        }
        else
        {
        	the_ram_addr_byte[0x06]=0xff;
        	the_ram_addr_byte[0x0a]=0x00;
        }
        
        the_ram_addr_byte[0x07]=CAN_246_Trans_YK_Buf[7];  // 
        the_ram_addr_byte[0x08]=the_ram_addr_byte[0x06];  // 
        the_ram_addr_byte[0x09]=the_ram_addr_byte[0x07];  // reserved
        the_ram_addr_byte[0x0b]=the_ram_addr_byte[0x0a];  // 
                                
        CAN0_CPUASKCMU_Trans_Buf_Sav_Ptr++;
        
        CAN_246_YK_Begin_WAIT_CONFIRM=Time_2048ms_Counter;           ;;;��Чʱ���ʱ��ʼ
        CAN_246_Trans_YK_Buf[0]=CAN_246_TRANS_YK_STA_WAIT_CONFIRM;   ;;;02
    }
}















/************************************************/
/* CAN0_Monitor     function                    */
/************************************************/
/*===========================================================================*/
void CAN0_Monitor(void)
{
    if(((SIO_CAN_Need_Reset & 0x1000)!=0) || ((*(far BYTE *)(CAN0_BaseAddr+CAN_MODE) & 0x01)!=0) 
    ||((*(far BYTE *)(CAN0_BaseAddr+CAN_STATUS) & 0x80)!=0))
    {
        disable();
        Initial_CAN(0);
        enable();
        SIO_CAN_Need_Reset=SIO_CAN_Need_Reset & 0xefff;	//-���������λ12Ӧ��Ϊ0
        (*((far BYTE *)(SOFT_ERR_FLAG+0x00b0)))++;
    }
 
    
    ;;;��Ȼ����������˰� ����CAN����,����Ӧ����ȫ��ͬ
 
 
    if(((SIO_CAN_Need_Reset & 0x2000)!=0) || ((*(far BYTE *)(CAN1_BaseAddr+CAN_MODE) & 0x01)!=0) 
    ||((*(far BYTE *)(CAN1_BaseAddr+CAN_STATUS) & 0x80)!=0))
    {
        disable();
        Initial_CAN(1);
        enable();
        SIO_CAN_Need_Reset=SIO_CAN_Need_Reset & 0xdfff;
        (*((far BYTE *)(SOFT_ERR_FLAG+0x00b1)))++;
    }
}



