// 
// 
// 

#include "JSON.h"

JSON::JSON()
{
	;
}

String JSON::getString()
{
	String jsonStr = "<{";
	for(int i = 0; i < numKeyValuePairs; ++i)
	{
		jsonStr += quotify(keys[i]) + ":" + quotify(values[i]);
		if (i != numKeyValuePairs - 1) 
		{
			jsonStr += ",";
		}
	}
	jsonStr += "}>";
	return jsonStr;
}

void JSON::add(String key, String value)
{
	if (numKeyValuePairs < MAX_NUM_KEYS)
	{
		keys[numKeyValuePairs] = key;
		keys[numKeyValuePairs] = value;
		numKeyValuePairs += 1;
	}
}

void JSON::add(String key, int value)
{
	add(key, String(value));
}

String JSON::quotify(String string)
{
	return "\"" + string + "\"";
}
