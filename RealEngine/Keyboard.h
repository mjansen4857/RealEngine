#pragma once

#include <queue>
#include <bitset>

class Keyboard {
    friend class Window;
public:
    class Event {
    public:
        enum class Type {
            Press,
            Release,
            Invalid
        };
    private:
        Type type;
        unsigned char code;
    public:
        Event() : type(Type::Invalid), code(0u) {

        }

        Event(Type type, unsigned char code) noexcept : type(type), code(code) {

        }

        bool isPress() const noexcept {
            return type == Type::Press;
        }

        bool isRelease() const noexcept {
            return type == Type::Release;
        }

        bool isValid() const noexcept {
            return type != Type::Invalid;
        }

        unsigned char getCode() const noexcept {
            return code;
        }
    };

public:
    Keyboard() = default;
    Keyboard(const Keyboard&) = delete;
    Keyboard& operator = (const Keyboard&) = delete;

    bool isKeyPressed(unsigned char keycode) const noexcept;
    Event readKey() noexcept;
    bool isKeyBufferEmpty() const noexcept;
    void flushKeyBuffer() noexcept;
    char readChar() noexcept;
    bool isCharBufferEmpty() const noexcept;
    void flushCharBuffer() noexcept;
    void flush() noexcept;
    void setAutorepeat(bool autoRepeat) noexcept;
    bool isAutorepeatEnabled() const noexcept;

private:
    void onKeyPressed(unsigned char keycode) noexcept;
    void onKeyReleased(unsigned char keycode) noexcept;
    void onChar(char character) noexcept;
    void clearState() noexcept;
    template<typename T>
    static void trimBuffer(std::queue<T>& buffer) noexcept;

    static constexpr unsigned int nKeys = 256u;
    static constexpr unsigned int bufferSize = 16u;
    bool autoRepeatEnabled = false;
    std::bitset<nKeys> keyStates;
    std::queue<Event> keyBuffer;
    std::queue<char> charBuffer;
};