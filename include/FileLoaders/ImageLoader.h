#pragma once

struct LayerMeta
{
    char* Name;

    // It is possible for the size to be indepent of the image size
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