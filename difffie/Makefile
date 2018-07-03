LDFLAGS_OPENCV := $(shell pkg-config --libs opencv)

CPPFLAGS += -std=c++14 -O3 -Wall
LDFLAGS += ${LDFLAGS_OPENCV}

progs = difffie

.phony: all
all: ${progs}

%: %.cpp
	${CXX} -o $@ $^ ${CPPFLAGS} ${LDFLAGS}

.phony: clean
clean:
	${RM} ${progs} *.o
