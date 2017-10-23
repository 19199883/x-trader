# Makefile 

CC:=g++
DEBUG:=y
TOP:=../..
DIST_DIR:=$(TOP)/x-shfemd/dist
INCPATH:=-I$(TOP)/3rd/tinyxml/include  -I$(TOP)/x-shfemd/include -I$(TOP)/3rd/shfe_md/include 
LIBPATH:=-L$(TOP)/3rd/boost/lib -L$(TOP)/3rd/tinyxml/lib -L$(TOP)/3rd/utils/lib -L$(TOP)/x-shfemd/bin
CPPFLAGS:=-std=c++11 $(INCPATH)
CFLAGS:=
LDFLAGS:= 
OBJPATH:=./obj
BINPATH:=./bin
LIBS:=-lpthread -ltinyxml -lutils -lssl -lbowsprit -lclogger -lcork -lvrt -lmy_quote_shfe_my_jrcmcc_lib \
	 -lboost_filesystem -lmdclient -lboost_system
DEPS:=.depends
OUT:=x-shfemd
PACKAGE_NAME:=$(OUT)

SUBDIR:=$(TOP)/x-shfemd/service 

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

dist:
	-rm -fr $(DIST_DIR)/$(PACKAGE_NAME)
	-mkdir -p $(DIST_DIR)/$(PACKAGE_NAME)
	-cp -arf $(TOP)/x-shfemd/service/bin/* $(DIST_DIR)/$(PACKAGE_NAME)/

distclean:

clean:
	-@rm $(OUT)
	-@rm $(OBJS)

help:
	@echo "make (all):\t This is the default command when target unspecified "
	@echo "make clean:\t clean intermediate objects, target"
	@echo "make distclean:\t clean depends, intermediate objects, target"
