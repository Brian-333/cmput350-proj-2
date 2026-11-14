#include "WindowManager.h"

namespace ECSEngine {

/**
 * Create the window with a 1:1 world to window scale.
 * @param width The width of the window in pixels.
 * @param height The height of the window in pixels.
 * @param title The title of the window.
 */
WindowManager::WindowManager(unsigned int width, unsigned int height, const std::string& title)
    : mWindow(std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), title)), mWorldUnitsPerPixel(1.0f) {}

/**
 * Get the window.
 * @return The window.
 */
std::shared_ptr<sf::RenderWindow> WindowManager::GetWindow() const { return mWindow; }

/**
 * Get the bounds of the window.
 * @return A rectangle at (0, 0) with the window's size in pixels.
 */
Rect WindowManager::GetWindowBounds() const {
    return Rect({0,0}, mWindow->getSize().x, mWindow->getSize().y);
}

/**
 * Draw a filled rectangle for debugging.
 * @param r The rectangle in window coordinates.
 * @param red The red channel of the fill color.
 * @param green The green channel of the fill color.
 * @param blue The blue channel of the fill color.
 * @param alpha The alpha channel of the fill color.
 */
void WindowManager::DebugDrawRect(const Rect& r, int red, int green, int blue, int alpha) {
    sf::RectangleShape rect;

    rect.setPosition({r.topLeft.x, r.topLeft.y});
    rect.setSize({r.width, r.height});
    rect.setFillColor(sf::Color(red, green, blue, alpha));

    mWindow->draw(rect);
}

/**
 * Draw the outline of a rectangle for debugging.
 * @param r The rectangle in window coordinates.
 * @param red The red channel of the outline color.
 * @param green The green channel of the outline color.
 * @param blue The blue channel of the outline color.
 * @param alpha The alpha channel of the outline color.
 */
void WindowManager::DebugFrameRect(const Rect& r, int red, int green, int blue, int alpha) {
    sf::RectangleShape rect;

    rect.setPosition({r.topLeft.x, r.topLeft.y});
    rect.setSize({r.width, r.height});
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color(red, green, blue, alpha));
    rect.setOutlineThickness(1);

    mWindow->draw(rect);
}

/**
 * Convert a rectangle from window coordinates to world coordinates.
 * @param rect The rectangle in window coordinates.
 * @return The rectangle in world coordinates.
 */
Rect WindowManager::WindowToWorld(const Rect& rect) const {
    return Rect(
        WindowToWorld(rect.topLeft),
        rect.width * mWorldUnitsPerPixel,
        rect.height * mWorldUnitsPerPixel
    );
}

/**
 * Convert a point from window coordinates to world coordinates.
 * @param pt The point in window coordinates.
 * @return The point in world coordinates.
 */
Point2D WindowManager::WindowToWorld(const Point2D& pt) const {
    return Point2D(
        pt.x * mWorldUnitsPerPixel + mWorldCameraRect.topLeft.x,
        pt.y * mWorldUnitsPerPixel + mWorldCameraRect.topLeft.y
    );
}

/**
 * Convert a rectangle from world coordinates to window coordinates.
 * @param rect The rectangle in world coordinates.
 * @return The rectangle in window coordinates.
 */
Rect WindowManager::WorldToWindow(const Rect& rect) const { 
    return Rect(
        WorldToWindow(rect.topLeft),
        rect.width / mWorldUnitsPerPixel,
        rect.height / mWorldUnitsPerPixel
    );
}

/**
 * Convert a point from world coordinates to window coordinates.
 * @param pt The point in world coordinates.
 * @return The point in window coordinates.
 */
Point2D WindowManager::WorldToWindow(const Point2D& pt) const {
    return Point2D(
        (pt.x - mWorldCameraRect.topLeft.x) / mWorldUnitsPerPixel,
        (pt.y - mWorldCameraRect.topLeft.y) / mWorldUnitsPerPixel
    );
}

/**
 * Move the camera so that a world point appears at a window point.
 * @param worldPt The point in world coordinates.
 * @param screenPt The point in window coordinates.
 */
void WindowManager::SetCamera(const Point2D& worldPt, const Point2D& screenPt) {
    // pWorld - pWindow * mWorldUnitsPerPixel + mWindowSize * 0.5f * mWorldUnitsPerPixel;
    Point2D newTopLeft = worldPt - mWorldUnitsPerPixel * screenPt;
    mWorldCameraRect = Rect(
        newTopLeft,
        mWorldCameraRect.width,
        mWorldCameraRect.height
    );
}

/**
 * Get the center of the camera view.
 * @return The center of the camera view in world coordinates.
 */
Point2D WindowManager::GetWorldCenter() const { 
    return mWorldCameraRect.topLeft + Point2D(mWorldCameraRect.width / 2, mWorldCameraRect.height / 2);
}

/**
 * Get the scale of the camera view.
 * @return The number of world units per window pixel.
 */
float WindowManager::GetWorldScale() const { 
    return mWorldUnitsPerPixel;
}

/**
 * Move the camera view so it is centered on a world point, keeping its size.
 * @param worldCenter The new center in world coordinates.
 */
void WindowManager::SetWorldCenter(const Point2D& worldCenter) {
    mWorldCameraRect = Rect(
        worldCenter - Point2D(mWorldCameraRect.width / 2, mWorldCameraRect.height / 2),
        mWorldCameraRect.width,
        mWorldCameraRect.height
    );
}

/**
 * Set the scale of the camera view, keeping its top-left corner.
 * @param worldUnitsPerPixel The number of world units per window pixel.
 */
void WindowManager::SetWorldScale(float worldUnitsPerPixel) {
    mWorldCameraRect = Rect(
        mWorldCameraRect.topLeft,
        mWorldCameraRect.width / mWorldUnitsPerPixel * worldUnitsPerPixel,
        mWorldCameraRect.height / mWorldUnitsPerPixel * worldUnitsPerPixel
    );
    mWorldUnitsPerPixel = worldUnitsPerPixel;
}

/**
 * Set the area of the world the camera shows. The scale is taken from the rectangle's width.
 * @param worldCameraRect The camera view in world coordinates.
 */
void WindowManager::SetWorldRect(const Rect& worldCameraRect) {
    mWorldCameraRect = worldCameraRect;
    mWorldUnitsPerPixel = (worldCameraRect.width / static_cast<float>(mWindow->getSize().x));
}

/**
 * Convert a length from window pixels to world units.
 * @param x The length in window pixels.
 * @return The length in world units.
 */
float WindowManager::WindowToWorld(float x) const { return x * mWorldUnitsPerPixel; }

/**
 * Convert a length from world units to window pixels.
 * @param x The length in world units.
 * @return The length in window pixels.
 */
float WindowManager::WorldToWindow(float x) const { return x / mWorldUnitsPerPixel; }

}  // namespace ECSEngine
