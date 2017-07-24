# 编译可执行文件
.PHONY: all
all: clg

# 安装可执行文件至~/bin/
.PHONY: install
install: clg
	mv ./clg ~/bin/

######
clg: main.o mine.o publib.o
	gcc main.o mine.o publib.o -o clg

main.o: main.c game.h publib.h
	gcc -c main.c

mine.o: mine.c publib.h
	gcc -c mine.c

publib.o: publib.c
	gcc -c publib.c
######

# 以debug模式编译生成可执行文件
.PHONY: debug
debug: flag = -Wall -g
debug: a.out

######
a.out: main_d.o mine_d.o sudoku_d.o publib.d.o
	gcc main_d.o mine_d.o sudoku_d.o publib.d.o

main_d.o: main.c game.h publib.h
	gcc $(flag) -c main.c -o main_d.o

mine_d.o: mine.c publib.h
	gcc $(flag) -c mine.c -o mine_d.o

sudoku_d.o: sudoku.c publib.h
	gcc $(flag) -c sudoku.c -o sudoku_d.o

publib.d.o: publib.c
	gcc $(flag) -c publib.c -o publib.d.o
######

# 清除
.PHONY: clean_ALL
clean_ALL:
	rm -f *.o a.out clg

.PHONY: clean_program
clean_program:
	rm -f main.o mine.o publib.o clg

.PHONY: clean_debug
clean_debug:
	rm -f *_d.o a.out
