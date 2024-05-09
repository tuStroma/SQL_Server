#include <server_infrastructure.h>
#include "../SQL_context.h"
#include "../common/SQL_table.h"

class SQL_Client : public net::client::IClient<SQL_context>
{
private:
	std::string logged_as = "";

	// Loggin process
	std::condition_variable server_login_response;

public:
	SQL_Client() : net::client::IClient<SQL_context>() {}

	bool LoginProcess()
	{
		std::mutex m;
		std::unique_lock<std::mutex> lk(m);

		while (logged_as == "")
		{
			std::string username, password;
			std::cout << "Login:\t\t";		std::getline(std::cin, username);
			std::cout << "Password:\t";		std::getline(std::cin, password); //std::cin >> password;


			net::common::Message<SQL_context> login(Login, username.length() + password.length() + 2);
			login.putString(username.c_str());
			login.putString(password.c_str());

			this->Send(login);

			server_login_response.wait(lk);
		}

		return true;
	}

protected:
	virtual void OnMessage(net::common::Message<SQL_context>* msg)
	{
		char username[30];
		msg->getString(username);

		switch(msg->getHeader().getType())
		{
		case LoginSuccess: 
		{
			std::cout << "Logged as: " << username << "\n"; 
			logged_as = std::string(username);
			server_login_response.notify_one();
			break;
		}

		case LoginFail:
		{
			std::cout << "Incorrect password for user: " << username << "\n";
			server_login_response.notify_one();
			break;
		}
		case SendTable:
		{
			common::SQL_table table = common::SQL_table::Deserialize(msg->getBody(), msg->getSize());
			table.print();
		}
		}

		delete msg;
	}
	virtual void OnDisconnect() {
		std::cout << "Disconnected\n";
	}
};

void loginScreen(SQL_Client& client)
{
	std::string username, password;
	std::cout << "Login:\t\t";		std::getline(std::cin, username);
	std::cout << "Password:\t";		std::getline(std::cin, password); //std::cin >> password;


	net::common::Message<SQL_context> login(Login, username.length() + password.length() + 2);
	login.putString(username.c_str());
	login.putString(password.c_str());

	client.Send(login);
}

int main()
{
	SQL_Client client = SQL_Client();
	if (!client.Connect("127.0.0.1", 60001))
		return 0;

	client.LoginProcess();

	while (true)
	{
		std::string command;
		std::cin >> command;

		if (command == "q")
			break;
		else if (command == "employee")
		{
			net::common::Message<SQL_context> select(SelectEmployees, 0);
			client.Send(select);
		}
		else if (command == "project")
		{
			std::cout << "Search project:\t";
			std::string project;
			std::cin.get(); std::getline(std::cin, project);

			net::common::Message<SQL_context> select(SelectProjects, project.length() + 1);
			select.putString(project.c_str());
			client.Send(select);
		}
		else if (command == "w")
		{
		}
	}

	client.Disconnect();
}

