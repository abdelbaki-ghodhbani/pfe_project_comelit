################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/circular_buffer.c \
../Src/esp82xx_driver.c \
../Src/esp82xx_lib.c \
../Src/hardware_module.c \
../Src/main.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/circular_buffer.o \
./Src/esp82xx_driver.o \
./Src/esp82xx_lib.o \
./Src/hardware_module.o \
./Src/main.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/circular_buffer.d \
./Src/esp82xx_driver.d \
./Src/esp82xx_lib.d \
./Src/hardware_module.d \
./Src/main.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F401RETx -DSTM32F4 -DSTM32F401xE -c -I../Inc -I"C:/Users/A/Desktop/Embedded-STM32-WIFI/F4_chip_headers/chip_headers/CMSIS/Include" -I"C:/Users/A/Desktop/Embedded-STM32-WIFI/F4_chip_headers/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/circular_buffer.cyclo ./Src/circular_buffer.d ./Src/circular_buffer.o ./Src/circular_buffer.su ./Src/esp82xx_driver.cyclo ./Src/esp82xx_driver.d ./Src/esp82xx_driver.o ./Src/esp82xx_driver.su ./Src/esp82xx_lib.cyclo ./Src/esp82xx_lib.d ./Src/esp82xx_lib.o ./Src/esp82xx_lib.su ./Src/hardware_module.cyclo ./Src/hardware_module.d ./Src/hardware_module.o ./Src/hardware_module.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

