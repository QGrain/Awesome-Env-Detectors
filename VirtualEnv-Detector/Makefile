CC=gcc
CXX=g++
CFLAGS+=-O2
CXXFLAGS+=-O2

TARGET=VirtualEnv-Detector
SRCS=VirtualEnv-Detector.c

# 跨平台编译支持
ifeq ($(linux),true)
	RM=rm
	EXT=
	SRCS+=VMware/vmware_linux.c
else
	RM=del
	EXT=.exe
	SRCS+=VMware\vmware_win.c
endif

# 支持att&t和intel语法的内联汇编
ifeq ($(assembly),intel)
	CFLAGS+=-masm=intel
	CXXFLAGS+=-masm=intel
else
	CFLAGS+=
	CXXFLAGS+=
endif

# basename 返回一个字符串 . 之前的所有字段,即去掉后缀，同时addsuffix 加上后缀.o
OBJS=$(addsuffix .o, $(basename $(SRCS)))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS)  $(OBJS)  -o $@

# VMware\vmware_linux.o: VMware\vmware_linux.c
# 	$(CC) $(CFLAGS)  -c $^  -o $@
# VirtualEnv-Detector.o: VirtualEnv-Detector.c
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