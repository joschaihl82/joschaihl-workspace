################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/codegen.c \
../src/error.c \
../src/file.c \
../src/main.c \
../src/parse.c \
../src/preprocess.c \
../src/string.c \
../src/tokenize.c \
../src/type.c 

C_DEPS += \
./src/codegen.d \
./src/error.d \
./src/file.d \
./src/main.d \
./src/parse.d \
./src/preprocess.d \
./src/string.d \
./src/tokenize.d \
./src/type.d 

OBJS += \
./src/codegen.o \
./src/error.o \
./src/file.o \
./src/main.o \
./src/parse.o \
./src/preprocess.o \
./src/string.o \
./src/tokenize.o \
./src/type.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/codegen.d ./src/codegen.o ./src/error.d ./src/error.o ./src/file.d ./src/file.o ./src/main.d ./src/main.o ./src/parse.d ./src/parse.o ./src/preprocess.d ./src/preprocess.o ./src/string.d ./src/string.o ./src/tokenize.d ./src/tokenize.o ./src/type.d ./src/type.o

.PHONY: clean-src

