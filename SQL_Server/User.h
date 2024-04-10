#pragma once

class User {

public:
	enum class Privilidge : int
	{
		Unverified = -1,
		Basic = 0,
		Writer = 1,
		Admin = 2
	};

private:
	Privilidge privilidge = Privilidge::Unverified;

public:

	User() {}
	User(Privilidge privilidge)
		:privilidge(privilidge)
	{}

	void setPrivilidge(Privilidge privilidge)
	{
		this->privilidge = privilidge;
	}

	Privilidge getPrivilidge()
	{
		return privilidge;
	}
};