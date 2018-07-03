#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <dirent.h>

namespace Fondue {

    static std::string read_file(const char* path)
	{
        using namespace std;

		FILE* file = fopen(path, "rt");
		fseek(file, 0, SEEK_END);
		unsigned long length = ftell(file);
        char data[length + 1];
        memset(data, 0, length + 1);
		fseek(file, 0, SEEK_SET);
        fread(data, 1, length, file);
		fclose(file);

        string result(data);
        return result;
	}

    static std::string find_file_in(const char* directory, const char* file)
    {
        using namespace std;
        DIR *dir = opendir(directory);
        if(dir)
        {
            struct dirent *ent;
            while((ent = readdir(dir)) != NULL)
            {
                std::string name(ent->d_name);
                cout << (ent->d_name) << endl;
                if (name.find(file) != std::string::npos)
                    return name;
            }
        }
        else
        {
            cerr << "Error opening directory" << endl;
        }
        return 0;
    }

}
