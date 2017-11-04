#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/AppSettings.o \
	${OBJECTDIR}/src/Constants.o \
	${OBJECTDIR}/src/FittingFunctions.o \
	${OBJECTDIR}/src/Geometry.o \
	${OBJECTDIR}/src/GraphicsHelper.o \
	${OBJECTDIR}/src/HistProcessor.o \
	${OBJECTDIR}/src/StringUtils.o \
	${OBJECTDIR}/src/TlistProcessorFrame.o \
	${OBJECTDIR}/src/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread -stdlib=libc++ -std=c++11 -m64
CXXFLAGS=-pthread -stdlib=libc++ -std=c++11 -m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/Applications/root_v6.06.02/lib -L. tlist-processor.so

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tlist-processor

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tlist-processor: tlist-processor.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tlist-processor: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	g++ -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tlist-processor ${OBJECTFILES} ${LDLIBSOPTIONS} -lGui -lCore -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lMultiProc -lpthread -stdlib=libc++ -lm -ldl -lRooFit -lRooFitCore -lHtml -lMinuit -lFumili

${OBJECTDIR}/src/AppSettings.o: src/AppSettings.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AppSettings.o src/AppSettings.cpp

${OBJECTDIR}/src/Constants.o: src/Constants.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Constants.o src/Constants.cpp

${OBJECTDIR}/src/FittingFunctions.o: src/FittingFunctions.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FittingFunctions.o src/FittingFunctions.cpp

${OBJECTDIR}/src/Geometry.o: src/Geometry.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Geometry.o src/Geometry.cpp

${OBJECTDIR}/src/GraphicsHelper.o: src/GraphicsHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/GraphicsHelper.o src/GraphicsHelper.cpp

${OBJECTDIR}/src/HistProcessor.o: src/HistProcessor.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/HistProcessor.o src/HistProcessor.cpp

${OBJECTDIR}/src/StringUtils.o: src/StringUtils.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/StringUtils.o src/StringUtils.cpp

${OBJECTDIR}/src/TlistProcessorFrame.o: src/TlistProcessorFrame.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TlistProcessorFrame.o src/TlistProcessorFrame.cpp

${OBJECTDIR}/src/main.o: src/main.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cc

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} -r ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tlist-processor.so
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tlist-processor

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
