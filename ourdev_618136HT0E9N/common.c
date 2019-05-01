#include "common.h"
#include "spi_flash.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "ff.h"
#include "diskio.h"
#include "util.h"

void fat_menu_init(void)
{
	
	uint8_t key = 0;

	SPI1_Init();
	
	while (1)
	{
		USART_WriteString("\r\n============ 请选择...===============\r\n");
		USART_WriteString("格式化--------------------------------- 1\r\n");
		USART_WriteString("创建文件------------------------------- 2\r\n");
		USART_WriteString("删除文件 ------------------------------ 3\r\n");
		USART_WriteString("列表文件------------------------------- 4\r\n");
		USART_WriteString("重启系统 ------------------------------ 5\r\n");
		USART_WriteString("磁盘信息------------------------------- 6\r\n");
		USART_WriteString("创建目录------------------------------- 7\r\n");
		USART_WriteString("编辑文件------------------------------- 8\r\n");
		USART_WriteString("读取文件------------------------------- 9\r\n");
		USART_WriteString("========================================\r\n");
		key = GetKey();
		if (key == 0x31)	//Format
		{
			USART_WriteString("\r\n----1----");
			format_disk();		
		}
		else if (key == 0x32)	//Creat File
		{
			USART_WriteString("\r\n----2----");
			creat_file();			
		}
		else if (key == 0x33)	//Delete File
		{
			USART_WriteString("\r\n----3----");
			delete_file();		

		}
		else if (key == 0x34)	//Delete File
		{
			USART_WriteString("\r\n----4----");
			list_file();
		}
		else if (key == 0x36)	//Disk info
		{
			USART_WriteString("\r\n----6----");
			get_disk_info();				
		}
		else if (key == 0x37)	//Creat Dir
		{
			USART_WriteString("\r\n----7----");
			creat_dir();				
		}
		else if (key == 0x38)	//Edit File
		{
			USART_WriteString("\r\n----8----");
			edit_file();				
		}
		else if (key == 0x39)	//Read File
		{
			USART_WriteString("\r\n----9----");
			read_file();				
		}
		else if (key == 0x35)
		{
			USART_WriteString("\r\n----5----");
			Sys_Soft_Reset();
		}		
		else
		{
			USART_WriteString("\r\n----%c----",key);
			USART_WriteString("\r\n只接受1-6，请重新输入");
			
		}
	}

}
void edit_file(void)
{
	FATFS fs;
//	FATFS *fls = &fs;
	FIL	file;
	FRESULT res; 
	DIR dirs;
	FILINFO finfo;
	char key = 0;
	char path[20];

		uint32_t index = 0x00;
		uint32_t reindex = 0x00;
		uint8_t file_buff[512] = {0};

	uint32_t files_num = 0;
	uint8_t length = 255;
	res = f_mount(0,&fs);
	if (res != FR_OK)
	{
		USART_WriteString("\r\n挂载文件系统失败,错误代码: %u",res);
		return;
	}
	res = f_opendir(&dirs,"/");
	USART_WriteString("\r\n------------文件列表------------");
	if (res == FR_OK) 
	{
		//i = strlen(path);		
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		{
			if (finfo.fattrib & AM_DIR)//如果是文件夹 
			{ 
				continue;
			} 
			else 
			{   
				files_num++;
				//显示文件名,显示文件实际大小 ,文件实际大小采用四舍五入法
				USART_WriteString("\r\n/%12s%7ld KB ",  &finfo.fname[0],(finfo.fsize+512)/1024);

			}
		}
		if( files_num == 0 )//无文件
		{
			USART_WriteString("\r\n无文件!");    
		}
	}
	else
	{
		USART_WriteString("\r\n打开根目录失败!");
		USART_WriteString("\r\n错误代码: %u",res);
	}
	USART_WriteString("\r\n输入要编辑文件全名,以回车结束...");
	get_file_name((uint8_t *)path,length,FI);
	res = f_open(&file,path,FA_READ | FA_WRITE);
	if (res == FR_OK)
	{
	
		USART_WriteString("\r\n打开文件 %s 成功",path);
		USART_WriteString("\r\n现在是文件写测试,请输入要写入的数据!");		
		USART_WriteString("\r\n按 ESC 或者 Ctrl+C 结束编辑!\r\n");
		
		while(1)
		{
			key = GetKey();
			if ((key == 0x1B) && (index == 0x00))//key ESC
			{
				USART_WriteString("\r\n数据还没有输入,现在处于编辑模式...");								
				continue;
			}
			else if ((key == 0x1B))	//key ESC
			{
				USART_WriteString("\r\n保存数据...");
				res = f_write(&file,file_buff,index,&reindex);
				if ((res == FR_OK) && (reindex == index))
				{
					USART_WriteString("\r\n保存数据成功!");
					f_close(&file);
					index = 0x00;
					reindex = 0x00;									
				}
				else
				{
					USART_WriteString("\r\n保存数据失败!");
					USART_WriteString("\r\n错误代码: %u",res);									
				}
				break;
			}
			else if (key == 0x03) //key Ctrl+C
			{
				USART_WriteString("\r\n结束文件编辑!");
				USART_WriteString("\r\n保存数据...");
				res = f_write(&file,file_buff,index,&reindex);
				if ((res == FR_OK) && (reindex == index))
				{
					USART_WriteString("\r\n保存数据成功!");
					f_close(&file);
					index = 0x00;
					reindex = 0x00;									
				}
				else
				{
					USART_WriteString("\r\n保存数据失败!");
					USART_WriteString("\r\n错误代码: %u",res);									
				}
				break;
			}
			else if ((key < 0x21) || (key > 0x80))
			{
				continue;
			}
			else
			{
				file_buff[index++] = key;
				USART_WriteString("%c",key);
				if (index > 512)
				{
					index = 0x00;
				}
			}
		}
	}
	else
	{
		USART_WriteString("\r\n打开文件失败,错误代码: %u",res);
	}
}
void read_file(void)
{

	FATFS fs;
//	FATFS *fls = &fs;
	FIL	file;
	FRESULT res; 
	DIR dirs;
	FILINFO finfo;
	char path[20];
	char buffer[512] = {0};
	uint32_t i;
	uint8_t length = 255;
	uint32_t re,files_num = 0;
	res = f_mount(0,&fs);
	if (res != FR_OK)
	{
		USART_WriteString("\r\n挂载文件系统失败,错误代码: %u",res);
		return;
	}
	res = f_opendir(&dirs,"/");
	if (res == FR_OK) 
	{
		//i = strlen(path);
		USART_WriteString("\r\n-----------文件列表-------");
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		{
			if (finfo.fattrib & AM_DIR)//如果是文件夹 
			{ 
				continue;
			} 
			else 
			{   
				files_num++;
				//显示文件名,显示文件实际大小 ,文件实际大小采用四舍五入法
				USART_WriteString("\r\n/%12s%7ld KB ",  &finfo.fname[0],(finfo.fsize+512)/1024);

			}
		}
		if( files_num == 0 )//无文件
		{
			USART_WriteString("\r\n无文件,请返回先创建文件!");
			return;
			
		}
	}
	else
	{
		USART_WriteString("\r\n打开根目录失败!");
		USART_WriteString("\r\n错误代码: %u",res);
	}
	USART_WriteString("\r\n输入文件全名,以回车结束...");
	get_file_name((uint8_t *)path,length,FI);
	res = f_open(&file,path,FA_READ);
	USART_WriteString("\r\n正在打开文件,以下是文件数据:\r\n");

	if (res == FR_OK)
	{
		while (1)
		{

			for(i = 0;i < 512;i++)
			{
				buffer[i] = 0x00;
			}
			res = f_read(&file,buffer,512,&re);
			USART_WriteString("%s",buffer);

			if (res || re == 0)
			{
				USART_WriteString("\r\n文件读取结束,关闭文件!");
				f_close(&file);
				break;  
			}	
		}
	}
	f_mount(0,NULL);
}

void creat_dir(void)
{
	FATFS fs;        
	FRESULT res;     
	char path[20];
	uint8_t length = 255;
	res = f_mount(0,&fs);
	if (res != FR_OK)
	{
		USART_WriteString("\r\n挂载文件系统失败,错误代码: %u",res);
		return;
	}	
	USART_WriteString("\r\n请输入文件夹名,回车确认...格式 8 + 3...");
	
	get_file_name((uint8_t *)path,length,DI);

	res = f_mkdir(path);
	if (res == FR_OK)
	{
		USART_WriteString("\r\n创建文件目录成功!");
	}
	else
	{
		USART_WriteString("\r\n创建目录失败...");
		USART_WriteString("\r\n错误代码: %u",res);
	}
	f_mount(0,NULL);
}


void get_file_name(uint8_t *file_name,uint8_t length,uint8_t type)
{
	uint8_t key;
	uint8_t name_leng = 0;
	USART_WriteString("\r\n");
	
	while (1)
	{
		key = GetKey();
		if ((key == 13) && (name_leng == 0))
		{
			USART_WriteString("\r\n");
			continue;
		}
		else if ((key == 0x2F) || (key == 0x5C))
		{
			USART_WriteString("%c",key);
			continue;
		}
		else if ((key == 13) && (name_leng > 0))
		{
			USART_WriteString("\r\n");
			if (type == FI)
			{
				if (check_file_name(file_name,name_leng) == 0)
				{
					break;
				}
			}
			else
			{
				break;
			}			
			
		}
		else
		{
			USART_WriteString("%c",key);
			file_name[name_leng] = key;
			name_leng++;
			if (name_leng > 12)
			{
				USART_WriteString("\r\n文件名格式: 8 + 3，只支持8个字符,3个扩展名!");
				USART_WriteString("\r\n请重新输入...");
				name_leng = 0;
				continue;
			}
		}
	}
}
void format_disk(void)
{
	FATFS fs;
//	FATFS *fls = &fs;
	uint8_t res;
	res = f_mount(0,&fs);
	if (res != FR_OK)
	{
		USART_WriteString("\r\n挂载文件系统失败,错误代码: %u",res);
		return;
	}	
	USART_WriteString("\r\n正在格式化磁盘,请稍候...");

	res = f_mkfs(0,1,4096);
	if (res == FR_OK)
	{
		USART_WriteString("\r\n格式化成功...");
	}
	else
	{
		USART_WriteString("\r\n格式化失败...");
		USART_WriteString("\r\n错误代码: %u",res);
	}
	f_mount(0,NULL);
}

void creat_file(void)
{
	FIL file;
	FIL *pf = &file;
	FATFS fs;
//	FATFS *fls = &fs;
	uint8_t res;
	uint8_t name[16] = {0};
	uint8_t length = 0;
	USART_WriteString("\r\n请输入文件名,回车确认...格式 8 + 3...");
	USART_WriteString("\r\n例:123.dat\r\n");
	get_file_name(name,length,FI);	
	res = f_mount(0,&fs);
	if (res != FR_OK)
	{
		USART_WriteString("\r\n挂载文件系统失败,错误代码: %u",res);
		return;
	}	
	res = f_open(pf,(TCHAR *)name,FA_READ | FA_WRITE | FA_CREATE_NEW);
	if (res == FR_OK)
	{
		USART_WriteString("\r\n创建文件成功!");
		res = f_close(pf);
		if (res != FR_OK)
		{
			USART_WriteString("\r\n创建文件成功,但关闭文件时,失败!");
			USART_WriteString("\r\n错误代码: %u",res);				
		}				
	}
	else
	{
		USART_WriteString("\r\n创建文件失败!");
		USART_WriteString("\r\n错误代码: %u",res);	
	}
	f_mount(0,NULL);
}
void delete_file(void)
{
	FATFS fs;
//	FATFS *fls = &fs;
	FRESULT res;
	uint8_t name[16] = {0};
	uint8_t length = 0;
	get_file_name(name,length,FI);	
	res = f_mount(0,&fs);
	if (res != FR_OK)
	{
		USART_WriteString("\r\n挂载文件系统失败,错误代码: %u",res);
		return;
	}	
	res = f_unlink((TCHAR *)name);

	if (res == FR_OK)
	{
		USART_WriteString("\r\n删除文件成功!");
	}
	else if (res == FR_NO_FILE)
	{
		USART_WriteString("\r\n找不到文件或目录!");
	}
	else if (res == FR_NO_PATH)
	{
		USART_WriteString("\r\n找不到路径!");
	}
	else
	{
		USART_WriteString("\r\n错误代码: %u",res);
	}
	f_mount(0,NULL);
}
void list_file(void)
{
	FATFS fs;
	FILINFO finfo;
//	FATFS *fls = &fs;
	FRESULT res;
	DIR dirs;
	int i;
	int files_num=0;
	res = f_mount(0,&fs);
	if (res != FR_OK)
	{
		USART_WriteString("\r\n挂载文件系统失败,错误代码: %u",res);
		return;
	}	
	res = f_opendir(&dirs,"/");
	USART_WriteString("\r\n------------文件列表------------");
	if (res == FR_OK)
	{
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		{
			i = strlen(finfo.fname);
			if (finfo.fattrib & AM_DIR)//如果是文件夹 
			{
				files_num++;
				USART_WriteString("\r\n/%s", &finfo.fname[0]);
				switch(i)//作用：输出文件名左对齐
				{
				case 1:USART_WriteString(" ");
				case 2:USART_WriteString(" ");
				case 3:USART_WriteString(" ");
				case 4:USART_WriteString(" ");
				case 5:USART_WriteString(" ");
				case 6:USART_WriteString(" ");
				case 7:USART_WriteString(" ");
				case 8:USART_WriteString("%15s"," ");
				}

				
			} 
			else 
			{
				continue;
			}	
		}

	}
	else
	{
		USART_WriteString("\r\n打开根目录失败!");
		USART_WriteString("\r\n错误代码: %u",res);
	}
	res = f_opendir(&dirs, "/");
	if (res == FR_OK) 
	{
		//i = strlen(path);
		
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		{
			if (finfo.fattrib & AM_DIR) 
			{ 
				continue;
			} 
			else 
			{   
				files_num++;				
				USART_WriteString("\r\n/.%12s%7ld B ",  &finfo.fname[0],finfo.fsize);				
			}
		}
		if( files_num==0 )//无文件
		{
			USART_WriteString("\r\n无文件!");    
		}
	}
	else
	{
		USART_WriteString("\r\n打开根目录失败!");
		USART_WriteString("\r\n错误代码: %u",res);
	}
	f_mount(0,NULL);

}

void get_disk_info(void)
{
	FATFS fs;
	FATFS *fls = &fs;
	FRESULT res;
	DWORD clust,tot_sect,fre_sect;	

	res = f_mount(0,&fs);
	if (res != FR_OK)
	{
		USART_WriteString("\r\n挂载文件系统失败,错误代码: %u",res);
		return;
	}	

	res = f_getfree("/",&clust,&fls);
	if (res == FR_OK) 
	{
		//tot_sect = (fls->max_clust - 2) * fls->csize;
		tot_sect = (fls->last_clust - 2) * fls->csize;
		fre_sect = (clust * fls->csize * fls->ssize) / 1024;

		USART_WriteString("\r\nfree space in unit of KB (assuming %d Bytes/sector)",fls->ssize);
		USART_WriteString("\r\nB total drive space.\r\n %d KB available.",fre_sect);
	}
	else
	{
		USART_WriteString("\r\n获得磁盘信息失败!");
		USART_WriteString("\r\n错误代码: %u",res);
	}
	
	f_mount(0,NULL);
}
uint8_t check_file_name(uint8_t *file_name,uint8_t length)
{
	uint8_t res;
	if (length > 13)
	{
		res = 1;
	}
	else
	{
		if (file_name[length - 4] == '.')
		{
			res = 0;
		}
		else
		{
			res = 2;
		}
	}
	return res;

}

void SerialPutChar(uint8_t c)
{
	USART_SendData(USART1, c);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_PutString(uint8_t *s)
{
	while (*s != '\0')
	{
		SerialPutChar(*s);
		s ++;
	}
}

/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param  key: The key pressed
  * @retval 1: Correct
  *         0: Error
  */
uint32_t SerialKeyPressed(uint8_t *key)
{

  if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
  {
    *key = (uint8_t)USART1->DR;
    return 1;
  }
  else
  {
    return 0;
  }
}
/**
  * @brief  Get a key from the HyperTerminal
  * @param  None
  * @retval The Key Pressed
  */
uint8_t GetKey(void)
{
  uint8_t key = 0;

  /* Waiting for user input */
  while (1)
  {
    if (SerialKeyPressed((uint8_t*)&key)) break;
  }
  return key;

}

void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
}

int fputc(int ch,FILE *t) 
{ 
	USART_SendData(USART1, (u8) ch);  
	/* Loop until the end of transmission */ 
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

	return ch; 
}

