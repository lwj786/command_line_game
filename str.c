/* 说明：一些字符串处理函数
 * 作者：lwj786
 * 日期：2016年 11月 27日 星期日 14:26:00 CST
 */

/* 字符部分比对
 * parameter: str 需比较的字符串部分（的起始位置）, cmp 比对项
 * return: rt true 1 or flase 0
 */
int parts_cmp(char *str, char *cmp)
{
    int i = 0,rt = 0;

    for (;; ++i) {
        if (cmp[i] == 0)    //能执行到cmp结束，说明之前都相同
            break;
        else if (str[i] == cmp[i])
            rt = 1;
        else
            rt = 0;

        if (rt == 0) break;
    }

    return rt;
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
            index = i;
            break;
        }

    return index;
}
