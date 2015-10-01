#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <pqxx/pqxx>
#include <string>
#include <iostream>

using namespace pqxx;

// PATTERN: Singleton

class Connector
{
	static Connector *_instance;

public:
	static Connector* Instance();
	static connection* connect();

protected:
	Connector() = default;

private:
	const std::string requestPassword() const;
};

#endif // CONNECTOR_H