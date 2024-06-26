ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

CXX        = g++
CXXFLAGS   = -g -c -Wall
LDFLAGS    = 
LDLIBS     = -lRubberDuck
OBJS       = Queue_3P.o
DEPS       = Queue_3P.h

ifeq ($(detected_OS),Windows)
	LIBPATH   = -L..\..\bin
	INCLUDES = -I..\..\lib
	BINPATH    = ..\..\bin
	COPY       = copy
	RM         = del
	EXECUTABLE = Queue_3P.exe
endif

ifeq ($(detected_OS),Linux)
	LIBPATH   = -L../../bin
	INCLUDES = -I../../lib
	BINPATH    = ../../bin
	COPY       = cp
	RM         = rm
	EXECUTABLE = Queue_3P
endif

all: $(EXECUTABLE)
	$(COPY) $(EXECUTABLE) $(BINPATH)
	@echo All done!

clean: 
	$(RM) -f $(OBJS) $(EXECUTABLE)
	@echo Clean done!
	
%.o: %.cpp $(DEPS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $<

$(EXECUTABLE): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS) $(LIBPATH)
	

