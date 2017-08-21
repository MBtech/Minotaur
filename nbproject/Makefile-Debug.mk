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
	${OBJECTDIR}/Client.o \
	${OBJECTDIR}/Server.o \
	${OBJECTDIR}/app.o \
	${OBJECTDIR}/count.o \
	${OBJECTDIR}/countExec.o \
	${OBJECTDIR}/splitExec.o \
	${OBJECTDIR}/splitter.o \
	${OBJECTDIR}/spout.o \
	${OBJECTDIR}/spoutExec.o \
	${OBJECTDIR}/test.o \
	${OBJECTDIR}/utils.o \
	${OBJECTDIR}/i_spout.o \
	${OBJECTDIR}/i_spoutExec.o \
	${OBJECTDIR}/tokenizer.o \
	${OBJECTDIR}/tokenizerExec.o \
    ${OBJECTDIR}/decider.o \
    ${OBJECTDIR}/deciderExec.o \
    ${OBJECTDIR}/emergency.o \
    ${OBJECTDIR}/emergencyExec.o \
    ${OBJECTDIR}/pattern.o \
    ${OBJECTDIR}/patternExec.o \
    ${OBJECTDIR}/notify.o \
    ${OBJECTDIR}/notifyExec.o \
    ${OBJECTDIR}/storage.o \
    ${OBJECTDIR}/storageExec.o \

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-lzmq
CXXFLAGS=-lzmq

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/lib

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/minotaur

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/minotaur: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/minotaur ${OBJECTFILES} ${LDLIBSOPTIONS}

IOT_APP=iot_health

${OBJECTDIR}/Client.o: Client.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Client.o Client.cpp

${OBJECTDIR}/Server.o: Server.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Server.o Server.cpp

${OBJECTDIR}/app.o: app.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/app.o app.cpp

${OBJECTDIR}/count.o: count.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/count.o count.cpp

${OBJECTDIR}/countExec.o: countExec.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/countExec.o countExec.cpp

${OBJECTDIR}/splitExec.o: splitExec.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/splitExec.o splitExec.cpp

${OBJECTDIR}/splitter.o: splitter.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/splitter.o splitter.cpp

${OBJECTDIR}/spout.o: spout.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/spout.o spout.cpp

${OBJECTDIR}/spoutExec.o: spoutExec.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/spoutExec.o spoutExec.cpp

${OBJECTDIR}/test.o: test.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/test.o test.cpp

${OBJECTDIR}/utils.o: utils.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils.o utils.cpp
###########################
${OBJECTDIR}/i_spout.o: ${IOT_APP}/i_spout.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/i_spout.o ${IOT_APP}/i_spout.cpp

${OBJECTDIR}/i_spoutExec.o: ${IOT_APP}/i_spoutExec.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/i_spoutExec.o ${IOT_APP}/i_spoutExec.cpp

${OBJECTDIR}/tokenizer.o: ${IOT_APP}/tokenizer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tokenizer.o ${IOT_APP}/tokenizer.cpp

${OBJECTDIR}/tokenizerExec.o: ${IOT_APP}/tokenizerExec.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tokenizerExec.o ${IOT_APP}/tokenizerExec.cpp

${OBJECTDIR}/decider.o: ${IOT_APP}/decider.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/decider.o ${IOT_APP}/decider.cpp

${OBJECTDIR}/deciderExec.o: ${IOT_APP}/deciderExec.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/deciderExec.o ${IOT_APP}/deciderExec.cpp

${OBJECTDIR}/emergency.o: ${IOT_APP}/emergency.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/emergency.o ${IOT_APP}/emergency.cpp

${OBJECTDIR}/emergencyExec.o: ${IOT_APP}/emergencyExec.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/emergencyExec.o ${IOT_APP}/emergencyExec.cpp

${OBJECTDIR}/pattern.o: ${IOT_APP}/pattern.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/pattern.o ${IOT_APP}/pattern.cpp

${OBJECTDIR}/patternExec.o: ${IOT_APP}/patternExec.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/patternExec.o ${IOT_APP}/patternExec.cpp

${OBJECTDIR}/notify.o: ${IOT_APP}/notify.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/notify.o ${IOT_APP}/notify.cpp

${OBJECTDIR}/notifyExec.o: ${IOT_APP}/notifyExec.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/notifyExec.o ${IOT_APP}/notifyExec.cpp

${OBJECTDIR}/storage.o: ${IOT_APP}/storage.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/storage.o ${IOT_APP}/storage.cpp

${OBJECTDIR}/storageExec.o: ${IOT_APP}/storageExec.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/storageExec.o ${IOT_APP}/storageExec.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
