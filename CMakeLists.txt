cmake_minimum_required(VERSION 3.2)
project(HannaTimer)

include_directories(src build/basalt/src)

set(CMAKE_USE_RELATIVE_PATHS "TRUE")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "build")
set(CMAKE_AR "/usr/local/opt/pebble-toolchain/arm-cs-tools/bin/arm-none-eabi-ar")
set(CMAKE_CXX_COMPILER "/usr/local/opt/pebble-toolchain/arm-cs-tools/bin/arm-none-eabi-c++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_COMPILER "/usr/local/opt/pebble-toolchain/arm-cs-tools/bin/arm-none-eabi-gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -I/usr/local/opt/pebble-sdk/Pebble/basalt/include -Isrc")
set(CMAKE_NM "/usr/local/opt/pebble-toolchain/arm-cs-tools/bin/arm-none-eabi-nm")
set(CMAKE_OBJCOPY "/usr/local/opt/pebble-toolchain/arm-cs-tools/bin/arm-none-eabi-objcopy")
set(CMAKE_OBJDUMP "/usr/local/opt/pebble-toolchain/arm-cs-tools/bin/arm-none-eabi-objdump")
set(CMAKE_RANLIB "/usr/local/opt/pebble-toolchain/arm-cs-tools/bin/arm-none-eabi-ranlib")
set(CMAKE_STRIP "/usr/local/opt/pebble-toolchain/arm-cs-tools/bin/arm-none-eabi-strip")
set(FAKE_TARGET fakeTarget)

set(SOURCE_FILES
   appinfo.json
   wscript)

add_executable(HannaTimer
${SOURCE_FILES}
src/common.h
src/common.c
src/main.c
src/lap_timer.c
src/stop_timer.c
)

add_custom_target(fakeTarget
   cd /Users/sven/Repository/HannaTimer/;pebble build
)
add_dependencies(${PROJECT_NAME} ${FAKE_TARGET})