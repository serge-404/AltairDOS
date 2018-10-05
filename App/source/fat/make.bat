del libff.lib
del libscr.lib
del libmenu.lib

cpm c -o -x -c screen.c  

cpm c -o -x -c windows.c 

cpm c -o -x -c controls.c

cpm libr r libscr.lib screen.obj windows.obj controls.obj


cpm c -o -x -c diskio.c

cpm c -o -x -c ff.c

cpm c -o -x -c ffp.c

cpm libr r libff.lib ffp.obj ff.obj diskio.obj 


cpm c -o -x -c filemgr.c

cpm c -o -x -c stringz.c

cpm c -o -x -c menu1.c

cpm c -o -x -c menu2.c

cpm c -o -x -c scanFAT.c

cpm c -o -x -c scanCPM.c

cpm libr r libmenu.lib stringz.obj scanFAT.obj scanCPM.obj 

cpm libr r libmenu.lib filemgr.obj menu2.obj menu1.obj


cpm c -o -x fat.c -lff -lmenu -lscr

