if (Test-Path -Path "build_mingw") { Remove-Item -Path "build_mingw" -Recurse }

New-Item -Path 'build_mingw' -ItemType Directory
cd ".\build_mingw\"
cmake -G "MinGW Makefiles" -DGTEST_COMPILE=ON -DBUILD_GMOCK=OFF ..
cd googletest
make
cd ../..
.\build_mingw\googletest\bin\mt_test.exe