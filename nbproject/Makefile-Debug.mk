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
	${OBJECTDIR}/AppSettings.o \
	${OBJECTDIR}/FittingFunctions.o \
	${OBJECTDIR}/Geometry.o \
	${OBJECTDIR}/GraphicsHelper.o \
	${OBJECTDIR}/HistProcessor.o \
	${OBJECTDIR}/StringUtils.o \
	${OBJECTDIR}/TlistProcessorFrame.o \
	${OBJECTDIR}/main.o


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

${OBJECTDIR}/AppSettings.o: AppSettings.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AppSettings.o AppSettings.cpp

${OBJECTDIR}/FittingFunctions.o: FittingFunctions.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FittingFunctions.o FittingFunctions.cpp

${OBJECTDIR}/Geometry.o: Geometry.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry.o Geometry.cpp

${OBJECTDIR}/GraphicsHelper.o: GraphicsHelper.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GraphicsHelper.o GraphicsHelper.cpp

${OBJECTDIR}/HistProcessor.o: HistProcessor.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HistProcessor.o HistProcessor.cpp

${OBJECTDIR}/StringUtils.o: StringUtils.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StringUtils.o StringUtils.cpp

${OBJECTDIR}/TlistProcessorFrame.o: TlistProcessorFrame.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TlistProcessorFrame.o TlistProcessorFrame.cpp

${OBJECTDIR}/main.o: main.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/Applications/root_v6.06.02/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cc

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
