CXX=$(shell root-config --cxx)
CFLAGS=-c -g -Wall $(shell root-config --cflags) -I./src -I./include -O3
LDLIBS=$(shell root-config --glibs)
LDFLAGS=$(shell root-config --ldflags)
#SOURCES=./src/SL_Event.cc ./src/FileManager.cc ./src/Filter.cc
SOURCES=$(shell ls ./src/*.cc)
HEADERS=$(shell ls ./include/*.h*)
TEMP=$(shell ls ./src/*.cc~)
TEMP2=$(shell ls ./include/*.hh~)
OBJECTS=$(SOURCES:.cc=.o) 
MAIN=NeutronCorrection.C
MAINO=./src/NeutronCorrection.o

EXECUTABLE=NeutronCorrection

INCLUDEPATH=include
SRCPATH=src

## .so libraries
#EVENTLIBPATH=/mnt/daqtesting/lenda/sam_analysis/LendaEvent/
EVENTLIBPATH=/user/lipschut/Introspective/

LIB=LendaEvent
EVENTLIB=$(addsuffix $(LDFLAGS),$(LIB))


.PHONY: clean get put all test sclean

all: $(EXECUTABLE) 

$(EXECUTABLE) : $(OBJECTS) $(MAINO) $(HEADERS)
	@echo "Building target" $@ "..." 
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(MAINO) $(LDLIBS) -L$(EVENTLIBPATH) -l$(EVENTLIB) -O3
	@echo
	@echo "Build succeed"


.cc.o:
	@echo "Compiling" $< "..."
	@$(CXX) $(CFLAGS) $< -o $@ 

$(MAINO): $(MAIN)
	@echo "Compiling" $< "..."
	@$(CXX) $(CFLAGS) $< -o $@  


##%Dictionary.o: include/%.hh src/%LinkDef.h
##	$(ROOTCINT) -f $(patsubst %.o,%.cc,$@) -c $^;
##	$(CXX) -p -fPIC $(CFLAGS) -c $(patsubst %.o,%.cc,$@) $(patsubst %.o,%.h,$@)


test:
	@echo $(HEADERS)

clean:
	-rm ./$(OBJECTS)
	-rm ./$(EXECUTABLE)
	-rm ./$(MAINO)

sclean:
	-rm ./$(TEMP)
	-rm ./$(TEMP2)
	-rm ./$(OBJECTS)
	-rm ./$(EXECUTABLE)
	-rm ./$(MAINO)

