################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/arg.cpp \
../src/common.cpp \
../src/console.cpp \
../src/ggml-rpc.cpp \
../src/json-schema-to-grammar.cpp \
../src/llama-grammar.cpp \
../src/llama-sampling.cpp \
../src/llama-vocab.cpp \
../src/llama.cpp \
../src/log.cpp \
../src/ngram-cache.cpp \
../src/sampling.cpp \
../src/train.cpp \
../src/unicode-data.cpp \
../src/unicode.cpp 

C_SRCS += \
../src/ggml-aarch64.c \
../src/ggml-alloc.c \
../src/ggml-backend.c \
../src/ggml-quants.c \
../src/ggml.c 

CPP_DEPS += \
./src/arg.d \
./src/common.d \
./src/console.d \
./src/ggml-rpc.d \
./src/json-schema-to-grammar.d \
./src/llama-grammar.d \
./src/llama-sampling.d \
./src/llama-vocab.d \
./src/llama.d \
./src/log.d \
./src/ngram-cache.d \
./src/sampling.d \
./src/train.d \
./src/unicode-data.d \
./src/unicode.d 

C_DEPS += \
./src/ggml-aarch64.d \
./src/ggml-alloc.d \
./src/ggml-backend.d \
./src/ggml-quants.d \
./src/ggml.d 

OBJS += \
./src/arg.o \
./src/common.o \
./src/console.o \
./src/ggml-aarch64.o \
./src/ggml-alloc.o \
./src/ggml-backend.o \
./src/ggml-quants.o \
./src/ggml-rpc.o \
./src/ggml.o \
./src/json-schema-to-grammar.o \
./src/llama-grammar.o \
./src/llama-sampling.o \
./src/llama-vocab.o \
./src/llama.o \
./src/log.o \
./src/ngram-cache.o \
./src/sampling.o \
./src/train.o \
./src/unicode-data.o \
./src/unicode.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -pthread -Dcpu=a -fsched-spec-load -fno-finite-math-only  -fstack-protector-all -fopenmp -march=native  -ffast-math -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -pthread -Dcpu=a -fsched-spec-load -fstack-protector-all -fopenmp -march=native -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/arg.d ./src/arg.o ./src/common.d ./src/common.o ./src/console.d ./src/console.o ./src/ggml-aarch64.d ./src/ggml-aarch64.o ./src/ggml-alloc.d ./src/ggml-alloc.o ./src/ggml-backend.d ./src/ggml-backend.o ./src/ggml-quants.d ./src/ggml-quants.o ./src/ggml-rpc.d ./src/ggml-rpc.o ./src/ggml.d ./src/ggml.o ./src/json-schema-to-grammar.d ./src/json-schema-to-grammar.o ./src/llama-grammar.d ./src/llama-grammar.o ./src/llama-sampling.d ./src/llama-sampling.o ./src/llama-vocab.d ./src/llama-vocab.o ./src/llama.d ./src/llama.o ./src/log.d ./src/log.o ./src/ngram-cache.d ./src/ngram-cache.o ./src/sampling.d ./src/sampling.o ./src/train.d ./src/train.o ./src/unicode-data.d ./src/unicode-data.o ./src/unicode.d ./src/unicode.o

.PHONY: clean-src

