/* 说明：一些字符串处理函数
 * 作者：lwj786
 * 日期：2016年 11月 27日 星期日 14:26:00 CST
 */

#include "publib.h"

/* 获取字符长度*/
int get_str_len(char *str)
{
    int length = 0;

    for (; str[length] != 0; ++length)
        ;

    return length;
}

/* 字符串部分比对*/
int parts_cmp(char *source, char *target)
{
    int i = 0;

    for (; target[i] != 0; ++i)
        if (source[i] != target[i]) return 0;

    return 1;
}

/* 字符串比较*/
int cmp_str(char *source, char *target)
{
    if (get_str_len(source) == get_str_len(target)
        && parts_cmp(source, target))
        return 1;

    return 0;
}

#define _e 2.718281828
#define _PI 3.141592654

/* 字符转数字函数*/
double str2num(char *str)
{
    double integer_part = 0.0, decimal_part = 0.0;
    int flag = 1;

    if (*str == '-') {
        flag = -1;
        ++str;
    }

    /* 字符常量处理*/
    if (*str == 'e') {
        integer_part = _e;
        ++str;
    } else if (*str == 'p') {
        integer_part = _PI;
        str += 2;
    }

    /* 整数部分处理*/
    while (*str != '.' && *str != 0)
        if (*str >= 48 && *str < 58)
            integer_part = integer_part * 10 + (*str++ - 48);

    /* 小数部分处理*/
    if (*str != 0) {
        while (*++str != 0)
            ;
        while (*--str != '.')    //小数部分由'\0'向'.'移动转换
            if (*str >= 48 && *str < 58)
                decimal_part = decimal_part * 0.1 + (*str - 48) * 0.1;
    }

    return flag * (integer_part + decimal_part);
}

/* 索引
 * retrun: 字符的第一个索引或-1
 */
int index_of(char *str, char c)
{
    int i = 0, index = -1;

    for (;str[i] != 0;)
        if (str[i++] == c) {
            index = --i;
            break;
        }

    return index;
}

/* 确认 默认为否*/
int comfirm(char *hint)
{
    int c;

    printf("%s", hint);
    if ((c = getchar()) == 'y') {
        /* 清空缓冲区*/
        while (getchar() != '\n')
            ;

        return 1;
    } else {
        /* 输入为'\n'时不必清空缓冲区*/
        if (c != '\n') {
            while (getchar() != '\n')
                ;
        }

        return 0;
    }
}

/* 由标准输入构造命令*/
void structure_parameter(char *content_of_input, struct parameter_struct *p_parameter)
{
    int i, parameter_count;

    /* 参数计数 分配空间*/
    if (*content_of_input != ' ' && *content_of_input != 0) ++p_parameter->count;
    for (i = 0; content_of_input[i] != 0; ++i) {
        if (content_of_input[i] == ' '
            && (content_of_input[i + 1] !=' ' && content_of_input[i + 1] != 0)
            )
            ++p_parameter->count;
    }
    p_parameter->vector = (char **)malloc(sizeof(char *) * p_parameter->count);

    /* 构造参数向量*/
    parameter_count = p_parameter->count;
    for (--i; i >= 0; --i) {
        if (content_of_input[i] == ' ')
            content_of_input[i] = 0;
        if (content_of_input[i] == 0 && content_of_input[i + 1] != 0)
            p_parameter->vector[--parameter_count] = content_of_input + (i + 1);
    }
    if (*content_of_input != ' ' && *content_of_input != 0)
        p_parameter->vector[0] = content_of_input;
}

/* check_coordinate() 检查坐标
 * get_coordinate() 获取坐标
 * check_coordinate_format() 检查坐标格式, 模式：STRICT 严格 x,y, LOOSE 宽松 define x <=> x,x
 */
int check_coordinate(int x, int y, int x_max, int y_max)
{
    if ((x < 0 || x > x_max)
        || (y < 0 || y > y_max))
        return 0;

    return 1;
}

void get_coordinate(char *coordinate, int *p_x, int *p_y)
{
    int i;

    for (i = 0; coordinate[i] != 0; ++i)
        if (coordinate[i] == ',') break;

    if (coordinate[i] == 0) {
        *p_x = *p_y = str2num(coordinate);
    } else {
        coordinate[i] = 0;
        *p_x = str2num(coordinate), *p_y = str2num(coordinate + i + 1);
    }

}

int check_coordinate_format(char *coordinate, int mode)
{
    int i, count;

    for (i = 0, count = 0; coordinate[i] != 0; ++i) {
        if (coordinate[i] == ',') ++count;

        if (!(coordinate[i] >= '0' && coordinate[i] <= '9')
            && coordinate[i] != ',')
            return 0;
        if (coordinate[i] == ','
            && (i == 0 || coordinate[i + 1] == 0))
            return 0;
    }

    if ((! count) && mode == STRICT) return 0;

    return 1;
}
