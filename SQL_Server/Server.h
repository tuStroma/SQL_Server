#pragma once

#include <server_infrastructure.h>
#include "../SQL_context.h"
#include "DataBase.h"
#include "User.h"


class SQL_Server : public net::server::IServer<SQL_context>
{
	DataBase db;

	// Users
	std::unordered_map<uint64_t, User*> users;
	std::mutex users_m;
	std::unique_lock<std::mutex> users_lock;

public:
	SQL_Server(int port, DataBase db) : net::server::IServer<SQL_context>(port), db(db) {
		users_lock = std::unique_lock<std::mutex>(users_m);
		users_lock.unlock();
	}
protected:
	virtual void OnMessage(net::common::Message<SQL_context>* msg, uint64_t sender)
	{

		switch (msg->getHeader().getType())
		{
		case Login:
		{
			char username[50], password[50];
			msg->getString(username);
			msg->getString(password);

			std::cout << "Login: \"" + (std::string)username + "\", password: \"" + (std::string)password << "\"\n";

			std::string login = "SELECT PRIVILIDGE FROM USER"
								"   WHERE USERNAME == '" + (std::string)username + "' AND PASSWORD == '" + (std::string)password + "';";

			common::SQL_table result = db.select(login);

			if (result.getResultsNum() == 1)
			{
				net::common::Message<SQL_context> login_response(LoginSuccess, strlen(username) + 1);
				login_response.putString(username);
				this->Send(login_response, sender);

				users_lock.lock();
				users[sender]->setPrivilidge(User::Privilidge(atoi(result.getValue(0,0))));
				users_lock.unlock();
			}
			else
			{
				net::common::Message<SQL_context> login_response(LoginFail, strlen(username) + 1);
				login_response.putString(username);
				this->Send(login_response, sender);
			}
			break;
		}
		case SelectEmployees:
		{
			common::SQL_table t = db.select("SELECT * FROM EMPLOYEE;");
			void* serialized;
			int size = t.Serialize(&serialized);

			net::common::Message<SQL_context> table(SendTable, size);
			table.put(serialized, size);
			this->Send(table, sender);
			break;
		}
		}

		delete msg;
	}

	virtual bool OnClientConnect(std::string address, uint64_t client_id)
	{
		std::cout << "Welcome " << client_id << " with address " << address << "\n";
		users_lock.lock();
		users[client_id] = new User();
		users_lock.unlock();
		return true;
	}

	virtual void OnClientDisconnect(uint64_t client_id)
	{
		std::cout << "Client " << client_id << " disconnected\n";
		users_lock.lock();
		users.erase(client_id);
		users_lock.unlock();
	}
};
