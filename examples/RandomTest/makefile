ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

CXX        = g++
CXXFLAGS   = -g -c -Wall -std=gnu++17
LDFLAGS    = 
LDLIBS     = -lRubberDuck
OBJS       = RandomTest.o
DEPS       = 

ifeq ($(detected_OS),Windows)
	LIBPATH   = -L..\..\bin
	INCLUDES = -I..\..\lib
	BINPATH    = ..\..\bin
	COPY       = copy
	RM         = del
	EXECUTABLE = RandomHistogramTest.exe
endif

ifeq ($(detected_OS),Linux)
	LIBPATH   = -L../../bin
	INCLUDES = -I../../lib
	BINPATH    = ../../bin
	COPY       = cp
	RM         = rm
	EXECUTABLE = RandomHistogramTest
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
	

