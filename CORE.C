/*****************************************************************************/
/*       FileName  :   CORE.C                                                */
/*       Content   :   DSA-208 CORE Module                                   */
/*       Date      :   Fri  02-22-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/









/************************************************/
/* Core_ObjectNo_To_UnitAddr      function      */
/************************************************/
/*===========================================================================*/
BYTE Ex_YK_CDTBB_ObjectNo_To_UnitAddr(void)
{
  unsigned short temp_rnd,temp_rnd1;

  for(temp_rnd=0;temp_rnd<14;temp_rnd++)
  {
      if(  (exchange_buf[5] >= Ex_YK_start[temp_rnd])
         &&(exchange_buf[5] <= Ex_YK_end[temp_rnd]) )
      {
          for(temp_rnd1  = byte0(port_info[temp_rnd].mirror_unit);
              temp_rnd1 <= byte1(port_info[temp_rnd].mirror_unit);
              temp_rnd1++ )
          {
              if(   (exchange_buf[5] >= unit_info[temp_rnd1].yk_start)
                  &&(exchange_buf[5] < (unit_info[temp_rnd1].yk_start+unit_info[temp_rnd1].yk_num))
                )
              {
                  exchange_buf[17] = exchange_buf[5] - unit_info[temp_rnd1].yk_start;     // yk_no in unit
                  exchange_buf[5]  = temp_rnd1-byte0(port_info[temp_rnd].mirror_unit);    // unit addr
                  exchange_target_port=temp_rnd;
                  return YES;
              }
          }
      }
  }
  
  return NO;
}










/************************************************/
/* Ex_Produce_Transmit_Info     function        */
/* Input :    exchange_buf[xxx]                 */
/* Output:    port_transmit[port][xxx]          */
/************************************************/
/*===========================================================================*/
BYTE Ex_Produce_Transmit_Info(void)      ;;;交换_制作_转送_信息
{
    unsigned short temp_rnd;
    unsigned short temp_rnd1;
    unsigned char *temp_pt;

    if(exchange_target_port<14)             ;;;交换的目的口只有14个
    {
        if(port_transmit_flag[exchange_target_port] == 0x55)
        {
            temp_pt = &port_transmit[exchange_target_port][0];         ;;;这个里面保存需要记录的数值
            for(temp_rnd=1;temp_rnd<16;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];
            }

            byte0(temp_rnd1)=exchange_buf[9];
            byte1(temp_rnd1)=exchange_buf[10];
            temp_rnd1=temp_rnd1+16;
            for(temp_rnd=16;temp_rnd<temp_rnd1;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];          ;;;进一步处理报文
            }
            port_transmit_flag[exchange_target_port] = 0xaa;   ;;;接收了新内容就给这个吗
            return YES;
        }   
        else
            return NO;
    }
    else
    {
        if(set_transmit_flag == 0x55)
        {
            temp_pt = &set_transmit[0]; 
            for(temp_rnd=1;temp_rnd<16;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];
            }

            byte0(temp_rnd1)=exchange_buf[9];
            byte1(temp_rnd1)=exchange_buf[10];
            if(temp_rnd1>250) 
                return NO;
            temp_rnd1=temp_rnd1+16;
            for(temp_rnd=16;temp_rnd<temp_rnd1;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];
            }
            set_transmit_flag = 0xaa;
            return YES;
        }   
        else
            return NO;
    }
}











void Ex_scan_yk_range_for_port()
{
  unsigned short temp_rnd,temp_rnd1;
  unsigned char  the_start_flag[14],the_end_flag[14];
  
  for(temp_rnd=0;temp_rnd<14;temp_rnd++)
  { 
     the_start_flag[temp_rnd] = 0;
     the_end_flag[temp_rnd]   = 0;
     
     Ex_YK_start[temp_rnd]   = 1;
     Ex_YK_end[temp_rnd]     = 0;
  }

  for(temp_rnd=0;temp_rnd<256;temp_rnd++)	//-总共256个单元可以定义,这里是第一次对定义的单元进行处理
  {
      if(unit_info[temp_rnd].yk_num!=0)	//-没有配置为0说明有遥控定义吗
      {
          for(temp_rnd1=0;temp_rnd1<14;temp_rnd1++)
          {
              if(  (temp_rnd>=byte0(port_info[temp_rnd1].mirror_unit))		//-开始单元号
                 &&(temp_rnd<=byte1(port_info[temp_rnd1].mirror_unit)) )	//-结束单元号,,检查有遥控的单元是否在通讯口配置范围内
              {		//-每一个单元只会对应一个通讯口,而且是事先配置好的,特定的通讯口规约也是配置死的
                  if(the_start_flag[temp_rnd1]==0)
                  {
                      the_start_flag[temp_rnd1]=0xff;	//-ff说明这个通讯口有遥控命令
                      Ex_YK_start[temp_rnd1]=unit_info[temp_rnd].yk_start;
                  }
                  
                  if(unit_info[temp_rnd].yk_start<Ex_YK_start[temp_rnd1])
                  {
                      Ex_YK_start[temp_rnd1]=unit_info[temp_rnd].yk_start;	//-记录的是接在这个通讯口上所有单元遥控开始最小的值
                  }

                  if(the_end_flag[temp_rnd1]==0)
                  {
                      the_end_flag[temp_rnd1]=0xff;
                      Ex_YK_end[temp_rnd1] = unit_info[temp_rnd].yk_start + unit_info[temp_rnd].yk_num-1;
                  }
                  if((unit_info[temp_rnd].yk_start+unit_info[temp_rnd].yk_num)>Ex_YK_end[temp_rnd1]) 
                      Ex_YK_end[temp_rnd1] = unit_info[temp_rnd].yk_start + unit_info[temp_rnd].yk_num-1; 	//-记录的是本通讯口上所有单元遥控号最大的那个   
              }
          }
      }
  }
}

void core_init()
{
  yx_chg_in_pt = 0;
  yx_soe_in_pt = 0;
  for(temp_loop=0;temp_loop<4;temp_loop++)
  {
     yx_chg_tr_ex_pt[temp_loop] = 0;	//-对于很多计数单元初始期间都是0
     yx_soe_tr_ex_pt[temp_loop] = 0;
     yx_chg_tr_in_pt[temp_loop] = 0;
     yx_soe_tr_in_pt[temp_loop] = 0;
  }

  BH_Report_Bank_Sav_Ptr=0;
  
  for(temp_loop=0;temp_loop<12;temp_loop++)
  {
      BH_Report_Bank_Tak_Ptr[temp_loop]=0;
  } 
}

void Ex_init()
{
  unsigned char temp_rnd;

  core_init();
  Ex_scan_yk_range_for_port();

  for(temp_rnd=0;temp_rnd<14;temp_rnd++)
  {
     port_transmit_flag[temp_rnd] = 0x55;	//-端口发送标志初始状态为55,,呵呵搞这么多数组做标志位,够可以的
     port_transmit[temp_rnd][0]   = PORT_EXCHANGE_STA_IDLE;		//-空闲的
  }  
}


//应用 core  前提已释放 temp_loop,temp_loop1,temp_int,temp_lp_int 

void core_get_bh_bank_report()
{
	BYTE need_refind;
	
    if(Core_Src_Unit>=BH_REPORT_BANK_SIZE)  Core_Src_Unit=0;

    Core_Src_Pt_B=&BH_Saved_Report_Bank[Core_Src_Unit][0];
    need_refind=NO;
re_find:
    while((Core_Src_Pt_B[4]!=( (Core_Src_Pt_B[0]+Core_Src_Pt_B[1]+Core_Src_Pt_B[2]+Core_Src_Pt_B[3])&0xff ))
        ||(Core_Src_Pt_B[2]!=0)||(need_refind==YES))
    {
    	need_refind=NO;
        Core_Src_Unit++;
        if(Core_Src_Unit>=BH_REPORT_BANK_SIZE) Core_Src_Unit=0;
        if(Core_Src_Unit==BH_Report_Bank_Sav_Ptr)
        {
            Core_Src_Len=0;
            return;
        }
        Core_Src_Pt_B=&BH_Saved_Report_Bank[Core_Src_Unit][0];
    }
    
    byte1(Core_Temp_Loop)=Core_Src_Pt_B[6];
    if(byte1(Core_Temp_Loop)>10) 
    {
    	need_refind=YES;
    	goto re_find;
    }	
    for(byte0(Core_Temp_Loop)=0;byte0(Core_Temp_Loop)<byte1(Core_Temp_Loop);byte0(Core_Temp_Loop)++)
    {
    	if((Core_Src_Pt_B[2]!=byte0(Core_Temp_Loop))||(Core_Src_Pt_B[3]>26)
    	 ||(Core_Src_Pt_B[4]!=( (Core_Src_Pt_B[0]+Core_Src_Pt_B[1]+Core_Src_Pt_B[2]+Core_Src_Pt_B[3])&0xff )))  // frame_NO err
    	{
	    	need_refind=YES;
    		goto re_find;
    	}

		if(byte0(Core_Temp_Loop)==0)
		{
			if(Core_Src_Pt_B[3]>2)
				byte1(Core_Temp_ShortInt)=Core_Src_Pt_B[3]-2;
			else
				byte1(Core_Temp_ShortInt)=0;
			for(byte0(Core_Temp_ShortInt)=0;byte0(Core_Temp_ShortInt)<byte1(Core_Temp_ShortInt);byte0(Core_Temp_ShortInt)++)
			{
				BH_Bank_Report[byte0(Core_Temp_ShortInt)]=Core_Src_Pt_B[byte0(Core_Temp_ShortInt)+8];
			}
			Core_Src_Len=byte1(Core_Temp_ShortInt);
		    BH_Bank_Report[509]=Core_Src_Pt_B[1];
		    BH_Bank_Report[510]=Core_Src_Pt_B[0];
		    BH_Bank_Report[511]=Core_Src_Pt_B[5];
		}
		else
		{
			if((Core_Src_Pt_B[0]!=BH_Bank_Report[510])||(Core_Src_Pt_B[5]!=BH_Bank_Report[511])
			 ||(Core_Src_Pt_B[1]!=BH_Bank_Report[509]))
			{
		    	need_refind=YES;
    			goto re_find;
			}
			
			byte1(Core_Temp_ShortInt)=Core_Src_Pt_B[3];
			for(byte0(Core_Temp_ShortInt)=0;byte0(Core_Temp_ShortInt)<byte1(Core_Temp_ShortInt);byte0(Core_Temp_ShortInt)++)
			{
				BH_Bank_Report[Core_Src_Len+byte0(Core_Temp_ShortInt)]=Core_Src_Pt_B[byte0(Core_Temp_ShortInt)+6];
			}
			Core_Src_Len+=byte1(Core_Temp_ShortInt);
		}	

        Core_Src_Unit++;
        if(Core_Src_Unit>=BH_REPORT_BANK_SIZE) Core_Src_Unit=0;
        Core_Src_Pt_B=&BH_Saved_Report_Bank[Core_Src_Unit][0];
    }
    
}

void core_get_yx_set_unit()
{
  if((Core_Src_Unit>0x1f)&&(Core_Src_Unit<0x30))
     Core_Temp_Loop = 4;
  else
     Core_Temp_Loop = unit_info[Core_Src_Unit].yx_num;	//-还是那个问题,不同单元的性质不同,设置的值也有不同
  //Core_Src_Unit Core_Src_Pt_B
  Core_Temp_ShortInt= unit_info[Core_Src_Unit].yx_start;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Src_Pt_B =  byte0(YX_property[Core_Temp_ShortInt + Core_Temp_Loop1]);	//-这些值都是提前制定好的,要用时调入
      Core_Src_Pt_B ++;
     *Core_Src_Pt_B = byte1(YX_property[Core_Temp_ShortInt + Core_Temp_Loop1]);
      Core_Src_Pt_B ++;    
  }
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Src_Pt_B =  byte0(YX_YM[Core_Temp_ShortInt+Core_Temp_Loop1]);
      Core_Src_Pt_B ++;
     *Core_Src_Pt_B = byte1(YX_YM[Core_Temp_ShortInt+Core_Temp_Loop1]);
      Core_Src_Pt_B ++;    
  }

  Core_Temp_ShortInt = Core_Temp_ShortInt*8;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<(Core_Temp_Loop*8);Core_Temp_Loop1++)
  {
     if((Core_Temp_ShortInt + Core_Temp_Loop1)<256)
     {
        *Core_Src_Pt_B =  byte0(YX_double[Core_Temp_ShortInt + Core_Temp_Loop1]);
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B = byte1(YX_double[Core_Temp_ShortInt + Core_Temp_Loop1]);
         Core_Src_Pt_B ++;    
     }
     else
     {
        *Core_Src_Pt_B =  0xc8;
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B =  0xc8;
         Core_Src_Pt_B ++;    
     }
  }

  Core_Temp_ShortInt = Core_Temp_ShortInt*2;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<(Core_Temp_Loop*16);Core_Temp_Loop1++)
  {
     if((Core_Temp_ShortInt + Core_Temp_Loop1)<512)
     {
        *Core_Src_Pt_B =  byte0(YX_delay[Core_Temp_ShortInt + Core_Temp_Loop1]);
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B = byte1(YX_delay[Core_Temp_ShortInt + Core_Temp_Loop1]);    
         Core_Src_Pt_B ++;
     }
     else
     {
        *Core_Src_Pt_B =  0xc8;
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B =  0x00;    
         Core_Src_Pt_B ++;
     }
  }
}

void core_get_yc_set_unit()
{
  if((Core_Src_Unit>0x0f)&&(Core_Src_Unit<0x20))
      Core_Temp_Loop = 6;
  else
      Core_Temp_Loop = unit_info[Core_Src_Unit].yc_line_num; //-有的范围内长度是做死的,有的是可以修改的

  //Core_Src_Unit Core_Src_Pt_B
  Core_Temp_ShortInt = unit_info[Core_Src_Unit].yc_line_start;  //-数组的值代表一个单元的配置值
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     if(Core_Temp_Loop1<unit_info[Core_Src_Unit].yc_line_num)  //-以字为单位,,双重限制个数,取小的
     {
       *Core_Src_Pt_B =  YC_coef_table[(Core_Temp_ShortInt + Core_Temp_Loop1)*4];
        Core_Src_Pt_B ++;
       *Core_Src_Pt_B =  YC_coef_table[(Core_Temp_ShortInt + Core_Temp_Loop1)*4 + 1];
        Core_Src_Pt_B ++;    
     }
     else
     {
       *Core_Src_Pt_B =  0;
        Core_Src_Pt_B ++;
       *Core_Src_Pt_B =  0;
        Core_Src_Pt_B ++;    
     }
  }
}

void core_update_YC()
{
  Core_Temp_Loop = unit_info[Core_Src_Unit].yc_val_num;  //-这个是事先配置的长度
  Core_Det_Pt = (unsigned short *)&(YC_State[unit_info[Core_Src_Unit].yc_val_start]); //-空间不重要,就是一个数值而已,重要的是偏移量,数据是时时存在的
  if(Core_Temp_Loop>Core_Src_Len)                                                     //-关键是时效性 
     Core_Temp_Loop = Core_Src_Len; //-最大仅仅记录最大数量的数据,如果少了,不管,,多了后面的就舍弃
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Det_Pt   =  (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;   //-把一个分开发送的数据拼接恢复起来
     Core_Src_Pt_B += 2;
     Core_Det_Pt    ++;
  }
    
}

void core_update_DC()
{
  Core_Temp_Loop = unit_info[Core_Src_Unit].dc_num;
  Core_Det_Pt = (unsigned short *)&(YC_State[unit_info[Core_Src_Unit].dc_start]);  //-更新遥测状态的值
  if(Core_Temp_Loop>Core_Src_Len)
     Core_Temp_Loop = Core_Src_Len;  //-见识了这么多的东西,这里仅仅是为了可靠,切记现场什么可能都有
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Det_Pt   =  (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
     Core_Src_Pt_B += 2;
     Core_Det_Pt    ++;
  }
    
}
















/************************************************/
/* Input :  Core_Src_Len                        */  ;;;实际变化的YX有几个B
/*          Core_Src_Pt_B                       */  ;;;port_report[]的首地址,里面记录了YX值
/*          Core_Src_Unit                       */  ;;;单元地址
/* Output:                                      */
/* REG varible used: Core_Det_Pt                */
/*                   Core_Src_Pt                */
/*                   Core_Temp_Loop             */
/*                   Core_Temp_Loop1            */
/*                   Core_Temp_ShortInt         */
/************************************************/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void core_update_YX()         //-保证变化了的都修改一遍就行,但在这之前的有没有变化的也重写一遍,之后的就没有必要了
{
  unsigned short chg_buf0,chg_buf1;
  unsigned short  yx_buf0;

  Core_Det_Pt = (unsigned short *)&(YX_State[unit_info[Core_Src_Unit].yx_start]);   //-本台装置所配遥信开始的地址(每个IED各有一行)

  Core_Temp_Loop   = unit_info[Core_Src_Unit].yx_num;     //-这台装置的遥信数(数的是字)配置的
  Core_Temp_Loop   = Core_Temp_Loop * 2;              	  //-又变成了字节
  if(Core_Temp_Loop>Core_Src_Len)       //-可能由于是更新数据,没有变化的数据就没有必要再写一遍了,不是的,是两者取其小
      Core_Temp_Loop = Core_Src_Len;       //-虽然配置的遥信长度可能很长但是他是以接收到的最大长度为依据的,且给每台装置配地址时不需要连续,只要总数不超过256就行
  Core_Temp_Loop &= 0xfe;  //avoid odd   避免单数 当然如果配置小了就按小的算,多余的丢失,,现在内部的值就是变化的YX数,而不再是配置数了
  
  if(HOST_ZF_enable!=YES)       //-是 主动_再发_使能 的意思吗 ;必备的理论知识是必须有的,这样才能有灵感,?只有可以自发才能更新吗,否则作为无效信息
  {
     Core_Temp_Loop = Core_Temp_Loop/2;
     for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)     //-这个地方可读性就很强,且不要过多考虑硬件
     {
        *Core_Det_Pt = (*(Core_Src_Pt_B+1))*256 + (*Core_Src_Pt_B);        //-把两个字节的内容组合成了一个字的内容进行保存,是YX号和内容
         Core_Det_Pt ++;
         Core_Src_Pt_B +=2;         //-把报表port_report[]中的内容存入YX_State[]中;这个地方就给了这样的数值,刚开始应该是随机的,经过一段时间才正常
     }    
     return;
  }
  
  Core_Src_Len = 0;             //-又作为寄存器在使用
  while(Core_Temp_Loop>0)       //-实际需要更新的长短度B;切忌这个地方是循环语句,每次只处理一种情况
  {
      Core_Src_Pt = &yx_change[yx_chg_in_pt].offset_no;           ;;;取的是这个数组的地址
     *Core_Src_Pt =  unit_info[Core_Src_Unit].yx_start + Core_Src_Len;     ;;;记录哪个遥信发生了变化

      Core_Temp_ShortInt = 0x5555;

      chg_buf0  = 0;
      chg_buf1  = 0;        ;;;难道从这儿可以猜测到一台最多可以有32路YX

      yx_buf0   = (*(Core_Src_Pt_B+1))*256 + (*Core_Src_Pt_B);      ;;;取出一个YX的有效值
      if(yx_buf0 != *Core_Det_Pt)         ;;;比较是否数据有更新,等就是没有更新,刚接收到的YX数据与上一次的数据进行比较,若没有数据更新就跳过去
      {
          Core_Temp_ShortInt = 0xaaaa;        ;;;说明有YX更新
          chg_buf0           = yx_buf0 ^ (*Core_Det_Pt);   ;;;记录下变化YX的数值,这个并不是1就是发生0就是没有,由于是异或所以1表示有变化,0表示没有变化
      }
     *Core_Det_Pt     = yx_buf0;    ;;;若没有更新就把数值再写一下,这种看似无意义的操作实际上就是我一直要找的,第一步向YX_State[]中写数据的地方
      Core_Src_Pt_B   += 2;        ;;;移向port_report[]的下一个数据
      Core_Det_Pt     ++;
      Core_Temp_Loop  -= 2;        ;;;比较了一个就减去一个
      Core_Src_Len    ++;

      if((*Core_Src_Pt & 1) != 0)    ;;;如chg_bit0对应的位值为1，则表示该位YX产生变位
      {
          if(Core_Temp_ShortInt == 0xaaaa)
          {
             *Core_Src_Pt = *Core_Src_Pt / 2;   Core_Src_Pt++;
             *Core_Src_Pt = *(Core_Det_Pt-2);   Core_Src_Pt++;
             *Core_Src_Pt = *(Core_Det_Pt-1);   Core_Src_Pt++;
             *Core_Src_Pt = 0;                  Core_Src_Pt++; 
             *Core_Src_Pt = chg_buf0;
              yx_chg_in_pt ++;
              yx_chg_in_pt &= 0xff;         
              Core_Temp_ShortInt = 0x5555;
          }
      }
      else
      {
          if(Core_Temp_Loop>0)
          {
               yx_buf0 = ((*(Core_Src_Pt_B+1)))*256 + (*Core_Src_Pt_B);       ;;;如果下位还有遥信就取值
               if(yx_buf0 != (*Core_Det_Pt))         ;;;继续看有没有变化等就是没有变化
               {
                   Core_Temp_ShortInt = 0xaaaa;
                   chg_buf1           = yx_buf0 ^ (*Core_Det_Pt);    ;;;按照常理这个地方应该是yx_buf0
               }
              *Core_Det_Pt     = yx_buf0;
               Core_Src_Pt_B   = Core_Src_Pt_B + 2;
               Core_Temp_Loop -= 2;
               Core_Src_Len    ++;
          }

          Core_Det_Pt++;         ;;;由于它是固定给32路遥信的,所以没有也给0

          if(Core_Temp_ShortInt == 0xaaaa)     ;;;判断是否有变化,等表示有
          {                                                     ;;;是对yx_change[]数组填写内容;内容除以2,并把指针移向下一位                             
             *Core_Src_Pt = *Core_Src_Pt / 2;   Core_Src_Pt++; ;;;确定变位YX的偏移量        
             *Core_Src_Pt = *(Core_Det_Pt-2);   Core_Src_Pt++; ;;;变位YX帧的YX值，每帧2个字，WORD0
             *Core_Src_Pt = *(Core_Det_Pt-1);   Core_Src_Pt++; ;;;变位YX帧的YX值，每帧2个字，WORD1   记录的是上一次的YX值
             *Core_Src_Pt = chg_buf0;           Core_Src_Pt++; ;;;如chg_bit0对应的位值为1，则表示该位YX产生变位
             *Core_Src_Pt = chg_buf1;
              yx_chg_in_pt ++;             ;;;自动累加寄存器用于保存变位YX帧的个数
              yx_chg_in_pt &= 0xff;        ;;;确保循环 
              Core_Temp_ShortInt = 0x5555; ;;;都记录下来了就没有必要再重复记录了就给5555
          }
      }
  }
}























void core_insert_SOECOS()
{
   // before insert SOE & COS, must fill YX_State with New YX value to generate COS
   
   Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;
  //ms
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //sec min
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //hour day
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  // channel state

   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Temp_ShortInt=((*Core_Det_Pt) & 0x87ff) + (unit_info[Core_Src_Unit].yx_start*16 & 0x0fff);
   *Core_Det_Pt = Core_Temp_ShortInt;

   yx_soe_in_pt++;
   if(yx_soe_in_pt>1023)
     yx_soe_in_pt = 0;

   Core_Temp_ShortInt&=0x0fff;
   Core_Temp_Loop=Core_Temp_ShortInt/16;
   Core_Det_Pt = (WORD *)&YX_State[Core_Temp_Loop];
   Core_Src_Pt = &yx_change[yx_chg_in_pt].offset_no;
  *Core_Src_Pt = Core_Temp_Loop/2;           Core_Src_Pt++; // offset
   if(((Core_Temp_Loop)& 0x01)==0)
   {
       *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX0
       *Core_Src_Pt = *(Core_Det_Pt+0x01);   Core_Src_Pt++; // YX1
       *Core_Src_Pt = (WORD)(1<<(Core_Temp_ShortInt%16));    Core_Src_Pt++;   // CHG0
       *Core_Src_Pt = 0x00;                                 // CHG1
   } 
   else
   {
       *Core_Src_Pt = *(Core_Det_Pt-0x01);   Core_Src_Pt++; // YX0
       *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX1
       *Core_Src_Pt = 0x00;                  Core_Src_Pt++; // CHG0
       *Core_Src_Pt = (WORD)(1<<(Core_Temp_ShortInt%16));   // CHG1
   }
   
   yx_chg_in_pt ++;
   yx_chg_in_pt &= 0xff;         
}

void core_insert_SOE()
{
   Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;
  //ms
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //sec min
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //hour day
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  // channel state

   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   *Core_Det_Pt = ((*Core_Det_Pt) & 0x87ff) + (unit_info[Core_Src_Unit].yx_start*16 & 0x0fff);

   yx_soe_in_pt++;
   if(yx_soe_in_pt>1023)		//-就是对应的空间记录接收到的IED上送的值
     yx_soe_in_pt = 0;
}

void core_update_YM()
{
  Core_Temp_Loop = unit_info[Core_Src_Unit].ym_num;		//-很有可能这些实际功能都被精简丢了哦
  if(*Core_Src_Pt_B>Core_Temp_Loop)			//-现在是很痛苦的过程,甚至还有很长一段时间的痛苦阶段,必须坚持到最后,别忘了自己的目标
     return;
  
  Core_Det_Pt = (unsigned short *)&(YM_State[unit_info[Core_Src_Unit].ym_start+(*Core_Src_Pt_B)]);
  if(Core_Temp_Loop<(Core_Src_Len+(*Core_Src_Pt_B)))
     Core_Temp_Loop = Core_Temp_Loop - *Core_Src_Pt_B;
  else
     Core_Temp_Loop = Core_Src_Len;

  Core_Temp_Loop = Core_Temp_Loop*2;
  Core_Src_Pt_B  ++;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Det_Pt    = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
      Core_Src_Pt_B += 2;
      Core_Det_Pt   ++;
  }
    
}
