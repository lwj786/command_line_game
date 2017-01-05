/* 说明：一些字符串处理函数
 * 作者：lwj786
 * 日期：2016年 11月 27日 星期日 14:26:00 CST
 */

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
            index = i;
            break;
        }

    return index;
}
