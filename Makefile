###############################################################################
#
#  	    File        : Makefile
#
#       Abstract    : Example Makefile for a C++ Project
#
#       Environment : Atollic TrueSTUDIO(R)
#
###############################################################################

# System configuration
CC = arm-atollic-eabi-gcc
CXX = arm-atollic-eabi-g++
RM=rm -rf

# Assembler, Compiler and Linker flags and linker script settings
LINK_SCRIPT="stm32f4_flash.ld"
LINKER_FLAGS= -lm -mthumb -mhard-float -mcpu=cortex-m4 \
			-mfpu=fpv4-sp-d16 -Wl,--gc-sections -T$(LINK_SCRIPT) -static  \
			-Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group  \
			-specs=nosys.specs  -Wl,-cref "-Wl,-Map=$(BIN_DIR)/Stm32UavcanNew.map" \
			-Wl,--defsym=malloc_getpagesize_P=0x1000

UDEFS = -DUAVCAN_STM32_TIMER_NUMBER=2          \
         -DUAVCAN_STM32_NUM_IFACES=1            \
         -DUAVCAN_STM32_BAREMETAL=1 				\
         -DUAVCAN_CPP_VERSION=UAVCAN_CPP11      \
		 -DSTM32F4xx=1
		
#          -DUAVCAN_STM32_IRQ_PRIORITY_MASK=4		\

include Libraries/libuavcan/libuavcan/include.mk
UINCDIR += -I$(LIBUAVCAN_INC)

include Libraries/libuavcan/libuavcan_drivers/stm32/driver/include.mk
UINCDIR += -I$(LIBUAVCAN_STM32_INC)
         
UINCDIR += -ILibraries/libuavcan/dsdlc_generated

ASSEMBLER_FLAGS=-c -g -O0 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -mhard-float  \
			-D"USE_STM32F4_DISCOVERY" -D"HSE_VALUE=8000000" -D"STM32F4XX" -D"STM32F40XX" \
			-D"USE_STDPERIPH_DRIVER"  -x assembler-with-cpp  \
			$(UDEFS) \
			-IUtilities/STM32F4-Discovery \
			-Isrc -ILibraries/CMSIS/Include -ILibraries/Device/STM32F4xx/Include \
			-ILibraries/STM32F4xx_StdPeriph_Driver/inc 
COMPILER_FLAGS=-c -g -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -O0 -Wextra -ffunction-sections -fdata-sections \
			-mthumb -mhard-float  -D"USE_STM32F4_DISCOVERY" -D"HSE_VALUE=8000000" -D"STM32F4XX" -D"STM32F40XX" \
			-D"USE_STDPERIPH_DRIVER" \
			$(UDEFS) \
			-IUtilities/STM32F4-Discovery -Isrc -ILibraries/CMSIS/Include \
			-ILibraries/Device/STM32F4xx/Include -ILibraries/STM32F4xx_StdPeriph_Driver/inc \
			-I$(UINCDIR)
CXXCOMPILER_FLAGS=-fno-threadsafe-statics -c -g -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -O0 -Wextra -ffunction-sections -fdata-sections -mthumb \
			-mhard-float  -fno-exceptions -fno-rtti -D"USE_STM32F4_DISCOVERY" -D"HSE_VALUE=8000000" -D"STM32F4XX" -D"STM32F40XX" \
			-D"USE_STDPERIPH_DRIVER"   \
			$(UDEFS) \
			-IUtilities/STM32F4-Discovery -Isrc -ILibraries/CMSIS/Include -ILibraries/Device/STM32F4xx/Include \
			-ILibraries/STM32F4xx_StdPeriph_Driver/inc \
			$(UINCDIR)

# Define output directory
OBJECT_DIR = Debug
BIN_DIR = $(OBJECT_DIR)

# Define sources and objects
SRC := $(wildcard */*/*/*/*/*/*/*.c) \
	$(wildcard */*/*/*/*/*/*.c) \
	$(wildcard */*/*/*/*/*.c) \
	$(wildcard */*/*/*/*.c) \
	$(wildcard */*/*/*.c) \
	$(wildcard */*/*.c) \
	$(wildcard */*.c) \
	$(wildcard */*/*/*/*/*/*/*.cpp) \
	$(wildcard */*/*/*/*/*/*.cpp) \
	$(wildcard */*/*/*/*/*.cpp) \
	$(wildcard */*/*/*/*.cpp) \
	$(wildcard */*/*/*.cpp) \
	$(wildcard */*/*.cpp) \
	$(wildcard */*.cpp)
SRCSASM := $(wildcard */*/*/*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*.s) \
	$(wildcard */*/*/*/*.s) \
	$(wildcard */*/*/*.s) \
	$(wildcard */*/*.s) \
	$(wildcard */*.s)
OBJS := $(SRC:%.c=$(OBJECT_DIR)/%.o) $(SRCSASM:%.s=$(OBJECT_DIR)/%.o)
OBJS := $(OBJS:%.S=$(OBJECT_DIR)/%.o)  
OBJS := $(OBJS:%.cpp=$(OBJECT_DIR)/%.o)


###############
# Build project
# Major targets
###############
all: buildelf

buildelf: $(OBJS) 
	$(CXX) -o "$(BIN_DIR)/Stm32UavcanNew.elf" $(OBJS) $(LINKER_FLAGS)
	arm-atollic-eabi-objcopy -O ihex "$(BIN_DIR)/Stm32UavcanNew.elf" "$(BIN_DIR)/Stm32UavcanNew.hex"
	/opt/Atollic_TrueSTUDIO_for_STM32_x86_64_9.3.0/ide/jre/bin/java -jar /opt/Atollic_TrueSTUDIO_for_STM32_x86_64_9.3.0/Tools/arm-atollic-reports.jar sizeinfo list "$(BIN_DIR)/Stm32UavcanNew.elf"

clean:
	$(RM) $(OBJS) "$(BIN_DIR)/Stm32UavcanNew.elf" "$(BIN_DIR)/Stm32UavcanNew.map"


##################
# Specific targets
##################
$(OBJECT_DIR)/src/main.o: src/main.cpp
	@mkdir -p $(dir $@) 2> /dev/null
	$(CXX) $(CXXCOMPILER_FLAGS) src/main.cpp -o $(OBJECT_DIR)/src/main.o 


##################
# Implicit targets
##################
$(OBJECT_DIR)/%.o: %.c
	@mkdir -p $(dir $@) 2> /dev/null
	$(CC) $(COMPILER_FLAGS) $< -o $@

$(OBJECT_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@) 2> /dev/null
	$(CXX) $(CXXCOMPILER_FLAGS) $< -o $@

$(OBJECT_DIR)/%.o: %.s
	@mkdir -p $(dir $@) 2> /dev/null
	$(CC) $(ASSEMBLER_FLAGS) $< -o $@
	
$(OBJECT_DIR)/%.o: %.S
	@mkdir -p $(dir $@) 2> /dev/null
	$(CC) $(ASSEMBLER_FLAGS) $< -o $@
