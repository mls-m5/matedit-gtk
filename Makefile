COMPILER=g++
RUNSTRING=./${TARGET}

OBJECTS=main.o mainwindow.o completionwindow.o document.o selectionwindow.o
OBJECTS+= fileutil.o workspace.o console.o clangindex.o


LIBS=-lgtksourceviewmm-3.0
LIBS+= `pkg-config --libs gtkmm-3.0 gtksourceviewmm-3.0`
LIBS+= /usr/lib/llvm-3.4/lib/libclang.so
CXXFLAGS= -g -std=c++11
CXXFLAGS+=`pkg-config --cflags gtkmm-3.0 gtksourceviewmm-3.0` -fPIC
CXXFLAGS+= -I/usr/lib/llvm-3.4/include/

TARGET=./matedit-gtk


all: .depend ${TARGET}

#Calculating dependincies
.depend: $(wildcard ./*.cpp ./*.h) Makefile
	$(CXX) $(CXXFLAGS) -MM *.cpp > ./.depend

${TARGET}: ${OBJECTS}
	${COMPILER} ${FLAGS} -o ${TARGET} ${OBJECTS} ${LIBS}

include .depend

#För att kunna köra filen direkt
run: ${TARGET}
	${RUNSTRING}

clean:
	rm $(OBJECTS) $(MOCFILES)
	rm .depend

rebuild: clean ${TARGET}
