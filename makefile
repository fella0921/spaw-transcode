
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))

current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))

IINC=-I$(current_dir)/include/opencore-amrnb/ -I$(current_dir)/include/opencore-amrwb/
LIB=-L/usr/lib/ -L$(current_dir)/lib/

CC := gcc
RM := rm -rf
MKDIR := mkdir

DIR_OBJS := objs
DIR_TARGET := target
DIRS := $(DIR_OBJS) $(DIR_TARGET)

TARGET := $(DIR_TARGET)/spawtranscode.out
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
OBJS := $(addprefix $(DIR_OBJS)/, $(OBJS))

$(TARGET) : $(DIRS) $(OBJS)
	$(CC) -static -o $@ $(OBJS) $(LIB) -lopencore-amrnb -lopencore-amrwb 
	@echo "Target => $@"
	
$(DIRS) :
	$(MKDIR) $@
	
$(DIR_OBJS)/%.o : %.c
		$(CC) -o $@ -g -c $^ $(IINC) 


	
	
.PHONY : rebuild clean all

rebuild : clean all

all : $(TARGET)

clean :
	$(RM) $(DIRS)
