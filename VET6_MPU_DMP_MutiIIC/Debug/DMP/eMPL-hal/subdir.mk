################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DMP/eMPL-hal/eMPL_outputs.c 

OBJS += \
./DMP/eMPL-hal/eMPL_outputs.o 

C_DEPS += \
./DMP/eMPL-hal/eMPL_outputs.d 


# Each subdirectory must supply rules for building sources it contributes
DMP/eMPL-hal/%.o DMP/eMPL-hal/%.su DMP/eMPL-hal/%.cyclo: ../DMP/eMPL-hal/%.c DMP/eMPL-hal/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_DMP -DMPL_LOG_NDEBUG=1 -DEMPL -DMPU9250 -DEMPL_TARGET_STM32F4 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/driver/eMPL" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/driver/include" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/driver/stm32L" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/eMPL-hal" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/mllite" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/mpl" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/icode" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DMP-2f-eMPL-2d-hal

clean-DMP-2f-eMPL-2d-hal:
	-$(RM) ./DMP/eMPL-hal/eMPL_outputs.cyclo ./DMP/eMPL-hal/eMPL_outputs.d ./DMP/eMPL-hal/eMPL_outputs.o ./DMP/eMPL-hal/eMPL_outputs.su

.PHONY: clean-DMP-2f-eMPL-2d-hal

