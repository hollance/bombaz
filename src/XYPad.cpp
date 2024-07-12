#include <JuceHeader.h>
#include "XYPad.h"

XYPad::XYPad() : point(0.5f, 0.5f)
{
    setOpaque(false);
}

void XYPad::paint(juce::Graphics& g)
{
    int x = int(getWidth() * point.x + 0.5f);
    int y = int(getHeight() * point.y + 0.5f);
    g.drawImage(lineHorzImage, 0, y - 1, 250, 2, 0, 0, 500, 4);
    g.drawImage(lineVertImage, x - 1, 0, 2, 250, 0, 0, 4, 500);
    g.drawImage(pointerImage, x - 15, y - 11, 29, 29, 0, 0, 58, 58);
}

void XYPad::mouseDown(const juce::MouseEvent&)
{
    if (xParameter != nullptr) {
        xParameter->beginChangeGesture();
    }
    if (yParameter != nullptr) {
        yParameter->beginChangeGesture();
    }
}

void XYPad::mouseDrag(const juce::MouseEvent& event)
{
    point.x = juce::jlimit(0.0f, 1.0f, event.x / float(getWidth()));
    point.y = juce::jlimit(0.0f, 1.0f, event.y / float(getHeight()));

    if (xParameter != nullptr) {
        xParameter->setValueNotifyingHost(point.x);
    }
    if (yParameter != nullptr) {
        yParameter->setValueNotifyingHost(1.0f - point.y);
    }
    repaint();
}

void XYPad::mouseUp(const juce::MouseEvent&)
{
    if (xParameter != nullptr) {
        xParameter->endChangeGesture();
    }
    if (yParameter != nullptr) {
        yParameter->endChangeGesture();
    }
}

void XYPad::updatePoint()
{
    if (xParameter != nullptr) {
        point.x = xParameter->convertTo0to1(xParameter->get());
    }
    if (yParameter != nullptr) {
        point.y = 1.0f - yParameter->convertTo0to1(yParameter->get());
    }
}
