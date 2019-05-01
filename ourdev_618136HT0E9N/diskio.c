#include "diskio.h"

#define ATA		0
#define MMC		1
#define USB		2

/*
┌───────────────┬─────────────┬───────────────┐
│Function                      │Required                  │Note                          │
├───────────────┼─────────────┼───────────────┤
│disk_initialize               │Always                    │                              │
├───────────────┼─────────────┤                              │
│disk_status                   │Always                    │                              │
├───────────────┼─────────────┤                              │
│disk_read                     │Always                    │                              │
├───────────────┼─────────────┤                              │
│disk_write                    │_FS_READONLY == 0         │                              │
├───────────────┼─────────────┤                              │
│disk_ioctl (CTRL_SYNC)        │_FS_READONLY == 0         │                              │
├───────────────┼─────────────┤                              │
│disk_ioctl (GET_SECTOR_COUNT) │_USE_MKFS == 1            │Disk I/O functions.           │
├───────────────┼─────────────┤                              │
│disk_ioctl (GET_SECTOR_SIZE)  │_MAX_SS >= 1024           │                              │
├───────────────┼─────────────┤                              │
│disk_ioctl (GET_BLOCK_SIZE)   │_USE_MKFS == 1            │                              │
├───────────────┼─────────────┤                              │
│disk_ioctl (CTRL_ERASE_SECTOR)│_USE_ERASE == 1           │                              │
├───────────────┼─────────────┤                              │
│get_fattime                   │_FS_READONLY == 0         │                              │
├───────────────┼─────────────┼───────────────┤
│ff_convert                    │_USE_LFN >= 1             │Unicode support functions.    │
├───────────────┼─────────────┤                              │
│ff_wtoupper                   │_USE_LFN >= 1             │Available in option/cc*.c.    │
├───────────────┼─────────────┼───────────────┤
│ff_cre_syncobj                │_FS_REENTRANT == 1        │                              │
├───────────────┼─────────────┤                              │
│ff_del_syncobj                │_FS_REENTRANT == 1        │                              │
├───────────────┼─────────────┤                              │
│ff_req_grant                  │_FS_REENTRANT == 1        │O/S dependent functions.      │
├───────────────┼─────────────┤Samples available in          │
│ff_rel_grant                  │_FS_REENTRANT == 1        │option/syscall.c.             │
├───────────────┼─────────────┤                              │
│ff_mem_alloc                  │_USE_LFN == 3             │                              │
├───────────────┼─────────────┤                              │
│ff_mem_free                   │_USE_LFN == 3             │                              │
└───────────────┴─────────────┴───────────────┘
*/

// Inidialize a Drive
DSTATUS disk_initialize(BYTE drv)		/* Physical drive nmuber(0..) */
{
	if(drv)
		return STA_NOINIT;
	else
		return (SPIFLASH_disk_initialize() ? STA_NOINIT : 0);
}



DSTATUS disk_status(BYTE drv)			/* Physical drive nmuber(0..) */
{
	return 0;
	
#if 0
	DSTATUS stat;
	int result;

	switch(drv) {
	case ATA :
		result = ATA_disk_status();
		// translate the reslut code here

		return stat;

	case MMC :
		result = MMC_disk_status();
		// translate the reslut code here

		return stat;

	case USB :
		result = USB_disk_status();
		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
#endif
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read(
	BYTE drv,		/* Physical drive nmuber(0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address(LBA) */
	BYTE count		/* Number of sectors to read(1..255) */
)
{	
	int i;
	for(i=0;i<count;i++)
	{
		W25X_Read_Sector(sector,buff);
		sector ++;
		buff += FLASH_SECTOR_SIZE;
	}
	
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write(
	BYTE drv,			/* Physical drive nmuber(0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address(LBA) */
	BYTE count			/* Number of sectors to write(1..255) */
)
{
	int i;
	for(i=0;i<count;i++)
	{	W25X_Erase_Sector(sector);
	
		W25X_Write_Sector(sector,(u8*)buff);
		sector ++;
		buff += FLASH_SECTOR_SIZE;
	}
	
	return RES_OK;	
}
#endif /* _READONLY */




/************************************************************************************
CTRL_SYNC			Make sure that the disk drive has finished pending write process. 
					When the disk I/O module has a write back cache, flush the dirty sector immediately. 
					This command is not used in read-only configuration. 
GET_SECTOR_SIZE		Returns sector size of the drive into the WORD variable pointed by Buffer. 
					This command is not used in fixed sector size configuration, _MAX_SS is 512. 
GET_SECTOR_COUNT	Returns number of available sectors on the drive into the DWORD variable pointed by Buffer.
					This command is used by only f_mkfs function to determine the volume size to be created. 
GET_BLOCK_SIZE		Returns erase block size of the flash memory in unit of sector into the DWORD variable pointed by Buffer.
					The allowable value is 1 to 32768 in power of 2. 
					Return 1 if the erase block size is unknown or disk devices. 
					This command is used by only f_mkfs function and it attempts to align data area to the erase block boundary. 
CTRL_ERASE_SECTOR	Erases a part of the flash memory specified by a DWORD array {<start sector>, <end sector>} pointed by Buffer. 
					When this feature is not supported or not a flash memory media, this command has no effect.
					The FatFs does not check the result code and the file function is not affected even if the sectors are not erased well. 
					This command is called on removing a cluster chain when _USE_ERASE is 1. 
************************************************************************************/
DRESULT disk_ioctl(
	BYTE drv,		/* Physical drive nmuber(0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;
	DWORD nFrom,nTo;
	int i;
	char *buf = buff;
	
	switch(ctrl)
	{
		case CTRL_SYNC :
			break;
		
		//扇区擦除
		case CTRL_ERASE_SECTOR:
			nFrom = *((DWORD*)buff);
			nTo = *(((DWORD*)buff)+1);
			for(i=nFrom;i<=nTo;i++)
				W25X_Erase_Sector(i);
				
			break;
		
		case GET_BLOCK_SIZE:
			//*(DWORD*)buff = 4096*16;
			buf[1] = (u8)(FLASH_BLOCK_SIZE & 0xFF);
			buf[0] = (u8)(FLASH_BLOCK_SIZE >> 8);
			break;
	
	
		case GET_SECTOR_SIZE:
			//*(DWORD*)buff = FLASH_SECTOR_SIZE;
			buf[0] = (u8)(FLASH_SECTOR_SIZE & 0xFF);
			buf[1] = (u8)(FLASH_SECTOR_SIZE >> 8);
			break;
		
		case GET_SECTOR_COUNT:
			//*(DWORD*)buff = FLASH_SECTOR_COUNT;
			buf[0] = (u8)(FLASH_SECTOR_COUNT & 0xFF);
			buf[1] = (u8)(FLASH_SECTOR_COUNT >> 8);
			break;
			
		default:
			res = RES_PARERR;
			break;
	}
	return res;
}

DWORD get_fattime(void)
{
/*
 t = Time_GetCalendarTime();
    t.tm_year -= 1980;  //年份改为1980年起
    t.tm_mon++;          //0-11月改为1-12月
    t.tm_sec /= 2;       //将秒数改为0-29
    
    date = 0;
    date = (t.tm_year << 25) | (t.tm_mon<<21) | (t.tm_mday<<16)|\
            (t.tm_hour<<11) | (t.tm_min<<5) | (t.tm_sec);

    return date;
*/
	DWORD dt = 31<<25;
	dt += 12<<21;
	dt += 12<<16;
	dt += 12<<11;
	dt += 12<<5 + 12;
	return dt;
}


