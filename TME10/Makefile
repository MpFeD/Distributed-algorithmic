CC=mpicc
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src




all: directories ${BIN}/tme10


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


${BIN}/tme10: ${OBJ}/tme10.o
	mpicc -o ${BIN}/tme10 ${OBJ}/tme10.o

${OBJ}/tme10.o: ${SRC}/tme10.c
	mpicc -o ${OBJ}/tme10.o -c ${SRC}/tme10.c

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~
