# MYSQL

```c
/*
 * main.c
 *
 *  Created on: 2016年9月2日
 *      Author: ken
 */
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int err;
	char outbuf[100], inbuf[100];
	MYSQL mysql, *connection;
	mysql_init(&mysql);

	connection = mysql_real_connect(&mysql, "localhost", "root", "xinge", "db1", 0, NULL, 0);
	if(!connection)
	{
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}

	err = mysql_query(connection, "SET names utf8;");
	if( err )
	{
		printf("%s\n",mysql_error(&mysql));
		goto err;
	}


	err = mysql_query(connection, "SELECT * FROM table1");
	if( err )
	{
		printf("%s\n",mysql_error(&mysql));
		goto err;
	}
	MYSQL_RES *result;
	result = mysql_store_result(connection);

	MYSQL_FIELD *field;
	while(field = mysql_fetch_field(result))
	{
		printf("%s\t", field->name);
	}
	printf("\n");
	MYSQL_ROW row;

	while(row = mysql_fetch_row(result))
	{
		printf("%s	%s\n", row[0], row[1]);
	}



	mysql_free_result(result);


//	sprintf(outbuf, "输入名字：>");
//	write(STDOUT_FILENO, outbuf, strlen(outbuf));
//
//	read(STDIN_FILENO, inbuf, sizeof(inbuf));
//	inbuf[strlen(inbuf) - 1] = '\0';
//
//	sprintf(outbuf, "INSERT into table1 values ('%s', 17);", inbuf);
//	err = mysql_query(connection, outbuf);
//	if( err )
//	{
//		printf("%s\n",mysql_error(&mysql));
//		goto err;
//	}


err:
	mysql_close(connection);
	printf("close success\n");
	return 0;
}


```
