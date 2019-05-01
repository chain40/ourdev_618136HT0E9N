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
		USART_WriteString("\r\n============ ��ѡ��...===============\r\n");
		USART_WriteString("��ʽ��--------------------------------- 1\r\n");
		USART_WriteString("�����ļ�------------------------------- 2\r\n");
		USART_WriteString("ɾ���ļ� ------------------------------ 3\r\n");
		USART_WriteString("�б��ļ�------------------------------- 4\r\n");
		USART_WriteString("����ϵͳ ------------------------------ 5\r\n");
		USART_WriteString("������Ϣ------------------------------- 6\r\n");
		USART_WriteString("����Ŀ¼------------------------------- 7\r\n");
		USART_WriteString("�༭�ļ�------------------------------- 8\r\n");
		USART_WriteString("��ȡ�ļ�------------------------------- 9\r\n");
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
			USART_WriteString("\r\nֻ����1-6������������");
			
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
		USART_WriteString("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}
	res = f_opendir(&dirs,"/");
	USART_WriteString("\r\n------------�ļ��б�------------");
	if (res == FR_OK) 
	{
		//i = strlen(path);		
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		{
			if (finfo.fattrib & AM_DIR)//������ļ��� 
			{ 
				continue;
			} 
			else 
			{   
				files_num++;
				//��ʾ�ļ���,��ʾ�ļ�ʵ�ʴ�С ,�ļ�ʵ�ʴ�С�����������뷨
				USART_WriteString("\r\n/%12s%7ld KB ",  &finfo.fname[0],(finfo.fsize+512)/1024);

			}
		}
		if( files_num == 0 )//���ļ�
		{
			USART_WriteString("\r\n���ļ�!");    
		}
	}
	else
	{
		USART_WriteString("\r\n�򿪸�Ŀ¼ʧ��!");
		USART_WriteString("\r\n�������: %u",res);
	}
	USART_WriteString("\r\n����Ҫ�༭�ļ�ȫ��,�Իس�����...");
	get_file_name((uint8_t *)path,length,FI);
	res = f_open(&file,path,FA_READ | FA_WRITE);
	if (res == FR_OK)
	{
	
		USART_WriteString("\r\n���ļ� %s �ɹ�",path);
		USART_WriteString("\r\n�������ļ�д����,������Ҫд�������!");		
		USART_WriteString("\r\n�� ESC ���� Ctrl+C �����༭!\r\n");
		
		while(1)
		{
			key = GetKey();
			if ((key == 0x1B) && (index == 0x00))//key ESC
			{
				USART_WriteString("\r\n���ݻ�û������,���ڴ��ڱ༭ģʽ...");								
				continue;
			}
			else if ((key == 0x1B))	//key ESC
			{
				USART_WriteString("\r\n��������...");
				res = f_write(&file,file_buff,index,&reindex);
				if ((res == FR_OK) && (reindex == index))
				{
					USART_WriteString("\r\n�������ݳɹ�!");
					f_close(&file);
					index = 0x00;
					reindex = 0x00;									
				}
				else
				{
					USART_WriteString("\r\n��������ʧ��!");
					USART_WriteString("\r\n�������: %u",res);									
				}
				break;
			}
			else if (key == 0x03) //key Ctrl+C
			{
				USART_WriteString("\r\n�����ļ��༭!");
				USART_WriteString("\r\n��������...");
				res = f_write(&file,file_buff,index,&reindex);
				if ((res == FR_OK) && (reindex == index))
				{
					USART_WriteString("\r\n�������ݳɹ�!");
					f_close(&file);
					index = 0x00;
					reindex = 0x00;									
				}
				else
				{
					USART_WriteString("\r\n��������ʧ��!");
					USART_WriteString("\r\n�������: %u",res);									
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
		USART_WriteString("\r\n���ļ�ʧ��,�������: %u",res);
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
		USART_WriteString("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}
	res = f_opendir(&dirs,"/");
	if (res == FR_OK) 
	{
		//i = strlen(path);
		USART_WriteString("\r\n-----------�ļ��б�-------");
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		{
			if (finfo.fattrib & AM_DIR)//������ļ��� 
			{ 
				continue;
			} 
			else 
			{   
				files_num++;
				//��ʾ�ļ���,��ʾ�ļ�ʵ�ʴ�С ,�ļ�ʵ�ʴ�С�����������뷨
				USART_WriteString("\r\n/%12s%7ld KB ",  &finfo.fname[0],(finfo.fsize+512)/1024);

			}
		}
		if( files_num == 0 )//���ļ�
		{
			USART_WriteString("\r\n���ļ�,�뷵���ȴ����ļ�!");
			return;
			
		}
	}
	else
	{
		USART_WriteString("\r\n�򿪸�Ŀ¼ʧ��!");
		USART_WriteString("\r\n�������: %u",res);
	}
	USART_WriteString("\r\n�����ļ�ȫ��,�Իس�����...");
	get_file_name((uint8_t *)path,length,FI);
	res = f_open(&file,path,FA_READ);
	USART_WriteString("\r\n���ڴ��ļ�,�������ļ�����:\r\n");

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
				USART_WriteString("\r\n�ļ���ȡ����,�ر��ļ�!");
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
		USART_WriteString("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}	
	USART_WriteString("\r\n�������ļ�����,�س�ȷ��...��ʽ 8 + 3...");
	
	get_file_name((uint8_t *)path,length,DI);

	res = f_mkdir(path);
	if (res == FR_OK)
	{
		USART_WriteString("\r\n�����ļ�Ŀ¼�ɹ�!");
	}
	else
	{
		USART_WriteString("\r\n����Ŀ¼ʧ��...");
		USART_WriteString("\r\n�������: %u",res);
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
				USART_WriteString("\r\n�ļ�����ʽ: 8 + 3��ֻ֧��8���ַ�,3����չ��!");
				USART_WriteString("\r\n����������...");
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
		USART_WriteString("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}	
	USART_WriteString("\r\n���ڸ�ʽ������,���Ժ�...");

	res = f_mkfs(0,1,4096);
	if (res == FR_OK)
	{
		USART_WriteString("\r\n��ʽ���ɹ�...");
	}
	else
	{
		USART_WriteString("\r\n��ʽ��ʧ��...");
		USART_WriteString("\r\n�������: %u",res);
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
	USART_WriteString("\r\n�������ļ���,�س�ȷ��...��ʽ 8 + 3...");
	USART_WriteString("\r\n��:123.dat\r\n");
	get_file_name(name,length,FI);	
	res = f_mount(0,&fs);
	if (res != FR_OK)
	{
		USART_WriteString("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}	
	res = f_open(pf,(TCHAR *)name,FA_READ | FA_WRITE | FA_CREATE_NEW);
	if (res == FR_OK)
	{
		USART_WriteString("\r\n�����ļ��ɹ�!");
		res = f_close(pf);
		if (res != FR_OK)
		{
			USART_WriteString("\r\n�����ļ��ɹ�,���ر��ļ�ʱ,ʧ��!");
			USART_WriteString("\r\n�������: %u",res);				
		}				
	}
	else
	{
		USART_WriteString("\r\n�����ļ�ʧ��!");
		USART_WriteString("\r\n�������: %u",res);	
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
		USART_WriteString("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}	
	res = f_unlink((TCHAR *)name);

	if (res == FR_OK)
	{
		USART_WriteString("\r\nɾ���ļ��ɹ�!");
	}
	else if (res == FR_NO_FILE)
	{
		USART_WriteString("\r\n�Ҳ����ļ���Ŀ¼!");
	}
	else if (res == FR_NO_PATH)
	{
		USART_WriteString("\r\n�Ҳ���·��!");
	}
	else
	{
		USART_WriteString("\r\n�������: %u",res);
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
		USART_WriteString("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}	
	res = f_opendir(&dirs,"/");
	USART_WriteString("\r\n------------�ļ��б�------------");
	if (res == FR_OK)
	{
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		{
			i = strlen(finfo.fname);
			if (finfo.fattrib & AM_DIR)//������ļ��� 
			{
				files_num++;
				USART_WriteString("\r\n/%s", &finfo.fname[0]);
				switch(i)//���ã�����ļ��������
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
		USART_WriteString("\r\n�򿪸�Ŀ¼ʧ��!");
		USART_WriteString("\r\n�������: %u",res);
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
		if( files_num==0 )//���ļ�
		{
			USART_WriteString("\r\n���ļ�!");    
		}
	}
	else
	{
		USART_WriteString("\r\n�򿪸�Ŀ¼ʧ��!");
		USART_WriteString("\r\n�������: %u",res);
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
		USART_WriteString("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
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
		USART_WriteString("\r\n��ô�����Ϣʧ��!");
		USART_WriteString("\r\n�������: %u",res);
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

