#pragma once

#include "windows.h"
#include <algorithm>
#include <fstream>
#define size_ 3

struct SizeP {
    int x = 55;
    int y = 55;
};
struct RectInfo
{
    SizeP size;
    ImVec2 Pos;
    bool pushed = false;
};

std::string RectList[size_][size_] =
{
    {"", "", ""},
    {"", "", ""}
};
bool is_valid_spawn( int x_size, int y_size, int i, int j)
{
   
    /*
    for (int x = 0; x < x_size; x++)
    {
        for (int y = 0; y < y_size; y++)
        {
            for (int i1 = -1; i1 < 2; i1++)
            {
                for (int i2 = -1; i2 < 2; i2++)
                {
                    if (RectList[i + x + i1][j + y + i2] == 1)
                    {
                    
                        return false;
                    }
                }
            }

        }
    }
    */
    return true;
}
int find_rects(int mat[size_][size_])
{
    /*
    static int copied[size_][size_];

    std::copy(&mat[0][0], &mat[0][0] + size_ * size_, &copied[0][0]);
    int x, y;
    int count = 0;
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            if (copied[i][j] == 1)
            {
                count++;
                x = y = 0;
                while (i + x < size_ && copied[i + x][j] != 0)
                    x++;
                while (j + y < size_ && copied[i][j + y] != 0)
                    y++;
                for (int a = 0; a < x; a++)
                {
                    for (int b = 0; b < y; b++)
                    {
                        copied[i + a][j + b] = 0;
                    }
                }

            }
        }
    }

    return count;
    */
}

void write()
{
    std::ofstream f("settings.txt");
    for (int i = 0; i < size_; i++)
        for (int j = 0; j < size_; j++)
            f << RectList[i][j] << std::endl;
}
void read()
{
    std::ifstream f("settings.txt");

    for (int i = 0; i < size_; i++) {
      
        for (int j = 0; j < size_; j++)
            f >> RectList[i][j];
    }
    f.close();
}