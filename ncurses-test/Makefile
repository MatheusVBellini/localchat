PROGRAM=prog

all: ${PROGRAM}

${PROGRAM}: main.cpp ../src/tui/Window.cpp
	g++ -g -lncurses -o $@ $^
clean:
	rm -rf ${PROGRAM}
run:
	./${PROGRAM}
