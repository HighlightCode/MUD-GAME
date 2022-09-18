#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include "../Libraries/BasicLib/BasicLib.h"
//#include "SimpleMUDLogs.h"
#include "../MyUtils.h"
#include "ItemDatabase.h"
#include <mysql.h>
#pragma comment(lib, "libmysql.lib")

using BasicLib::LowerCase;
using BasicLib::tostring;

namespace MUDGame
{
	MYSQL* conn, connection;
	MYSQL_RES* result;
	MYSQL_ROW row;

	// declare the static map of the Item database.
	std::map<entityid, Item> EntityDatabase<Item>::m_map;

	bool ItemDatabase::Load() {

		MYSQL* conn, connection;

		MYSQL_RES* result = nullptr;

		MYSQL_ROW row;

		string sQuery = "SELECT * FROM item";

		mysql_init(&connection);
		
		conn = mysql_real_connect(&connection, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
		
		if (!conn)
		{
			fprintf(stderr, "MYSQL CONNECTION ERROR : %s \n", mysql_error(conn));
			return 1;
		}

		if (mysql_query(conn, sQuery.c_str()) != 0) 
		{
			fprintf(stderr, "[INFO] MYSQL QUERY ERROR : %s \n", mysql_error(conn));
			return 1;
		}

		result = mysql_store_result(conn);

		auto numRows = mysql_num_rows(result);

		printf("[INFO] %d number of Items from Item Database load successfully . \n", static_cast<int>(numRows));

		// Detatch Every Row
		while ((row = mysql_fetch_row(result)))
		{
			// ID 
			entityid id = atoi(row[0]);
			m_map[id].ID() = id;
			// name
			m_map[id].Name() = row[1];
			// type
			m_map[id].Type() = GetItemType(std::string(row[2]));
			// min
			m_map[id].Min() = atoi(row[3]);
			// max
			m_map[id].Max() = atoi(row[4]);
			// speed
			m_map[id].Speed() = atoi(row[5]);
			// price
			m_map[id].Price() = atoi(row[6]);
			// item Attribute Settings
			for (int i = 0; i < NUMATTRIBUTES; i++)
			{
				m_map[i].m_attributes[i] = atoi(row[i+7]);
			}
		}
		return true;
	}
}