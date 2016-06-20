#include "stdafx.h"


#include "Settings.h"


#define FILE_NAME "Settings"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Settings::Settings()
{
    SetDefault();

    File file(gContext, FILE_NAME, Urho3D::FILE_READ);

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
    File file(gContext, FILE_NAME, Urho3D::FILE_WRITE);

    for(uint i = 0; i < settings.Size(); i++)
    {
        String str = String(i) + " ";

        Variant element = settings[i];

        VariantType type = element.GetType();

        str += String((int)type) + " ";

        if(type == Urho3D::VAR_INT)
        {
            str += String(element.GetInt());
        }
        else if(type == Urho3D::VAR_FLOAT)
        {
            str += String(element.GetFloat());
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
void Settings::SetDefault()
{
    settings.Clear();

#pragma warning(push)
#pragma warning(disable:4548 4709)

    settings.Push(Variant(640));   // WINDOW_WIDTH     0
    settings.Push(Variant(480));    // WINDOW_HEIGHT    1

#pragma warning(pop)
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Settings::ReadFromFile(File &file, Vector<Variant> &set)
{
    set.Clear();

    while(!file.IsEof())
    {
        String str = file.ReadLine();

        Vector<String> parts = str.Split(' ');

        uint num = Urho3D::ToUInt(parts[0]);

        if(num != set.Size())
        {
            return;
        }

        VariantType type = (VariantType)Urho3D::ToInt(parts[1]);

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

        if(type == Urho3D::VAR_INT)
        {
            set.Push(Variant(Urho3D::ToInt(str)));
        }
        else if(type == Urho3D::VAR_FLOAT)
        {
            set.Push(Variant(Urho3D::ToFloat(str)));
        }
    }
}
