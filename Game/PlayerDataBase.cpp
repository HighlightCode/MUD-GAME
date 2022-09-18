#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <sstream> 
#include "../Libraries/BasicLib/BasicLib.h"
//#include "SimpleMUDLogs.h"
#include "PlayerDatabase.h"
#include "../MyUtils.h"
#include <mysql.h>
#pragma comment(lib, "libmysql.lib")

using BasicLib::LowerCase;
using BasicLib::tostring;
using std::string;
using std::ifstream;
using std::ofstream;


namespace MUDGame
{
	// declare the static variables of the player database.
	std::map<entityid, Player> EntityDatabase<Player>::m_map;

	void PlayerDatabase::LoadPlayer(string p_name) {
		
		MYSQL* conn, connection;
		
		MYSQL_RES* result;
		
		MYSQL_ROW row;

		int numFields = 0;
		
		std::string sQuery = "SELECT * FROM player WHERE name='" + p_name + "'";

		mysql_init(&connection);
		
		conn = mysql_real_connect(&connection, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
		
		if (!conn)
		{
			fprintf(stderr, "MYSQL CONNECTION ERROR : %s \n", mysql_error(conn));
			return;
		}
		
		if (mysql_query(conn, sQuery.c_str()) != 0) 
		{
			fprintf(stderr, "[INFO] MYSQL QUERY ERROR : %s \n", mysql_error(conn));
			return;
		}
		
		result = mysql_store_result(conn);
		
		row = mysql_fetch_row(result);

		entityid id = atoi(row[0]);
		m_map[id].ID()						= id;
		m_map[id].Name()					= row[1];
		m_map[id].Password()				= row[2];
		m_map[id].Rank()					= GetRank(std::string(row[3]));
		m_map[id].StatPoints()			= atoi(row[4]);
		m_map[id].Experience()			= atoi(row[5]);
		m_map[id].Level()					= atoi(row[6]);
		m_map[id].CurrentRoom()		= atoi(row[7]);
		m_map[id].Money()				= atoi(row[8]);
		m_map[id].HitPoints()				= atoi(row[9]);
		m_map[id].NextAttackTime()	= atoi(row[10]);
		
		for (int i = 0; i < NUMATTRIBUTES; i++)
		{
			m_map[i].SetBaseAttr(i, atoi(row[i + 11]));
		}
		
		m_map[id].Weapon() = atoi(row[20]);
		
		m_map[id].Armor() = atoi(row[21]);


		sQuery = "SELECT * FROM " + p_name + "_inventory";
		
		if (mysql_query(conn, sQuery.c_str()) != 0) 
		{
			fprintf(stderr, "[INFO] MYSQL QUERY ERROR : %s \n", mysql_error(conn));
			return;
		}
		
		result = mysql_store_result(conn);
		
		int i = 0;
		
		while ((row = mysql_fetch_row(result)))
		{
			m_map[id].m_inventory[i] = atoi(row[1]);
			i++;
		}
	}

	void PlayerDatabase::SavePlayer(entityid p_player) {
		std::map<entityid, Player>::iterator itr = m_map.find(p_player);
		if (itr == m_map.end())
			return;
		Player* p = &itr->second;


		MYSQL* conn, connection;

		MYSQL_RES* result = nullptr;

		std::stringstream sQuery;
		
		mysql_init(&connection);
		
		conn = mysql_real_connect(&connection, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
		
		if (!conn)
		{
			fprintf(stderr, "MYSQL CONNECTION ERROR : %s", mysql_error(conn));
			return;
		}
		
		sQuery << "REPLACE INTO player SET " <<
			"id = " << p->ID() << ", " <<
			"name='" << p->Name() << "', " <<
			"pass='" << p->Password() << "', " <<
			"rank='" << GetRankString(p->Rank()) << "', " <<
			"statpoints=" << p->StatPoints() << ", " <<
			"experience=" << p->Experience() << ", " <<
			"level=" << p->Level() << ", " <<
			"room=" << p->CurrentRoom() << ", " <<
			"money=" << p->Money() << ", " <<
			"hitpoints=" << p->HitPoints() << ", " <<
			"nextattacktime=" << p->NextAttackTime() << ", " <<
			"strength=" << p->GetBaseAttr(STRENGTH) << ", " <<
			"health=" << p->GetBaseAttr(HEALTH) << ", " <<
			"agility=" << p->GetBaseAttr(AGILITY) << ", " <<
			"maxhitpoints=" << p->GetBaseAttr(MAXHITPOINTS) << ", " <<
			"accuracy=" << p->GetBaseAttr(ACCURACY) << ", " <<
			"dodging=" << p->GetBaseAttr(DODGING) << ", " <<
			"strikedamage=" << p->GetBaseAttr(STRIKEDAMAGE) << ", " <<
			"damageabsorb=" << p->GetBaseAttr(DAMAGEABSORB) << ", " <<
			"hpregen=" << p->GetBaseAttr(HPREGEN) << ", " <<
			"weapon=" << p->Weapon() << ", " <<
			"armor=" << p->Armor();

		if (mysql_query(conn, sQuery.str().c_str()) != 0) 
		{
			fprintf(stderr, "[INFO] MYSQL QUERY ERROR : %s \n", mysql_error(conn));
			return;
		}

		// Check if table playername_inventory is here
		std::string sQur = "SHOW TABLES";
		//query << "SHOW TABLES";
		bool tableExists = false;
		string tableName = p->Name() + "_inventory";
		if (mysql_query(conn, sQur.c_str()) != 0) {
			fprintf(stderr, "[INFO] MYSQL QUERY ERROR : %s \n", mysql_error(conn));
			return;
		}
		result = mysql_store_result(conn);
		//mysqlpp::StoreQueryResult res = query.store();

		// TODO 
		/*for (mysqlpp::StoreQueryResult::iterator itr = res.begin(); itr != res.end(); ++itr) {
			mysqlpp::Row row = *itr;
			if (row["Tables_in_onlinegame"] == tableName) {
				tableExists = true;
				break;
			}
		}*/

		// TODO
		// create the inventory table if not exist
		/*if (!tableExists) {
			query << "CREATE TABLE " << tableName << "(" <<
				"id int auto_increment," <<
				"inventory int," <<
				"primary key(id)," <<
				"foreign key(id) references item(id)" <<
				")";
			query.exec();
		}*/
		// replace every row in playername_inventory
	//	query << "TRUNCATE TABLE " << tableName;
	//	query.exec();

		//TODO
		/*query << "REPLACE INTO " << tableName << " VALUES ";
		int i = 1;
		for (; i < PLAYERITEMS; ++i) {
			query << "(" << i << ", " << p->m_inventory[i] << "),";
		}
		query << "(" << i << ", " << p->m_inventory[i] << ")";
		query.exec();*/
	}


	bool PlayerDatabase::Load() {

		MYSQL* conn, connection;

		MYSQL_RES* result = nullptr;

		MYSQL_ROW row;

		std::string sQuery = "SELECT * FROM player_name";

		mysql_init(&connection);

		conn = mysql_real_connect(&connection, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
		
		if (!conn)
		{
			fprintf(stderr, "MYSQL CONNECTION ERROR : %s \n", mysql_error(conn));
			return 1;
		}


		if (mysql_query(conn, sQuery.c_str()) != 0) 
		{
			fprintf(stderr, "[INFO] MYSQL PLAYER DATABASE QUERY ERROR : %s \n" , mysql_error(conn));
			return false;
		}

		result = mysql_store_result(conn);

		auto numRows = mysql_num_rows(result);

		printf("[INFO] %d number of players from Player Database load successfully . \n", static_cast<int>(numRows));

		while ((row = mysql_fetch_row(result)))
		{
			LoadPlayer(row[1]);
		}
		return true;
	}


	bool PlayerDatabase::Save() {
		// -> Create a connection to the database onlinegame
		/*mysqlpp::Connection con;
		if (!con.connect("onlinegame", "localhost", "root")) {
			USERLOG.Log((std::string)con.error());
			return false;
		}*/

		MYSQL* conn, connection;
		mysql_init(&connection);
		conn = mysql_real_connect(&connection, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
		if (!conn)
		{
			fprintf(stderr, "MYSQL CONNECTION ERROR : %s", mysql_error(conn));
			return 1;
		}

		// iterator itr = begin();
		std::map<entityid, Player>::iterator itr = m_map.begin();


		// while (itr != end() ) {
		while (itr != m_map.end()) {
			// replace player_name values ( .. 
			/*mysqlpp::Query query = con.query();
			query << "REPLACE INTO player_name SET " <<
				"id = " << itr->first << ", "
				"name = " << itr->second.Name();*/
			std::stringstream sQ;
			sQ << "REPLACE INTO player_name SET " <<
				"id = " << itr->first << ", "
				"name = " << itr->second.Name(); 
			//std::string sQuery = "REPLACE INTO player_name SET " + "id = " + (itr->first) + ", " + "name = " + itr->second.Name();
			if (mysql_query(conn, sQ.str().c_str()) != 0) {
				fprintf(stderr, "[INFO] MYSQL QUERY ERROR : %s \n", mysql_error(conn));
				return 1;
			}
			// SavePlayer ( itr->ID() );
			SavePlayer(itr->first);
			// ++itr;
			++itr;
			//}
		}
		return true;
	}

	bool PlayerDatabase::AddPlayer(Player& p_player) {
		// early return
		if (has(p_player.ID()))
			return false;
		if (hasfull(p_player.Name()))
			return false;

		// insert the player into the map
		m_map[p_player.ID()] = p_player;

		// -> Create a connection to the database
		//mysqlpp::Connection con;
		//if (!con.connect("onlinegame", "localhost", "root")) {
		//	USERLOG.Log((std::string)con.error());
		//	return false;
		//}
		//mysqlpp::Query query = con.query();
		//// insert into player_name values (.. 
		//query << "INSERT INTO player_name VALUES (null, '" + p_player.Name() + "')";
		//query.exec();

		MYSQL* conn, connection;
		
		mysql_init(&connection);

		std::string sQuery = "INSERT INTO player_name VALUES (null, '" + p_player.Name() + "')";
		
		conn = mysql_real_connect(&connection, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

		if (!conn)
		{
			fprintf(stderr, "MYSQL CONNECTION ERROR : %s \n", mysql_error(conn));
			return 1;
		}

		if (mysql_query(conn, sQuery.c_str()) != 0) 
		{
			fprintf(stderr, "[INFO] MYSQL ADDPLAYER QUERY ERROR : %s \n", mysql_error(conn));
			return 1;
		}

		// SavePlayer( p_player.ID() );
		SavePlayer(p_player.ID());

		return true;
	}


	void PlayerDatabase::Logout(entityid p_player)
	{
		Player& p = get(p_player);

		std::cout <<
			SocketLib::GetIPString(p.Conn()->GetRemoteAddress()) +
			" - User " + p.Name() + " logged off. \n";

		p.Conn() = 0;

		p.LoggedIn() = false;
		
		p.Active() = false;

		// make sure the player is saved to db
		SavePlayer(p_player);
	}


}