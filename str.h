/* 说明：一些字符串处理函数
 * 作者：lwj786
 * 日期：2016年 11月 27日 星期日 14:27:12 CST
 */

#ifndef _STR_H_
#define _STR_H_

/* 获取字符长度*/
int get_str_len(char *str);

/* 字符部分比对*/
int parts_cmp(char *str, char *cmp);

/* 字符串比较*/
int cmp_str(char *source, char *target);

/* 字符转数字函数*/
double str2num(char *str);

/* 索引
 * retrun: 字符的第一个索引或-1
 */
int index_of(char *str, char c);

#endif
