CC=mpicc
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src




all: directories ${BIN}/arbre_mpi ${BIN}/echo_squel


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


${BIN}/arbre_mpi: ${OBJ}/arbre_mpi.o
	mpicc -o ${BIN}/arbre_mpi ${OBJ}/arbre_mpi.o

${OBJ}/arbre_mpi.o: ${SRC}/arbre_mpi.c
	mpicc -o ${OBJ}/arbre_mpi.o -c ${SRC}/arbre_mpi.c

${BIN}/echo_squel: ${OBJ}/echo_squel.o
	mpicc -o ${BIN}/echo_squel ${OBJ}/echo_squel.o

${OBJ}/echo_squel.o: ${SRC}/echo_squel.c
	mpicc -o ${OBJ}/echo_squel.o -c ${SRC}/echo_squel.c


clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~
