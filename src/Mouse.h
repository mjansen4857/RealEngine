#pragma once
#include <queue>

class Mouse{
    friend class Window;
public:
    class Event{
    public:
        enum class Type{
            LPress,
            LRelease,
            RPress,
            RRelease,
            WheelUp,
            WheelDown,
            Move,
            Enter,
            Leave,
            Invalid
        };
    private:
        Type type;
        bool leftPressed;
        bool rightPressed;
        int x;
        int y;
    public:
        Event() noexcept : type(Type::Invalid), leftPressed(false), rightPressed(false), x(0), y(0) {

        }

        Event(Type type, const Mouse& parent) noexcept : type(type), leftPressed(parent.leftPressed), rightPressed(parent.rightPressed), x(parent.x), y(parent.y){

        }

        bool isValid() const noexcept {
            return type != Type::Invalid;
        }

        Type getType() const noexcept {
            return type;
        }

        std::pair<int, int> getPos() const noexcept {
            return {x, y};
        }

        int getXPos() const noexcept {
            return x;
        }

        int getYPos() const noexcept {
            return y;
        }

        bool isLeftPressed() const noexcept {
            return leftPressed;
        }

        bool isRightPressed() const noexcept {
            return rightPressed;
        }
    };
public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse& operator = (const Mouse&) = delete;
    std::pair<int, int> getPos() const noexcept;
    int getXPos() const noexcept;
    int getYPos() const noexcept;
    bool isInWindow() const noexcept;
    bool isLeftPressed() const noexcept;
    bool isRightPressed() const noexcept;
    Mouse::Event read() noexcept;
    bool isBufferEmpty() const noexcept {
        return buffer.empty();
    }
    void flush() noexcept;
private:
    void onMouseMove(int newX, int newY) noexcept;
    void onMouseLeave() noexcept;
    void onMouseEnter() noexcept;
    void onLeftPressed(int newX, int newY) noexcept;
    void onLeftReleased(int newX, int newY) noexcept;
    void onRightPressed(int newX, int newY) noexcept;
    void onRightReleased(int newX, int newY) noexcept;
    void onWheelUp(int newX, int newY) noexcept;
    void onWheelDown(int newX, int newY) noexcept;
    void trimBuffer() noexcept;
    void onWheelDelta(int newX, int newY, int delta) noexcept;

    static constexpr unsigned int bufferSize = 16u;
    int x;
    int y;
    bool leftPressed = false;
    bool rightPressed = false;
    bool inWindow = false;
    int wheelDeltaCarry = 0;
    std::queue<Event> buffer;
};