FAT is utility for serving FAT and Ordos volumes under AltairDos (or other CP/M
where IDEBDOS driver exists). FAT can operating with two modes: CLI (OS command
line) or like two-panel commander. Type `FAT HELP` to read about command systax,
`FAT MENU` for panels commander.

Syntax: `FAT <command> [<filemask> [<filemask>]]`

	where <command>: dir  <filemask> - show files list
        		type  <filemask> - type file content, KOI8r
			atype, ktype <fl>- type Alt=cp1251 or KOI7 file
			fmtord <ord_dsk> - format ORDOS disk @D:
			del   <filemask> - delete file or catalog
			copy  <inpmask> <outmask> - copy from inp to out
			ren   <inpmask> <outmask> - rename inp to out
			mkdir <dirname>  - create catalog by full path
			info  <drive>    - show filesystem information
			stat  <filemask> - show file attributes
			chmod <filemask> - change file attributes

<filemask> - file specification with full dirrectory path and(or) wildcards ?*
		
CP/M filemask: A0:*.ext - files with any names and extension
		`ext` in user 0 of drive A. `0`-optional

ORDOS filemask: @D:* - any files. Path allways starts with @

FAT filemask:   0:/dir/dir2/file1.* - all files with `file1` name in
		`dir/dir2` catalog of logical drive 0 (phis.drive 0 = master,
		1st partition), catalog and drive names are optional

PARTITION Drives[] = {
    {0, 0},     /* Logical drive 0 ==> Physical drive 0, 1st partition */
    {1, 0},     /* Logical drive 1 ==> Physical drive 1, 1st partition */
    {0, 1},     /* Logical drive 2 ==> Physical drive 0, 2nd partition */
    {1, 1},     /* Logical drive 3 ==> Physical drive 1, 2nd partition */
    {0, 2},     /* Logical drive 4 ==> Physical drive 0, 3rd partition */
    {1, 2},     /* Logical drive 5 ==> Physical drive 1, 3rd partition */
    {0, 3},     /* Logical drive 6 ==> Physical drive 0, 4th partition */
    {1, 3}      /* Logical drive 7 ==> Physical drive 1, 4th partition */
};

Examples: 

	 FAT copy 1:/test/aaa.bbb B3:cc.ddd
         - copy FAT(MSDOS) file from slave drive first partition from
           dirrectory /test/ to CPM disk B user 3 with newname

	 FAT copy @D:test.as 4:/asm/test.asm
         - copy ordos file from disk D to FAT(MSDOS) filesystem - master drive
           third partition to dirrectory /asm/ filename test.asm

	 FAT atype 4:/asm/test.asm
         - view content of alternative codepage text file from FAT(MSDOS) vol

FAT utility builded with Hitech C compiler v3.09 (CPM freeware)

