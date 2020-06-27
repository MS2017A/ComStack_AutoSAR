################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs901/ccs/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccs901/ccs/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ITI/AUTO SAR/projects/freertos_demo" --include_path="D:/ITI/AUTO SAR/Code Composer/TivaWare_C_Series-2.1.4.178/examples/boards/ek-tm4c123gxl" --include_path="D:/ITI/AUTO SAR/Code Composer/TivaWare_C_Series-2.1.4.178" --include_path="D:/ITI/AUTO SAR/Code Composer/TivaWare_C_Series-2.1.4.178/third_party" --include_path="D:/ITI/AUTO SAR/Code Composer/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/include" --include_path="D:/ITI/AUTO SAR/Code Composer/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS" --include_path="D:/ITI/AUTO SAR/Code Composer/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="D:/ITI/AUTO SAR/projects/freertos_demo/configuration" --include_path="D:/ITI/AUTO SAR/projects/freertos_demo/Det/include" --include_path="D:/ITI/AUTO SAR/projects/freertos_demo/CommunicationStack/CanDrv/include" --include_path="D:/ITI/AUTO SAR/projects/freertos_demo/CommunicationStack/CanIf/include" --include_path="D:/ITI/AUTO SAR/projects/freertos_demo/CommunicationStack/CanTp/include" --include_path="D:/ITI/AUTO SAR/projects/freertos_demo/CommunicationStack/Com/include" --include_path="D:/ITI/AUTO SAR/projects/freertos_demo/CommunicationStack/PduR/include" --include_path="D:/ITI/AUTO SAR/projects/freertos_demo/Common" --include_path="D:/ITI/AUTO SAR/projects/freertos_demo/MCU/include" --include_path="D:/ITI/AUTO SAR/projects/freertos_demo/third_party/FreeRTOS/include" --advice:power=all -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


