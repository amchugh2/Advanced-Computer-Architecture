TARGET = cache-sim

OFILES = cache-sim.o

CXX = g++

CFLAGS = -g -Wall -std=c++17

LDFLAGS = 

$(TARGET):	$(OFILES)
		$(CXX) $(OFILES) $(LDFLAGS) -o $@

clean:
	rm -f *~ $(OFILES) $(TARGET)

cache-sim.o:	cache-sim.cc

TARGET:		cache-sim.o cache-sim.cc

