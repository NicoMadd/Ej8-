################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Collections/list.c \
../src/Collections/queue.c 

OBJS += \
./src/Collections/list.o \
./src/Collections/queue.o 

C_DEPS += \
./src/Collections/list.d \
./src/Collections/queue.d 


# Each subdirectory must supply rules for building sources it contributes
src/Collections/%.o: ../src/Collections/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


