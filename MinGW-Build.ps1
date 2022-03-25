if (Test-Path -Path "build_mingw") { Remove-Item -Path "build_mingw" -Recurse }

New-Item -Path 'build_mingw' -ItemType Directory
cd ".\build_mingw\"
cmake -G "MinGW Makefiles" .. -DCMAKE_BUILD_TYPE=DEBUG
make
cd ..