#include <iostream>
#include <thread>
#include <chrono>

constexpr auto PI = 3.14159265;

void blogDonut(int d_hole, int dfd, float t_space, float p_space, float A, float B);
void renderDonut(int donut_hole, int r2, float t_inc, float p_inc, float A, float B);

// This program is based off of the explanation given here.
// https://www.a1k0n.net/2011/07/20/donut-math.html
// blogDonut is my attempt at replicating the donut using the math provided
// renderDonut is my attempt at replicating the donut without using the math provided

int main()
{
    // donut params
    int donut_hole = 6;
    int dist_from_donut = 4;

    // constants for donut rendering
    const float t_inc = 0.02, p_inc = 0.08;
    // angle of spin around x axis
    float A = 0;
    // angle of spin around y axis
    float B = 0.5;

    while(true)
    {
        renderDonut(donut_hole, dist_from_donut, t_inc, p_inc, A, B);

        A += 0.07;
        B += 0.02;

        // waits for specified timeframe
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5ms);
    }

    return 0;
}

// My attempt at replicating the blog's implementation
void blogDonut(int d_hole, int dfd, float t_space, float p_space, float A, float B)
{
    char chars[40][189] = {};
    memset(&chars[0], ' ', 7560);
    float zbuffer[40][189] = { {0} };

    for (float theta = 0.0; theta < 2 * PI; theta += t_space)
    {
        for (float phi = 0.0; phi < 2 * PI; phi += p_space) {
            float sintheta = sin(theta), costheta = cos(theta);
            float sinphi = sin(phi), cosphi = cos(phi);
            float sinA = sin(A), sinB = sin(B);
            float cosA = cos(A), cosB = cos(B);

            const float k2 = 45;//35;
            const float k1 = 189 * 20 * 3 / (8 * (dfd + d_hole));

            float x = (d_hole + dfd * costheta) * (cosB * cosphi + sinA * sinB * sinphi) - dfd * cosA * sinB * sintheta;
            float y = (d_hole + dfd * costheta) * (cosphi * sinB - cosB * sinA * sinphi) + dfd * cosA * cosB * sintheta;
            float z = cosA * (d_hole + dfd * costheta) * sinphi + dfd * sinA * sintheta + k2;
            float ooz = 1 / z;

            int xp = (int) (189 / 2 + k1 * ooz * x);
            int yp = (int) ((40 / 2 - k1 * ooz * y) / 2.1) + 10;

            if (yp < 40 && yp > 0 && xp > 0 && xp < 189 && zbuffer[yp][xp] < ooz)
            {
                float luminance = cosphi * costheta * sinB - cosA * costheta * sinphi
                    - sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);
                if (luminance > 0)
                {
                    int lumi_i = luminance * 8;
                    zbuffer[yp][xp] = ooz;
                    chars[yp][xp] = ".,-~:;=!*#$@"[lumi_i];
                }
            }
        }
    }

    void* pt = &chars[0];
    printf("\x1b[H");
    fwrite(pt, 1, 7560, stderr);
}

// my own implementation of the donut
void renderDonut(int donut_hole, int r2, float t_inc, float p_inc, float A, float B)
{
    char chars[40][189] = {};
    memset(&chars[0], ' ', 7560);
    float zbuffer[40][189] = { {0} };

    // precomputed values
    float sinA = sin(A), sinB = sin(B);
    float cosA = cos(A), cosB = cos(B);
    // Distance between "screen" and viewer. Scales 2d-projected coordinates.
    const float k1 = 189 * 25 * 3 / (8 * (r2 + donut_hole));
    // adds distance between "screen" and donut
    const float k2 = 50;

    // outlines large circle shape
    for (float theta = 0; theta < 2 * PI; theta += t_inc)
    {
        // precomputed values
        float sintheta = sin(theta), costheta = cos(theta);

        // calculates current point on large circle
        float large_x = donut_hole * costheta;
        float large_y = donut_hole * sintheta;
        
        // Draws smaller circle around current point on larger circle
        for (float phi = 0; phi < 2 * PI; phi += p_inc)
        {
            // Precomputed values
            float sinphi = sin(phi), cosphi = cos(phi);

            // gets x & y points of donut by adding smaller circle's radius (at an angle) to current coordinate
            // (r2 * cosphi) represents total horizontal displacement (x and y are both horizontal for this circle)
            // multiplying (r2 * cosphi) by sintheta or costheta gives x or y displacement respectively
            float x = large_x + r2 * cosphi * costheta;
            float y = large_y + r2 * cosphi * sintheta;
            // z is vertical displacement from current coordinate
            float z = r2 * sinphi;

            // x, y, z after being plugged into rotation matrices for rotation around x and y axes
            float rotatedX = x * cosB + sinB * (sinA * y + cosA * z);
            float rotatedY = cosA * y - sinA * z;
            float rotatedZ = (sinA * y + cosA * z) * cosB - x * sinB + k2;
            float ooz = 1 / rotatedZ;

            // projects donut onto 2D "screen" k2 units away from viewer
            int xp = (int)(rotatedX * k1 * ooz + 190 / 2);
            int yp = (int)((rotatedY * k1 * ooz + 84 / 2) / 2.1);

            if (0 < xp && 189 > xp && 0 < yp && 40 > yp && zbuffer[yp][xp] < ooz)
            {
                // luminance scales off point's distance from screen
                float dist_from_screen = sqrt(pow(rotatedX, 2) + pow(rotatedY, 2) + pow(rotatedZ, 2));
                float luminance = 3.16 - (dist_from_screen / 17);

                if (luminance > 0) 
                {
                    int lumi_i = luminance * 12;
                    zbuffer[yp][xp] = ooz;
                    chars[yp][xp] = ".,-~:;=!*#$@"[lumi_i];
                }
            }
        }
    }

    //places cursor at home console position
    printf("\x1b[H");
    //puts chars into console
    fwrite(&chars[0], 1, 7560, stderr);
}