#Compiler and Compile options
FC=gfortran
DEBUG:=y
CFLAGS = -fPIC
LDFLAGS:= -shared -Wl,--exclude-libs,ALL
OBJPATH:=./obj
BINPATH:=./bin
OUT:=st_test.so
DEPS:=.depends

SUBDIR:=./src 
vpath %.f90 $(SUBDIR)

ifeq ($(strip $(DEBUG)),y)
	CFLAGS+= -g3 -O0
	OUT:=$(addsuffix d, $(OUT))
else
	CFLAGS+=-O2
endif

SRCS:=$(foreach d, $(SUBDIR), $(wildcard $(d)/*.f90))
OBJS:=$(patsubst %.f90,%.o,$(SRCS))
OBJS:=$(addprefix $(OBJPATH)/, $(notdir $(OBJS)))
OUT:=$(addprefix $(BINPATH)/, $(OUT))

all:$(OUT)
	@echo $(OUT)	

$(OUT): $(OBJS)
	-@mkdir -p $(BINPATH)
	$(FC) $(OBJS) $(LDFLAGS) -o $@	
	@echo "---------build target finshed-----------"

$(OBJPATH)/%.o: %.f90
	-@mkdir -p $(OBJPATH)
	$(FC) -ffree-line-length-none  -J ./obj -c $< $(CFLAGS) -o $@


##Use "make run" can run the Target file
run:
	@./$(OUT)

#Clean the Object files
clean:
	rm $(OBJS) $(MODOBJS) $(wildcard *.mod)

#Clean both the Object files and Target file
clean-all:
	rm $(TARGET) $(OBJS) $(MODOBJS) $(wildcard *.mod)
