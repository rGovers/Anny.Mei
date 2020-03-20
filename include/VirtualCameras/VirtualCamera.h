#pragma once

class VirtualCamera
{
private:

protected:

public:
    virtual unsigned int GetFrameSize() const = 0;
    virtual unsigned char* GetVideoBuffer() const = 0;
    
    virtual void PushFrame() const = 0;
};