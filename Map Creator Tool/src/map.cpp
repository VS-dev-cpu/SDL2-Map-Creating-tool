#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

#ifdef __EMSCRIPTEN__
#include "map.h"
#else
#include "include/map.h"
#endif

bool loadMap(const char *path, map *loadTo)
{
    string line;
    ifstream file;
    file.open(path, ios::in);

    int object = 0;
    int texture = 0;

    if (file.is_open())
    {
        while (getline(file, line))
        {
            size_t found = line.find("texture");
            int a = 0;
            int b = 0;
            if (found != string::npos && found == 0)
            {
                found = line.find(" ");
                if (found != string::npos)
                {
                    a = found;
                }
                found = line.find(" ", found + 1);
                if (found != string::npos)
                {
                    b = found;
                }

                string indexSS = line.substr(a + 1, b - a);
                int index = 0;
                stringstream indexS(indexSS);

                indexS >> index;
                string pathS = line.substr(b + 1);

                strcpy(loadTo->paths[index], pathS.c_str());
                if (index > texture)
                    texture = index;
            }
            else
            {
                found = line.find("object");
                a = 0;
                b = 0;
                int spaces[10];
                int current = 0;
                if (found != string::npos && found == 0)
                {
                    found = line.find(" ");
                    if (found != string::npos)
                    {
                        spaces[current] = found;
                        current++;
                    }

                    while (true)
                    {
                        found = line.find(" ", found + 1);
                        if (found != string::npos)
                        {
                            spaces[current] = found;
                            current++;
                        }
                        else
                        {
                            spaces[current] = -1;
                            break;
                        }
                    }

                    current = 0;

                    int dataPack[obj_properties];
                    int currentDP = 0;

                    int a = 0;
                    int b = 0;

                    while (true)
                    {

                        if (spaces[current] != -1 && spaces[current + 1] != -1)
                        {
                            a = spaces[current];
                            b = spaces[current + 1];
                            current++;
                        }
                        else
                        {
                            break;
                        }

                        int data = 0;
                        string dataS = line.substr(a, b - a);
                        stringstream dataSS(dataS);
                        dataSS >> data;

                        if (currentDP < loadTo->properties)
                        {
                            dataPack[currentDP] = data;
                            currentDP++;
                        }
                        else
                        {
                            break;
                        }
                    }

                    int data = 0;
                    string dataS = line.substr(b + 1);
                    stringstream dataSS(dataS);
                    dataSS >> data;

                    dataPack[7] = data;

                    for (int i = 0; i < loadTo->properties; i++)
                    {
                        loadTo->object[object][i] = dataPack[i];
                    }
                    object++;
                }
            }
        }
        loadTo->usedTextures = texture + 1;
        cout << loadTo->usedTextures << endl;
        loadTo->usedObjects = object;

        loadTo->offset_x = 0;
        loadTo->offset_y = 0;

        file.close();
    }
    else
    {
        return false;
    }
    return true;
}

bool saveMap(const char *path, map toSave)
{
    ofstream file;
    file.open(path, ios::in | ios::trunc);

    if (file.is_open())
    {
        for (int i = 0; i < toSave.usedTextures; i++)
        {
            file << "texture " << i << " " << toSave.paths[i] << "\n";
        }

        file << "\n";

        for (int a = 0; a < toSave.usedObjects; a++)
        {
            file << "object";
            for (int b = 0; b < toSave.properties; b++)
            {
                file << " " << toSave.object[a][b];
            }
            file << "\n";
        }

        file.close();
    }
    else
    {
        return false;
    }
    return true;
}