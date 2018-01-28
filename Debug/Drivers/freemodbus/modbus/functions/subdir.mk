################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/freemodbus/modbus/functions/mbfunccoils.c \
../Drivers/freemodbus/modbus/functions/mbfuncdiag.c \
../Drivers/freemodbus/modbus/functions/mbfuncdisc.c \
../Drivers/freemodbus/modbus/functions/mbfuncholding.c \
../Drivers/freemodbus/modbus/functions/mbfuncinput.c \
../Drivers/freemodbus/modbus/functions/mbfuncother.c \
../Drivers/freemodbus/modbus/functions/mbutils.c 

OBJS += \
./Drivers/freemodbus/modbus/functions/mbfunccoils.o \
./Drivers/freemodbus/modbus/functions/mbfuncdiag.o \
./Drivers/freemodbus/modbus/functions/mbfuncdisc.o \
./Drivers/freemodbus/modbus/functions/mbfuncholding.o \
./Drivers/freemodbus/modbus/functions/mbfuncinput.o \
./Drivers/freemodbus/modbus/functions/mbfuncother.o \
./Drivers/freemodbus/modbus/functions/mbutils.o 

C_DEPS += \
./Drivers/freemodbus/modbus/functions/mbfunccoils.d \
./Drivers/freemodbus/modbus/functions/mbfuncdiag.d \
./Drivers/freemodbus/modbus/functions/mbfuncdisc.d \
./Drivers/freemodbus/modbus/functions/mbfuncholding.d \
./Drivers/freemodbus/modbus/functions/mbfuncinput.d \
./Drivers/freemodbus/modbus/functions/mbfuncother.d \
./Drivers/freemodbus/modbus/functions/mbutils.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/freemodbus/modbus/functions/%.o: ../Drivers/freemodbus/modbus/functions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32L053xx -I"C:/STM32Toolchain/Nucleo/Inc" -I"C:/STM32Toolchain/Nucleo/Drivers/STM32L0xx_HAL_Driver/Inc" -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/modbus/rtu" -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/modbus/include" -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/port" -I"C:/STM32Toolchain/Nucleo/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy" -I"C:/STM32Toolchain/Nucleo/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"C:/STM32Toolchain/Nucleo/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


