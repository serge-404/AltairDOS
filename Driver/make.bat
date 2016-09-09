cpm m80n ndrv,=ndrv
cpm l80m /p:100,ndrv,ndrv/n/e
del  ndrv.rel
del  driver.bak
ren driver.sys driver.bak
ren ndrv.com driver.sys