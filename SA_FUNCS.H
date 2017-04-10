/*
 * @(#)sa_funcs.h	1.1
 * Special functions for 80C296-SA and compatible processors.
 */
#ifndef _sa_funcs_h
#define _sa_funcs_h
       ;;;以下这些函数已经编写好了 只要正确声明就可以调用,具体怎么样不需要特别注意
void            enable(void);                ;;;特殊函数
void            disable(void);

void            enable_pts(void);                           /* dummy function */
void            disable_pts(void);                          /* dummy function */

void            power_down(void);
void            idle(void);           ;;;进入省电方式

#endif  /* _sa_funcs_h */
