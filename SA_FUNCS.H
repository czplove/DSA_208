/*
 * @(#)sa_funcs.h	1.1
 * Special functions for 80C296-SA and compatible processors.
 */
#ifndef _sa_funcs_h
#define _sa_funcs_h
       ;;;������Щ�����Ѿ���д���� ֻҪ��ȷ�����Ϳ��Ե���,������ô������Ҫ�ر�ע��
void            enable(void);                ;;;���⺯��
void            disable(void);

void            enable_pts(void);                           /* dummy function */
void            disable_pts(void);                          /* dummy function */

void            power_down(void);
void            idle(void);           ;;;����ʡ�緽ʽ

#endif  /* _sa_funcs_h */