
$env:path+=";D:\Programs\texlive\2021\bin\win32"
$env:path+=";D:\Program Files\Python 3.10.2 (64-bit)"
$env:path+=";D:\Program Files\Python 3.10.2 (64-bit)\Scripts"
$env:path+=";D:\Program Files\Python 3.10.2 (64-bit)\Lib\site-packages\pygments"

function Tex-Compile ($file) { xelatex -shell-escape -syntex=1 -interaction=nonstopmode $file }
Tex-Compile main.tex