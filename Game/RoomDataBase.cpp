#include <fstream>
#include <vector>
#include "../Libraries/BasicLib/BasicLib.h"
#include "RoomDatabase.h"
#include "../MyUtils.h"
#include <sstream>
#include <mysql.h>

using BasicLib::LowerCase;
using BasicLib::tostring;
using std::string;
using std::ifstream;
using std::ofstream;


namespace MUDGame
{
	// declare the static vector of the room database.
	std::vector<Room> EntityDatabaseVector<Room>::m_vector;


	void RoomDatabase::LoadTemplates()
	{
		MYSQL* conn, connection;

		MYSQL_RES* result = nullptr;

		MYSQL_ROW row;

		int numRows = 0;

		string sQuery = "SELECT * FROM room";

		mysql_init(&connection);

		conn = mysql_real_connect(&connection, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
		
		if (!conn)
		{
			fprintf(stderr, "MYSQL CONNECTION ERROR : %s", mysql_error(conn));
			return;
		}

		if (mysql_query(conn, sQuery.c_str()) != 0) 
		{
			fprintf(stderr, "[INFO] MYSQL QUERY ERROR : %s", mysql_error(conn));
			return;
		}

		result = mysql_store_result(conn);

		numRows = static_cast<int>(mysql_num_rows(result));

		printf("[INFO] %d number of room data from Room Database load successfully . \n", numRows);

		m_vector.resize(static_cast<size_t>(numRows) + 1);

		while ((row = mysql_fetch_row(result)))
		{
			// ID
			entityid id = atoi(row[0]);
			m_vector[id].ID() = id;
			// NAME
			m_vector[id].Name() = row[1];
			// DESCRIPTION
			m_vector[id].Description() = row[2];
			// TYPE
			m_vector[id].Type() = GetRoomType((std::string)row[3]);
			// AREA
			m_vector[id].InArea() = GetArea((std::string)row[4]);
			// VECTOR NORTH
			m_vector[id].m_rooms[NORTH] = atoi(row[5]);
			// VECTOR EAST
			m_vector[id].m_rooms[EAST] = atoi(row[6]);
			// VECTOR SOUTH
			m_vector[id].m_rooms[SOUTH] = atoi(row[7]);
			// VECTOR WEST
			m_vector[id].m_rooms[WEST] = atoi(row[8]);
			// ENEMYS
			m_vector[id].SpawnWhich() = atoi(row[9]);
			// MAX ENEMIES
			m_vector[id].MaxEnemies() = atoi(row[10]);
		}
	}

	//void RoomDatabase::LoadData()
	//{
	//	MYSQL* conn, connection;
	//	MYSQL_RES* result = nullptr;
	//	MYSQL_ROW row;
	//	int numFields = 0;

	//	mysql_init(&connection);
	//	conn = mysql_real_connect(&connection, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
	//	if (!conn)
	//	{
	//		fprintf(stderr, "MYSQL CONNECTION ERROR : %s", mysql_error(conn));
	//		return;
	//	}
	//	std::stringstream sQuery;
	//	sQuery << "SELECT * FROM room_volatile";
	//	if (mysql_query(conn, sQuery.str().c_str()) != 0) {
	//		fprintf(stderr, "[INFO] MYSQL QUERY ERROR : %s", mysql_error(conn));
	//		return;
	//	}
	//	result = mysql_store_result(conn);

	//	while ((row = mysql_fetch_row(result)))
	//	{
	//		// ID
	//		entityid id = atoi(row[0]);
	//		m_vector[id].m_items.clear();	// Clear all existing items

	//		entityid current, last = 0;
	//		std::stringstream items;
	//		items << row[2];
	//		while (BasicLib::extract(items, current) != last) {
	//			last = current;
	//			m_vector[id].m_items.push_back(last);
	//		}
	//		m_vector[id].Money() = atoi(row[2]);
	//	}

	//}

}