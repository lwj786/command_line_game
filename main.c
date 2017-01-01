/* 说明：主要负责匹配游戏并向其传入参数
 * 作者：lwj786
 * 日期：2016年 11月 21日 星期一 21:54:03 CST
 */

#include <stdio.h>

#include "str.h"
#include "game.h"

#define GAMES_NUM 1

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
    printf("游戏：mine 扫雷\n");
    printf("使用[command] [game name] --help | -h 查看帮助\n");

    return 0;
}

int main(int argc, char *argv[])
{
    struct argument_struct arg[GAMES_NUM + 2] = {
        {"-h", &help}, {"--help", &help},
        {"mine", &mine_main}
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
