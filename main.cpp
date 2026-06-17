#include <iostream>
#include <stack>
#include <chrono>
#include <fstream>
#include <raylib.h>
#include <cstring>

using u8 = u_int8_t;
using u16 = u_int16_t;

KeyboardKey key_map[16] = {KEY_X, KEY_ONE, KEY_TWO, KEY_THREE, KEY_Q, KEY_W, KEY_E, KEY_A, KEY_S, KEY_D, KEY_Z, KEY_C, KEY_FOUR, KEY_R, KEY_F, KEY_V};

struct sys
{
    u8 mem[4096];

    bool dis[64][32];

    u16 PC = 0x200;

    u16 I = 0;

    std::stack<u16> stk;

    u8 delay_timer = 0;

    u8 sound_timer = 0;

    u8 V[16];

    sys()
    {
        std::memset(V, 0, sizeof(V));
        std::memset(mem, 0, sizeof(mem));
        std::memset(dis, 0, sizeof(dis));

        const u8 fontset[80] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

        for (int i = 0; i < 80; ++i)
        {
            mem[0x50 + i] = fontset[i];
        }
    }
};

int main(void)
{
    sys E;
    std::ifstream file("ibm_logo.ch8", std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        char *buffer = new char[size];

        if (file.read(buffer, size))
        {
            for (int i = 0; i < size; ++i)
            {
                E.mem[0x200 + i] = (u8)buffer[i];
            }
        }

        delete[] buffer;
        file.close();
        std::cout << "ROM loaded successfully!\n";
    }
    else
    {
        std::cout << "Failed to open ROM file.\n";
        return 1;
    }

    int scale = 10;
    InitWindow(64 * scale, 32 * scale, "deez");

    srand(time(NULL));
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        for (int i = 0; i < 12; i++)
        {
            // FETCH
            u8 I1 = E.mem[E.PC];
            u8 I2 = E.mem[E.PC + 1];

            E.PC += 2;

            u16 opcode = (I1 << 8 | I2);

            // DECODE
            u8 A = opcode >> 12;
            u8 X = (opcode & 0x0F00) >> 8;
            u8 Y = (opcode & 0x00F0) >> 4;
            u8 N = (opcode & 0x000F);
            u8 NN = (opcode & 0x00FF);
            u16 NNN = (opcode & 0x0FFF);

            // EXECUTE
            switch (A)
            {
            case 0x0:
                if (NNN == 0xE0)
                {
                    for (int i = 0; i < 64; i++)
                    {
                        for (int j = 0; j < 32; j++)
                        {
                            E.dis[i][j] = false;
                        }
                    }
                }
                else if (NNN == 0xEE)
                {
                    u16 val = E.stk.top();
                    E.stk.pop();
                    E.PC = val;
                }
                break;
            case 0x1:
                E.PC = NNN;
                break;
            case 0x2:
                E.stk.push(E.PC);
                E.PC = NNN;
                break;
            case 0x3:
                if (E.V[X] == NN)
                    E.PC += 2;
                break;
            case 0x4:
                if (E.V[X] != NN)
                    E.PC += 2;
                break;
            case 0x5:
                if (E.V[X] == E.V[Y])
                    E.PC += 2;
                break;
            case 0x6:
                E.V[X] = NN;
                break;
            case 0x7:
                E.V[X] += NN;
                break;
            case 0x8:
                switch (N)
                {
                case 0x0:
                    E.V[X] = E.V[Y];
                    break;
                case 0x1:
                    E.V[X] = E.V[X] | E.V[Y];
                    break;
                case 0x2:
                    E.V[X] = E.V[X] & E.V[Y];
                    break;
                case 0x3:
                    E.V[X] = E.V[X] ^ E.V[Y];
                    break;
                case 0x4:
                {
                    u8 val = E.V[X];
                    E.V[X] = E.V[X] + E.V[Y];
                    if (E.V[X] < val)
                    {
                        E.V[15] = 1;
                    }
                    else
                    {
                        E.V[15] = 0;
                    }
                    break;
                }
                case 0x5:
                {
                    u8 val = E.V[X];
                    E.V[X] = E.V[X] - E.V[Y];
                    if (E.V[X] > val)
                    {
                        E.V[15] = 0;
                    }
                    else
                    {
                        E.V[15] = 1;
                    }
                    break;
                }
                case 0x7:
                {
                    u8 val = E.V[Y];
                    E.V[X] = E.V[Y] - E.V[X];
                    if (E.V[X] > val)
                    {
                        E.V[15] = 0;
                    }
                    else
                    {
                        E.V[15] = 1;
                    }
                    break;
                }
                case 0x6:
                {
                    u8 flag = E.V[X] & 0x1;
                    E.V[X] = E.V[X] >> 1;
                    E.V[15] = flag;
                    break;
                }
                case 0xE:
                {
                    u8 flag = (E.V[X] & 0x80) >> 7;
                    E.V[X] = E.V[X] << 1;
                    E.V[15] = flag;
                    break;
                }
                }
                break;
            case 0x9:
                if (E.V[X] != E.V[Y])
                    E.PC += 2;
                break;
            case 0xA:
                E.I = NNN;
                break;
            case 0xC:
                E.V[X] = (rand() % 256) & NN;
                break;
            case 0xB:
            {
                E.PC = NNN + E.V[0];
                break;
            }
            case 0xE:
                if (NN == 0x9E)
                {
                    if (E.V[X] <= 15 && IsKeyDown(key_map[E.V[X]]))
                    {
                        E.PC += 2;
                    }
                }
                else
                {
                    if (E.V[X] <= 15 && IsKeyUp(key_map[E.V[X]]))
                    {
                        E.PC += 2;
                    }
                }
                break;
            case 0xD:
            {
                u8 x_coord = E.V[X] & 63;
                u8 y_coord = E.V[Y] & 31;
                E.V[15] = 0;

                for (u8 i = 0; i < N; i++)
                {
                    if (y_coord + i >= 32)
                        break;

                    u16 address = E.I + i;

                    for (int j = 0; j < 8; j++)
                    {
                        if (x_coord + j >= 64)
                            break;

                        bool val1 = E.dis[x_coord + j][y_coord + i];
                        bool val3 = (E.mem[address] & (0x80 >> j)) != 0;

                        if (val1 && val3)
                        {
                            E.dis[x_coord + j][y_coord + i] = false;
                            E.V[15] = 1;
                        }
                        else if (!val1 && val3)
                        {
                            E.dis[x_coord + j][y_coord + i] = true;
                        }
                    }
                }
                break;
            }
            case 0xF:
            {
                switch (NN)
                {
                case 0x29:
                {
                    u8 character = E.V[X] & 0x0F;
                    E.I = 0x50 + 5 * character;
                    break;
                }
                case 0x07:
                {
                    E.V[X] = E.delay_timer;
                    break;
                }
                case 0x15:
                {
                    E.delay_timer = E.V[X];
                    break;
                }
                case 0x18:
                {
                    E.sound_timer = E.V[X];
                    break;
                }
                case 0x1E:
                {
                    E.I += E.V[X];
                    break;
                }
                case 0x0A:
                {
                    int key = GetKeyPressed();
                    if (key == 0)
                    {
                        E.PC -= 2;
                    }
                    else
                    {
                        /* Standard mapping:
                           1 2 3 4 -> 1 2 3 C
                           Q W E R -> 4 5 6 D
                           A S D F -> 7 8 9 E
                           Z X C V -> A 0 B F
                        */

                        auto result = std::find(std::begin(key_map), std::end(key_map), key);

                        if (result == std::end(key_map))
                        {
                            E.PC -= 2;
                        }
                        else
                        {
                            auto index = std::distance(std::begin(key_map), result);
                            E.V[X] = index;
                        }
                    }
                    break;
                }
                case 0x33:
                {
                    u8 d3 = E.V[X] % 10;
                    u8 d2 = (E.V[X] / 10) % 10;
                    u8 d1 = (E.V[X] / 100) % 10;
                    E.mem[E.I] = d1;
                    E.mem[E.I + 1] = d2;
                    E.mem[E.I + 2] = d3;
                    break;
                }
                case 0x55:
                {
                    for (u8 i = 0; i < X + 1; i++)
                    {
                        u16 index = E.I + i;
                        E.mem[index] = E.V[i];
                    }
                    break;
                }
                case 0x65:
                {
                    for (u8 i = 0; i < X + 1; i++)
                    {
                        u16 index = E.I + i;
                        E.V[i] = E.mem[index];
                    }
                    break;
                }
                }
                break;
            }
            }
        }

        if (E.delay_timer > 0)
            E.delay_timer--;
        if (E.sound_timer > 0)
            E.sound_timer--;

        BeginDrawing();
        ClearBackground(BLACK);

        for (int x = 0; x < 64; x++)
        {
            for (int y = 0; y < 32; y++)
            {
                if (E.dis[x][y] == true)
                {
                    DrawRectangle(x * scale, y * scale, scale, scale, WHITE);
                }
            }
        }
        EndDrawing();
    }
}