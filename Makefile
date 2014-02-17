# This make file will compile all .cpp and .c files in the directory. Be aware, the C++ compiler is used for both C and C++ files. Resulting objects are then linked into a single executable.

# make settings
NEWLINE=1 #newline after each object
ifdef NEWLINE
	NEWLINE = @echo ""
endif

# compile settings
CXX?=g++
LINK=g++
PROGRAM=a.out
DEL=rm -rf
LIBS=-lm -fopenmp -lsfml-graphics -lsfml-window -lsfml-system
WARN=-w -g
OP=-O3
FLAGS= $(WARN) $(OP)

# setup object list
LOCAL_CPP_OBJECTS= $(patsubst %.cpp, %.o, $(wildcard ./*.cpp))
LOCAL_C_OBJECTS= $(patsubst %.c, %.o, $(wildcard ./*.c))
LOADER_OBJECTS= $(patsubst %.c, %.o, $(wildcard ./obj_loader/*.cpp)) # the OBJ loader sub dir
OBJECTS=$(LOCAL_C_OBJECTS) $(LOCAL_CPP_OBJECTS) $(LOADER_OBJECTS)

# make targets
makeall:
	make $(PROGRAM)
	$(NEWLINE)

$(PROGRAM): $(OBJECTS) 
	@echo "--$(PROGRAM)--"
	$(LINK) $(FLAGS) $(OBJECTS) $(LIBS) -o $(PROGRAM)

%.o: %.cpp
	@echo "--$@--"
	$(CXX) -c $(FLAGS) $< $(LIBS) -o $@
	$(NEWLINE)

%.o: %.c
	@echo "--$@--"
	$(CXX) -c $(FLAGS) $< $(LIBS) -o $@
	$(NEWLINE)

# clean
.PHONEY: clean
clean:
	$(DEL) $(OBJECTS)

.PHONEY: cleanall
cleanall:
	$(DEL) $(OBJECTS)
	$(DEL) $(PROGRAM)
