# Makefile 

CC:=g++
DEBUG:=n
TOP:=..
DIST_DIR:=$(TOP)/x-ees-ctp/dist
INCPATH:=-I$(TOP)/3rd/ees/include -I$(TOP)/3rd/tinyxml/include \
		 -I$(TOP)/3rd/utils/include -I$(TOP)/3rd/shfe_md/include \
		 -I$(TOP)/3rd/ctp/include
LIBPATH:=-L$(TOP)/3rd/boost/lib -L$(TOP)/3rd/ees/lib -L$(TOP)/3rd/ctp/lib \
		 -L$(TOP)/3rd/tinyxml/lib -L$(TOP)/3rd/utils/lib \
		 -L$(TOP)/3rd/shfe_md/lib
CPPFLAGS:=-std=c++11 $(INCPATH)
CFLAGS:=
LDFLAGS:= 
OBJPATH:=./obj
BINPATH:=./bin
LIBS:=-lEESTraderApi -lEESQuoteApi -ltinyxml -lutils -ldl -lpthread  \
	  -lbowsprit -lclogger -lcork -lvrt \
	  -lboost_filesystem  -lboost_system -lthostmduserapi_se
DEPS:=.depends
OUT:=x-ees-ctp
PACKAGE_NAME:=$(OUT)

SUBDIR:=$(TOP)/x-ees-ctp/src 

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
	-cp -arf $(TOP)/x-ees-ctp/bin/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/x-ees-ctp/config/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/x-ees-ctp/scripts/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/3rd/ees/lib/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/3rd/ctp/lib/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/3rd/gfortran/lib/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/3rd/shfe_md/lib/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf $(TOP)/3rd/shfe_md/config/* $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf /usr/local/lib/libboost_filesystem.so*  $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -arf /usr/local/lib/libboost_system.so*  $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -Lp /usr/local/lib/libbowsprit.so.2 $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -Lp /lib64/libcheck.so.0 $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -Lp /usr/local/lib/libclogger.so.2 $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -Lp /usr/local/lib/libcork.so.15 $(DIST_DIR)/$(PACKAGE_NAME)/
	-cp -Lp /usr/local/lib/libvrt.so.2 $(DIST_DIR)/$(PACKAGE_NAME)/
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
