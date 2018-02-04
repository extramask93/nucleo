################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/BV1750FVI.c \
../Src/SoilMoisture.c \
../Src/adc.c \
../Src/am2302.c \
../Src/battery.c \
../Src/co2.c \
../Src/comp.c \
../Src/gpio.c \
../Src/i2c.c \
../Src/lcd.c \
../Src/main.c \
../Src/mbtask.c \
../Src/power.c \
../Src/rtc.c \
../Src/spi.c \
../Src/stm32l0xx_hal_msp.c \
../Src/stm32l0xx_it.c \
../Src/system_stm32l0xx.c \
../Src/tim.c \
../Src/usart.c 

OBJS += \
./Src/BV1750FVI.o \
./Src/SoilMoisture.o \
./Src/adc.o \
./Src/am2302.o \
./Src/battery.o \
./Src/co2.o \
./Src/comp.o \
./Src/gpio.o \
./Src/i2c.o \
./Src/lcd.o \
./Src/main.o \
./Src/mbtask.o \
./Src/power.o \
./Src/rtc.o \
./Src/spi.o \
./Src/stm32l0xx_hal_msp.o \
./Src/stm32l0xx_it.o \
./Src/system_stm32l0xx.o \
./Src/tim.o \
./Src/usart.o 

C_DEPS += \
./Src/BV1750FVI.d \
./Src/SoilMoisture.d \
./Src/adc.d \
./Src/am2302.d \
./Src/battery.d \
./Src/co2.d \
./Src/comp.d \
./Src/gpio.d \
./Src/i2c.d \
./Src/lcd.d \
./Src/main.d \
./Src/mbtask.d \
./Src/power.d \
./Src/rtc.d \
./Src/spi.d \
./Src/stm32l0xx_hal_msp.d \
./Src/stm32l0xx_it.d \
./Src/system_stm32l0xx.d \
./Src/tim.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32L053xx -I"C:/STM32Toolchain/Nucleo/Inc" -I"C:/STM32Toolchain/Nucleo/Drivers/STM32L0xx_HAL_Driver/Inc" -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/modbus/rtu" -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/modbus/include" -I"C:/STM32Toolchain/Nucleo/Drivers/freemodbus/port" -I"C:/STM32Toolchain/Nucleo/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy" -I"C:/STM32Toolchain/Nucleo/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"C:/STM32Toolchain/Nucleo/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


