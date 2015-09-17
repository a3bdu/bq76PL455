#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-C30_dsPIC33FJ128MC804.mk)" "nbproject/Makefile-local-C30_dsPIC33FJ128MC804.mk"
include nbproject/Makefile-local-C30_dsPIC33FJ128MC804.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=C30_dsPIC33FJ128MC804
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/140319_DEV_1401-BMSPROTO33TI-r07p03.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/140319_DEV_1401-BMSPROTO33TI-r07p03.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=main.c init.c timer.c spi.c adc.c can.c ecan1config.c ecan1drv.c "DEE Emulation 16-bit.c" "Flash Operations.s"

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/main.o ${OBJECTDIR}/init.o ${OBJECTDIR}/timer.o ${OBJECTDIR}/spi.o ${OBJECTDIR}/adc.o ${OBJECTDIR}/can.o ${OBJECTDIR}/ecan1config.o ${OBJECTDIR}/ecan1drv.o "${OBJECTDIR}/DEE Emulation 16-bit.o" "${OBJECTDIR}/Flash Operations.o"
POSSIBLE_DEPFILES=${OBJECTDIR}/main.o.d ${OBJECTDIR}/init.o.d ${OBJECTDIR}/timer.o.d ${OBJECTDIR}/spi.o.d ${OBJECTDIR}/adc.o.d ${OBJECTDIR}/can.o.d ${OBJECTDIR}/ecan1config.o.d ${OBJECTDIR}/ecan1drv.o.d "${OBJECTDIR}/DEE Emulation 16-bit.o.d" "${OBJECTDIR}/Flash Operations.o.d"

# Object Files
OBJECTFILES=${OBJECTDIR}/main.o ${OBJECTDIR}/init.o ${OBJECTDIR}/timer.o ${OBJECTDIR}/spi.o ${OBJECTDIR}/adc.o ${OBJECTDIR}/can.o ${OBJECTDIR}/ecan1config.o ${OBJECTDIR}/ecan1drv.o ${OBJECTDIR}/DEE\ Emulation\ 16-bit.o ${OBJECTDIR}/Flash\ Operations.o

# Source Files
SOURCEFILES=main.c init.c timer.c spi.c adc.c can.c ecan1config.c ecan1drv.c DEE Emulation 16-bit.c Flash Operations.s


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-C30_dsPIC33FJ128MC804.mk dist/${CND_CONF}/${IMAGE_TYPE}/140319_DEV_1401-BMSPROTO33TI-r07p03.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ128MC804
MP_LINKER_FILE_OPTION=,-Tp33FJ128MC804.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Flash\ Operations.o: Flash\ Operations.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Flash\ Operations.o.d 
	@${RM} ${OBJECTDIR}/Flash\ Operations.o.ok ${OBJECTDIR}/Flash\ Operations.o.err 
	@${RM} "${OBJECTDIR}/Flash Operations.o" 
	@${FIXDEPS} "${OBJECTDIR}/Flash Operations.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_AS} $(MP_EXTRA_AS_PRE)  "Flash Operations.s" -o "${OBJECTDIR}/Flash Operations.o" -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 --defsym=__MPLAB_DEBUG=1 --defsym=__ICD2RAM=1 --defsym=__DEBUG=1 --defsym=__MPLAB_DEBUGGER_ICD3=1 -g  -MD "${OBJECTDIR}/Flash Operations.o.d"$(MP_EXTRA_AS_POST)
	
else
${OBJECTDIR}/Flash\ Operations.o: Flash\ Operations.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Flash\ Operations.o.d 
	@${RM} ${OBJECTDIR}/Flash\ Operations.o.ok ${OBJECTDIR}/Flash\ Operations.o.err 
	@${RM} "${OBJECTDIR}/Flash Operations.o" 
	@${FIXDEPS} "${OBJECTDIR}/Flash Operations.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_AS} $(MP_EXTRA_AS_PRE)  "Flash Operations.s" -o "${OBJECTDIR}/Flash Operations.o" -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 -g  -MD "${OBJECTDIR}/Flash Operations.o.d"$(MP_EXTRA_AS_POST)
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o.ok ${OBJECTDIR}/main.o.err 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    
	
${OBJECTDIR}/init.o: init.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/init.o.d 
	@${RM} ${OBJECTDIR}/init.o.ok ${OBJECTDIR}/init.o.err 
	@${RM} ${OBJECTDIR}/init.o 
	@${FIXDEPS} "${OBJECTDIR}/init.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/init.o.d" -o ${OBJECTDIR}/init.o init.c    
	
${OBJECTDIR}/timer.o: timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/timer.o.d 
	@${RM} ${OBJECTDIR}/timer.o.ok ${OBJECTDIR}/timer.o.err 
	@${RM} ${OBJECTDIR}/timer.o 
	@${FIXDEPS} "${OBJECTDIR}/timer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/timer.o.d" -o ${OBJECTDIR}/timer.o timer.c    
	
${OBJECTDIR}/spi.o: spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/spi.o.d 
	@${RM} ${OBJECTDIR}/spi.o.ok ${OBJECTDIR}/spi.o.err 
	@${RM} ${OBJECTDIR}/spi.o 
	@${FIXDEPS} "${OBJECTDIR}/spi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/spi.o.d" -o ${OBJECTDIR}/spi.o spi.c    
	
${OBJECTDIR}/adc.o: adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/adc.o.d 
	@${RM} ${OBJECTDIR}/adc.o.ok ${OBJECTDIR}/adc.o.err 
	@${RM} ${OBJECTDIR}/adc.o 
	@${FIXDEPS} "${OBJECTDIR}/adc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/adc.o.d" -o ${OBJECTDIR}/adc.o adc.c    
	
${OBJECTDIR}/can.o: can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/can.o.d 
	@${RM} ${OBJECTDIR}/can.o.ok ${OBJECTDIR}/can.o.err 
	@${RM} ${OBJECTDIR}/can.o 
	@${FIXDEPS} "${OBJECTDIR}/can.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/can.o.d" -o ${OBJECTDIR}/can.o can.c    
	
${OBJECTDIR}/ecan1config.o: ecan1config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ecan1config.o.d 
	@${RM} ${OBJECTDIR}/ecan1config.o.ok ${OBJECTDIR}/ecan1config.o.err 
	@${RM} ${OBJECTDIR}/ecan1config.o 
	@${FIXDEPS} "${OBJECTDIR}/ecan1config.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ecan1config.o.d" -o ${OBJECTDIR}/ecan1config.o ecan1config.c    
	
${OBJECTDIR}/ecan1drv.o: ecan1drv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ecan1drv.o.d 
	@${RM} ${OBJECTDIR}/ecan1drv.o.ok ${OBJECTDIR}/ecan1drv.o.err 
	@${RM} ${OBJECTDIR}/ecan1drv.o 
	@${FIXDEPS} "${OBJECTDIR}/ecan1drv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ecan1drv.o.d" -o ${OBJECTDIR}/ecan1drv.o ecan1drv.c    
	
${OBJECTDIR}/DEE\ Emulation\ 16-bit.o: DEE\ Emulation\ 16-bit.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/DEE\ Emulation\ 16-bit.o.d 
	@${RM} ${OBJECTDIR}/DEE\ Emulation\ 16-bit.o.ok ${OBJECTDIR}/DEE\ Emulation\ 16-bit.o.err 
	@${RM} "${OBJECTDIR}/DEE Emulation 16-bit.o" 
	@${FIXDEPS} "${OBJECTDIR}/DEE Emulation 16-bit.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/DEE Emulation 16-bit.o.d" -o "${OBJECTDIR}/DEE Emulation 16-bit.o" "DEE Emulation 16-bit.c"    
	
else
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o.ok ${OBJECTDIR}/main.o.err 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    
	
${OBJECTDIR}/init.o: init.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/init.o.d 
	@${RM} ${OBJECTDIR}/init.o.ok ${OBJECTDIR}/init.o.err 
	@${RM} ${OBJECTDIR}/init.o 
	@${FIXDEPS} "${OBJECTDIR}/init.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/init.o.d" -o ${OBJECTDIR}/init.o init.c    
	
${OBJECTDIR}/timer.o: timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/timer.o.d 
	@${RM} ${OBJECTDIR}/timer.o.ok ${OBJECTDIR}/timer.o.err 
	@${RM} ${OBJECTDIR}/timer.o 
	@${FIXDEPS} "${OBJECTDIR}/timer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/timer.o.d" -o ${OBJECTDIR}/timer.o timer.c    
	
${OBJECTDIR}/spi.o: spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/spi.o.d 
	@${RM} ${OBJECTDIR}/spi.o.ok ${OBJECTDIR}/spi.o.err 
	@${RM} ${OBJECTDIR}/spi.o 
	@${FIXDEPS} "${OBJECTDIR}/spi.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/spi.o.d" -o ${OBJECTDIR}/spi.o spi.c    
	
${OBJECTDIR}/adc.o: adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/adc.o.d 
	@${RM} ${OBJECTDIR}/adc.o.ok ${OBJECTDIR}/adc.o.err 
	@${RM} ${OBJECTDIR}/adc.o 
	@${FIXDEPS} "${OBJECTDIR}/adc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/adc.o.d" -o ${OBJECTDIR}/adc.o adc.c    
	
${OBJECTDIR}/can.o: can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/can.o.d 
	@${RM} ${OBJECTDIR}/can.o.ok ${OBJECTDIR}/can.o.err 
	@${RM} ${OBJECTDIR}/can.o 
	@${FIXDEPS} "${OBJECTDIR}/can.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/can.o.d" -o ${OBJECTDIR}/can.o can.c    
	
${OBJECTDIR}/ecan1config.o: ecan1config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ecan1config.o.d 
	@${RM} ${OBJECTDIR}/ecan1config.o.ok ${OBJECTDIR}/ecan1config.o.err 
	@${RM} ${OBJECTDIR}/ecan1config.o 
	@${FIXDEPS} "${OBJECTDIR}/ecan1config.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ecan1config.o.d" -o ${OBJECTDIR}/ecan1config.o ecan1config.c    
	
${OBJECTDIR}/ecan1drv.o: ecan1drv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ecan1drv.o.d 
	@${RM} ${OBJECTDIR}/ecan1drv.o.ok ${OBJECTDIR}/ecan1drv.o.err 
	@${RM} ${OBJECTDIR}/ecan1drv.o 
	@${FIXDEPS} "${OBJECTDIR}/ecan1drv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ecan1drv.o.d" -o ${OBJECTDIR}/ecan1drv.o ecan1drv.c    
	
${OBJECTDIR}/DEE\ Emulation\ 16-bit.o: DEE\ Emulation\ 16-bit.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/DEE\ Emulation\ 16-bit.o.d 
	@${RM} ${OBJECTDIR}/DEE\ Emulation\ 16-bit.o.ok ${OBJECTDIR}/DEE\ Emulation\ 16-bit.o.err 
	@${RM} "${OBJECTDIR}/DEE Emulation 16-bit.o" 
	@${FIXDEPS} "${OBJECTDIR}/DEE Emulation 16-bit.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/DEE Emulation 16-bit.o.d" -o "${OBJECTDIR}/DEE Emulation 16-bit.o" "DEE Emulation 16-bit.c"    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/140319_DEV_1401-BMSPROTO33TI-r07p03.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/140319_DEV_1401-BMSPROTO33TI-r07p03.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/140319_DEV_1401-BMSPROTO33TI-r07p03.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/140319_DEV_1401-BMSPROTO33TI-r07p03.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/140319_DEV_1401-BMSPROTO33TI-r07p03.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/C30_dsPIC33FJ128MC804
	${RM} -r dist/C30_dsPIC33FJ128MC804

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
