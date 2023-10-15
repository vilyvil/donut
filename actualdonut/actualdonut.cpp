#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

constexpr auto PI = 3.14159265;

void blogDonut(int dfd, int d_hole, float t_space, float p_space, float A, float B);
void renderDonut(int donut_hole, int r2, float t_inc, float p_inc, float A, float B);

// Heavily inspired by:
// https://www.a1k0n.net/2011/07/20/donut-math.html

float max_lumi = -100;
float min_lumi = 100;

int main()
{
    //
    int donut_hole = 6;
    int dist_from_donut = 4;

    //constants for donut rendering
    //const float t_inc = 0.02, p_inc = 0.08;
    const float t_inc = 0.2, p_inc = 0.2;
    //angle of spin around x axis
    float A = 0;
    //angle of spin around y axis
    float B = 0.5;

    renderDonut(donut_hole, dist_from_donut, t_inc, p_inc, A, B);

    while(true)
    {
        //renderDonut(donut_hole, dist_from_donut, t_inc, p_inc, A, B);

        A += 0.07;
        B += 0.02;

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(16ms);
    }

    return 0;
}

void blogDonut(int dfd, int d_hole, float t_space, float p_space, float A, float B)
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

// my own implementation.
void renderDonut(int donut_hole, int r2, float t_inc, float p_inc, float A, float B)
{
    char chars[40][189] = {};
    memset(&chars[0], ' ', 7560);
    float zbuffer[40][189] = { {0} };

    // Precomputed values
    float sinA = sin(A), sinB = sin(B);
    float cosA = cos(A), cosB = cos(B);
    // Distance between "screen" and viewer. Scales 2d-projected coordinates.
    const float k1 = 189 * 25 * 3 / (8 * (r2 + donut_hole));
    // Adds distance between "screen" and donut
    const float k2 = 50;

    // Outlines large circle shape
    for (float theta = 0; theta < 2 * PI; theta += t_inc)
    {
        // Precomputed values
        float sintheta = sin(theta), costheta = cos(theta);

        // Calculates current point on large circle
        float large_x = donut_hole * costheta;
        float large_y = donut_hole * sintheta;
        
        // Draws smaller circle around current point on larger circle
        for (float phi = 0; phi < 2 * PI; phi += p_inc)
        {
            // Precomputed values
            float sinphi = sin(phi), cosphi = cos(phi);

            /*float offsetMg = r2 * cosphi;
            float xOffset = offsetMg * costheta;
            float yOffset = offsetMg * sintheta;*/

            // Gets current point on donut by adding radius (at an angle) to current coordinate
            // (r2 * cosphi) represents the magnitude of displacement from the current point on the large donut circle
            // multiplying 
            float x = large_x + r2 * cosphi * costheta;
            float y = large_y + r2 * cosphi * sintheta;
            float z = r2 * sinphi;

            // x, y, z after being plugged into rotation matrices for rotation around x and y axes
            float rotatedX = x * cosB + sinB * (sinA * y + cosA * z);
            float rotatedY = cosA * y - sinA * z;
            float rotatedZ = (sinA * y + cosA * z) * cosB - x * sinB + k2;
            float ooz = 1 / rotatedZ;

            std::cout << "(" << rotatedX << ", " << rotatedY << ", " << rotatedZ << ")\n";

            // Projects donut onto 2D "screen" k2 units away from viewer
            int xp = (int)(rotatedX * k1 * ooz + 190 / 2);
            int yp = (int)((rotatedY * k1 * ooz + 84 / 2) / 2.1);

            if (0 < xp && 189 > xp && 0 < yp && 40 > yp && zbuffer[yp][xp] < ooz)
            {
                // Luminance scales off point's distance from screen
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