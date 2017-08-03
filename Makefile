# Makefile 

CC:=g++
DEBUG:=y
TOP:=.
INCPATH:=-I$(TOP)/3rd/x1/include -I$(TOP)/3rd/tinyxml/include  -I$(TOP)/3rd/utils/include -I$(TOP)/3rd/level2/include 
LIBPATH:=-L$(TOP)/3rd/x1/lib -L$(TOP)/3rd/tinyxml/lib -L$(TOP)/3rd/utils/lib -L$(TOP)/3rd/level2/lib
CPPFLAGS:=-std=c++11 $(INCPATH)
CFLAGS:=
LDFLAGS:= 
OBJPATH:=./obj
BINPATH:=./bin
LIBS:=-lpthread -lx1_dfitc_api -ltinyxml -lutils -ldl -lpthread  -lssl -lbowsprit -lcheck -lclogger -lcork -lvrt -lmy_quote_dce_level2_jrudp_lib \
	  -lmy_common_v2 -luuid
DEPS:=.depends
OUT:=x-trader_dce

SUBDIR:=./src 

vpath %.cpp $(SUBDIR)

ifeq ($(strip $(DEBUG)),y)
	CFLAGS+= -g3 -O0
	OUT:=$(addsuffix d, $(OUT))
else
	CFLAGS+=-O2
endif

SRCS:=$(foreach d, $(SUBDIR), $(wildcard $(d)/*.cpp))
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))
OBJS:=$(addprefix $(OBJPATH)/, $(notdir $(OBJS)))
OUT:=$(addprefix $(BINPATH)/, $(OUT))


all:$(OUT)
	@echo $(OUT)	

$(OUT):$(DEPS) $(OBJS)
	-@mkdir -p $(BINPATH)
	$(CC) $(OBJS) 	-o $@  $(LDFLAGS) $(LIBPATH) $(LIBS)	
	@echo "---------build target finshed-----------"


$(OBJPATH)/%.o:%.cpp
	-@mkdir -p $(OBJPATH)
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) $(INCPATH)  -o $@

$(DEPS):$(SRCS)
	-@rm $(DEPS)
	$(CC)  -MM $(CPPFLAGS) $(INCPATH) $^  >>$(DEPS) 

-include $(DEPS)

distclean:
	-@rm $(DEPS)
	-@rm $(OUT)
	-@rm $(OBJS)

clean:
	-@rm $(OUT)
	-@rm $(OBJS)

help:
	@echo "make (all):\t This is the default command when target unspecified "
	@echo "make clean:\t clean intermediate objects, target"
	@echo "make distclean:\t clean depends, intermediate objects, target"
