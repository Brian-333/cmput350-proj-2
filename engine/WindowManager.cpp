#include "WindowManager.h"

namespace ECSEngine {

WindowManager::WindowManager(unsigned int width, unsigned int height, const std::string& title)
    : mWindow(std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), title)),
      mWorldUnitsPerPixel(1.0f) {}

std::shared_ptr<sf::RenderWindow> WindowManager::GetWindow() const { return mWindow; }

Rect WindowManager::GetWindowBounds() const {
    return Rect({0, 0}, mWindow->getSize().x, mWindow->getSize().y);
}

void WindowManager::DebugDrawRect(const Rect& r, int red, int green, int blue, int alpha) {
    sf::RectangleShape rect;

    rect.setPosition({r.topLeft.x, r.topLeft.y});
    rect.setSize({r.width, r.height});
    rect.setFillColor(sf::Color(red, green, blue, alpha));

    mWindow->draw(rect);
}

void WindowManager::DebugFrameRect(const Rect& r, int red, int green, int blue, int alpha) {
    sf::RectangleShape rect;

    rect.setPosition({r.topLeft.x, r.topLeft.y});
    rect.setSize({r.width, r.height});
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color(red, green, blue, alpha));
    rect.setOutlineThickness(1);

    mWindow->draw(rect);
}

Rect WindowManager::WindowToWorld(const Rect& rect) const {
    return Rect(WindowToWorld(rect.topLeft), rect.width * mWorldUnitsPerPixel,
                rect.height * mWorldUnitsPerPixel);
}

Point2D WindowManager::WindowToWorld(const Point2D& pt) const {
    return Point2D(pt.x * mWorldUnitsPerPixel + mWorldCameraRect.topLeft.x,
                   pt.y * mWorldUnitsPerPixel + mWorldCameraRect.topLeft.y);
}

Rect WindowManager::WorldToWindow(const Rect& rect) const {
    return Rect(WorldToWindow(rect.topLeft), rect.width / mWorldUnitsPerPixel,
                rect.height / mWorldUnitsPerPixel);
}

Point2D WindowManager::WorldToWindow(const Point2D& pt) const {
    return Point2D((pt.x - mWorldCameraRect.topLeft.x) / mWorldUnitsPerPixel,
                   (pt.y - mWorldCameraRect.topLeft.y) / mWorldUnitsPerPixel);
}

void WindowManager::SetCamera(const Point2D& worldPt, const Point2D& screenPt) {
    // pWorld - pWindow * mWorldUnitsPerPixel + mWindowSize * 0.5f * mWorldUnitsPerPixel;
    Point2D newTopLeft = worldPt - mWorldUnitsPerPixel * screenPt;
    mWorldCameraRect = Rect(newTopLeft, mWorldCameraRect.width, mWorldCameraRect.height);
}

Point2D WindowManager::GetWorldCenter() const {
    return mWorldCameraRect.topLeft +
           Point2D(mWorldCameraRect.width / 2, mWorldCameraRect.height / 2);
}

float WindowManager::GetWorldScale() const { return mWorldUnitsPerPixel; }

void WindowManager::SetWorldCenter(const Point2D& worldCenter) {
    mWorldCameraRect =
        Rect(worldCenter - Point2D(mWorldCameraRect.width / 2, mWorldCameraRect.height / 2),
             mWorldCameraRect.width, mWorldCameraRect.height);
}

void WindowManager::SetWorldScale(float worldUnitsPerPixel) {
    mWorldCameraRect = Rect(mWorldCameraRect.topLeft,
                            mWorldCameraRect.width / mWorldUnitsPerPixel * worldUnitsPerPixel,
                            mWorldCameraRect.height / mWorldUnitsPerPixel * worldUnitsPerPixel);
    mWorldUnitsPerPixel = worldUnitsPerPixel;
}

void WindowManager::SetWorldRect(const Rect& worldCameraRect) {
    mWorldCameraRect = worldCameraRect;
    mWorldUnitsPerPixel = (worldCameraRect.width / static_cast<float>(mWindow->getSize().x));
}

float WindowManager::WindowToWorld(float x) const { return x * mWorldUnitsPerPixel; }

float WindowManager::WorldToWindow(float x) const { return x / mWorldUnitsPerPixel; }

}  // namespace ECSEngine
