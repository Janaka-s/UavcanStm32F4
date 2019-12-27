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
LINKER_FLAGS= -lm -mthumb -mhard-float -mcpu=cortex-m4 \
			-mfpu=fpv4-sp-d16 -Wl,--gc-sections -T$(LINK_SCRIPT) -static  \
			-Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group  \
			-specs=nosys.specs  -Wl,-cref "-Wl,-Map=$(BIN_DIR)/Stm32Uavcan.map" \
			-Wl,--defsym=malloc_getpagesize_P=0x1000
LINK_SCRIPT="stm32f4_flash.ld"

UDEFS = -DUAVCAN_STM32_TIMER_NUMBER=7          \
         -DUAVCAN_STM32_NUM_IFACES=1            \
         -DUAVCAN_STM32_BAREMETAL=1 				\
         -DUAVCAN_CPP_VERSION=UAVCAN_CPP11      \
		 -DSTM32F4xx=1
		
#          -DUAVCAN_STM32_IRQ_PRIORITY_MASK=4		\

include Libraries/libuavcan/include.mk
UINCDIR += -I$(LIBUAVCAN_INC)

include Libraries/libuavcan_stm32/driver/include.mk
UINCDIR += -I$(LIBUAVCAN_STM32_INC)
         
UINCDIR += -ILibraries/libuavcan/dsdlc_generated

ASSEMBLER_FLAGS=-c -g -O0 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -mhard-float  \
			-D"USE_STM32F4_DISCOVERY" -D"HSE_VALUE=8000000" -D"STM32F4XX" -D"STM32F40XX" \
			-D"USE_STDPERIPH_DRIVER"  -x assembler-with-cpp  \
			$(UDEFS) \
			-IUtilities/STM32F4-Discovery \
			-Isrc -ILibraries/CMSIS/Include -ILibraries/Device/STM32F4xx/Include \
			-ILibraries/STM32F4xx_StdPeriph_Driver/inc 
COMPILER_FLAGS=-c -g -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -O0 -Wall -ffunction-sections -fdata-sections \
			-mthumb -mhard-float  -D"USE_STM32F4_DISCOVERY" -D"HSE_VALUE=8000000" -D"STM32F4XX" -D"STM32F40XX" \
			-D"USE_STDPERIPH_DRIVER" \
			$(UDEFS) \
			-IUtilities/STM32F4-Discovery -Isrc -ILibraries/CMSIS/Include \
			-ILibraries/Device/STM32F4xx/Include -ILibraries/STM32F4xx_StdPeriph_Driver/inc \
			-I$(UINCDIR)
CXXCOMPILER_FLAGS=-fno-threadsafe-statics -c -g -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -O0 -Wall -ffunction-sections -fdata-sections -mthumb \
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
	$(CXX) -o "$(BIN_DIR)/Stm32Uavcan.elf" $(OBJS) $(LINKER_FLAGS)

clean:
	$(RM) $(OBJS) "$(BIN_DIR)/Stm32Uavcan.elf" "$(BIN_DIR)/Stm32Uavcan.map"


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
