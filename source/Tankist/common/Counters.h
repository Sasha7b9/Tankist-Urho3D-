#pragma once


class Counters
{
public:
    void AddClientIn(int numBytes);
    void AddClientOut(int numBytes);
    void AddServerIn(int numbytes);
    void AddServerOut(int numBytes);

    void Update();

    int GetClientIn();
    int GetClientOut();
    int GetServerIn();
    int GetServerOut();

private:
    int bytesClientIn = 0;
    int bytesClientOut = 0;
    int bytesServerIn = 0;
    int bytesServerOut = 0;

    int speedClientIn = 0;
    int speedClientOut = 0;
    int speedServerIn = 0;
    int speedServerOut = 0;
};