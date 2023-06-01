rmdir /s /q "ipch"
rmdir /s /q "Debug"
rmdir /s /q "Release"

rmdir /s /q .\Client\Default\Debug
rmdir /s /q .\Client\Default\ipch
rmdir /s /q .\Client\Default\Release
rmdir /s /q .\Client\Default\x64

rmdir /s /q .\Engine\Default\Debug
rmdir /s /q .\Engine\Default\ipch
rmdir /s /q .\Engine\Default\Release
rmdir /s /q .\Engine\Default\x64

rmdir /s /q .\x64

del /f /q .\Client\Bin\Client.exe
del /f /q .\Client\Bin\Client.ilk
del /f /q .\Client\Bin\Client.pdb

del /f /q .\Engine\Bin\Engine.exe
del /f /q .\Engine\Bin\Engine.ilk
del /f /q .\Engine\Bin\Engine.pdb

del /f /q Framework.VC.db
