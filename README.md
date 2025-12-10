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

to copy builds from docker to regular file system

```
docker cp 733882269c04:/workspace/build C:\Users\d\Desktop\git\cmake_test_env\build
```