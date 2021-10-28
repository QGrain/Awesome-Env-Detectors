CC=gcc
CXX=g++
CFLAGS+=-O2
CXXFLAGS+=-O2

# 跨平台编译支持
ifeq ($(linux),true)
	RM=rm
	EXT=
else
	RM=del
	EXT=.exe
endif

# 支持att&t和intel语法的内联汇编
ifeq ($(assembly),intel)
	CFLAGS+=-masm=intel
	CXXFLAGS+=-masm=intel
else
	CFLAGS+=
	CXXFLAGS+=
endif

TARGET=DebugEnv-Detector

# 获取1级子目录，find命令失败。EX_FILES过滤排除的文件
# SRC_DIRS:=$(shell find ./ -maxdepth 1 -type d)
# SRC_DIRS=. feature-api
# EX_FILES=
# SRCS=$(filter-out ${EX_FILES}, $(foreach dir, $(SRC_DIRS), $(wildcard $(addprefix $(dir)/*, .c))))
SRCS=DebugEnv-Detector.c feature-api\antidbgwinapi.c feature-sys\systrace.c feature-behavior\antidbgbehavior.c checkpeb\checkpeb.c

# basename 返回一个字符串 . 之前的所有字段,即去掉后缀，同时addsuffix 加上后缀.o
OBJS=$(addsuffix .o, $(basename $(SRCS)))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS)  $(OBJS)  -o $@

# feature-api\antidbgwinapi.o: feature-api\antidbgwinapi.c
# 	$(CC) $(CFLAGS)  -c $^  -o $@
# feature-sys\systrace.o: feature-sys\systrace.c
# 	$(CC) $(CFLAGS)  -c $^  -o $@
# feature-behavior\antidbgbehavior.o: feature-behavior\antidbgbehavior.c
# 	$(CC) $(CFLAGS)  -c $^  -o $@
# DebugEnv-Detector.o: DebugEnv-Detector.c
# 	$(CC) $(CFLAGS)  -c $^  -o $@

# 通配符 @:目标文件，^:所有的依赖文件，<:第一个依赖文件
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS)  -c $^  -o $@


#Windows平台，使用del而不是rm
.PHONY: clean clean-all
clean:
	$(RM) $(OBJS)

clean-all:
	$(RM) $(OBJS) $(TARGET)$(EXT)