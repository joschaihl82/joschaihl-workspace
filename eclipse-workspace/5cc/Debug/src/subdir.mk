################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/codegen.c \
../src/main.c \
../src/parser.c \
../src/tokenizer.c \
../src/type.c \
../src/util.c 

C_DEPS += \
./src/codegen.d \
./src/main.d \
./src/parser.d \
./src/tokenizer.d \
./src/type.d \
./src/util.d 

OBJS += \
./src/codegen.o \
./src/main.o \
./src/parser.o \
./src/tokenizer.o \
./src/type.o \
./src/util.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/codegen.d ./src/codegen.o ./src/main.d ./src/main.o ./src/parser.d ./src/parser.o ./src/tokenizer.d ./src/tokenizer.o ./src/type.d ./src/type.o ./src/util.d ./src/util.o

.PHONY: clean-src

