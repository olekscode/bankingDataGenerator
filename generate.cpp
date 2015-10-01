#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <pqxx/pqxx>

#include "connector.h"

using namespace std;
using namespace pqxx;

string* generateDatePair()
{
	string* date_pair = new string[2];

	string months[12] = {"Jan", "Feb",
				 		"Mar", "Apr", "May",
				 		"Jun", "Jul", "Aug",
				 		"Sep", "Oct", "Nov",
				 		"Dec"};

	int day1 = rand() % 28 + 1;
	int month1 = rand() % 12;
	int year1 = 2015;

	date_pair[0] = to_string(day1) + " "
				+ months[month1] + " "
				+ to_string(year1);

	date_pair[1] = to_string(day1) + " "
				+ months[month1] + " "
				+ to_string(year1 + rand() % 3 + 1);

	return date_pair;
}

string generateClientInsertQuery(string tax_code, int i)
{
	string index = to_string(i);

	return string("INSERT INTO client (\
						first_name, middle_name, last_name, \
						tax_code, address\
					) VALUES ('")
				+ "FN" + index + "', '"
				+ "MN" + index + "', '"
				+ "LN" + index + "', '"
				+ tax_code + "', '"
				+ "ADDR" + index + "');";
}

string generateDepositInsertQuery(string tax_code, int i)
{
	string* date_pair = generateDatePair();

	string query = string("INSERT INTO deposit (\
								client_tax_code, number, \
								start_date, expiration_date, \
								flat_amount, interest_rate\
							) VALUES ('")
					+ tax_code + "', '"
					+ to_string(i) + "', '"
					+ date_pair[0] + "', '"
					+ date_pair[1] + "', '"
					+ to_string(rand() % 10000 + 1000) + "', '"
					+ to_string(rand() % 10 + 1) + "');";
		
	delete[] date_pair;
	return query;
}

string generateOperationInsertQuery(int deposit_id, int i)
{
	string types_of_operation[3] = {
		"interest_charge",
		"interest_withdrawal",
		"deposit_withdrawal"
	};

	string* date_pair = generateDatePair();

	string query = string("INSERT INTO operation (\
								deposit_id, number, \
								date, amount, \
								type\
							) VALUES ('")
					+ to_string(deposit_id) + "', '"
					+ to_string(i) + "', '"
					+ date_pair[0] + "', '"
					+ to_string(rand() % 3000 + 1000) + "', '"
					+ types_of_operation[rand() % 3] + "');";
	
	delete[] date_pair;
	return query;
}

int main(int argc, char* argv[])
{
	connection* C = Connector::connect();

	if (C->is_open()) {
	 cout << "Opened database successfully: " << C->dbname() << endl;
	}
	else {
	 cerr << "Can't open database" << endl;
	 return 1;
	}

	// Create transactional object
    work W(*C);

	string tax_code;
	int deposit_id = 0;

	string query = "DELETE FROM operation;\
					DELETE FROM deposit;\
					DELETE FROM client;\
					ALTER SEQUENCE deposit_id_seq RESTART 1;";

	srand(time(nullptr));

	for (int i = 1; i <= atoi(argv[1]); ++i) {
		tax_code = to_string(1000000000 + i);

		query += generateClientInsertQuery(tax_code, i);

		for (int j = 1; j <= rand() % 10 + 1; ++j) {
			++deposit_id;
			query += generateDepositInsertQuery(tax_code, j);

			W.exec(query);

			query = "";

			for (int k = 1; k <= rand() % 10 + 1; ++k) {
				query += generateOperationInsertQuery(deposit_id, k);
			}

			W.exec(query);

			query = "";
		}
	}

	W.commit();
	C->disconnect();
	delete C;
	return 0;
}