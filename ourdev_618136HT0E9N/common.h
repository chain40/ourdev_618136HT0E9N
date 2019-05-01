
#ifndef _COMMON_H_
#define _COMMON_H_  1

#include "stm32f10x.h"

#define FI 1
#define DI 2

void fat_menu_init(void);
void usart1_init(void);

uint32_t SerialKeyPressed(uint8_t *key);
void Serial_PutString(uint8_t *s);
uint8_t GetKey(void);
void Sys_Soft_Reset(void);
void get_file_name(uint8_t *file_name,uint8_t length,uint8_t type);
uint8_t check_file_name(uint8_t *file_name,uint8_t length);

void format_disk(void);
void creat_file(void);
void delete_file(void);
void list_file(void);
void get_disk_info(void);
void creat_dir(void);
void edit_file(void);
void read_file(void);

#endif

