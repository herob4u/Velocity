mkdir -p bin # create directory for executable
rm -f ./bin/OpenCL-Wrapper.exe # prevent execution of old version if compiling fails
g++ ./src/*.cpp -o ./bin/OpenCL-Wrapper.exe -std=c++17 -pthread -I./src/OpenCL/include -L./src/OpenCL/lib -lOpenCL # compile on Linux
#g++ ./src/*.cpp -o ./bin/OpenCL-Wrapper.exe -std=c++17 -pthread -I./src/OpenCL/include -L/system/vendor/lib64 -lOpenCL # compile on Android
./bin/OpenCL-Wrapper.exe # run executable
