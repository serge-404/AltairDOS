cpm m80n TESTDEV,=TESTDEV

echo 'Answer `No` (N) to question about origin move'

cpm l80m TESTDEV,TESTDEV/n/e
del TESTDEV.rel
del TESTDEV.ORD
ren TESTDEV.COM TESTDEV.ORD
