COMPILER=g++
RUNSTRING=./${TARGET}

OBJECTS=main.o mainwindow.o completionwindow.o document.o selectionwindow.o fileutil.o workspace.o


LIBS=-lgtksourceviewmm-3.0
LIBS+= `pkg-config --libs gtkmm-3.0 gtksourceviewmm-3.0`
LIBS+= completer.a
FLAGS= -g -std=c++11
FLAGS+=`pkg-config --cflags gtkmm-3.0 gtksourceviewmm-3.0` -fPIC

TARGET=./matedit-gtk


all: .depend ${TARGET}

#Calculating dependincies
.depend: $(wildcard ./*.cpp ./*.h) Makefile
	$(CXX) $(FLAGS) -MM *.cpp > ./.dependtmp
	cat ./.dependtmp | sed 's/h$$/h \n\t \$(CXX) $$< -c $(FLAGS) -o $$@/' > ./.depend
	rm ./.dependtmp

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
