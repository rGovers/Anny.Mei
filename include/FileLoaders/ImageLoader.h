#pragma once

struct LayerMeta
{
    char* Name;

    int ImageWidth;
    int ImageHeight;

    int xOffset;
    int yOffset;
    
    int Width;
    int Height;
};

struct Layer
{
    LayerMeta MetaData;

    char* Data;
};

class ImageLoader
{
private:

protected:

public:
    virtual int GetLayerCount() const = 0;
    virtual LayerMeta ToLayerMeta(int a_index) const = 0;
    virtual Layer* ToLayer(int a_index) const = 0;
};