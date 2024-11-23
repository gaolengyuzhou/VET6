################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../485/RS485.c \
../485/control.c 

OBJS += \
./485/RS485.o \
./485/control.o 

C_DEPS += \
./485/RS485.d \
./485/control.d 


# Each subdirectory must supply rules for building sources it contributes
485/%.o 485/%.su 485/%.cyclo: ../485/%.c 485/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/HW_485/485" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-485

clean-485:
	-$(RM) ./485/RS485.cyclo ./485/RS485.d ./485/RS485.o ./485/RS485.su ./485/control.cyclo ./485/control.d ./485/control.o ./485/control.su

.PHONY: clean-485

