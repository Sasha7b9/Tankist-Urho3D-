#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ImageT : public Image
{
    URHO3D_OBJECT(ImageT, Image);

public:
    ImageT(Context *context);
    ~ImageT() = default;

    static void RegisterObject(Context *context);

    void SetSize(int width, int height);

    void SetPoint(int x, int y, const Color& color);

    void DrawLine(int x0, int y0, int x1, int y1, const Color &color);

    void DrawRectangle(int x, int y, int width, int height, const Color &color);

    void FillRectangle(int x, int y, int width, int height, const Color &color);

    void FillRegion(int x, int y, const Color &color);

    void DrawCircle(float x, float y, float radius, const Color &color, float step = 1.0f);

    IntVector2 GetHotSpot() const;

    void SetHotSpot(int x, int y);

private:
    ImageT& operator=(const ImageT&) {};

    void Replace4Points(int x, int y, const Color &color);

    void Replace4PointsBound(int x, int y, const Color &color);
    
    Color boundingColor;

    IntVector2 hotSpot;

    ImageT(ImageT const&) : Image(nullptr) {};
};
