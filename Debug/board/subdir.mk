################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/board.c \
../board/clock_config.c \
../board/peripherals.c \
../board/pin_mux.c 

OBJS += \
./board/board.o \
./board/clock_config.o \
./board/peripherals.o \
./board/pin_mux.o 

C_DEPS += \
./board/board.d \
./board/clock_config.d \
./board/peripherals.d \
./board/pin_mux.d 


# Each subdirectory must supply rules for building sources it contributes
board/%.o: ../board/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -D__USE_CMSIS -DPRINTF_FLOAT_ENABLE=0 -DCR_INTEGER_PRINTF -DCPU_LPC54628J512ET180=1 -DSDK_DEBUGCONSOLE=0 -D__MCUXPRESSO -DDEBUG -DCPU_LPC54628J512ET180_cm4 -DCPU_LPC54628J512ET180 -I"/home/ben/projects/mcuxpresso_workspace/Joy/board" -I"/home/ben/projects/mcuxpresso_workspace/Joy/source" -I"/home/ben/projects/mcuxpresso_workspace/Joy" -I"/home/ben/projects/mcuxpresso_workspace/Joy/drivers" -I"/home/ben/projects/mcuxpresso_workspace/Joy/CMSIS" -I"/home/ben/projects/mcuxpresso_workspace/Joy/startup" -I"/home/ben/projects/mcuxpresso_workspace/Joy/utilities" -O0 -fno-common -g -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

