#pragma once
#include <string>

using namespace std;

struct data
{
	string userID;
	string publicKey;
	data *next;
	data *prev;
	data() : userID(""), publicKey(""){}
};

class InputList
{
private:
	bool empty;
	data* top;
	data* ptr;
public:
	InputList();
	~InputList();
	InputList* setData(string ID, string key);
	string getUserID(data *list);
	string getPublicKey(data *list);
	data* exist(string ID);
	void clean(InputList* dataList);
	string toString(InputList *list);
};