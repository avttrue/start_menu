/*	сделано на примере http://www.hardforum.ru/t53507/
 
 	параметрический список в синтаксисе key=value
 	@ - зарезервированный флаг выбранного ключа (@=0 - выбран первый в списке ключ)
	
	Пример использования:
	"key=777"
	cfg config("cfg.cfg");
    int value = std::stoi(config["key"]);
*/

#ifndef _CFG_
#define _CFG_

#include <map>
#include <string>

using namespace std;

class cfg: public map<string, string>
{
	public:
    	cfg(const char *filename);
    	bool check();
    	void save(const char *filename);
    	string operator [] (const string &s);
    	void setSelIndex(int index);
    	string getSelValue();
		int count = 0;							// количество ключей
};

#endif