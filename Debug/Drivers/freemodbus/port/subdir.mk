################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/freemodbus/port/mbcrc.c \
../Drivers/freemodbus/port/portevent.c \
../Drivers/freemodbus/port/portserial.c \
../Drivers/freemodbus/port/porttimer.c 

OBJS += \
./Drivers/freemodbus/port/mbcrc.o \
./Drivers/freemodbus/port/portevent.o \
./Drivers/freemodbus/port/portserial.o \
./Drivers/freemodbus/port/porttimer.o 

C_DEPS += \
./Drivers/freemodbus/port/mbcrc.d \
./Drivers/freemodbus/port/portevent.d \
./Drivers/freemodbus/port/portserial.d \
./Drivers/freemodbus/port/porttimer.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/freemodbus/port/%.o: ../Drivers/freemodbus/port/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32L053xx -I"C:/STM32Toolchain/Nucleo/Inc" -I"C:/STM32Toolchain/Nucleo/Drivers/STM32L0xx_HAL_Driver/Inc" -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/modbus/rtu" -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/modbus/include" -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/port" -I"C:/STM32Toolchain/Nucleo/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy" -I"C:/STM32Toolchain/Nucleo/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"C:/STM32Toolchain/Nucleo/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


