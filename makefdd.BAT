echo OFF
echo 'Compile DOS for FDD'

cpm m80 =dosfdd

echo 'Answer `No` (N) to question about origin move'

cpm l80m dosfdd,dosfdd/n/e
del dosfdd.rel
del dosfdd.bin
ren dosfdd.com dosfdd.bin

ECHO ' '
ECHO 'Use POWER.COM utility to copy DOS code from `dosfdd.bin` to Floppy:'
ECHO '  power load dosfdd.bin 5000  # then replace disk to new system disk'
ECHO '  power write 0 1 5000 160    # after, new system disk ready to boot'
ECHO ' '

