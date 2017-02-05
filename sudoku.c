/* 说明：数独
 * 作者：lwj786
 * 日期：2017年 01月 31日 星期二 19:41:35 CST
 */

#include <stdio.h>

#include "str.h"

/* 用法帮助*/
void sudoku_help()
{
    ;
}

/* 参数处理*/
int process_sudoku_argv(int argc, char *argv[])
{
    int i;

    for (i = 0; i < argc; ++i) {
        if (cmp_str(argv[i], "-h")) {
            sudoku_help();
            return 1;
        }
    }

    return 0;
}

int sudoku_main(int argc, char *argv[])
{
    process_sudoku_argv(--argc, ++argv);

    return 0;
}
