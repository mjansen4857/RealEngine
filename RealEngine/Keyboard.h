#pragma once

#include <queue>
#include <bitset>
#include <optional>

class Keyboard {
    friend class Window;
public:
    class Event {
    public:
        enum class Type {
            Press,
            Release
        };
    private:
        Type type;
        unsigned char code;
    public:
        Event(Type type, unsigned char code) noexcept : type(type), code(code) {

        }

        bool isPress() const noexcept {
            return type == Type::Press;
        }

        bool isRelease() const noexcept {
            return type == Type::Release;
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
    std::optional<Event> readKey() noexcept;
    bool isKeyBufferEmpty() const noexcept;
    void flushKeyBuffer() noexcept;
    std::optional<char> readChar() noexcept;
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