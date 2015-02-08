#####################################################################################################
# Included originally in the yagarto projects. Original Author : Michael Fischer & Hussam Al-Hertani
# Modified by Asmir Abdulahovic
#####################################################################################################
# Start of default section
#
CCPREFIX = arm-none-eabi-
CC   = $(CCPREFIX)gcc
CP   = $(CCPREFIX)objcopy
AS   = $(CCPREFIX)gcc -x assembler-with-cpp
GDBTUI = $(CCPREFIX)gdbtui
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary -S
MCU  = cortex-m0
 
# List all C defines here
DDEFS = -DSTM32F0XX -DUSE_STDPERIPH_DRIVER
#
# Define project name and Ram/Flash mode here
PROJECT        = tringZ1
 
# List C source files here
LIBSDIRS    = ./STM32F0-Discovery_FW_V1.0.0/Libraries
CORELIBDIR = $(LIBSDIRS)/CMSIS/Include
DEVDIR  = $(LIBSDIRS)/CMSIS/ST/STM32F0xx
STMSPDDIR    = $(LIBSDIRS)/STM32F0xx_StdPeriph_Driver
STMSPSRCDDIR = $(STMSPDDIR)/src
STMSPINCDDIR = $(STMSPDDIR)/inc
DISCOVERY    = ./STM32F0-Discovery_FW_V1.0.0/Utilities/STM32F0-Discovery

#list of src files to include in build process
SRC  = ./src/main.c
SRC += ./src/utils.c
SRC += ./src/usart_io.c
SRC += ./src/rtc_utils.c
SRC += ./src/system_stm32f0xx.c

## used parts of the STM-Library
SRC += $(STMSPSRCDDIR)/stm32f0xx_gpio.c
SRC += $(STMSPSRCDDIR)/stm32f0xx_rcc.c
SRC += $(STMSPSRCDDIR)/stm32f0xx_rtc.c
SRC += $(STMSPSRCDDIR)/stm32f0xx_usart.c
SRC += $(STMSPSRCDDIR)/stm32f0xx_misc.c
SRC += $(STMSPSRCDDIR)/stm32f0xx_pwr.c

# List assembly startup source file here
STARTUP = ./startup/startup_stm32f0xx.s
 
# List all directories here
INCDIRS = $(DEVDIR)/Include \
          $(CORELIBDIR) \
          $(STMSPINCDDIR) \
          $(DISCOVERY)    \
          ./inc     
# List the user directory to look for the libraries here
LIBDIRS += $(LIBSDIRS)
 
# List all user libraries here
LIBS =
 
# Define optimisation level here
OPT = -Os
 

# Define linker script file here
LINKER_SCRIPT = ./linker/stm32f0_linker.ld

 
INCDIR  = $(patsubst %,-I%, $(INCDIRS))
LIBDIR  = $(patsubst %,-L%, $(LIBDIRS))
LIB     = $(patsubst %,-l%, $(LIBS))
##reference only flags for run from ram...not used here
##DEFS    = $(DDEFS) $(UDEFS) -DRUN_FROM_FLASH=0 -DVECT_TAB_SRAM

## run from Flash
DEFS    = $(DDEFS) -DRUN_FROM_FLASH=1

OBJS  = $(STARTUP:.s=.o) $(SRC:.c=.o)
MCFLAGS = -mcpu=$(MCU)
 
ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb  -Wa,-amhls=$(<:.s=.lst) 
CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb   -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
LDFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb -nostartfiles -T$(LINKER_SCRIPT) -Wl,-Map=$(PROJECT).map,--cref,--no-warn-mismatch $(LIBDIR) $(LIB)
 
#
# makefile rules
#
all: $(OBJS) $(PROJECT).elf  $(PROJECT).hex $(PROJECT).bin
	$(TRGT)size $(PROJECT).elf
 
%.o: %.c
	$(CC) -c $(CPFLAGS) -I . $(INCDIR) $< -o $@

%.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@

%elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	$(HEX) $< $@
	
%bin: %elf
	$(BIN)  $< $@
	

flash_stlink: $(PROJECT).bin
	st-flash write $(PROJECT).bin 0x8000000

erase_stlink:
	st-flash erase

debug_stlink: $(PROJECT).elf
	urxvt -e st-util &
	$(GDBTUI) --eval-command="target remote localhost:4242"  $(PROJECT).elf -ex 'load'
		
clean:
	-rm -rf $(OBJS)
	-rm -rf $(PROJECT).elf
	-rm -rf $(PROJECT).map
	-rm -rf $(PROJECT).hex
	-rm -rf $(PROJECT).bin
	-rm -rf $(SRC:.c=.lst)
	-rm -rf $(ASRC:.s=.lst)
# *** EOF ***
