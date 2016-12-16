CC=mpicc
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src




all: directories ${BIN}/Hello ${BIN}/HelloMaster ${BIN}/HelloNeighbor ${BIN}/mpi_server


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


${BIN}/Hello: ${OBJ}/Hello.o
	mpicc -o ${BIN}/Hello ${OBJ}/Hello.o

${OBJ}/Hello.o: ${SRC}/Hello.c
	mpicc -o ${OBJ}/Hello.o -c ${SRC}/Hello.c 

${BIN}/HelloMaster: ${OBJ}/HelloMaster.o
	mpicc -o ${BIN}/HelloMaster ${OBJ}/HelloMaster.o

${OBJ}/HelloMaster.o: ${SRC}/HelloMaster.c
	mpicc -o ${OBJ}/HelloMaster.o -c ${SRC}/HelloMaster.c

${BIN}/HelloNeighbor: ${OBJ}/HelloNeighbor.o
	mpicc -o ${BIN}/HelloNeighbor ${OBJ}/HelloNeighbor.o

${OBJ}/HelloNeighbor.o: ${SRC}/HelloNeighbor.c
	mpicc -o ${OBJ}/HelloNeighbor.o -c ${SRC}/HelloNeighbor.c

${BIN}/mpi_server: ${OBJ}/mpi_server.o
	mpicc -o ${BIN}/mpi_server ${OBJ}/mpi_server.o -lrt

${OBJ}/mpi_server.o: ${SRC}/mpi_server.c
	mpicc -o ${OBJ}/mpi_server.o -c ${SRC}/mpi_server.c 

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~
