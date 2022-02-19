mkdir build
cd build
cmake -G "MinGW Makefiles" -DGTEST_COMPILE=ON -DBUILD_GMOCK=OFF ..
cd googletest
make