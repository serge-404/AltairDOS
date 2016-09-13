echo off
echo 'Compile NC.MAC'
m80n E:NC=NC
echo 'End of compile'
echo 'Linking NC.REL'
l80m E:NC,NC/n/e
echo 'NC.COM saved'
echo 'Erase NC.REL'
era  E:NC.rel
echo 27,'6 Completed ',27,'7'
echo on

