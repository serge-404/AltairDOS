/*---------------------------------------------------------------*/
/* FAT file system module test program R0.0. (c)2007 Serge       */
/*---------------------------------------------------------------*/

#include <cpm.h>
#include "filemgr.h"
#include "stringz.h"

extern CFCB yfcb;
extern char buffer[MAX_BUFF+1];
extern FRESULT res; 
extern FILINFO finfo;

FRESULT scanCPM(char* path, dir_callback OnFile)
{
  register BYTE idx;
  BYTE uid=OS_getuid();

  if ((! path) || (! *path)) return FR_INVALID_OBJECT;

  OS_setuid(PathToFcb(path, &yfcb));

  bdos(CPMSDMA, &buffer[0]);                                         
  idx=bdos(CPMFFST, &yfcb);                                     
  while (idx<4) {
    res=OnFile(FcbToFInfo((void*)&buffer[idx*32], &finfo));
    if (res!=FR_OK) break;
    idx=bdos(CPMFNXT, &yfcb);                                   
  }
  OS_setuid(uid);
  return FR_OK; 
}

