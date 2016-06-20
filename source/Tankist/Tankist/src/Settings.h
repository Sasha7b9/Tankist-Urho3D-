#pragma once


#define WINDOW_WIDTH    0, 0
#define WINDOW_HEIGHT   0, 1


class Settings
{
public:

    Settings();
    ~Settings();

    int Get(int, uint var);
    float Get(float, uint var);

    void Set(int, uint var, int value);
    void Set(float, uint var, float value);

    void Save();

private:
    void SetDefault();
    void ReadFromFile(File &file, Vector<Variant> &set);

    Vector<Variant> settings;
};
