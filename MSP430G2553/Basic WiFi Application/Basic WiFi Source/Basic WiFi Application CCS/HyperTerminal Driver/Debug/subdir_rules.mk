################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
dispatcher.obj: C:/GitRepository/sweetWiFi/MSP430G2553/Basic\ WiFi\ Application/Basic\ WiFi\ Source/Source/HyperTerminal\ Driver/dispatcher.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=eabi -O2 --opt_for_speed=0 -g --include_path="C:/ccsv6/ccsv6/ccs_base/msp430/include" --include_path="C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Source/CC3000HostDriver" --include_path="C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Source/BasicWiFi Application" --include_path="C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/include" --advice:power=all --define=__MSP430G2553__ --define=CC3000_TINY_DRIVER --define=CC3000_UNENCRYPTED_SMART_CONFIG --define=__CCS__ --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="dispatcher.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

