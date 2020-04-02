#include "Keyboard.h"

bool Keyboard::isKeyPressed(unsigned char keycode) const noexcept {
    return keyStates[keycode];
}

Keyboard::Event Keyboard::readKey() noexcept {
    if(!isKeyBufferEmpty()){
        Keyboard::Event e = keyBuffer.front();
        keyBuffer.pop();
        return e;
    }
    return Keyboard::Event();
}

bool Keyboard::isKeyBufferEmpty() const noexcept {
    return keyBuffer.empty();
}

char Keyboard::readChar() noexcept {
    if(!isCharBufferEmpty()){
        unsigned char charcode = charBuffer.front();
        charBuffer.pop();
        return charcode;
    }
    return 0;
}

bool Keyboard::isCharBufferEmpty() const noexcept {
    return charBuffer.empty();
}

void Keyboard::flushKeyBuffer() noexcept {
    keyBuffer = std::queue<Event>();
}

void Keyboard::flushCharBuffer() noexcept {
    charBuffer = std::queue<char>();
}

void Keyboard::flush() noexcept {
    flushKeyBuffer();
    flushCharBuffer();
}

void Keyboard::setAutorepeat(bool autoRepeat) noexcept {
    autoRepeatEnabled = autoRepeat;
}

bool Keyboard::isAutorepeatEnabled() const noexcept {
    return autoRepeatEnabled;
}

void Keyboard::onKeyPressed(unsigned char keycode) noexcept {
    keyStates[keycode] = true;
    keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
    trimBuffer(keyBuffer);
}

void Keyboard::onKeyReleased(unsigned char keycode) noexcept {
    keyStates[keycode] = false;
    keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
    trimBuffer(keyBuffer);
}

void Keyboard::onChar(char character) noexcept {
    charBuffer.push(character);
    trimBuffer(charBuffer);
}

void Keyboard::clearState() noexcept {
    keyStates.reset();
}

template<typename T>
void Keyboard::trimBuffer(std::queue<T> &buffer) noexcept {
    while(buffer.size() > bufferSize){
        buffer.pop();
    }
}