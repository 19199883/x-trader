# Makefile 

CC:=gcc
DEBUG:=y
TOP:=../../../
VERSION:=v2.6.x
DIST_DIR:=$(TOP)/dist
# to be changed
PROJECT_NAME:=st501_rb_i_day

INCPATH:= ./
	  
LIBPATH:=

CPPFLAGS:= -v $(INCPATH)
CFLAGS:=  -fPIC
LDFLAGS:= -shared -Wl,--exclude-libs,ALL
OBJPATH:=./obj
BINPATH:=./bin
LIBS:= 

DEPS:=.depends

# bo be changed
OUT:=st501_rb_i_day.so
PACKAGE_NAME:=$(OUT)

SUBDIR:=./src
CURDIR:=.

vpath %.c $(SUBDIR)

ifeq ($(strip $(DEBUG)),y)
	CFLAGS+= -g3 -O0	
else
	CFLAGS+=-O2
endif

SRCS:=$(foreach d, $(SUBDIR), $(wildcard $(d)/*.c))
OBJS:=$(patsubst %.c,%.o,$(SRCS))
OBJS:=$(addprefix $(OBJPATH)/, $(notdir $(OBJS)))
OUT:=$(addprefix $(BINPATH)/, $(OUT))


all:$(OUT)
	@echo $(OUT)	

$(OUT):$(DEPS) $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	-@mkdir -p $(BINPATH)
	$(CC) $(LIBPATH) -o $@  $(OBJS) $(LDFLAGS) $(LIBS) 
	@echo "---------build target finshed-----------"


$(OBJPATH)/%.o:%.c
	-@mkdir -p $(OBJPATH)
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) $(INCPATH)  -o $@
	@echo Finished building: $<

$(DEPS):$(SRCS)
	-@rm $(DEPS)
	$(CC)  -M $(CPPFLAGS) $(INCPATH) $^  >>$(DEPS) 

-include $(DEPS)

dist:
	-rm -fr $(DIST_DIR)/$(PACKAGE_NAME)
distclean:
	-@rm -f $(DEPS)
	-@rm -f $(OUT)
	-@rm -f $(OBJS)
	
clean:
	-@rm -f $(OUT)
	-@rm -f $(OBJS)

help:
	@echo "make (all):\t This is the default command when target unspecified "
	@echo "make clean:\t clean intermediate objects, target"
	@echo "make distclean:\t clean depends, intermediate objects, target"
