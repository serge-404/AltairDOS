m80n E:M35ZRKH,=M35ZRKH
l80m /p:100,E:M35ZRKH,M35ZRKH/n/e

m80n E:M35ZMSH,=M35ZMSH
l80m /p:100,E:M35ZMSH,M35ZMSH/n/e

echo off
era  E:M35ZMSH.rel
era  E:M35ZRKH.rel
echo on

power load m35zrkh.com 8000
power save m35zrkh.bin 8000 16
power load m35zmsh.com 8000
power save m35zmsh.bin 8000 16
