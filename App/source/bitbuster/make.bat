for %%i in (*.com) do echo %%i & copy /b SCREEN.BIN + %%i %%i.tmp & pack %%i.tmp & copy /b DEPACK.BIN + %%i.tmp.pck .\ready\%%i & del %%i.tmp & del %%i.tmp.pck

