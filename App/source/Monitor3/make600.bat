m80n E:M36ZRKH,=M36ZRKH
l80m /p:100,E:M36ZRKH,M36ZRKH/n/e

m80n E:M36ZMSH,=M36ZMSH
l80m /p:100,E:M36ZMSH,M36ZMSH/n/e

echo off
era  E:M36ZMSH.rel
era  E:M36ZRKH.rel
echo on

power load m36zrkh.com 8000
power save m36zrkh.bin 8000 16
power load m36zmsh.com 8000
power save m36zmsh.bin 8000 16
