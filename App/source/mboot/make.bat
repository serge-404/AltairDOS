cpm m80n MBOOT,=MBOOT

echo 'Answer `No` (N) to question about origin move'

cpm l80m /P:0,MBOOT,MBOOT/n/e
del MBOOT.rel
del MBOOT.ORD
ren MBOOT.COM MBOOT.ORD
