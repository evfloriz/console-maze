/*

Tutorial found here: https://www.youtube.com/watch?v=xW8skO7MFYw&list=WL&index=5&t=0s

*/

#include <iostream>
#include <Windows.h>
#include <math.h>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 15.0f;
float fPlayerY = 14.0f;
float fPlayerA = -3.14159;

int nMapHeight = 16;
int nMapWidth = 16;

float fFOV = 3.14159 / 4.0;
float fDepth = 16.0f;



int main()
{
    // Create Screen Buffer
    wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    wstring map;
    map += L"################";
    map += L"#........#.....#";
    map += L"#........#.....#";
    map += L"#######..#..#..#";
    map += L"#........#..#..#";
    map += L"#........#..#..#";
    map += L"#..#######..#..#";
    map += L"#...........#..#";
    map += L"#...........#..#";
    map += L"#############..#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..#############";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    
    while(1)
    {
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();
        
        
        
        // controls
        // handle ccw rotation
        if (GetAsyncKeyState((unsigned short)'K') & 0x8000)
            fPlayerA -= (1.2f) * fElapsedTime;
        if (GetAsyncKeyState((unsigned short)'L') & 0x8000)
            fPlayerA += (1.2f) * fElapsedTime;
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            fPlayerX += cosf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
                fPlayerX -= cosf(fPlayerA) * 5.0f * fElapsedTime;

            fPlayerY += sinf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
                fPlayerY -= sinf(fPlayerA) * 5.0f * fElapsedTime;
        }
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            fPlayerX -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
                fPlayerX += cosf(fPlayerA) * 5.0f * fElapsedTime;

            fPlayerY -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
                fPlayerY += sinf(fPlayerA) * 5.0f * fElapsedTime;
        }
        // strafing
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
        {
            fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
                fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;

            fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
                fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
        }
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
        {
            fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
                fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
            
            fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
                fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
        }



        
        for(int x=0; x<nScreenWidth; x++)
        {
            // for each column, calculate projected ray angle into world space
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

            float fDistanceToWall = 0;
            bool bHitWall = false;
            bool bBoundary = false;
            
            // find unit vector for ray
            float fEyeX = cosf(fRayAngle);
            float fEyeY = sinf(fRayAngle);

            while(!bHitWall && fDistanceToWall < fDepth)
            {
                fDistanceToWall += 0.1f;

                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                // test if ray is out of bounds
                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else
                {
                    // test if ray is in a wall
                    if (map[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;

                        vector<pair<float, float>> p; // distance, dot product

                        for (int tx = 0; tx < 2; tx++)
                            for (int ty = 0; ty < 2; ty++)
                            {
                                float vy = (float)nTestY + ty - fPlayerY;
                                float vx = (float)nTestX + tx - fPlayerX;
                                float d = sqrt(vx*vx + vy * vy);
                                float dot = (fEyeX * vx/d) + (fEyeY * vy/d);
                                p.push_back(make_pair(d, dot));
                            }
                        
                        // sort pairs from closest to farthest (lambda function)
                        sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) {return left.first < right.first;});

                        float fBound = 0.01f;
                        if (acos(p.at(0).second) < fBound) bBoundary = true;
                        if (acos(p.at(1).second) < fBound) bBoundary = true;
                        //if (acos(p.at(2).second) < fBound) bBoundary = true;
                    }
                }
            }

            // calculate distance to ceiling and floor
            int nCeiling = float(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;

            short nShade = ' ';

            if (fDistanceToWall <= fDepth / 4.0f)           nShade = 0x2588;            // very close
            else if (fDistanceToWall <= fDepth / 3.0f)      nShade = 0x2593;
            else if (fDistanceToWall <= fDepth / 2.0f)      nShade = 0x2592;
            else if (fDistanceToWall <= fDepth)             nShade = 0x2591;
            else                                            nShade = ' ';               // very far away

            if (bBoundary)          nShade = ' ';

            for (int y=0; y<nScreenHeight; y++)
            {
                if (y < nCeiling)
                    screen[y*nScreenWidth+x] = ' ';
                else if (y > nCeiling && y <= nFloor)
                    screen[y*nScreenWidth+x] = nShade;
                else
                {
                    // shade floor based on distance
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25)       nShade = '#';
                    else if (b < 0.5)   nShade = '=';
                    else if (b < 0.75)  nShade = '-';
                    else if (b < 0.95)  nShade = '.';
                    //else                nShade = ' ';         // why doesnt this work?
                    
                    screen[y*nScreenWidth+x] = nShade;
                    
                }
                    

                
            }
        }
        
        
        // Display Stats
        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f, FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime);

        // Display Map
        for (int nx=0; nx<nMapWidth; nx++)
            for (int ny=0; ny<nMapWidth; ny++)
            {
                screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
            }

        // Display Position
        screen[((int)fPlayerY + 1) * nScreenWidth + (int)fPlayerX] = 'O';

        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0,0}, &dwBytesWritten);
    }


    



    return 0;
}