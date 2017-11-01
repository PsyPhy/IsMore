echo off
@echo off
setlocal
for %%a in (%*) do (
  echo Processing file: [%%a]
  ..\Debug\MDFtoEMT.exe "%%a"
)

pause