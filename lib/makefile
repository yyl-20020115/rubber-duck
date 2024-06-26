ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

CXX        = g++
CXXFLAGS   = -g -c -Wall
LDFLAGS    = 
LDLIBS     =
ifeq ($(OS),Windows_NT) 
    OBJS       = Error.o Random.o ProcessNotice.o Simulator.o DataCollection.o Coroutine.o CProcess.o Petri.o
else
	OBJS       = Error.o Random.o ProcessNotice.o Simulator.o DataCollection.o Task.o Coroutine.o CProcess.o Petri.o
endif
DEPS       = Error.h Random.h ProcessNotice.h Simulator.h Queue.h EventList.h EventNotice.h DataCollection.h CProcess.h Coroutine.h Petri.h
TARGET_LIB = libRubberDuck.a

ifeq ($(detected_OS),Windows)
	LIBPATH    = ..\bin
	COPY       = copy
	RM         = del
endif
ifeq ($(detected_OS),Linux)
	LIBPATH    = ../bin
	COPY       = cp 
	RM         = rm
endif

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $<

#打包 .o 文件到库文件 TARGET_LIB
all: $(OBJS)
	ar rcs $(TARGET_LIB) $^          
	$(COPY) $(TARGET_LIB) $(LIBPATH)
	@echo All done!
	
clean: 
	$(RM) -f $(OBJS) $(TARGET_LIB)
	@echo Clean done!
