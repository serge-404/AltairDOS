cpm era libff.lib
cpm era libfdisk.lib

cpm c -o -x -c diskio.c
cpm c -o -x -c ff.c
cpm c -o -x -c ffp.c
cpm c -o -x -c f_mkfs.c

cpm libr r libff.lib f_mkfs.obj ffp.obj ff.obj diskio.obj 

cpm c -o -x -c fdisk1.c
cpm c -o -x -c fdisk2.c

cpm libr r libfdisk.lib fdisk2.obj fdisk1.obj

cpm c -o -x fdisk.c -lfdisk -lff
