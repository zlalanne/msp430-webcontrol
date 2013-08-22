################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./sweetWiFi.obj" \
"./board.obj" \
"../lnk_msp430g2553.cmd" \
"C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Basic WiFi Application CCS/CC3000 Spi/Debug/CC3000 Spi.lib" \
"C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Basic WiFi Application CCS/CC3000HostDriver/Debug/CC3000HostDriver.lib" \
"C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Basic WiFi Application CCS/HyperTerminal Driver/Debug/HyperTerminal Driver.lib" \
-l"libc.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Library/subdir_vars.mk
-include subdir_rules.mk
-include Library/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: BasicWiFi\ Application.out

# Tool invocations
BasicWiFi\ Application.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP430 Linker'
	"C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=eabi -O2 --opt_for_speed=0 -g --advice:power="all" --define=__MSP430G2553__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --define=CC3000_TINY_DRIVER --define=__CCS__ --diag_warning=225 --display_error_number --printf_support=minimal -z --stack_size=200 -m"BasicWiFi Application.map" --heap_size=0 -i"C:/ccsv6/ccsv6/ccs_base/msp430/include" -i"C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/lib" -i"C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/include" --reread_libs --warn_sections --display_error_number --xml_link_info="BasicWiFi Application_linkInfo.xml" --rom_model -o "BasicWiFi Application.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(MSP430_EXECUTABLE_OUTPUTS__QUOTED) "BasicWiFi Application.out"
	-$(RM) "board.pp" "sweetWiFi.pp" 
	-$(RM) "board.obj" "sweetWiFi.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

