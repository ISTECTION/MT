if (Test-Path -Path "build_msvc") { Remove-Item -Path "build_msvc" -Recurse }

New-Item -Path 'build_msvc' -ItemType Directory
cd ".\build_msvc\"
cmake ..
cmake --build .
cd ..