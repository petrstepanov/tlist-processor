# Environment
OS:=$(shell uname)
ifeq ($(OS),Darwin)
  CXX=clang++
else
  CXX=g++
endif

SRC_DIR=src
OBJ_DIR=build
BIN_DIR=dist

APP_NAME=tlist-processor
DICT_FILENAME=tlist-dict.cpp
DICT_PCM_FILENAME=tlist-dict_rdict.pcm
DSYM_DIR=tlist-processor.so.dSYM

# Variables
#CXXFLAGS=-O3 `root-config --cflags` -fPIC # -pthread -stdlib=libc++ -std=c++11 -m64 -I/Applications/root_v6.06.02/include
CXXFLAGS=`root-config --cflags` -fPIC
LDFLAGS=`root-config --ldflags`
GLIBS=`root-config --glibs` -lRooFit -lRooFitCore -lHtml -lMinuit -lFumili

H_EXT = h
HEADERS = $(shell find $(SRC_DIR) -type f -name *.$(H_EXT))

SRC_EXT = cpp
SOURCES = $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT))

OBJECTS_TEMP = $(SOURCES:.cpp=.o)
OBJECTS = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(OBJECTS_TEMP))

EXECUTABLE=$(BIN_DIR)/$(APP_NAME)
DICTIONARY=$(DICT_FILENAME)
SHARED_LIBRARY=$(APP_NAME).so

# convenience variable for making directories
dir_guard=@mkdir -p $(@D)

# Empty target ensures that list of all 'end products' are called
all: executable

debug: CXXFLAGS += -g #-ggdb -DDEBUG -g
debug: executable

executable: directories $(DICTIONARY) $(SHARED_LIBRARY) $(OBJECTS) $(EXECUTABLE)

$(DICTIONARY): $(HEADERS) $(SRC_DIR)/LinkDef.hpp
	rootcling -f $@ -c $(CXXFLAGS) -p $^

# https://root.cern.ch/interacting-shared-libraries-rootcint (they forgot $(GLIBS) damn)
$(SHARED_LIBRARY): $(DICTIONARY) $(SOURCES)
	$(CXX) -shared -o $@ $(LDFLAGS) $(CXXFLAGS) $(GLIBS) $^

$(EXECUTABLE): $(OBJECTS) $(SHARED_LIBRARY)
	@echo "Linking "$@
	$(CXX) -o $@ $(OBJECTS) $(SHARED_LIBRARY) $(GLIBS)
	# move .so library to bin folder
	mv $(SHARED_LIBRARY) $(BIN_DIR)/$(SHARED_LIBRARY)
	# change search location of the .so library to the executable directory of the app (macOS only)
ifeq ($(OS),Darwin)
	install_name_tool -change $(APP_NAME).so @executable_path/$(APP_NAME).so $(EXECUTABLE)
endif
	# move dictionary to the bin folder - they say you have to
	mv $(DICT_PCM_FILENAME) $(BIN_DIR)/$(DICT_PCM_FILENAME)
	# rm $(DICT_FILENAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(dir_guard)
	@echo "Compiling "$@
# compile with dependency files - why?
#	$(CXX) $(CXXFLAGS) -c -g -MMD -MP -MF "$@.d" -o $@ $<
# compile with debug symbols
#	$(CXX) $(CXXFLAGS) -c -g $< -o $@
# just compile
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	rm -f -r $(OBJ_DIR)
	rm -f -r $(BIN_DIR)
	rm -f $(DICTIONARY)
	rm -f -r $(DSYM_DIR)

directories:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

echo:
	$(info SOURCES: $(SOURCES))
	$(info HEADERS: $(HEADERS))

# List of special targets that do not generate files
.PHONY: directories echo
