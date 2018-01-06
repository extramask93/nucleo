################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/freemodbus/modbus/rtu/mbrtu.c 

OBJS += \
./Drivers/freemodbus/modbus/rtu/mbrtu.o 

C_DEPS += \
./Drivers/freemodbus/modbus/rtu/mbrtu.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/freemodbus/modbus/rtu/%.o: ../Drivers/freemodbus/modbus/rtu/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32L053xx -I"C:/STM32Toolchain/Nucleo/Inc" -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/modbus/rtu" -I../Drivers/freemodbus/modbus/port -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/modbus/include" -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/port" -I"C:/STM32Toolchain/Nucleo/Drivers/STM32L0xx_HAL_Driver/Inc" -I"C:/STM32Toolchain/Nucleo/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy" -I"C:/STM32Toolchain/Nucleo/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"C:/STM32Toolchain/Nucleo/Drivers/CMSIS/Include" -I"C:/STM32Toolchain/Nucleo/Inc"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


