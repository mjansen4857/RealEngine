#include "Mouse.h"
#include "RealWin.h"

std::pair<int, int> Mouse::getPos() const noexcept {
    return {x, y};
}

int Mouse::getXPos() const noexcept {
    return x;
}

int Mouse::getYPos() const noexcept {
    return y;
}

bool Mouse::isInWindow() const noexcept {
    return inWindow;
}

bool Mouse::isLeftPressed() const noexcept {
    return leftPressed;
}

bool Mouse::isRightPressed() const noexcept {
    return rightPressed;
}

Mouse::Event Mouse::read() noexcept {
    if(!isBufferEmpty()){
        Mouse::Event e = buffer.front();
        buffer.pop();
        return e;
    }
    return Mouse::Event();
}

void Mouse::flush() noexcept {
    buffer = std::queue<Event>();
}

void Mouse::onMouseMove(int newX, int newY) noexcept {
    x = newX;
    y = newY;

    buffer.push(Event(Event::Type::Move, *this));
    trimBuffer();
}

void Mouse::onMouseLeave() noexcept {
    inWindow = false;
    buffer.push(Event(Event::Type::Leave, *this));
    trimBuffer();
}

void Mouse::onMouseEnter() noexcept {
    inWindow = true;
    buffer.push(Event(Event::Type::Enter, *this));
    trimBuffer();
}

void Mouse::onLeftPressed(int newX, int newY) noexcept {
    x = newX;
    y = newY;
    leftPressed = true;

    buffer.push(Event(Event::Type::LPress, *this));
    trimBuffer();
}

void Mouse::onLeftReleased(int newX, int newY) noexcept {
    x = newX;
    y = newY;
    leftPressed = false;

    buffer.push(Event(Event::Type::LRelease, *this));
    trimBuffer();
}

void Mouse::onRightPressed(int newX, int newY) noexcept {
    x = newX;
    y = newY;
    rightPressed = true;

    buffer.push(Event(Event::Type::RPress, *this));
    trimBuffer();
}

void Mouse::onRightReleased(int newX, int newY) noexcept {
    x = newX;
    y = newY;
    rightPressed = false;

    buffer.push(Event(Event::Type::RRelease, *this));
    trimBuffer();
}

void Mouse::onWheelUp(int newX, int newY) noexcept {
    x = newX;
    y = newY;

    buffer.push(Event(Event::Type::WheelUp, *this));
    trimBuffer();
}

void Mouse::onWheelDown(int newX, int newY) noexcept {
    x = newX;
    y = newY;

    buffer.push(Event(Event::Type::WheelDown, *this));
    trimBuffer();
}

void Mouse::onWheelDelta(int newX, int newY, int delta) noexcept {
    wheelDeltaCarry += delta;

    while(wheelDeltaCarry >= WHEEL_DELTA){
        wheelDeltaCarry -= WHEEL_DELTA;
        onWheelUp(newX, newY);
    }

    while(wheelDeltaCarry <= -WHEEL_DELTA){
        wheelDeltaCarry += WHEEL_DELTA;
        onWheelDown(newX, newY);
    }
}

void Mouse::trimBuffer() noexcept {
    while(buffer.size() > bufferSize){
        buffer.pop();
    }
}