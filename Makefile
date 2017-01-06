# 编译可执行文件
.PHONY: all
all: clg

# 安装可执行文件至~/bin/
.PHONY: install
install: clg
	mv ./clg ~/bin/

######
clg: main.o mine.o str.o
	gcc main.o mine.o str.o -o clg

main.o: main.c game.h str.h
	gcc -c main.c

mine.o: mine.c str.h
	gcc -c mine.c

str.o: str.c
	gcc -c str.c
######

# 以debug模式编译生成可执行文件
.PHONY: debug
debug: flag = -Wall -g
debug: a.out

######
a.out: main_d.o mine_d.o str_d.o
	gcc main_d.o mine_d.o str_d.o

main_d.o: main.c game.h str.h
	gcc $(flag) -c main.c -o main_d.o

mine_d.o: mine.c str.h
	gcc $(flag) -c mine.c -o mine_d.o

str_d.o: str.c
	gcc $(flag) -c str.c -o str_d.o
######

# 清除
.PHONY: clean_ALL
clean_ALL:
	rm -f *.o a.out clg

.PHONY: clean_program
clean_program:
	rm -f main.o mine.o str.o clg

.PHONY: clean_debug
clean_debug:
	rm -f *_d.o a.out
