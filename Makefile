GPP        = g++ -g -O0 -pthread -Wall -Wextra -std=gnu++14
MKDEP      = g++ -MM -std=gnu++14
VALGRIND   = valgrind --leak-check=full --show-reachable=yes
MKFILE     = Makefile
DEPFILE    = Makefile.dep

SRCDIR     = ./src/
HDRDIR     = ./include/
OBJDIR     = ./objects/
SOURCES    = Instruction.cpp Variable.cpp Program.cpp Parse.cpp
FUNCSRCS   = MathFuncs.cpp StringFuncs.cpp AddressFuncs.cpp MiscFuncs.cpp ThreadFuncs.cpp
MORESRCS   = MisException.cpp FactoryMap.cpp 
SOCKSRCS   = TCPSocket.cpp TCPServerSocket.cpp Server.cpp Client.cpp
THRDSRCS   = Thread.cpp ThreadInstr.cpp Connection.cpp
MAINSRCS   = ${SRVRMAIN}.cpp ${CLNTMAIN}.cpp
ALLSRCS    = ${SOURCES} ${MORESRCS} ${FUNCSRCS} ${SOCKSRCS} ${THRDSRCS}
HEADERS    = Instruction.h Variable.h Program.h Parse.h OpFuncs.h
MOREHDRS   = MisException.h FactoryMap.h so_includes.h common.h
SOCKHDRS   = TCPSocket.h TCPServerSocket.h Server.h Client.h Connection.h
THRDHDRS   = Thread.h ThreadInstr.h
ALLHDRS    = ${HEADERS} ${MOREHDRS} ${SOCKHDRS} ${THRDHDRS}
OBJECTS    = ${ALLSRCS:.cpp=.o}
CLNTOBJ    = MisException.o TCPSocket.o Client.o ${CLNTMAIN}.o
FSOURCES   = ${addprefix ${SRCDIR},${ALLSRCS}}
FOBJECTS   = ${addprefix ${OBJDIR},${OBJECTS}}
FCLNTOBJ   = ${addprefix ${OBJDIR},${CLNTOBJ}}
SRVREXEC   = MIS_server
SRVRMAIN   = server_main
CLNTEXEC   = MIS_client
CLNTMAIN   = client_main
SRCFILES   = ${SOURCES} ${MKFILE}

all : ${SRVREXEC} ${CLNTEXEC}

${SRVREXEC} : ${OBJDIR} ${FOBJECTS} ${OBJDIR}${SRVRMAIN}.o
	${GPP} -o ${SRVREXEC} -I ${HDRDIR} ${FOBJECTS} ${OBJDIR}${SRVRMAIN}.o

${CLNTEXEC} : ${OBJDIR} ${FCLNTOBJ} 
	${GPP} -o ${CLNTEXEC} -I ${HDRDIR} ${FCLNTOBJ}

${OBJDIR}%.o : ${SRCDIR}%.cpp 
	${GPP} -c $< -I ${HDRDIR}
	mv ${@F} ${OBJDIR}

${OBJDIR} :
	mkdir ${OBJDIR}

clean :
	- rm -r ${OBJDIR}/ *.out *.err

spotless : clean
	- rm ${EXECBIN} ${DEPFILE}

${DEPFILE} :
	${MKDEP} -I ${HDRDIR} ${FSOURCES} >${DEPFILE}

deps :
	- rm ${DEPFILE}
	${MAKE} --no-print-directory ${DEPFILE}

include Makefile.dep

again :
	${MAKE} spotless deps all
