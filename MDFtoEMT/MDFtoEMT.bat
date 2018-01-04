echo on
setlocal
for %%a in (%*) do (
  echo Processing file: [%%a]
  MDFtoEMT.exe "%%a"
)

pause