if (Test-Path -Path "build_msvc") { Remove-Item -Path "build_msvc" -Recurse }

New-Item -Path 'build_msvc' -ItemType Directory
cd ".\build_mingw\"
cmake ..
cmake --build .
cd ..