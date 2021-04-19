################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Utilities/sequencer/subdir.mk
-include Utilities/lpm/tiny_lpm/subdir.mk
-include STM32_WPAN/Target/subdir.mk
-include STM32_WPAN/App/subdir.mk
-include Middlewares/ST/STM32_WPAN/utilities/subdir.mk
-include Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/subdir.mk
-include Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci/subdir.mk
-include Middlewares/ST/STM32_WPAN/ble/svc/Src/subdir.mk
-include Middlewares/ST/STM32_WPAN/ble/core/template/subdir.mk
-include Middlewares/ST/STM32_WPAN/ble/core/auto/subdir.mk
-include Drivers/STM32WBxx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Core/Libs/System/uSD\ subsystem/subdir.mk
-include Core/Libs/System/TIM2\ subsystem/subdir.mk
-include Core/Libs/System/TIM17\ subsystem/subdir.mk
-include Core/Libs/System/TIM16\ subsystem/subdir.mk
-include Core/Libs/System/Radiation\ subsystem/subdir.mk
-include Core/Libs/System/RF\ subsystem/subdir.mk
-include Core/Libs/System/FS\ subsystem/subdir.mk
-include Core/Libs/System/DMA\ subsystem/subdir.mk
-include Core/Libs/System/ADC\ subsystem/subdir.mk
-include Core/Libs/System/subdir.mk
-include Core/Libs/Main/subdir.mk
-include Core/Libs/FatFS_src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := MCU working project CubeIDE ver
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
MCU\ working\ project\ CubeIDE\ ver.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
MCU\ working\ project\ CubeIDE\ ver.list \

OBJCOPY_HEX += \
MCU\ working\ project\ CubeIDE\ ver.hex \


# All Target
all: main-build

# Main-build Target
main-build: MCU\ working\ project\ CubeIDE\ ver.elf secondary-outputs

# Tool invocations
MCU\ working\ project\ CubeIDE\ ver.elf: $(OBJS) $(USER_OBJS) C:\Users\STALKER\Desktop\CUBE\ WB\ TESTS\spectrometer_V2\MCU\ working\ project\ CubeIDE\ ver\STM32WB55CGUX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "MCU working project CubeIDE ver.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\STALKER\Desktop\CUBE WB TESTS\spectrometer_V2\MCU working project CubeIDE ver\STM32WB55CGUX_FLASH.ld" --specs=nosys.specs -Wl,-Map="MCU working project CubeIDE ver.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

MCU\ working\ project\ CubeIDE\ ver.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "MCU working project CubeIDE ver.list"
	@echo 'Finished building: $@'
	@echo ' '

MCU\ working\ project\ CubeIDE\ ver.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "MCU working project CubeIDE ver.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified
.SECONDARY:

-include ../makefile.targets
