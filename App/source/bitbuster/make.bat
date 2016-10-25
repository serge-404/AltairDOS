for %%i in (*.com) do echo %%i & pack %%i & copy /b DEPACK.BIN + %%i.pck .\ready\%%i & del %%i.tmp.pck

