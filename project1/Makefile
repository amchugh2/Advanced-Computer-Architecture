TARGET = predictors

OFILES = predictors.o

CXX = g++
CFLAGS = -g -Wall -std=c++17

LDFLAGS =

$(TARGET):	$(OFILES)
		$(CXX) $(OFILES) $(LDFLAGS) -o $@

clean:
	rm -f *~ $(OFILES) $(TARGET)

predictor.o:	predictors.cc

TARGET:		predictors.o predictors.cc
