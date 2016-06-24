#pragma once


#define WINDOW_WIDTH    0,      0
#define WINDOW_HEIGHT   0,      1
#define FULLSCREEN      true,   2
#define VOLUME_GAME     0,      3
#define VOLUME_CHAT     0,      4
#define MIC_LEVEL       0,      5
#define LANGUAGE        0,      6  /* 0 - en, 1 - ru */


class Settings
{
public:

    Settings();
    ~Settings();

    int Get(int, uint var);
    float Get(float, uint var);
    bool Get(bool, uint val);

    void Set(int, uint var, int value);
    void Set(float, uint var, float value);
    void Set(bool, uint var, bool value);

    void Save();

    bool FirstStart()
    {
        return firstStart;
    }

private:
    void SetDefault();
    void ReadFromFile(File &file, Vector<Variant> &set);

    Vector<Variant> settings;
    bool firstStart = true;
};
