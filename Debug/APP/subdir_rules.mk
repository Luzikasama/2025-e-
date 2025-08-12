################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
APP/%.o: ../APP/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"E:/Apps/ccs/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/14405/workspace_ccstheia/empty_LP_MSPM0G3507_nortos_ticlang" -I"C:/Users/14405/workspace_ccstheia/empty_LP_MSPM0G3507_nortos_ticlang/APP" -I"C:/Users/14405/workspace_ccstheia/empty_LP_MSPM0G3507_nortos_ticlang/Modules" -I"C:/Users/14405/workspace_ccstheia/empty_LP_MSPM0G3507_nortos_ticlang/Core" -I"C:/Users/14405/workspace_ccstheia/empty_LP_MSPM0G3507_nortos_ticlang/Debug" -I"E:/Apps/ti/mspm0_sdk_2_01_00_03/source/third_party/CMSIS/Core/Include" -I"E:/Apps/ti/mspm0_sdk_2_01_00_03/source" -gdwarf-3 -MMD -MP -MF"APP/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu11 $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


