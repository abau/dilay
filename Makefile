APP=dilay
FLAGS=-Wall -Wextra -pedantic
LIBS=-lglfw -lGL -lGLEW 
CPP=g++ 

NODEP=clean

MODULES=id.cpp util.cpp depth.cpp ray.cpp opengl.cpp mesh.cpp \
				triangle.cpp\
				winged-vertex.cpp winged-face.cpp winged-edge.cpp winged-mesh.cpp\
				adaptive-mesh.cpp camera.cpp state.cpp main.cpp

SRC-DIR=src
SRC=$(MODULES:%=$(SRC-DIR)/%)

OBJ-DIR=obj
OBJ=$(SRC:$(SRC-DIR)/%.cpp=$(OBJ-DIR)/%.o)

DEP=$(SRC:$(SRC-DIR)/%.cpp=$(OBJ-DIR)/%.dep)

ALL: $(OBJ) $(DEP)
		$(CPP) $(LIBS) $(FLAGS) -o $(APP) $(OBJ)
		
$(OBJ-DIR)/%.o: $(SRC-DIR)/%.cpp $(OBJ-DIR)/%.dep
		$(CPP) $(LIBS) $(FLAGS) -c $< -o $@

$(OBJ-DIR)/%.dep: $(SRC-DIR)/%.cpp
	mkdir -p $(OBJ-DIR)
	$(CPP) -MM -MT '$(patsubst $(SRC-DIR)/%.cpp,$(OBJ-DIR)/%.o,$<)' $< -MF $@

clean:
	rm -rf $(OBJ-DIR) $(APP)

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEP))))
    -include $(DEP)
endif
