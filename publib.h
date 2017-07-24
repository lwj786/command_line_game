/* 说明：一些字符串处理函数
 * 作者：lwj786
 * 日期：2016年 11月 27日 星期日 14:27:12 CST
 */

#ifndef _PUBLIB_H_
#define _PUBLIB_H_

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

/* 确认 默认为否*/
int comfirm(char *hint);

/* 由标准输入构造命令*/
struct
parameter_struct {
    int count;
    char **vector;
};

void structure_parameter(char *content_of_input, struct parameter_struct *p_parameter);

/* check_coordinate() 检查坐标
 * get_coordinate() 获取坐标
 * check_coordinate_format() 检查坐标格式, 模式：STRICT 严格 x,y, LOOSE 宽松 define x <=> x,x
 */
int check_coordinate(int x, int y, int x_max, int y_max);

void get_coordinate(char *coordinate, int *p_x, int *p_y);

#define STRICT 0
#define LOOSE 1
int check_coordinate_format(char *coordinate, int mode);

#endif
