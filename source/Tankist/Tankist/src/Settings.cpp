/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include "Settings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define FILE_NAME "Settings"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Settings::Settings()
{
    SetDefault();

    File file(gContext, FILE_NAME, FILE_READ);

    if(file.IsOpen())
    {
        Vector<Variant> set;
        ReadFromFile(file, set);
        if(settings.Size() == set.Size())
        {
            for(uint i = 0; i < settings.Size(); i++)
            {
                if(settings[i].GetType() != set[i].GetType())
                {
                    file.Close();
                    return;
                }
            }

            for(uint i = 0; i < settings.Size(); i++)
            {
                settings[i] = set[i];
            }

            firstStart = false;
        }
    }

    file.Close();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
Settings::~Settings()
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Settings::Save()
{
    File file(gContext, FILE_NAME, FILE_WRITE);

    for(uint i = 0; i < settings.Size(); i++)
    {
        String str = String(i) + " ";

        Variant element = settings[i];

        VariantType type = element.GetType();

        str += String((int)type) + " ";

        if(type == VAR_INT)
        {
            str += String(element.GetInt());
        }
        else if(type == VAR_FLOAT)
        {
            str += String(element.GetFloat());
        }
        else if(type == VAR_BOOL)
        {
            str += String(element.GetBool());
        }

        file.WriteLine(str);
    }

    file.Close();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Settings::Set(int, uint var, int value)
{
    settings[var] = Variant(value);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int Settings::Get(int, uint var)
{
    return settings[var].GetInt();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Settings::Set(float, uint var, float value)
{
    settings[var] = Variant(value);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
float Settings::Get(float, uint var)
{
    return settings[var].GetFloat();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Settings::Set(bool, uint var, bool value)
{
    settings[var] = Variant(value);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Settings::Get(bool, uint var)
{
    return settings[var].GetBool();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Settings::ReadFromFile(File &file, Vector<Variant> &set)
{
    set.Clear();

    while(!file.IsEof())
    {
        String str = file.ReadLine();

        Vector<String> parts = str.Split(' ');

        uint num = ToUInt(parts[0]);

        if(num != set.Size())
        {
            return;
        }

        VariantType type = (VariantType)ToInt(parts[1]);

        for(uint i = 0; i < parts[0].Length(); i++)
        {
            str.Erase(0);
        }

        while(str[0] == ' ')
        {
            str.Erase(0);
        }

        for(uint i = 0; i < parts[1].Length(); i++)
        {
            str.Erase(0);
        }

        while(str[0] == ' ')
        {
            str.Erase(0);
        }

        if(type == VAR_INT)
        {
            set.Push(Variant(ToInt(str)));
        }
        else if(type == VAR_FLOAT)
        {
            set.Push(Variant(ToFloat(str)));
        }
        else if(type == VAR_BOOL)
        {
            set.Push(Variant(ToBool(str)));
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Settings::SetDefault()
{
    settings.Clear();

#pragma warning(push)
#pragma warning(disable:4548 4709)

    settings.Push(Variant(1280));   // WINDOW_WIDTH     0
    settings.Push(Variant(1024));   // WINDOW_HEIGHT    1
    settings.Push(Variant(false));  // FULLSCREEN       2
    settings.Push(Variant(50));     // VOLUME_GAME      3
    settings.Push(Variant(50));     // VOLUME_CHAT      4
    settings.Push(Variant(50));     // MIC_LEVEL        5
    settings.Push(Variant(0));      // LANGUAGE         6

#pragma warning(pop)
}
