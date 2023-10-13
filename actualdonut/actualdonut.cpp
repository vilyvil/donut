#include <iostream>
#include <vector>

constexpr auto PI = 3.14159265;

void consoleDonut(int r2, int r1, int jump1, int jump2);
void blogDonut(int r2, int r1, int jump1, int jump2);
void paulDonut(int donut_hole, int r2, int jump1, int jump2);

// blogDonut and consoleDonut math and code heavily inspired by explanations from
// https://www.a1k0n.net/2011/07/20/donut-math.html

int main()
{
    int donut_hole = 1;
    int r2 = 1;

    int jump1 = 50;
    int jump2 = 15;
    
    consoleDonut(donut_hole, r2, jump2, jump1);

    return 0;
}

void consoleDonut(int r2, int r1, int jump1, int jump2)
{
    char chars[40][190] = { ' ' };

    for (int i = 0; i < jump1; i++)
    {
        std::vector<char> row;
        for (int j = 0; j < jump2; j++)
        {
            float theta = i * 2 * PI / jump1;
            float phi = j * 2 * PI / jump2;

            float x = (r2 + r1 * cos(theta)) * cos(phi);
            float y = r1 * sin(theta);
            float z = ((r2 + r1 * cos(theta)) * sin(phi)) * -1;
            float ooz = 1 / z;

            float k1 = 40 * 5 * 3 / (8 * (r1 + r2));

            int xp = (int) (190 / 2 + k1* ooz * x);
            int xy = (int) (40 / 2 - k1 * ooz * y);

            chars[xy][xp] = '#';
        }
    }

    
    for (int i = 0; i < 40; i++)
    {
        for (int j = 0; j < 190; j++)
        {
            std::cout << chars[i][j];
        }

        std::cout << "\n";
    }
}

void blogDonut(int r2, int r1, int jump1, int jump2)
{
    for (int i = 0; i < jump1; i++)
    {
        for (int j = 0; j < jump2; j++)
        {
            float theta = i * 2 * PI / jump1;
            float phi = j * 2 * PI / jump2;

            std::cout << "(" << (r2 + r1 * cos(theta)) * cos(phi) << "," << r1 * sin(theta) << "," << ((r2 + r1 * cos(theta)) * sin(phi)) * -1 << ")\n";
        }
    }
}

void paulDonut(int donut_hole, int r2, int jump1, int jump2)
{
    // First circle (rotation)
    for (int i = 0; i < jump1; i++)
    {
        float x1 = donut_hole * cos(i * 2 * PI / jump1);
        float y1 = donut_hole * sin(i * 2 * PI / jump1);
        for (int j = 0; j < jump2; j++)
        {
            float xyOffsetMg = r2 * cos(j * 2 * PI / jump2);
            float xOffset = xyOffsetMg * cos(i * 2 * PI / jump1);
            float yOffset = xyOffsetMg * sin(i * 2 * PI / jump1);
            float zOffset = r2 * sin(j * 2 * PI / jump2);

            std::cout << "(" << x1 + xOffset << "," << y1 + yOffset << "," << zOffset << ")\n";
        }
    }
}