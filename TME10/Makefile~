CC=mpicc
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src




all: directories ${BIN}/tme5


directories: ${OBJ} ${BIN} ${LIB} ${SRC} ${INC}

${OBJ}:
	mkdir ${OBJ}
${BIN}:
	mkdir ${BIN}
${LIB}:
	mkdir ${LIB}
${SRC}:
	mkdir ${SRC}
${INC}:
	mkdir ${INC}


${BIN}/tme5: ${OBJ}/tme5.o
	mpicc -o ${BIN}/tme5 ${OBJ}/tme5.o

${OBJ}/tme5.o: ${SRC}/tme5.c
	mpicc -o ${OBJ}/tme5.o -c ${SRC}/tme5.c

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~
