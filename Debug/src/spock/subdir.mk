################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/spock/Mascota.c \
../src/spock/Mision.c \
../src/spock/Spock.c \
../src/spock/Villano.c 

OBJS += \
./src/spock/Mascota.o \
./src/spock/Mision.o \
./src/spock/Spock.o \
./src/spock/Villano.o 

C_DEPS += \
./src/spock/Mascota.d \
./src/spock/Mision.d \
./src/spock/Spock.d \
./src/spock/Villano.d 


# Each subdirectory must supply rules for building sources it contributes
src/spock/%.o: ../src/spock/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


