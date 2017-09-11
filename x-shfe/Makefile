# Makefile 

CC:=g++
DEBUG:=y
TOP:=..
DIST_DIR:=$(TOP)/x-shfe/dist
INCPATH:=-I$(TOP)/3rd/femas2.0/include -I$(TOP)/3rd/tinyxml/include  -I$(TOP)/3rd/utils/include -I$(TOP)/3rd/shfe_md/include 
LIBPATH:=-L$(TOP)/3rd/boost/lib -L$(TOP)/3rd/femas2.0/lib -L$(TOP)/3rd/tinyxml/lib -L$(TOP)/3rd/utils/lib -L$(TOP)/3rd/shfe_md/lib
CPPFLAGS:=-std=c++11 $(INCPATH)
CFLAGS:=
LDFLAGS:= 
OBJPATH:=./obj
BINPATH:=./bin
LIBS:=-lpthread -lUSTPtraderapi -ltinyxml -lutils -ldl -lpthread  -lssl -lbowsprit -lcheck -lclogger -lcork -lvrt -lmy_quote_shfe_my_jrcmcc_lib \
	  -lmy_common_v2 -luuid -lboost_filesystem -lmdclient
DEPS:=.depends
OUT:=x-shfe
PACKAGE_NAME:=$(OUT)

SUBDIR:=$(TOP)/x-shfe/src 

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
	-cp -arf $(TOP)/x-shfe/bin/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/3rd/femas2.0/lib/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/3rd/gfortran/lib/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/3rd/shfe_me/lib/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/3rd/boost/lib/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -Lp /usr/local/lib/libbowsprit.so.2 $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -Lp /lib64/libcheck.so.0 $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -Lp /usr/local/lib/libclogger.so.2 $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -Lp /usr/local/lib/libcork.so.15 $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -Lp /usr/local/lib/libvrt.so.2 $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/x-shfe/config/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cd $(DIST_DIR); \
		tar -cvjf $(PACKAGE_NAME).tar.bz2  $(PACKAGE_NAME)/*     

distclean:
	-@rm $(TOP)/dist/$(PACKAGE_NAME).tar.bz2	
	-@rm -rf $(DIST_DIR)

clean:
	-@rm $(OUT)
	-@rm $(OBJS)

help:
	@echo "make (all):\t This is the default command when target unspecified "
	@echo "make clean:\t clean intermediate objects, target"
	@echo "make distclean:\t clean depends, intermediate objects, target"
