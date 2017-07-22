/*
 * init_file.c
 *
 *  Created on: Jul 21, 2017
 *      Author: ken
 */




/**  [section]
 *   key=value
 *   #  ;
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum LineType
{
	COMMENT,
	SECTION,
	KEY,
	ERR
};

static enum LineType get_current_line_type(const char *str);
static int is_aim_section(const char *cur_str, const char *section);
static int is_aim_key(const char *cur_str, char *key);
static int get_keyvalue_from_current_line(const char *cur_str, char *value, int nvalue);



int get_value(char *value, int nvalue, char *file_name, char *section, char *key)
{
	int err = 0;
	char *cur_line = NULL;
	int nline = 0;
	FILE *file;


	file = fopen(file_name, "r");
	if (!file)
	{
		return -1;
	}

	//寻找section
	while (1)
	{
		err = getline(&cur_line, &nline, file);
		if (err<0)
		{
			//未找到section
			goto _err;
		}


		if (get_current_line_type(cur_line) != SECTION)
		{
			continue;
		}

		if (is_aim_section(cur_line, section)!=1)
		{
            continue;
		}

		break;


	}

	//寻找key
    while (1)
    {
		err = getline(&cur_line, &nline, file);
		if (err<0)
		{
			goto _err; //未找到key
		}

		switch (get_current_line_type(cur_line))
		{
		case SECTION: //遇到下一个section 未找到
			goto _err;;
		case KEY:
			if (is_aim_key(cur_line, key) != 1) continue;
			return get_keyvalue_from_current_line(cur_line, value, nline);
		default:
			continue;
		}

    }

_err:
	free(cur_line);
	fclose(file);
	return err;

}

/*
 * 判断当前行的类型
 */
static enum LineType get_current_line_type(const char *str)
{
	unsigned int i = 0;
	for (; str[i]==' '; i++);//跳过空格
    if (str[i] == '\0') return ERR;
    if (str[i]=='#' || str[i]==';') return COMMENT;//注释
    if (str[i]=='[')
    {
    	for (i++; str[i]!=']'; i++)
    	{
    		if (str[i]=='\0') return ERR;

    	}
    	return SECTION;
    }
    for (i++; str[i]!='='; i++)
    {
    	if (str[i]=='\0') return ERR;
    }

	return KEY;
}

/*
 * 判断是否是指定的section  是返回 1
 */
static int is_aim_section(const char *cur_str, const char *section)
{
    unsigned int i = 0;

    for (;cur_str[i]!='['; i++);//找[
    for (i++;cur_str[i]==' '; i++);//跳过空字符

    if (strlen(&cur_str[i])<strlen(section)) return -1;
    if (strncmp(&cur_str[i], section, strlen(section)))
    {
    	return -1;
    }

    if (cur_str[i+strlen(section)] == ' ' || cur_str[i+strlen(section)] == ']')
    	return 1;

    return -1;
}
/*
 * 判断是否是指定的 key
 * 是 返回 1
 */
static int is_aim_key(const char *cur_str, char *key)
{
    unsigned int i = 0;

    for (;cur_str[i] ==' '; i++);//跳过空字符

    if (strlen(&cur_str[i])<strlen(key)) return -1;
    if (strncmp(&cur_str[i], key, strlen(key)))
    {
    	return -1;
    }

    if (cur_str[i+strlen(key)] == ' ' || cur_str[i+strlen(key)] == '=')
    	return 1;

    return -1;

}

/*
 * 读取出key 的值
 */
static int get_keyvalue_from_current_line(const char *cur_str, char *value, int nvalue)
{
    unsigned int i = 0;
    int count = 0;
    unsigned int begin = 0;

    for (;cur_str[i]!='='; i++);//找到 =
    for (i++;cur_str[i]==' '; i++);//跳过空字符

	for (begin=i; (cur_str[i]!=' ') && (cur_str[i]!='\0')
					&& (cur_str[i]!='\n'); ++count,++i);
//    for (begin=i; (cur_str[i]!=' ') && (cur_str[i]!='\0')
//    		&& (cur_str[i]!='#') && (cur_str[i]!=';') && (cur_str[i]!='\n'); ++count,++i);
    if (count<=0) return 0;//空
    if (count>nvalue+1) return -2;
    strncpy(value, &cur_str[begin], count);

    return count;
}


