#ifndef winx68k_fileio_h
#define winx68k_fileio_h

#include "common.h"
#include "dosio.h"

#define	FILEH		HANDLE

#define	FSEEK_SET	0
#define	FSEEK_CUR	1
#define	FSEEK_END	2

char* getFileName(char* filename);
//#define	getFileName	GetFileName

FILEH	File_Open(uint8_t *filename);
FILEH	File_Create(uint8_t *filename);
uint32_t	File_Seek(FILEH handle, int32_t pointer, uint16_t mode);
uint32_t	File_Read(FILEH handle, void *data, uint32_t length);
uint32_t	File_Write(FILEH handle, void *data, uint32_t length);
uint16_t	File_Close(FILEH handle);
uint16_t	File_Attr(uint8_t *filename);
#define	File_Open	file_open
#define	File_Create	file_create
#define	File_Seek	file_seek
#define	File_Read	file_lread
#define	File_Write	file_lwrite
#define	File_Close	file_close
#define	File_Attr	file_attr

void	File_SetCurDir(uint8_t *exename);
FILEH	File_OpenCurDir(uint8_t *filename);
FILEH	File_CreateCurDir(uint8_t *filename);
int16_t	File_AttrCurDir(uint8_t *filename);
#define	File_SetCurDir		file_setcd
#define	File_OpenCurDir		file_open_c
#define	File_CreateCurDir	file_create_c
#define	File_AttrCurDir		file_attr_c

#endif
