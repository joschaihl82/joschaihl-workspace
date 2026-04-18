################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/build-info.cpp \
../src/common.cpp \
../src/console.cpp \
../src/grammar-parser.cpp \
../src/llama.cpp \
../src/main.cpp \
../src/sampling.cpp \
../src/train.cpp 

C_SRCS += \
../src/ggml-alloc.c \
../src/ggml-backend.c \
../src/ggml-quants.c \
../src/ggml.c 

CPP_DEPS += \
./src/build-info.d \
./src/common.d \
./src/console.d \
./src/grammar-parser.d \
./src/llama.d \
./src/main.d \
./src/sampling.d \
./src/train.d 

C_DEPS += \
./src/ggml-alloc.d \
./src/ggml-backend.d \
./src/ggml-quants.d \
./src/ggml.d 

OBJS += \
./src/build-info.o \
./src/common.o \
./src/console.o \
./src/ggml-alloc.o \
./src/ggml-backend.o \
./src/ggml-quants.o \
./src/ggml.o \
./src/grammar-parser.o \
./src/llama.o \
./src/main.o \
./src/sampling.o \
./src/train.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -ffast-math -fopenmp -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -ffast-math -fopenmp -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/build-info.d ./src/build-info.o ./src/common.d ./src/common.o ./src/console.d ./src/console.o ./src/ggml-alloc.d ./src/ggml-alloc.o ./src/ggml-backend.d ./src/ggml-backend.o ./src/ggml-quants.d ./src/ggml-quants.o ./src/ggml.d ./src/ggml.o ./src/grammar-parser.d ./src/grammar-parser.o ./src/llama.d ./src/llama.o ./src/main.d ./src/main.o ./src/sampling.d ./src/sampling.o ./src/train.d ./src/train.o

.PHONY: clean-src

