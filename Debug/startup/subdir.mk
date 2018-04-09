################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../startup/startup_LPC54628.c 

OBJS += \
./startup/startup_LPC54628.o 

C_DEPS += \
./startup/startup_LPC54628.d 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -D__USE_CMSIS -DPRINTF_FLOAT_ENABLE=0 -DCR_INTEGER_PRINTF -DCPU_LPC54628J512ET180=1 -DSDK_DEBUGCONSOLE=0 -D__MCUXPRESSO -DDEBUG -DCPU_LPC54628J512ET180_cm4 -DCPU_LPC54628J512ET180 -DFSL_RTOS_FREE_RTOS -DSDK_OS_FREE_RTOS -I"/home/ben/projects/LPC54628/Ian/amazon-freertos/FreeRTOS/portable" -I"/home/ben/projects/LPC54628/Ian/amazon-freertos/portable" -I"/home/ben/projects/LPC54628/Ian/amazon-freertos/include" -I"/home/ben/projects/LPC54628/Ian/drivers/freertos" -I"/home/ben/projects/LPC54628/Ian/drivers" -I"/home/ben/projects/LPC54628/Ian/source" -I"/home/ben/projects/LPC54628/Ian/CMSIS" -I"/home/ben/projects/LPC54628/Ian/board" -I"/home/ben/projects/LPC54628/Ian" -I"/home/ben/projects/LPC54628/Ian/startup" -I"/home/ben/projects/LPC54628/Ian/utilities" -O0 -fno-common -g -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


