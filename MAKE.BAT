echo OFF

cpm m80 =dos

echo 'Answer `No` (N) to question about origin move'

cpm l80m dos,dos/n/e
del dos.rel
del dos.bin
ren dos.com dos.bin

cpm m80n mbrboot,=mbrboot
cpm l80m /p:0,mbrboot,mbrboot/n/e
del mbrboot.rel
del mbrboot.bin
ren mbrboot.com mbrboot.bin

echo OFF
ECHO ' '
ECHO 'Use FDISK.COM utility to copy MBR code from `mbrboot.bin` to HDD MBR'
ECHO ' and sysgen (copy OS code) from `dos.bin` to partition system tracks'
ECHO ' '


