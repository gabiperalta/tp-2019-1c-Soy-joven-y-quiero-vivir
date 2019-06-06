################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../funciones.c \
../memoria.c \
../segmentacionPaginada.c 

OBJS += \
./funciones.o \
./memoria.o \
./segmentacionPaginada.o 

C_DEPS += \
./funciones.d \
./memoria.d \
./segmentacionPaginada.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2019-1c-Soy-joven-y-quiero-vivir/biblioteca" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


