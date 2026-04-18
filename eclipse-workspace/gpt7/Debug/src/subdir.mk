################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/arg.cpp \
../src/build-info.cpp \
../src/common.cpp \
../src/console.cpp \
../src/ggml-backend-reg.cpp \
../src/ggml-backend.cpp \
../src/ggml-opt.cpp \
../src/ggml-threading.cpp \
../src/json-schema-to-grammar.cpp \
../src/llama-arch.cpp \
../src/llama-chat.cpp \
../src/llama-grammar.cpp \
../src/llama-mmap.cpp \
../src/llama-model-loader.cpp \
../src/llama-sampling.cpp \
../src/llama-vocab.cpp \
../src/llama.cpp \
../src/log.cpp \
../src/ngram-cache.cpp \
../src/sampling.cpp \
../src/simple.cpp \
../src/speculative.cpp \
../src/unicode-data.cpp \
../src/unicode.cpp 

C_SRCS += \
../src/ggml-alloc.c \
../src/ggml-quants.c \
../src/ggml.c 

CPP_DEPS += \
./src/arg.d \
./src/build-info.d \
./src/common.d \
./src/console.d \
./src/ggml-backend-reg.d \
./src/ggml-backend.d \
./src/ggml-opt.d \
./src/ggml-threading.d \
./src/json-schema-to-grammar.d \
./src/llama-arch.d \
./src/llama-chat.d \
./src/llama-grammar.d \
./src/llama-mmap.d \
./src/llama-model-loader.d \
./src/llama-sampling.d \
./src/llama-vocab.d \
./src/llama.d \
./src/log.d \
./src/ngram-cache.d \
./src/sampling.d \
./src/simple.d \
./src/speculative.d \
./src/unicode-data.d \
./src/unicode.d 

C_DEPS += \
./src/ggml-alloc.d \
./src/ggml-quants.d \
./src/ggml.d 

OBJS += \
./src/arg.o \
./src/build-info.o \
./src/common.o \
./src/console.o \
./src/ggml-alloc.o \
./src/ggml-backend-reg.o \
./src/ggml-backend.o \
./src/ggml-opt.o \
./src/ggml-quants.o \
./src/ggml-threading.o \
./src/ggml.o \
./src/json-schema-to-grammar.o \
./src/llama-arch.o \
./src/llama-chat.o \
./src/llama-grammar.o \
./src/llama-mmap.o \
./src/llama-model-loader.o \
./src/llama-sampling.o \
./src/llama-vocab.o \
./src/llama.o \
./src/log.o \
./src/ngram-cache.o \
./src/sampling.o \
./src/simple.o \
./src/speculative.o \
./src/unicode-data.o \
./src/unicode.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -pthread -Dcpu=a -fsched-spec-load -fno-finite-math-only  -fstack-protector-all -fopenmp -march=native  -ffast-math -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -pthread -Dcpu=a -fsched-spec-load -fstack-protector-all -fopenmp -march=native -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/arg.d ./src/arg.o ./src/build-info.d ./src/build-info.o ./src/common.d ./src/common.o ./src/console.d ./src/console.o ./src/ggml-alloc.d ./src/ggml-alloc.o ./src/ggml-backend-reg.d ./src/ggml-backend-reg.o ./src/ggml-backend.d ./src/ggml-backend.o ./src/ggml-opt.d ./src/ggml-opt.o ./src/ggml-quants.d ./src/ggml-quants.o ./src/ggml-threading.d ./src/ggml-threading.o ./src/ggml.d ./src/ggml.o ./src/json-schema-to-grammar.d ./src/json-schema-to-grammar.o ./src/llama-arch.d ./src/llama-arch.o ./src/llama-chat.d ./src/llama-chat.o ./src/llama-grammar.d ./src/llama-grammar.o ./src/llama-mmap.d ./src/llama-mmap.o ./src/llama-model-loader.d ./src/llama-model-loader.o ./src/llama-sampling.d ./src/llama-sampling.o ./src/llama-vocab.d ./src/llama-vocab.o ./src/llama.d ./src/llama.o ./src/log.d ./src/log.o ./src/ngram-cache.d ./src/ngram-cache.o ./src/sampling.d ./src/sampling.o ./src/simple.d ./src/simple.o ./src/speculative.d ./src/speculative.o ./src/unicode-data.d ./src/unicode-data.o ./src/unicode.d ./src/unicode.o

.PHONY: clean-src

