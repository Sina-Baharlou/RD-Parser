CXXFLAGS =	-std=c++0x -O2 -g -Wall -fmessage-length=0 -Wreorder -Wwrite-strings -Wsign-compare -fpermissive

OBJS =		RD-Parser.o

LIBS =		

TARGET =	RD-Parser

LIBSDIR = 	

$(TARGET):	$(OBJS)
	$(CXX) $(LIBSDIR) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
