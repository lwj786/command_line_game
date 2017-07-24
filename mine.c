/* 说明：扫雷
 * 作者：lwj786
 * 日期：2016年 11月 23日 星期三 21:46:07 CST
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "publib.h"

#define MINE 1

#define QUIT -1
#define RESTART 1

#define DEFEAT -1
#define VICTORY 1

#define ENABLE 1
#define DISABLE 0

struct record_struct {
    int trigger_x, trigger_y,
        result;
    time_t time_of_use;
};

struct
minefield_struct {
    int length, width, num_of_mines,
        **implicit;
    char **explicit;

    int game_count;
    int color_mode;

    struct record_struct *p_game_record;
};

/* 释放雷区*/
void free_minefield(struct minefield_struct *p_minefield)
{
    int i;

    for (i = 0; i < p_minefield->width; ++i)
        free(p_minefield->implicit[i]);
    free(p_minefield->explicit);

    for (i = 0; i < p_minefield->width; ++i)
        free(p_minefield->implicit[i]);
    free(p_minefield->implicit);

    p_minefield->explicit = NULL, p_minefield->implicit = NULL;
}

/* 分配雷区*/
void allocate_minefield(struct minefield_struct *p_minefield)
{
    int i;

    p_minefield->explicit = (char **)malloc(sizeof(char *) * p_minefield->width);
    for (i = 0; i < p_minefield->width; ++i)
        p_minefield->explicit[i] = (char *)malloc(sizeof(char) * p_minefield->length);

    p_minefield->implicit = (int **)malloc(sizeof(int *) * p_minefield->width);
    for (i = 0; i < p_minefield->width; ++i)
        p_minefield->implicit[i] = (int *)malloc(sizeof(int) * p_minefield->length);
}

/* 重设*/
void reset(struct minefield_struct *p_minefield)
{
    int process_argv(int argc, char *argv[], struct minefield_struct *p_minefield);

    int i;
    char content_of_input[BUFSIZ] = {0};
    struct parameter_struct parameter = {0, NULL};

    if (comfirm("重设(y/n): ")) {
        /* 由stdin获取输入*/
        printf("输入命令: ");
        fgets(content_of_input, BUFSIZ, stdin);
        for (i = 0;; ++i)
            if (content_of_input[i] == '\n') {
                content_of_input[i] = 0;
                break;
            }

        structure_parameter(content_of_input, &parameter);

        process_argv(parameter.count, parameter.vector, p_minefield);
        free_minefield(p_minefield), allocate_minefield(p_minefield);

        free(parameter.vector);
    }
}

/* judge() 胜负判断*/
int judge(struct minefield_struct *p_minefield)
{
    int x, y, rest_of_blocks = 0;

    for (x = 0; x < p_minefield->length; ++x)
        for (y = 0; y < p_minefield->width; ++y) {
            if (p_minefield->explicit[y][x] == '*') {
                /* 地雷显式化*/
                for (x = 0; x < p_minefield->length; ++x)
                    for (y = 0; y < p_minefield->width; ++y) {
                        if (p_minefield->implicit[y][x] == MINE)
                            p_minefield->explicit[y][x] = '*';
                    }

                return DEFEAT;
            } else if (p_minefield->explicit[y][x] == 'F'
                       || p_minefield->explicit[y][x] == '#') {
                ++rest_of_blocks;
            }
        }

    return (rest_of_blocks == p_minefield->num_of_mines) ? VICTORY : 0;
}

/* round_operation() 回合操作
 * 注意：坐标与偏移转换；坐标与数组下标有转置关系
 * 辅助函数：
 * operation_help() 操作帮助    comfirm_restart() 重开游戏    comfirm_quit() 退出游戏
 * detect_block() 探测          mark_block() 标记             remove_mark() 移除标记 
 */
int operation_help(int x, int y, struct minefield_struct *p_minefield)
{
    void mine_help(int argc, char *argv[]);

    mine_help(0, NULL);

    return 0;
}

int comfirm_restart(int x, int y, struct minefield_struct *p_minefield)
{
    if (comfirm("确定重开(y/n): ")) return RESTART;
    else return 0;
}

int comfirm_quit(int x, int y, struct minefield_struct *p_minefield)
{
    if (comfirm("确定退出(y/n): ")) return QUIT;
    else return 0;
}

/* detect_block()的辅助函数
 *  setup_range() 设定范围（并将坐标量转为偏移量）
 * has_block_which() （周围）是否有某类区块
 * has_unknown_block() （周围）是否有不明区块（即未检查）
 * has_blank_block() （周围）是否有空白区块（即周围无雷）
 * surrounding_mines() 周围的地雷（数目）
 * expand_and_detect() 扩大（范围）并检查
 */
void setup_range(int x, int *x_min, int *x_max,
                 int y, int *y_min, int *y_max,
                 int len_of_range,
                 int length, int width)
{
    /* 注意边界： 1 ~ length/width*/
    *x_min = (x - len_of_range > 1) ? (x - len_of_range) : 1,
    *x_max = (x + len_of_range < length) ? (x + len_of_range) : length;
    *y_min = (y - len_of_range > 1) ? (y - len_of_range) : 1,
    *y_max = (y + len_of_range < width) ? (y + len_of_range) : width;

    /* 坐标转偏移*/
    --*x_min, --*x_max,
    --*y_min, --*y_max;
}

int has_block_which(char type, int x, int y, struct minefield_struct *p_minefield)
{
    int x_min, x_max, y_min, y_max;

    /* 设定范围（并将坐标量转为偏移量）*/
    setup_range(x, &x_min, &x_max,
                y, &y_min, &y_max,
                1,
                p_minefield->length, p_minefield->width);

    for (x = x_min; x <= x_max; ++x)
        for (y = y_min; y <= y_max; ++y) {
            if (x == x_min || x == x_max
                || y == y_min || y ==y_max) {
                if (p_minefield->explicit[y][x] == type) return 1;
            }
        }

    return 0;
}

int has_unknown_block(int x, int y, struct minefield_struct *p_minefield)
{
    return has_block_which('#', x, y, p_minefield);
}

int has_blank_block(int x, int y, struct minefield_struct *p_minefield)
{
    return has_block_which(' ', x, y, p_minefield);
}

int surrounding_mines(int x, int y, struct minefield_struct *p_minefield)
{
    int num_of_mines = 0, x_min, x_max, y_min, y_max;

    /* 设定范围（并将坐标量转为偏移量）*/
    setup_range(x, &x_min, &x_max,
                y, &y_min, &y_max,
                1,
                p_minefield->length, p_minefield->width);

    for (x = x_min; x <= x_max; ++x)
        for (y = y_min; y <= y_max; ++y) {
            if (x == x_min || x == x_max
                || y == y_min || y == y_max) {
                if (p_minefield->implicit[y][x] == MINE) ++num_of_mines;
            }
        }

    return num_of_mines;
}

/* 探测结束的条件：探测到的最外层区块周围都有雷或为边界区块(且周围不得有未检查块)
 *                 即最外层区块中的空区块周围无未检查区块
 */
void expand_and_detect(int x, int y, struct minefield_struct *p_minefield)
{
    int count = 0, X, Y, x_min, x_max, y_min, y_max, signal = 1;

    /* 保存原x, y值, 循环中多次是用原x, y*/
    X = x, Y = y;

    /* (x + 1), (y + 1) : 偏移转坐标*/
    for (;signal != -1; ++count) {
        signal = -1;

        /* 设定范围（并将坐标量转为偏移量）*/
        setup_range(X, &x_min, &x_max,
                    Y, &y_min, &y_max,
                    count,
                    p_minefield->length, p_minefield->width);

        for (x = x_min; x <= x_max; ++x)
            for (y = y_min; y <= y_max; ++y) {
                if (p_minefield->implicit[y][x] != MINE
                    && p_minefield->explicit[y][x] == '#'
                    && has_blank_block(x + 1, y + 1, p_minefield)) {
                    p_minefield->explicit[y][x] = surrounding_mines(x + 1, y + 1, p_minefield) + '0';

                    if (p_minefield->explicit[y][x] == '0') p_minefield->explicit[y][x] = ' ';
                    }
            }

        /* 判断是否需继续探测*/
        for (x = x_min; x <= x_max; ++x)
            for (y = y_min; y <= y_max; ++y) {
                /* 只检查最外层*/
                if (x == x_min || x == x_max
                    || y == y_min || y == y_max) {
                    if (p_minefield->explicit[y][x] == ' '
                        && has_unknown_block(x + 1, y + 1, p_minefield))
                        signal = 1;
                }
            }
    }
}

int detect_block(int x, int y, struct minefield_struct *p_minefield)
{
    int record_index = p_minefield->game_count - 1;

    /* 坐标转偏移*/
    --x, --y;

    /* (x + 1), (y + 1) : 偏移转坐标*/
    if (p_minefield->explicit[y][x] == ' '
        || (p_minefield->explicit[y][x] >= '0' && p_minefield->explicit[y][x] <= '9')) {
        printf("\t(^_-)-| (%d, %d)安全的空地\n", x + 1, y + 1);
    } else if (p_minefield->explicit[y][x] == 'F') {
        printf("\t(-_^)-| (%d, %d)被标记了\n", x + 1, y + 1);
    } else if (p_minefield->implicit[y][x] == MINE) {
        p_minefield->explicit[y][x] = '*';
        p_minefield->p_game_record[record_index].trigger_x = x + 1,
        p_minefield->p_game_record[record_index].trigger_y = y + 1;
    } else {
        p_minefield->explicit[y][x] = surrounding_mines(x + 1, y + 1, p_minefield) + '0';

        if (p_minefield->explicit[y][x] == '0') {
            p_minefield->explicit[y][x] = ' ';
            expand_and_detect(x + 1, y + 1, p_minefield);
        }
    }

    return 0;
}

int mark_block(int x, int y, struct minefield_struct *p_minefield)
{
    /* 坐标转偏移*/
    --x, --y;

    /* (x + 1), (y + 1) : 偏移转坐标*/
    if (p_minefield->explicit[y][x] == '#')
        p_minefield->explicit[y][x] = 'F';
    else if (p_minefield->explicit[y][x] == 'F')
        printf("\t(^_-)-F (%d, %d)已经标记过了\n", x + 1, y + 1);
    else
        printf("\t(-_^)-F (%d, %d)不用标记\n", x + 1, y + 1);

    return 0;
}

int remove_mark(int x, int y, struct minefield_struct *p_minefield)
{
    /* 坐标转偏移*/
    --x, --y;

    /* (x + 1), (y + 1) : 偏移转坐标*/
    if (p_minefield->explicit[y][x] == 'F')
        p_minefield->explicit[y][x] = '#';
    else
        printf("\t/(-_-)\\ (%d, %d)并没有flag\n", x + 1, y + 1);

    return 0;
}

int round_operation(char *content_of_input, struct minefield_struct *p_minefield)
{
    /* OPERATION_NUM 可进行操作的数目
     * CTRL_OPERATION_NUM 控制游戏（进程）操作的数目
     */
    #define OPERATION_NUM 9
    #define CTRL_OPERATION_NUM 5

    int i, j, x = 0, y = 0, signal = 0;
    struct parameter_struct parameter = {0, NULL};

    char flags[OPERATION_NUM] = {
        'h', 'r', 'R', 'q', 'Q',
        'd', 'D', 'm', '-'
    };
    int (*operate[OPERATION_NUM])(int x, int y, struct minefield_struct *p_minefield) = {
        &operation_help, &comfirm_restart, NULL, &comfirm_quit, NULL,
        &detect_block, &detect_block, &mark_block, &remove_mark
    };

    structure_parameter(content_of_input, &parameter);

    /* 遍历参数按命令执行相应操作*/
    for (i = 0; i < parameter.count; ++i)
        for (j = 0; j < OPERATION_NUM; ++j) {
            if (parameter.vector[i][0] == flags[j]) {
                if (j < CTRL_OPERATION_NUM) {
                    if (operate[j] == NULL) {
                        return signal = (parameter.vector[i][0] == 'R') ?
                                        RESTART : QUIT;    //大写的重开与退出无需确认
                    } else if ((signal = (*operate[j])(x, y, p_minefield))) {
                        return signal;
                    }
                } else {
                    /* 将其后坐标参数进行相应操作*/
                    for (; ++i < parameter.count;)
                        if (check_coordinate_format(parameter.vector[i], LOOSE)) {
                            get_coordinate(parameter.vector[i], &x, &y);
                            if (check_coordinate(x, y, p_minefield->length, p_minefield->width))
                                (*operate[j])(x, y, p_minefield);
                        } else {
                            --i;
                            break;
                        }
                }
                break;
            }
        }

    #undef OPERATION_NUM
    #undef CTRL_OPERATION_NUM

    return 0;
}

/* 显示雷区
 * 辅助函数：digits_of() （正整数）的位数
 */
int digits_of(int num)
{
    int digits;

    for (digits = 1; (num / 10) != 0; ++digits)
        num %= 10;

    return digits;
}

/* 设置颜色（输出终端颜色序列）*/
void set_color(char *color, char *background_color)
{
    int index;

    if (color != NULL && background_color != NULL) {
        color[index = index_of(color, 'm')] = 0;
        printf("%s%s", color,
            background_color + index_of(background_color, ';'));

        color[index] = 'm';
    } else if (color != NULL) {
        printf("%s", color);
    } else if (background_color != NULL) {
        printf("%s", background_color);
    }
}

/* 清除颜色*/
void clean_color()
{
    printf("\033[00m");
}

void display_minefield(struct minefield_struct *p_minefield)
{
    char color_of_coordinate_frame[] = "\033[0;32m", color_of_num[] = "\033[1;33m",
        color_of_mine[] = "\033[0;30m", background_color_of_trigger_mine[] = "\033[0;41m",
        color_of_flag[] = "\033[0;31m";
    char *color = NULL, *background_color = NULL;
    int i, j, k, record_index = p_minefield->game_count - 1;

    if (p_minefield->color_mode == DISABLE) {
        *color_of_coordinate_frame = *color_of_num
        = *color_of_mine = *background_color_of_trigger_mine
        = *color_of_flag
        = 0;
    }

    for (i = 0; i <= p_minefield->width; ++i)
        for (j = 0; j <= p_minefield->length; ++j) {
            if (i == 0 ||  j == 0) {
                set_color(color_of_coordinate_frame, NULL);
                printf("%d", (i == 0) ? j : i);    //坐标轴
                clean_color();
            } else {
                if (p_minefield->p_game_record[record_index].trigger_x == j
                    && p_minefield->p_game_record[record_index].trigger_y == i)
                    background_color = background_color_of_trigger_mine;

                if (p_minefield->explicit[i - 1][j - 1] > '0'
                    && p_minefield->explicit[i - 1][j - 1] <='9')
                    color = color_of_num;
                else if (p_minefield->explicit[i - 1][j - 1] == '*')
                    color = color_of_mine;
                else if (p_minefield->explicit[i - 1][j - 1] == 'F')
                    color = color_of_flag;

                set_color(color, background_color);
                color = background_color = NULL;
                putchar(p_minefield->explicit[i - 1][j - 1]);
                clean_color();

                /* 空格补齐*/
                for (k = 1; k < digits_of(i); ++k)
                    putchar(' ');
            }

            if (j == p_minefield->length) putchar('\n');
            else putchar(' ');
        }
    putchar('\n');
}

/* 设置雷区*/
void setup_minefield(struct minefield_struct *p_minefield)
{
    int i, j, count;

    /* 显式*/
    for (i = 0; i < p_minefield->width; ++i)
        for (j = 0; j < p_minefield->length; ++j)
            p_minefield->explicit[i][j] = '#';

    /* 隐式*/
    for (i = 0; i < p_minefield->width; ++i)
        for (j = 0; j < p_minefield->length; ++j)
            p_minefield->implicit[i][j] = 0;

    srand(time(NULL));
    for (count = 0; count < p_minefield->num_of_mines;) {
        i = rand() % p_minefield->width, j = rand() % p_minefield->length;
        if (!p_minefield->implicit[i][j])
            p_minefield->implicit[i][j] = MINE, ++count;
    }
}

/* 显示游戏记录
 * 辅助函数：以时分秒打印日历时间
 */
void print_time_in_hms(time_t time)
{
    int hour = 0, minute = 0, second = time;

    minute = second / 60, second %= 60;
    hour = minute / 60, minute %= 60;

    if (hour) printf("%d时", hour);
    printf("%d分%d秒", minute, second);
}

void show_record(int game_count, struct record_struct *p_game_record)
{
    int i, count_of_victory = 0, count_of_defeat = 0, count_of_stop = 0;
    time_t min_time, max_time;
    max_time = min_time = p_game_record->time_of_use;

    for (i = 0; i < game_count; ++i) {
        /* 胜、中止记录*/
        if (p_game_record[i].result == VICTORY) ++count_of_victory;
        else if (p_game_record[i].result == 0) ++count_of_stop;

        /* 最长、最短时间*/
        if (p_game_record[i].time_of_use < min_time && p_game_record[i].result)
            min_time = p_game_record[i].time_of_use;
        if (p_game_record[i].time_of_use > max_time && p_game_record[i].result)
            max_time = p_game_record[i].time_of_use;
    }
    count_of_defeat = game_count
        - (count_of_victory + count_of_stop);

    printf("%d胜%d负", count_of_victory, count_of_defeat);
    if (count_of_stop) printf("%d中止", count_of_stop);
    putchar('\n');

    if (max_time != 0) {
        printf("用时最长：");
        print_time_in_hms(max_time);
        putchar('\n');
    }
    if (min_time != 0) {
        printf("用时最短：");
        print_time_in_hms(min_time);
        putchar('\n');
    }
}

/* 开始游戏*/
void start_minesweeper(struct minefield_struct *p_minefield)
{
    /* signal -- -1 退出, 1 重开*/
    int i, signal = 0, record_index;
    char content_of_input[BUFSIZ] = {0};

    allocate_minefield(p_minefield);
    for (;; signal = 0) {
        setup_minefield(p_minefield);

        /* 为游戏记录分配/重分配空间*/
        if (p_minefield->p_game_record == NULL) {
            p_minefield->p_game_record =
                (struct record_struct *)malloc(sizeof(struct record_struct));
        } else {
            p_minefield->p_game_record =
                (struct record_struct *)realloc(p_minefield->p_game_record,
                    sizeof(struct record_struct) * p_minefield->game_count);
        }
        record_index = p_minefield->game_count - 1;
        p_minefield->p_game_record[record_index].result = 0;

        printf("第%d局\n", p_minefield->game_count);

        /* 回合处理*/
        for(p_minefield->p_game_record[record_index].time_of_use = time(NULL);;) {
            display_minefield(p_minefield);

            /* 由stdin获取输入*/
            printf(": ");
            fgets(content_of_input, BUFSIZ, stdin);
            for (i = 0;; ++i)
                if (content_of_input[i] == '\n') {
                    content_of_input[i] = 0;
                    break;
                }

            signal = round_operation(content_of_input, p_minefield);
            if (signal) break;

            if ((p_minefield->p_game_record[record_index].result
                = judge(p_minefield))) {
                p_minefield->p_game_record[record_index].time_of_use =
                    time(NULL) - p_minefield->p_game_record[record_index].time_of_use;
                break;
            }
        }

        /* 终局处理*/
        if (signal != QUIT) {
            if (p_minefield->p_game_record[record_index].result == VICTORY) {
                printf("\t\\(^_^)/\n");
                printf("用时：%s\n", ctime(&(p_minefield->p_game_record[record_index].time_of_use)));
                display_minefield(p_minefield);
            } else if (p_minefield->p_game_record[record_index].result == DEFEAT) {
                printf("\t/(T-T)\\\n");
                printf("用时：%s\n", ctime(&(p_minefield->p_game_record[record_index].time_of_use)));
                display_minefield(p_minefield);
            }

            if (signal || comfirm("再来一局(y/n): ")) {
                ++p_minefield->game_count;
                reset(p_minefield);
            } else {
                show_record(p_minefield->game_count, p_minefield->p_game_record);
                break;
            }
        } else {
            show_record(p_minefield->game_count, p_minefield->p_game_record);
            break;
        }
    }

    free(p_minefield->p_game_record);
    free_minefield(p_minefield);
}

/* 用法帮助*/
void mine_help(int argc, char *argv[])
{
    if (argc) {
        printf("命令参数：\n");
        printf("\t -h 用法帮助\n");
        printf("\t -a 设置雷区面积（格数），后接参数格式为长乘宽或边长如 -a 9x10 | -a 10\n");
        printf("\t -m 设置地雷个数，后接个数或比例，如 -m 9 | -m 90%%\n");
        printf("\t -C 禁用彩色显示（默认彩色输出需终端支持）\n");
    }

    printf("游戏操作：\n");
    printf("\t h     操作帮助\n");
    printf("\t r|R   重开（R 不询问确定与否）\n");
    printf("\t q|Q   退出（Q 不询问确定与否）\n");
    printf("\t d|D   detect 探测地雷（D 强行探测标记块及忽略空白块警告）\n");
    printf("\t m     mark 标记地雷\n");
    printf("\t -     去除标记\n");
    printf("\th, r(R), q(Q) 执行此类命令忽略后面各命令参数; d|D, m, - 后加坐标值，格式 x,y | x 取y = x\n");
}

/* process_argv() 参数处理
 * external call: cmp_str() -- publib.h
 * 辅助函数：
 * set_area() 设置面积
 * external call: str2num() -- publib.h
 * set_num_of_mines() 设置雷数，在参数内容为比率时，不设置（以防面积设置在后）而返回比率
 * external call: str2num() -- publib.h
 * check_arg_format() 检查参数格式 依据参数项对设定内容的参数格式检查 返回正否
 */
void set_area(char *arg, struct minefield_struct *p_minefield)
{
    int index;

    if((index = index_of(arg, 'x')) != -1) {
        arg[index] = 0;
        p_minefield->length = str2num(arg);
        p_minefield->width = str2num(arg + index + 1);
    } else {
        p_minefield->length = p_minefield->width = str2num(arg);
    }
}

double set_num_of_mines(char *arg, struct minefield_struct *p_minefield)
{
    int index;
    double rate_of_mines = 0.0;

    if((index = index_of(arg, '%')) != -1) {
        arg[index] = 0;
        rate_of_mines = str2num(arg) / 100;
    } else {
        p_minefield->num_of_mines = str2num(arg);
    }

    return rate_of_mines;
}

int check_arg_format(char *setting_content, char *arg)
{
    int i;
    char vaild_character = 'x';

    if (arg[1] == 'm') vaild_character = '%';

    for (i = 0; setting_content[i] != 0; ++i) {
        /* 通检*/
        if (!(setting_content[i] >='0' && setting_content[i] <= '9')
            && setting_content[i] != vaild_character)
            return 0;

        /* 特检*/
        if (setting_content[i] == 'x' && setting_content[i + 1] == 0)
            return 0;

        if (setting_content[i] == '%' && setting_content[i + 1] != 0)
            return 0;
    }

    return 1;
}

int process_argv(int argc, char *argv[], struct minefield_struct *p_minefield)
{
    int i;
    double rate_of_mines = 0.0;

    for (i = 0; i < argc; ++i) {
        if (cmp_str(argv[i], "-h")) {
            mine_help(argc, argv);
            return 1;
        } else if (cmp_str(argv[i], "-a")) {
            if (++i < argc && check_arg_format(argv[i], argv[i - 1])) {
                set_area(argv[i], p_minefield);
            } else {
                printf("参数%s后设定内容格式错误，已忽略\n", argv[--i]);
            }
        } else if (cmp_str(argv[i], "-m")) {
            if (++i < argc && check_arg_format(argv[i], argv[i - 1])) {
                rate_of_mines = set_num_of_mines(argv[i], p_minefield);
            } else {
                printf("参数%s后设定内容格式错误，已忽略\n", argv[--i]);
            }
        } else if (cmp_str(argv[i], "-C")) {
            p_minefield->color_mode = DISABLE;
        }
    }

    if (rate_of_mines != 0.0)
        p_minefield->num_of_mines = rate_of_mines
            * (p_minefield->length * p_minefield->width);

    /* 检查*/
    if (p_minefield->length <= 0) p_minefield->length = 10;
    if (p_minefield->width <= 0) p_minefield->width = 10;
    if (p_minefield->num_of_mines <= 0) {
        p_minefield->num_of_mines = 0.09
            * (p_minefield->length * p_minefield->width);
        if (p_minefield->num_of_mines == 0) p_minefield->num_of_mines = 1;
    }

    return 0;
}

int mine_main(int argc, char *argv[])
{
    struct minefield_struct minefield = {
        9, 9, 10,
        NULL, NULL,
        1,
        ENABLE,
        NULL
    };

    /* （显示帮助）返回值为1，退出*/
    if (process_argv(--argc, ++argv, &minefield)) return 0;

    start_minesweeper(&minefield);

    return 0;
}
