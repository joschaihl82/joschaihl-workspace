################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/common.cpp \
../src/llama.cpp \
../src/main.cpp 

C_SRCS += \
../src/ggml.c 

CPP_DEPS += \
./src/common.d \
./src/llama.d \
./src/main.d 

C_DEPS += \
./src/ggml.d 

OBJS += \
./src/common.o \
./src/ggml.o \
./src/llama.o \
./src/main.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 --fast-math -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 --fast-math -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/common.d ./src/common.o ./src/ggml.d ./src/ggml.o ./src/llama.d ./src/llama.o ./src/main.d ./src/main.o

.PHONY: clean-src

