/* 说明：数独
 * 作者：lwj786
 * 日期：2017年 01月 31日 星期二 19:41:35 CST
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "publib.h"

#define True 1
#define False 0

#define TIMEOUT 3000

#define RED(string) "\033[1;31m"string"\033[00m"
#define GREEN(string) "\033[1;32m"string"\033[00m"
#define YELLOW(string) "\033[1;33m"string"\033[00m"
#define BLUE(string) "\033[1;34m"string"\033[00m"
#define CYAN(string) "\033[1;36m"string"\033[00m"

#define RESTART 1
#define QUIT -1

struct
list_struct {
    int list[9];
    int count;
};

struct
sudoku_settings_struct {
    int generate_only;
    int color_mode;
};

struct
sudoku_puzzle_struct {
    int **answer;
    int **solution;
    int **riddle;

    int win;
};

/* gen_sudoku_table() 生成数独表
 * 辅助函数：
 * copy_list() 列表复制
 * list_append() 列表（元素)增加
 * append_list() 成列表增加（不重复）
 * has_value() 检查是否存在对应值
 * get_candidate_list() 获取候选列表
 * update_practical_constraint() 更新实践约束
 * column_constraint() 列约束
 * row_constraint() 行约束
 * nine_grid_constraint() 九宫格（区域）约束
 * update_constraint() 更新约束列表
 */
void copy_list(struct list_struct *source, struct list_struct *target)
{
    int i;

    target->count = source->count;
    for (i = 0; i < source->count; ++i)
        target->list[i]= source->list[i];
}

void list_append(struct list_struct *source, int value)
{
    source->list[source->count++] = value;
}

void append_list(struct list_struct *source, struct list_struct *appended)
{
    int has_value(struct list_struct source, int value);

    int i;

    for (i = 0; i < appended->count; ++i)
        if (!has_value(*source, appended->list[i]))
            list_append(source, appended->list[i]);
}

int has_value(struct list_struct source, int value)
{
    int i = 0;

    for (; i < source.count; ++i)
        if (source.list[i] == value) return 1;

    return 0;
}

void get_candidate_list(struct list_struct *candidate, struct list_struct *constraint)
{
    int i;

    for (i = 0; i < 9; ++i)
        if(!has_value(*constraint, i + 1)) {
            list_append(candidate, i + 1);
    }
}

void update_practical_constraint(struct list_struct *constraint, struct list_struct *practical_constraint)
{
    int i;

    for (i = 0; i < practical_constraint->count; ++i)
        list_append(constraint, practical_constraint->list[i]);
}

void column_constraint(struct list_struct *constraint, int **sudoku_table, int current_column)
{
    int i = 0;

    for (; i < 9; ++i)
        if (sudoku_table[i][current_column] != 0
            && !has_value(*constraint, sudoku_table[i][current_column])) {
            list_append(constraint, sudoku_table[i][current_column]);
        }
}

void row_constraint(struct list_struct *constraint, int **sudoku_table, int current_row)
{
    int i = 0;

    for (; i < 9; ++i)
        if (sudoku_table[current_row][i] != 0
            && !has_value(*constraint, sudoku_table[current_row][i])) {
            list_append(constraint, sudoku_table[current_row][i]);
        }
}

void nine_grid_constraint(struct list_struct *constraint, int **sudoku_table,
    int feature_row, int feature_column, int current_row, int current_column)
{
    int i, j, row_pos_max, column_pos_max;

    row_pos_max = feature_row + 3,
    column_pos_max = feature_column + 3;

    for (i = feature_row; i < row_pos_max; ++i)
        for (j = feature_column; j < column_pos_max; ++j) {
            if ((i != current_row || j != current_column)    //九宫格中当前行与列不必重复检查
                && sudoku_table[i][j] != 0
                && !has_value(*constraint, sudoku_table[i][j])) {
                list_append(constraint, sudoku_table[i][j]);
            }
        }
}

void update_constraint(struct list_struct *constraint, int **sudoku_table,
    int feature_row, int feature_column, int current_row, int current_column)
{
    nine_grid_constraint(constraint, sudoku_table,
        feature_row, feature_column, current_row, current_column);
    row_constraint(constraint, sudoku_table, current_row);
    column_constraint(constraint, sudoku_table, current_column);

}

int generate_sudoku_table(int **sudoku_table)
{
    /* 回退记录 成员 i, j 记录位置 practical_constraint
     * 2 个记录可避免在某个节点的分支（即某位置有多候选值下连续取值）
     * 中有分支或有大于2个节点时出现反复
     * （由于没有对分支节点做完整记录可能无法顺利生成）
     */
    struct back_record_struct {
        int i, j;
        struct list_struct practical_constraint;
    } back_record[2] = {
        {-1, -1, {{0}, 0}}, {-1, -1, {{0}, 0}}
    };

    int i, j, feature_row, feature_column;
    struct list_struct candidate, constraint;    //candidate 存储候选值，constraint 存储约束

    clock_t start_time, end_time;

    srand(time(NULL));
    candidate.count = constraint.count = 0;
    start_time = clock();
    for (i = 0; i < 9; ++i) {
        for (j = 0; j < 9; ++j, candidate.count = constraint.count = 0) {
            /* 约束更新 标准约束与实践约束*/
            feature_row = i / 3 * 3;
            feature_column = j / 3 * 3;
            update_constraint(&constraint,
                sudoku_table,
                feature_row, feature_column, i, j);

            if (back_record[0].i == i && back_record[0].j == j) {
                update_practical_constraint(&constraint, &(back_record[0].practical_constraint));

                /* 多选值节点记录赋值给记录2*/
                if (constraint.count != 9) {
                    if (back_record[1].i == i && back_record[1].j == j) {
                        append_list(&(back_record[1].practical_constraint),
                            &(back_record[0].practical_constraint));
                    } else {
                        back_record[1].i = i,
                        back_record[1].j = j;
                        copy_list(&(back_record[0].practical_constraint),
                            &(back_record[1].practical_constraint));
                    }
                }
            }

            if (back_record[1].i == i && back_record[1].j == j)
                update_practical_constraint(&constraint, &(back_record[1].practical_constraint));

            /* 回退操作
             * 约束规则是sudoku的必要条件而非充分条件，因此仅仅满足约束也不能生成表
             * 通过回退并剔除当前的前一位选值
             */
            if (constraint.count == 9) {
                if (j == 0) i-=1, j = 8;
                else --j;

                if (back_record[0].i != i || back_record[0].j != j) {
                    back_record[0].i = i, back_record[0].j = j;
                    back_record[0].practical_constraint.count = 0;
                }

                list_append((&back_record[0].practical_constraint), sudoku_table[i][j]);
                sudoku_table[i][j] = 0;

                /* 循环后有j自加*/
                if (j == 0) i-=1, j = 8;
                else --j;

                continue;
            }

            get_candidate_list(&candidate, &constraint);

            sudoku_table[i][j] = candidate.list[rand() % candidate.count];

            end_time = clock();
            if (end_time - start_time > TIMEOUT) return 1;
        }
    }

    return 0;
}

/* 重设表*/
void reset_table(int **table)
{
    int i, j;

    for (i = 0; i < 9; ++i)
        for (j = 0; j < 9; ++j)
            table[i][j] = 0;
}

/* 生成数独表*/
int sudoku_gen(int **sudoku_table)
{
    while (generate_sudoku_table(sudoku_table))
        reset_table(sudoku_table);

    return 0;
}

/* 回合操作
 * 辅助函数：
 * judge_sudoku() 判断
 * fill_in() 填入数字
 * remove_number() 移除数字
 * reset_puzzle() 重设
 */
int judge_sudoku(struct sudoku_puzzle_struct *puzzle)
{
    int i, j;

    for (i = 0; i < 9; ++i)
        for (j = 0; i < 9; ++j)
            if (puzzle->solution[i][j] != puzzle->answer[i][j])
                return 0;

    puzzle->win = True;

    return 1;
}

void fill_in(char *num_str, char *coordinate, struct sudoku_puzzle_struct *puzzle)
{
    int i, j;    //数组下标与坐标：i = y - 1, j = x - 1

    get_coordinate(coordinate, &j, &i);
    if (check_coordinate(j, i, 9, 9)) {
        --i, --j;

        if (! (puzzle->riddle[i][j] || puzzle->solution[i][j]))
            puzzle->solution[i][j] = str2num(num_str);
    }

}

void remove_number(char *coordinate, struct sudoku_puzzle_struct *puzzle)
{
    int i, j;    //数组下标与坐标：i = y - 1, j = x - 1

    get_coordinate(coordinate, &j, &i);
    if (check_coordinate(j, i, 9, 9)) {
        --i, --j;

        if (! puzzle->riddle[i][j]) puzzle->solution[i][j] = 0;
    }
}

void reset_puzzle(struct sudoku_puzzle_struct *puzzle)
{
    int i, j;

    for (i = 0; i < 9; ++i)
        for (j = 0; j < 9; ++j)
            puzzle->solution[i][j] = puzzle->riddle[i][j];
}

int sudoku_round_operation(char *content_of_input, struct sudoku_puzzle_struct *puzzle)
{
    void sudoku_help(int argc, char *argv[]);

    int i, signal = 0;
    char *num_str;
    struct parameter_struct parameter = {0, NULL};

    structure_parameter(content_of_input, &parameter);

    for (i = 0; i < parameter.count; ++i) {
        if (cmp_str(parameter.vector[i], "h")) {
            sudoku_help(0, NULL);
        } else if (cmp_str(parameter.vector[i], "r")) {
            signal = comfirm("重新开局（y/n）:") ? RESTART:0;
            break;
        } else if (cmp_str(parameter.vector[i], "q")) {
            signal = comfirm("退出（y/n）:") ? QUIT:0;
            break;
        } else if (cmp_str(parameter.vector[i], "S")) {
            if (comfirm("重设（y/n）:")) reset_puzzle(puzzle);
        } else if (cmp_str(parameter.vector[i], "-")) {
            for (++i; i < parameter.count; ++i) {
                if (check_coordinate_format(parameter.vector[i], STRICT)) {
                    remove_number(parameter.vector[i], puzzle);
                } else {
                    --i;
                    break;
                }
            }
        } else if (parameter.vector[i][0] >= '1' && parameter.vector[i][0] <= '9'
            && parameter.vector[i][1] == '\0') {
            num_str = parameter.vector[i];

            for (++i; i < parameter.count; ++i) {
                if (check_coordinate_format(parameter.vector[i], STRICT)) {
                    fill_in(num_str, parameter.vector[i], puzzle);
                } else {
                    --i;
                    break;
                }
            }
        } else if (cmp_str(parameter.vector[i], "j")) {
            judge_sudoku(puzzle);
        }
    }

    return signal;
}

/* 显示表*/
void display_table(int **bottom_table, int **top_table, int color_mode)
{
    int i, j, k;

    if (! bottom_table) {
        for (i = 0; i < 9; ++i) {
            for (j = 0; j < 9; ++j) {
                if (top_table[i][j])
                    printf("%d", top_table[i][j]);
                else
                    putchar(' ');

                putchar(' ');
            }
            putchar('\n');
        }

        return;
    }

    for (i = 0; i < 9; ++i) {
        if (! i) {    //x轴
            printf("0 ");
            for (k = 1; k <= 9; ++k)
                printf(" %d ", k);
            putchar('\n');
        }

        for (j = 0; j < 9; ++j) {
            if (! j) printf("%d ", i + 1);    //y轴

            if (color_mode) {

                if ((i / 3 - j / 3) == 1 || (i / 3 - j / 3) == -1)
                    printf(BLUE("["));
                else
                    printf(CYAN("["));

                if (bottom_table[i][j] == top_table[i][j]) {
                    if (top_table[i][j])
                        printf(YELLOW("%d"), top_table[i][j]);
                    else
                        putchar(' ');
                } else {
                    if (top_table[i][j])
                        printf(GREEN("%d"), top_table[i][j]);
                    else
                        putchar(' ');
                }

                if ((i / 3 - j / 3) == 1 || (i / 3 - j / 3))
                    printf(BLUE("]"));
                else
                    printf(CYAN("]"));
            } else {
                putchar('[');
                if (top_table[i][j])
                    printf("%d", top_table[i][j]);
                else
                    putchar(' ');
                putchar(']');
            }
        }
        putchar('\n');
    }
}

/* 生成谜题*/
void generate_puzzle(struct sudoku_puzzle_struct *puzzle)
{
    int i, j, count, layout_table[9][9] = {0};

    reset_table(puzzle->answer);
    reset_table(puzzle->solution);
    reset_table(puzzle->riddle);

    sudoku_gen(puzzle->answer);

    srand(time(NULL));

    /* 全表随机九位*/
    count = 1;
    while (count < 9) {
        i = rand() % 9, j = rand() % 9;

        if (! layout_table[i][j]) {
            ++layout_table[i][j];
            ++count;
        }
    }

    /* 行、列随机九位*/
    for (i = 0; i < 9; ++i) {
        ++layout_table[i][rand() % 9];
        ++layout_table[rand() % 9][i];
    }

    /* 九宫格随机九位*/
    for (i = 0; i < 9; i += 3)
        for (j = 0; j < 9; j += 3)
            ++layout_table[i + rand() % 3][j + rand() % 3];

    for (i = 0; i < 9; ++i)
        for (j = 0; j < 9; ++j)
            if (layout_table[i][j])
                puzzle->solution[i][j] = puzzle->riddle[i][j]
                    = puzzle->answer[i][j];
}

/* 开始*/
void start_sudoku(struct sudoku_puzzle_struct *puzzle, struct sudoku_settings_struct *settings)
{
    int i, count, signal;
    char content_of_input[BUFSIZ] = {0};

    puzzle->solution = allocate_array_2d(9, 9);
    puzzle->riddle = allocate_array_2d(9, 9);

    for (count = 1, signal = 0;; ++count) {
        printf("第%d局\n", count);
        generate_puzzle(puzzle);

        /* 回合操作*/
        while (True) {
            display_table(puzzle->riddle, puzzle->solution, settings->color_mode);

            /* 由stdin获取输入*/
            printf(": ");
            fgets(content_of_input, BUFSIZ, stdin);
            for (i = 0;; ++i)
                if (content_of_input[i] == '\n') {
                    content_of_input[i] = 0;
                    break;
                }

            signal = sudoku_round_operation(content_of_input, puzzle);
            if (signal || puzzle->win) break;
        }

        /* 终局操作*/
        if (puzzle->win) {
            printf("!!!win!!!\n");
            signal = comfirm("重开,否则退出(y/n):") ? RESTART : QUIT;
        }

        if (signal == QUIT) break;
    }

    free_array_2d(puzzle->solution, 9);
    free_array_2d(puzzle->riddle, 9);
}

/* 用法帮助*/
void sudoku_help(int argc, char *argv[])
{
    if (argc) {
        printf("命令参数：\n");
        printf("\t -h 用法帮助\n");
        printf("\t -C 禁用彩色显示（默认彩色输出需终端支持）\n");
        printf("\t gen 仅生成（并输出）一个数独表\n");
    }

    printf("游戏操作：\n");
    printf("\t h     操作帮助\n");
    printf("\t r     重开\n");
    printf("\t q     退出\n");
    printf("\t S|D   reset 重设/清除已填内容\n");
    printf("\t -     去除已填数字\n");
    printf("\t 1~9   填入相应数字,坐标格式为 x,y （横向为x轴）\n");
    printf("\t j     判断\n");
}

/* 参数处理*/
int process_sudoku_argv(int argc, char *argv[], struct sudoku_settings_struct *p_sudoku_settings)
{
    int i;

    for (i = 0; i < argc; ++i) {
        if (cmp_str(argv[i], "-h")) {
            sudoku_help(argc, argv);
            return 1;
        } else if (cmp_str(argv[i], "-C")) {
            p_sudoku_settings->color_mode = False;
        } else if (cmp_str(argv[i], "gen")) {
            p_sudoku_settings->generate_only = True;
        }
    }

    return 0;
}

int sudoku_main(int argc, char *argv[])
{
    int **sudoku_table;

    struct sudoku_puzzle_struct sudoku_puzzle = {
        NULL, NULL, NULL,
        False
    };
    struct sudoku_settings_struct sudoku_settings = {
        False, True
    };

    sudoku_table = allocate_array_2d(9, 9);

    if (process_sudoku_argv(--argc, ++argv, &sudoku_settings))
        return 0;

    if (sudoku_settings.generate_only) {
        sudoku_gen(sudoku_table);
        display_table(NULL, sudoku_table, False);
    } else {
        sudoku_puzzle.answer = sudoku_table;
        start_sudoku(&sudoku_puzzle, &sudoku_settings);
    }

    free_array_2d(sudoku_table, 9);

    return 0;
}
