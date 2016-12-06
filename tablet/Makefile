TARGET=tabletpos

SRCS=$(TARGET).cpp

FLAGS=-O2 -Wall
INCLUDE=-I../include/gfxtablet

LIBDIR=
LIBS=

CC=g++
CPPFLAGS=$(FLAGS) $(INCLUDE)
LDFLAGS=$(LIBDIR) $(LIBS)

all: $(TARGET) 

$(TARGET): % : %.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) -MMD $(CPPFLAGS) -c -o $@ $<

-include $(SRCS:.cpp=.d)

clean:
	rm -f *~ *.bak *.o *.d

distclean: clean
	rm -f $(TARGET)

