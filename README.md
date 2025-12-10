## To build
```
docker build -t stm32-cmake .
```

project path : C:\Users\d\Desktop\git\cmake_test_env

```
docker run --rm -it -v ~/{project_path}:/workspace stm32-cmake bash
```

```
docker run --rm -it -v "C:\Users\d\Desktop\git\cmake_test_env:/workspace" stm32-cmake bash
```

once in container, to build .elf

```
mkdir -p build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cubeide-gcc.cmake ..
make -j$(nproc)
```

once in container, to build .bin

(ensure in build folder)

```
arm-none-eabi-objcopy -O binary cmake_test.elf cmake_test.bin
```

To get debugging working?

```
# Build with debug
mkdir -p debug
cd debug
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../cubeide-gcc.cmake ..
make -j32

# Run openocd in WINDOWS TERMINAL
# I have no idea where those files are / if they exist but it worked
openocd -f interface/stlink.cfg -f target/stm32g0x.cfg

# Back in docker
# Start GDB / Load symbols
arm-none-eabi-gdb
(gdb) file /workspace/debug/cmake_test.elf
# Connect to Openocd server
(gdb) target extended-remote host.docker.internal:3333 # since in docker need to expose
```