################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/lpm/tiny_lpm/stm32_lpm.c 

OBJS += \
./Utilities/lpm/tiny_lpm/stm32_lpm.o 

C_DEPS += \
./Utilities/lpm/tiny_lpm/stm32_lpm.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/lpm/tiny_lpm/stm32_lpm.o: ../Utilities/lpm/tiny_lpm/stm32_lpm.c Utilities/lpm/tiny_lpm/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../STM32_WPAN/App -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Utilities/lpm/tiny_lpm -I../Middlewares/ST/STM32_WPAN -I../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../Middlewares/ST/STM32_WPAN/utilities -I../Middlewares/ST/STM32_WPAN/ble/core -I../Middlewares/ST/STM32_WPAN/ble/core/auto -I../Middlewares/ST/STM32_WPAN/ble/core/template -I../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Utilities/sequencer -I../Middlewares/ST/STM32_WPAN/ble -I../Drivers/CMSIS/Include -I"C:/Users/STALKER/Desktop/CUBE WB TESTS/spectrometer_V2/MCU working project CubeIDE ver/Core/Libs/System" -I"C:/Users/STALKER/Desktop/CUBE WB TESTS/spectrometer_V2/MCU working project CubeIDE ver/Core/Libs/FatFS_src" -I"C:/Users/STALKER/Desktop/CUBE WB TESTS/spectrometer_V2/MCU working project CubeIDE ver/Core/Libs/Main" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Utilities/lpm/tiny_lpm/stm32_lpm.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

