CC=mpicc
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src




all: directories ${BIN}/diner_philosophes


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


${BIN}/diner_philosophes: ${OBJ}/diner_philosophes.o
	mpicc -o ${BIN}/diner_philosophes ${OBJ}/diner_philosophes.o

${OBJ}/diner_philosophes.o: ${SRC}/diner_philosophes.c
	mpicc -o ${OBJ}/diner_philosophes.o -c ${SRC}/diner_philosophes.c 

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~
