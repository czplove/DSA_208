

asm196  cstart.a96    md(sa-ef) cmain ;;;这条汇编指令可以对CSTART.A96进行编译生成目标文件

c196    reg.c         md(sa-ef) code xref ms ;;;编译的东西最后不一定都有作用
c196    ram.c         md(sa-ef) code xref ms
c196    rom.c         md(sa-ef) code xref ms
        文件名         选择处理器指令集     在列表文件中产生汇编代码列表   在列表文件中产生交叉符号参数列表    产生混合汇编源文件
c196    main.c        md(sa-ef) code xref ms
c196    int.c         md(sa-ef) code xref ms ip(0xfb)
c196    port.c        md(sa-ef) code xref ms 
c196    core.c        md(sa-ef) code xref ms 
c196    comuse.c      md(sa-ef) code xref ms 
c196    protocol.c    md(sa-ef) code xref ms 

c196    cdt.c         md(sa-ef) code xref ms 
c196    cdt9702.c     md(sa-ef) code xref ms 
c196    hostzf.c      md(sa-ef) code xref ms 
c196    set.c         md(sa-ef) code xref ms
c196    bus.c         md(sa-ef) code xref ms
c196    can.c         md(sa-ef) code xref ms
c196    zlp_cdt.c     md(sa-ef) code xref ms 
c196    rec_host.c    md(sa-ef) code xref ms
c196    DS_XXQ.c      md(sa-ef) code xref ms
c196    bjsf103.c     md(sa-ef) code xref ms
c196    ddb_bb.c      md(sa-ef) code xref ms 
c196    ddb_ws.c      md(sa-ef) code xref ms 
c196    XDL_MLX.c     md(sa-ef) code xref ms
c196    zpjk.c        md(sa-ef) code xref ms
c196    zlp_zwk.c     md(sa-ef) code xref ms 
c196    zlp_js.c      md(sa-ef) code xref ms 

rl196 -f alo

;;;上句的作用可能就是跳入ALO中去执行那些命令,如下
;-----------------------------------------------------
cstart.obj     main.obj     reg.obj     ram.obj       int.obj 
port.obj       core.obj     comuse.obj  protocol.obj  rec_host.obj  cdt.obj
hostzf.obj     set.obj      zlp_js.obj  bus.obj       can.obj       ds_xxq.obj     XDL_MLX.obj    zpjk.obj      
ddb_ws.obj     rom.obj      zlp_zwk.obj ddb_bb.obj    bjsf103.obj   CDT9702.obj    ZLP_CDT.obj   ;;;这些都是进过编译之后可以直接连接用的文件,例如有规约在此(不用的可删)

c96.lib to main.abs 
md(sa-ef) bu noit 
ram(0002000H-000f7ffH,000f800H-000ffffH(stack),0010000H-003ffffH,0f40000H-0f6ffffH)                                                &
rom(0f80000H-0f9ffffH,0fa0000H-0faffffH(cdt,hostzf,can),0fb0800H-0fbffffH(main,int,port,core,comuse,set,protocol,bus))
ss(2048) sfr noregoverlay
;-----------------------------------------------------

oh196 main.abs        ;;;转化成HEX文件




rem c196    printf.c    md(sa-ef) code xref ms   ;;;这句这个文件中没有用到,但有机会要解决它的特殊性