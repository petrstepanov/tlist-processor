# Environment
CXX=clang++
SRC_DIR=src
OBJ_DIR=build
BIN_DIR=dist

APP_NAME=tlist-processor
DICT_FILENAME=tlist-dict.cpp
DICT_PCM_FILENAME=tlist-dict_rdict.pcm

# Variables
CXXFLAGS=`root-config --cflags` # -pthread -stdlib=libc++ -std=c++11 -m64 -I/Applications/root_v6.06.02/include
LDFLAGS=`root-config --ldflags`
GLIBS=`root-config --glibs` -lRooFit -lRooFitCore -lHtml -lMinuit -lFumili
HEADERS=src/AppSettings.h \
        src/Constants.h \
        src/FittingFunctions.h \
        src/Geometry.h \
        src/GraphicsHelper.h \
        src/HistProcessor.h \
        src/StringUtils.h \
        src/TlistProcessorFrame.h
SOURCES=src/AppSettings.cpp \
        src/Constants.cpp \
        src/FittingFunctions.cpp \
        src/Geometry.cpp \
        src/GraphicsHelper.cpp \
        src/HistProcessor.cpp \
        src/StringUtils.cpp \
        src/TlistProcessorFrame.cpp \
        src/main.cpp

OBJECTS_TEMP=$(SOURCES:.cpp=.o)
OBJECTS=$(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(OBJECTS_TEMP))
EXECUTABLE=$(BIN_DIR)/$(APP_NAME)
DICTIONARY=$(DICT_FILENAME)
SHARED_LIBRARY=$(APP_NAME).so
# List of special targets that do not generate files
.PHONY: directories

# Empty target ensures that list of all 'end products' are called
all: directories $(DICTIONARY) $(SHARED_LIBRARY) $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(SHARED_LIBRARY)
	@echo "Linking "$@
	$(CXX) -o $@ $(OBJECTS) $(SHARED_LIBRARY) $(GLIBS)
	# move .so library to bin folder
	mv $(SHARED_LIBRARY) $(BIN_DIR)/$(SHARED_LIBRARY)
	# change search location of the .so library to the executable directory of the app
	install_name_tool -change $(APP_NAME).so @executable_path/$(APP_NAME).so $(EXECUTABLE)
	# move dictionary to the bin folder - they say you have to
	mv $(DICT_PCM_FILENAME) $(BIN_DIR)/$(DICT_PCM_FILENAME)

$(DICTIONARY): $(HEADERS) $(SRC_DIR)/LinkDef.h
	rootcling -f $@ -c $(CXXFLAGS) -p $^

# https://root.cern.ch/interacting-shared-libraries-rootcint (they forgot $(GLIBS) damn)
$(SHARED_LIBRARY): $(DICTIONARY) $(SOURCES)
	$(CXX) -shared -o $@ $(LDFLAGS) $(CXXFLAGS) $(GLIBS) $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
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
	rm -f *.pcm

directories:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
