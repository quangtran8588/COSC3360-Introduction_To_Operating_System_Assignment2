#include "InputList.h"

InputList::InputList()
{
	empty = true;
	top = NULL;
	ptr = NULL;
}

InputList::~InputList()
{
}

InputList* InputList::setData(string ID, string key)
{
	data *newList = new data();
	newList->userID = ID;
	newList->publicKey = key;
	if (empty) { newList->prev = NULL, newList->next = NULL; top = newList; ptr = newList; empty = false; }
	else
	{
		newList->prev = ptr;
		newList->next = NULL;
		ptr->next = newList;
		ptr = newList;
	}
	return this;
}


string InputList::getUserID(data *list)
{
	return list->userID;
}

string InputList::getPublicKey(data *list)
{
	return list->publicKey;
}

data* InputList::exist(string ID)
{
	data *temp = this->top;
	do
	{
		if (temp->userID == ID) return temp;
		else
		{
			if (temp->next != NULL) temp = temp->next;
			else temp = NULL;
		}
	} while (temp != NULL);
	return temp;
}

void InputList::clean(InputList* dataList)
{
	do
	{
		data* temp = dataList->top;
		if (temp->next != NULL)
		{
			temp->next->prev = NULL;
			dataList->top = temp->next;
			delete temp;
		}
		else
		{
			delete temp;
			empty = true;
			break;
		}
	} while (1);
}

string InputList::toString(InputList *dataList)
{
	string toString;
	data *temp = dataList->top;
	do
	{
		dataList->ptr = temp;
		toString += dataList->getUserID(temp) + " " + dataList->getPublicKey(temp) + "\n";
		if (temp->next != NULL) temp = dataList->ptr->next;
		else break;
	} while (1);
	return toString;
}
