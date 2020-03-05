TARGET = predictor

OFILES = predictor.o

CXX = g++
CFLAGS = -g -Wall -std=c++17

LDFLAGS =

$(TARGET):	$(OFILES)
		$(CXX) $(OFILES) $(LDFLAGS) -o $@

clean:
	rm -f *~ $(OFILES) $(TARGET)

predictor.o:	predictor.cc

TARGET:		predictor.o predictor.cc
