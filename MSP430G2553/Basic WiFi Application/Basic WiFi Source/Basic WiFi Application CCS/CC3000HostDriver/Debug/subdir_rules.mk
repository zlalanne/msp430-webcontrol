################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
cc3000_common.obj: C:/GitRepository/sweetWiFi/MSP430G2553/Basic\ WiFi\ Application/Basic\ WiFi\ Source/Source/CC3000HostDriver/cc3000_common.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=eabi -O2 --opt_for_speed=0 -g --include_path="C:/ccsv6/ccsv6/ccs_base/msp430/include" --include_path="C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Source/inc" --include_path="C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/include" --advice:power=all --define=__MSP430G2553__ --define=CC3000_TINY_DRIVER --define=CC3000_UNENCRYPTED_SMART_CONFIG --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="cc3000_common.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

evnt_handler.obj: C:/GitRepository/sweetWiFi/MSP430G2553/Basic\ WiFi\ Application/Basic\ WiFi\ Source/Source/CC3000HostDriver/evnt_handler.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=eabi -O2 --opt_for_speed=0 -g --include_path="C:/ccsv6/ccsv6/ccs_base/msp430/include" --include_path="C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Source/inc" --include_path="C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/include" --advice:power=all --define=__MSP430G2553__ --define=CC3000_TINY_DRIVER --define=CC3000_UNENCRYPTED_SMART_CONFIG --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="evnt_handler.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hci.obj: C:/GitRepository/sweetWiFi/MSP430G2553/Basic\ WiFi\ Application/Basic\ WiFi\ Source/Source/CC3000HostDriver/hci.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=eabi -O2 --opt_for_speed=0 -g --include_path="C:/ccsv6/ccsv6/ccs_base/msp430/include" --include_path="C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Source/inc" --include_path="C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/include" --advice:power=all --define=__MSP430G2553__ --define=CC3000_TINY_DRIVER --define=CC3000_UNENCRYPTED_SMART_CONFIG --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="hci.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

netapp.obj: C:/GitRepository/sweetWiFi/MSP430G2553/Basic\ WiFi\ Application/Basic\ WiFi\ Source/Source/CC3000HostDriver/netapp.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=eabi -O2 --opt_for_speed=0 -g --include_path="C:/ccsv6/ccsv6/ccs_base/msp430/include" --include_path="C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Source/inc" --include_path="C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/include" --advice:power=all --define=__MSP430G2553__ --define=CC3000_TINY_DRIVER --define=CC3000_UNENCRYPTED_SMART_CONFIG --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="netapp.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

nvmem.obj: C:/GitRepository/sweetWiFi/MSP430G2553/Basic\ WiFi\ Application/Basic\ WiFi\ Source/Source/CC3000HostDriver/nvmem.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=eabi -O2 --opt_for_speed=0 -g --include_path="C:/ccsv6/ccsv6/ccs_base/msp430/include" --include_path="C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Source/inc" --include_path="C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/include" --advice:power=all --define=__MSP430G2553__ --define=CC3000_TINY_DRIVER --define=CC3000_UNENCRYPTED_SMART_CONFIG --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="nvmem.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

socket.obj: C:/GitRepository/sweetWiFi/MSP430G2553/Basic\ WiFi\ Application/Basic\ WiFi\ Source/Source/CC3000HostDriver/socket.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=eabi -O2 --opt_for_speed=0 -g --include_path="C:/ccsv6/ccsv6/ccs_base/msp430/include" --include_path="C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Source/inc" --include_path="C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/include" --advice:power=all --define=__MSP430G2553__ --define=CC3000_TINY_DRIVER --define=CC3000_UNENCRYPTED_SMART_CONFIG --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="socket.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

wlan.obj: C:/GitRepository/sweetWiFi/MSP430G2553/Basic\ WiFi\ Application/Basic\ WiFi\ Source/Source/CC3000HostDriver/wlan.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp --abi=eabi -O2 --opt_for_speed=0 -g --include_path="C:/ccsv6/ccsv6/ccs_base/msp430/include" --include_path="C:/GitRepository/sweetWiFi/MSP430G2553/Basic WiFi Application/Basic WiFi Source/Source/inc" --include_path="C:/ccsv6/ccsv6/tools/compiler/msp430_4.2.1/include" --advice:power=all --define=__MSP430G2553__ --define=CC3000_TINY_DRIVER --define=CC3000_UNENCRYPTED_SMART_CONFIG --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="wlan.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


