#include "cfg.h"
#include <fstream>
#include <iostream>

cfg::cfg(const char *filename)
{
    std::ifstream in(filename);
    string  str;
    pair<string, string> p;

    while(getline(in, str))
    {
        size_type i = 0;

        while (isspace(*(str.begin())))
            str.erase(0, 1);
        
        i = str.find('=');

        if (i != string::npos)
        {
            p.first = str.substr(0, i);
            while (isspace(*(p.first.rbegin())))
                p.first.erase(p.first.size() - 1, 1);
          
            if (p.first.size())
            {
                // no case
                for (string::iterator j(p.first.begin()); j != p.first.end(); ++j)
                    *j = tolower(*j);

                if((str[str.length()-1] != '='))
                {
                    p.second = str.substr(i + 1);
            
                    while (isspace(*(p.second.begin())))
                        p.second.erase(0, 1);
                }
                else
                    p.second = "";

                if (p.first != "@")  
                    count++;
            }
        }
        else
        {
            p.first  = str;
            p.second = "";
            perror(("ERROR: wrong key: " + str).c_str());
        }

        map<string, string>::operator [] (p.first) = p.second;
    }
}

// записать в файл
void cfg::save(const char *filename)
{
    std::ofstream out(filename);
    for (auto it = begin(); it != end(); ++it)
    {
        if(it->first != "")
            out << it->first << "=" << it->second << "\n";
    }
    out.close();
}

// получить параметр по ключу
string cfg::operator [] (const string &key)
{
    // no case
    string s = key;
    for(string::iterator i(s.begin()); i != s.end(); ++i)
        *i = tolower(*i);

    return map<string, string>::operator[](s);
}

// установить индекс выбранной записи
void cfg::setSelIndex(int index)
{
    map<string, string>::erase("@");
    map<string, string>::insert(pair<string, string>("@", std::to_string(index)));
}

// получить выбранный параметр
string cfg::getSelValue()
{
    int selindex = stoi(map<string, string>::operator [] ("@"));
    int counter = 0;

    for (auto it = begin(); it != end(); ++it)
    {
        if(it->first != "" && it->first != "@")
        { 
            if(counter == selindex)
                return it->second;
            counter++;
        }
    }
    return "";
}

// проверить на наличие ошибок
bool cfg::check()
{
    int selindex = stoi(map<string, string>::operator [] ("@"));
    if (selindex < 0 || selindex >= count)
    {
        selindex = 0;
        std::cout << "\a";
        return false;
    }
    return true;
}



