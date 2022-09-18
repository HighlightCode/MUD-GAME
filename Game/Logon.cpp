#include "Logon.h"
#include "PlayerDatabase.h"
#include "../Libraries/BasicLib/BasicLib.h"

using namespace SocketLib;

namespace MUDGame
{
	void Logon::Handle(string p_data)
	{
		if (m_errors >= 6) {
			m_connection->Close();
			return;
		}
		switch (m_state) {
		case LogonState::NEWCONNECTION:
			if (BasicLib::LowerCase(p_data) == "new") {
				m_state = LogonState::NEWUSER;
				const std::string newString = bold + white + "Please enter your new login name: " + reset; 
				const int sizeString = newString.size();
				m_connection->Send(newString.c_str(), sizeString);
				return;
			}
			else {
				PlayerDatabase::iterator itr = PlayerDatabase::findfull(p_data);
				//	std::map<entityid, Player>::iterator itr = PlayerDatabase::findfull(p_data);
				if (itr == PlayerDatabase::end()) {
					m_errors++;
					const std::string ErrorFindUser = bold +red + "Sorry, user " +
						green + p_data + red + " does not exist.\r\n" +
						white + "Please enter your name, or \"new\" if you are new user: " + reset;
					const int sizeString = ErrorFindUser.size();
					m_connection->Send(ErrorFindUser.c_str(), sizeString);
					return;
				}
				else if (itr->LoggedIn()) {
					m_errors++;
					const std::string ErrorFindUser = bold + red + "Sorry, user " +
						green + p_data + red + " is logged in.\r\n" +
						white + "Please enter your name, or \"new\" if you are new user: " + reset;
					const int sizeString = ErrorFindUser.size();
					m_connection->Send(ErrorFindUser.c_str(), sizeString);
					return;
				}
				else {
					m_state = LogonState::ENTERPASS;
					m_name = p_data;
					m_pass = itr->Password();
					const std::string EnterPassword = bold + white + "Please enter the password: " + reset + bwhite;
					const int sizeString = EnterPassword.size();
					m_connection->Send(EnterPassword.c_str(), sizeString);
					return;
				}
			}
			break;

		// NEW USER COMMING
		case LogonState::NEWUSER:
			// Check if p_data is in Database
			// If yes m_errors++;
			if (PlayerDatabase::hasfull(p_data)) {
				m_errors++;
				m_state = LogonState::NEWCONNECTION;
				const std::string EnterNewUser = bold +red + bold + "Sorry, the name " + green + p_data +
															red + " is already used.\r\n" +
															white + "Please enter your name, or \"new\" if you are new user: " +	reset;
				const int sizeString = EnterNewUser.size();
				m_connection->Send(EnterNewUser.c_str(), sizeString);
				return;
			}
			// NAME is unable
			else if (!AcceptibleName(p_data)) {
				m_errors++;
				m_state = LogonState::NEWCONNECTION;
				const std::string EnterNewUser = bold + red + bold + "Sorry, the name " + green + p_data +
															red + " contains invalid characters.\r\n" +
															white + "Please enter your name, or \"new\" if you are new user: " + reset;
				const int sizeString = EnterNewUser.size();
				m_connection->Send(EnterNewUser.c_str(), sizeString);
				return;
			}
			else {
				m_state = LogonState::ENTERNEWPASS;
				m_name = p_data;
				const std::string EnterNewUser = reset + bold +	white + "Please enter your new password: " +
															reset + bwhite;
				const int sizeString = EnterNewUser.size();
				m_connection->Send(EnterNewUser.c_str(), sizeString);
				return;
			}
			break;
		case LogonState::ENTERNEWPASS:
			{
				const std::string enterGameWorld = reset + bold +
					white + "Thank you! You are now entering the realm...\r\n" +
					reset;
				const int sizeString = enterGameWorld.size();
				m_connection->Send(enterGameWorld.c_str(), sizeString);
				Player* p = new Player();
				p->ID() = PlayerDatabase::LastID() + 1;
				p->Name() = m_name;
				p->Password() = p_data;
				PlayerDatabase::AddPlayer(*p);
				delete p;
			}
			GotoGame(true);
			return;
			break;
		case LogonState::ENTERPASS:
			//	std::cout << "m_name=" << m_name << " m_pass=" << m_pass << " p_data=" << p_data << std::endl;
			if (m_pass == p_data) {
				std::string enterGameWorld = reset + bold +
					white + "Thank you! You are now entering the realm...\r\n" +
					reset;
				const int sizeString = enterGameWorld.size();
				m_connection->Send(enterGameWorld.c_str(), sizeString);
				GotoGame(false);
				return;
			}
			else {
				m_errors++;
				m_state = LogonState::NEWCONNECTION;
				std::string enterGameWorld = reset + bold +
					red + "The password is incorrect.\r\n" +
					white + "Please enter your name, or \"new\" if you are new user: " +
					reset;
				const int sizeString = enterGameWorld.size();
				m_connection->Send(enterGameWorld.c_str(), sizeString);
				return;
			}
			break;
		}
	}
	void Logon::Enter()
	{
		const std::string Logo1 = R"( _____ ______   ___  ___  ________)";
		const std::string Logo2 = R"(|\   _ \  _   \|\  \|\  \|\   ___ \)";
		const std::string Logo3 = R"(\ \  \\\__\ \  \ \  \\\  \ \  \_|\ \)";
		const std::string Logo4 = R"( \ \  \\|__| \  \ \  \\\  \ \  \ \\ \)";
		const std::string Logo5 = R"(  \ \  \    \ \  \ \  \\\  \ \  \_\\ \)";
		const std::string Logo6 = R"(   \ \__\    \ \__\ \_______\ \_______\)";
		const std::string Logo7 = R"(    \|__|     \|__|\|_______|\|_______|)";

		const std::string Logo8 = R"( ________  ________  _____ ______   _______)";
		const std::string Logo9 = R"(|\   ____\|\   __  \|\   _ \  _   \|\  ___ \)";
		const std::string Logo10 = R"(\ \  \___|\ \  \ |  \ \  \  \\\__\ \ \  \__/|)";
		const std::string Logo11 = R"( \ \  \  __\ \   __  \ \  \\|__| \  \ \  \_|/__)";
		const std::string Logo12 = R"(  \ \  \|\  \ \  \ \  \ \  \    \ \  \ \  \_|\ \)";
		const std::string Logo13 = R"(   \ \_______\ \__\ \__\ \__\    \ \__\ \_______\)";
		const std::string Logo14 = R"(    \|_______|\|__|\|__|\|__|     \|__|\|_______ |)";
		const static std::string EnterString = bold + red + Logo1 +"\n"
			+ Logo2 + "\n"
			+ Logo3 + "\n"
			+ Logo4 + "\n"
			+ Logo5 + "\n"
			+ Logo6 + "\n"
			+ Logo7 + "\r\n" + cyan
			+ Logo8 + "\n"
			+ Logo9 + "\n"
			+ Logo10 + "\n"
			+ Logo11 + "\n"
			+ Logo12 + "\n"
			+ Logo13 + "\n"
			+ Logo14 + "\n"
			+ green +"\r\nWELCOME TO MUD GAME WORLD! \n" +
			white + "Please enter your name, or \"new\" if you are new user: " + reset;
		const int sizeString = EnterString.size();
		m_connection->Send(EnterString.c_str(), sizeString);
	}

	void Logon::GotoGame(bool p_newbie)
	{
		Player& p = *PlayerDatabase::findfull(m_name);

		// log off the user if he's already connected.            
		if (p.LoggedIn())
		{
			p.Conn()->Close();
			p.Conn()->Handler()->Hungup();
			p.Conn()->ClearHandlers();
		}

		p.Newbie() = p_newbie;

		// record the users new connection
		p.Conn() = m_connection;

		// Add Connection To Game Handler
		// This will change Handler to LOGON -> GAME
		p.Conn()->RemoveHandler();
		p.Conn()->AddHandler(new Game(*p.Conn(), p.ID()));
	}

	bool Logon::AcceptibleName(const string& p_name)
	{
		static string inv = " \"'~!@#$%^&*+/\\[]{}<>()=.,?;:";

		// check any invalid characters
		if (p_name.find_first_of(inv) != string::npos)
			return false;

		// Length Check
		if (p_name.size() > 16 || p_name.size() < 3)
			return false;

		// Start Check
		if (!isalpha(p_name[0]))
			return false;

		if (p_name == "new")
			return false;

		return true;
	}

}  