################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/common-ggml.cpp \
../src/common.cpp \
../src/ggml-backend-reg.cpp \
../src/ggml-backend.cpp \
../src/ggml-opt.cpp \
../src/ggml-threading.cpp \
../src/gguf.cpp \
../src/gptj.cpp \
../src/main.cpp 

C_SRCS += \
../src/ggml-alloc.c \
../src/ggml-quants.c \
../src/ggml.c 

CPP_DEPS += \
./src/common-ggml.d \
./src/common.d \
./src/ggml-backend-reg.d \
./src/ggml-backend.d \
./src/ggml-opt.d \
./src/ggml-threading.d \
./src/gguf.d \
./src/gptj.d \
./src/main.d 

C_DEPS += \
./src/ggml-alloc.d \
./src/ggml-quants.d \
./src/ggml.d 

OBJS += \
./src/common-ggml.o \
./src/common.o \
./src/ggml-alloc.o \
./src/ggml-backend-reg.o \
./src/ggml-backend.o \
./src/ggml-opt.o \
./src/ggml-quants.o \
./src/ggml-threading.o \
./src/ggml.o \
./src/gguf.o \
./src/gptj.o \
./src/main.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/common-ggml.d ./src/common-ggml.o ./src/common.d ./src/common.o ./src/ggml-alloc.d ./src/ggml-alloc.o ./src/ggml-backend-reg.d ./src/ggml-backend-reg.o ./src/ggml-backend.d ./src/ggml-backend.o ./src/ggml-opt.d ./src/ggml-opt.o ./src/ggml-quants.d ./src/ggml-quants.o ./src/ggml-threading.d ./src/ggml-threading.o ./src/ggml.d ./src/ggml.o ./src/gguf.d ./src/gguf.o ./src/gptj.d ./src/gptj.o ./src/main.d ./src/main.o

.PHONY: clean-src

