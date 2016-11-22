/* 说明：主要负责匹配游戏并向其传入参数
 * 作者：lwj786
 * 日期：2016年 11月 21日 星期一 21:54:03 CST
 */

#include <stdio.h>
#define GAMES_NUM 0

struct
argument_struct{
    char *name;
    int (*method)(int argc, char *argv[]);
};

/* 用法帮助*/
int help(int argc, char *argv[])
{
    printf("用法：[command] {option} 或 [command] [game name] {option}\n");
    printf("主函数参数：\n");
    printf("\t -h, --help 用法帮助\n");

    return 0;
}

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

int main(int argc, char *argv[])
{
    struct argument_struct arg[GAMES_NUM + 2] = {
        {"-h", &help}, {"--help", &help}
    };
    int i, count;

    --argc, ++argv;    //略过命令名
    for (count = 0; argc > 0; --argc, ++argv) {
        for (i = 0; i < GAMES_NUM + 2; ++i) {
            if (parts_cmp(*argv, arg[i].name))
                (*arg[i].method)(argc, argv), ++count;
        }

        if (!count) printf("无此参数：%s\n", *argv);
        else count = 0;
    }

    return 0;
}
