PP=g++
CPPFLAGS= -Wall -g
OTHERS= graph.cpp

################################

OBJS=graph.o
MAIN=ep4.cpp

################################

.PHONY: clean

all: ep

graph.o: graph.h
ep3.o: ep4.cpp

ep: ${OBJS} 
		${PP} ${CPPFLAGS} -o ep4 ${MAIN} ${OTHERS}
		@rm -rf *.o ep4.dSYM

clean:
	@rm -rf ep4