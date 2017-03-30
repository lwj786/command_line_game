/* 说明：数独
 * 作者：lwj786
 * 日期：2017年 01月 31日 星期二 19:41:35 CST
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "str.h"

struct
list_struct {
    int list[9];    //用指针更好，但需内存管理
    int count;
};

/* 显示表*/
void display_table(int table[9][9])
{
    int i,j;

    for (i = 0; i < 9; ++i) {
        for (j = 0; j < 9; ++j) {
            printf("%d", table[i][j]);
            putchar(' ');
        }
        putchar('\n');
    }
}

/* gen_sudoku_table() 生成数独表
 * 辅助函数：
 * copy_list() 列表复制
 * list_append() 元素增加
 * append_list() 成列表赋值（不重复）
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

void column_constraint(struct list_struct *constraint, int sudoku_table[9][9], int current_column)
{
    int i = 0;

    for (; i < 9; ++i)
        if (sudoku_table[i][current_column] != 0
            && !has_value(*constraint, sudoku_table[i][current_column])) {
            list_append(constraint, sudoku_table[i][current_column]);
        }
}

void row_constraint(struct list_struct *constraint, int sudoku_table[9][9], int current_row)
{
    int i = 0;

    for (; i < 9; ++i)
        if (sudoku_table[current_row][i] != 0
            && !has_value(*constraint, sudoku_table[current_row][i])) {
            list_append(constraint, sudoku_table[current_row][i]);
        }
}

void nine_grid_constraint(struct list_struct *constraint, int sudoku_table[9][9],
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

void update_constraint(struct list_struct *constraint, int sudoku_table[9][9],
    int feature_row, int feature_column, int current_row, int current_column)
{
    nine_grid_constraint(constraint, sudoku_table,
        feature_row, feature_column, current_row, current_column);
    row_constraint(constraint, sudoku_table, current_row);
    column_constraint(constraint, sudoku_table, current_column);

}

int gen_sudoku_table(int sudoku_table[9][9])
{
    /* 回退记录 成员 i, _j 记录位置 practical_constraint
     * 2 个记录可避免在某个节点的分支（即某位置有多候选值下连续取值）
     * 中有分支或有大于2个节点时出现反复
     */
    struct back_record_struct {
        int i, j;
        struct list_struct practical_constraint;
    } back_record[2] = {
        {-1, -1, {{0}, 0}}, {-1, -1, {{0}, 0}}
    };

    int i, j, feature_row, feature_column;
    struct list_struct candidate, constraint;    //candidate 存储候选值，constraint 存储约束

    srand(time(NULL));
    candidate.count = constraint.count = 0;
    for (i = 0; i < 9; ++i)
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
        }

    return 1;
}

/* 用法帮助*/
void sudoku_help(int argc, char *argv[])
{
    if (argc) {
        printf("命令参数：\n");
        printf("\t -h 用法帮助\n");
    }

    printf("游戏操作：\n");
}

/* 参数处理*/
int process_sudoku_argv(int argc, char *argv[])
{
    int i;

    for (i = 0; i < argc; ++i) {
        if (cmp_str(argv[i], "-h")) {
            sudoku_help(0, NULL);
            return 1;
        }
    }

    return 0;
}

int sudoku_main(int argc, char *argv[])
{
    int sudoku_table[9][9] = {0};

    process_sudoku_argv(--argc, ++argv);

    gen_sudoku_table(sudoku_table);

    display_table(sudoku_table);

    return 0;
}
