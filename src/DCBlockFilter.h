#pragma once

class DCBlockFilter
{
public:
    DCBlockFilter() = default;

    void reset()
    {
        zx = 0.0f;
        zy = 0.0f;
    }

    void setCutoff(float sampleRate, float frequency)
    {
        r = 1.0f - 6.2831853071795864f * frequency / sampleRate;
    }

    float processSample(float x)
    {
        const float y = x - zx + r * zy;
        zx = x;
        zy = y;
        return y;
    }

private:
    float r = 0.995f;
    float zx = 0.0f;
    float zy = 0.0f;
};
