################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/TICCS/ccsv6/tools/compiler/c2000_6.2.5

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"../CMD/DSP2833x_Headers_nonBIOS.cmd" \
"./TIMER0.obj" \
"./SHJK_UART.obj" \
"./SHJK_PID.obj" \
"./SHJK_DAC.obj" \
"./DSP2833x_usDelay.obj" \
"./DSP2833x_Xintf.obj" \
"./DSP2833x_SysCtrl.obj" \
"./DSP2833x_Spi.obj" \
"./DSP2833x_Sci.obj" \
"./DSP2833x_PieVect.obj" \
"./DSP2833x_PieCtrl.obj" \
"./DSP2833x_GlobalVariableDefs.obj" \
"./DSP2833x_DefaultIsr.obj" \
"./DSP2833x_CpuTimers.obj" \
"./DSP2833x_CodeStartBranch.obj" \
"./DSP2833x_Adc.obj" \
"./DSP2833x_ADC_cal.obj" \
"../CMD/F28335.cmd" \
-l"rts2800_fpu32.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include SRC/subdir_vars.mk
-include CMD/subdir_vars.mk
-include SRC/subdir_rules.mk
-include CMD/subdir_rules.mk
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
EXE_OUTPUTS += \
TIMER0.out \

EXE_OUTPUTS__QUOTED += \
"TIMER0.out" \


# All Target
all: TIMER0.out

# Tool invocations
TIMER0.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C2000 Linker'
	"D:/TICCS/ccsv6/tools/compiler/c2000_6.2.5/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="LARGE_MODEL" --diag_warning=225 --large_memory_model --float_support=fpu32 -z -m"TIMER0.map" --stack_size=1000 --heap_size=1000 --warn_sections -i"D:/TICCS/ccsv6/tools/compiler/c2000_6.2.5/lib" -i"D:/TICCS/ccsv6/tools/compiler/c2000_6.2.5/include" -i"F:/DSPprj/lab3-timer0_LED" --reread_libs --xml_link_info="TIMER0_linkInfo.xml" --rom_model -o "TIMER0.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "SRC\DSP2833x_Adc.pp" "SRC\DSP2833x_CpuTimers.pp" "SRC\DSP2833x_DefaultIsr.pp" "SRC\DSP2833x_GlobalVariableDefs.pp" "SRC\DSP2833x_PieCtrl.pp" "SRC\DSP2833x_PieVect.pp" "SRC\DSP2833x_Sci.pp" "SRC\DSP2833x_Spi.pp" "SRC\DSP2833x_SysCtrl.pp" "SRC\DSP2833x_Xintf.pp" "SRC\SHJK_DAC.pp" "SRC\SHJK_PID.pp" "SRC\SHJK_UART.pp" "SRC\TIMER0.pp" 
	-$(RM) "DSP2833x_ADC_cal.obj" "DSP2833x_Adc.obj" "DSP2833x_CodeStartBranch.obj" "DSP2833x_CpuTimers.obj" "DSP2833x_DefaultIsr.obj" "DSP2833x_GlobalVariableDefs.obj" "DSP2833x_PieCtrl.obj" "DSP2833x_PieVect.obj" "DSP2833x_Sci.obj" "DSP2833x_Spi.obj" "DSP2833x_SysCtrl.obj" "DSP2833x_Xintf.obj" "DSP2833x_usDelay.obj" "SHJK_DAC.obj" "SHJK_PID.obj" "SHJK_UART.obj" "TIMER0.obj" 
	-$(RM) "SRC\DSP2833x_ADC_cal.pp" "SRC\DSP2833x_CodeStartBranch.pp" "SRC\DSP2833x_usDelay.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

