set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

# Make CMake happy about those compilers
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
