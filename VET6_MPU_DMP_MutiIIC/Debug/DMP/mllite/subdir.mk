################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DMP/mllite/data_builder.c \
../DMP/mllite/hal_outputs.c \
../DMP/mllite/message_layer.c \
../DMP/mllite/ml_math_func.c \
../DMP/mllite/mlmath.c \
../DMP/mllite/mpl.c \
../DMP/mllite/results_holder.c \
../DMP/mllite/start_manager.c \
../DMP/mllite/storage_manager.c 

OBJS += \
./DMP/mllite/data_builder.o \
./DMP/mllite/hal_outputs.o \
./DMP/mllite/message_layer.o \
./DMP/mllite/ml_math_func.o \
./DMP/mllite/mlmath.o \
./DMP/mllite/mpl.o \
./DMP/mllite/results_holder.o \
./DMP/mllite/start_manager.o \
./DMP/mllite/storage_manager.o 

C_DEPS += \
./DMP/mllite/data_builder.d \
./DMP/mllite/hal_outputs.d \
./DMP/mllite/message_layer.d \
./DMP/mllite/ml_math_func.d \
./DMP/mllite/mlmath.d \
./DMP/mllite/mpl.d \
./DMP/mllite/results_holder.d \
./DMP/mllite/start_manager.d \
./DMP/mllite/storage_manager.d 


# Each subdirectory must supply rules for building sources it contributes
DMP/mllite/%.o DMP/mllite/%.su DMP/mllite/%.cyclo: ../DMP/mllite/%.c DMP/mllite/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_DMP -DMPL_LOG_NDEBUG=1 -DEMPL -DMPU9250 -DEMPL_TARGET_STM32F4 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/driver/eMPL" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/driver/include" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/driver/stm32L" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/eMPL-hal" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/mllite" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/DMP/mpl" -I"D:/STM32CubeIDESTM32CubeIDEworkspace_1.16.1/MPU/icode" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DMP-2f-mllite

clean-DMP-2f-mllite:
	-$(RM) ./DMP/mllite/data_builder.cyclo ./DMP/mllite/data_builder.d ./DMP/mllite/data_builder.o ./DMP/mllite/data_builder.su ./DMP/mllite/hal_outputs.cyclo ./DMP/mllite/hal_outputs.d ./DMP/mllite/hal_outputs.o ./DMP/mllite/hal_outputs.su ./DMP/mllite/message_layer.cyclo ./DMP/mllite/message_layer.d ./DMP/mllite/message_layer.o ./DMP/mllite/message_layer.su ./DMP/mllite/ml_math_func.cyclo ./DMP/mllite/ml_math_func.d ./DMP/mllite/ml_math_func.o ./DMP/mllite/ml_math_func.su ./DMP/mllite/mlmath.cyclo ./DMP/mllite/mlmath.d ./DMP/mllite/mlmath.o ./DMP/mllite/mlmath.su ./DMP/mllite/mpl.cyclo ./DMP/mllite/mpl.d ./DMP/mllite/mpl.o ./DMP/mllite/mpl.su ./DMP/mllite/results_holder.cyclo ./DMP/mllite/results_holder.d ./DMP/mllite/results_holder.o ./DMP/mllite/results_holder.su ./DMP/mllite/start_manager.cyclo ./DMP/mllite/start_manager.d ./DMP/mllite/start_manager.o ./DMP/mllite/start_manager.su ./DMP/mllite/storage_manager.cyclo ./DMP/mllite/storage_manager.d ./DMP/mllite/storage_manager.o ./DMP/mllite/storage_manager.su

.PHONY: clean-DMP-2f-mllite

