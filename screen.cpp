#include "screen.hpp"
#include "base_text_buffer.hpp"
#include "painter.hpp"
#include "resize_event.hpp"
#include "SDL2/SDL_ttf.h"
#include <iostream>

Screen::Char::Char(wchar_t ach, Color afg, Color abg):
    ch(ach),
    fg(afg),
    bg(abg)
{}

Screen::Screen(Widget *parent):
    Widget(parent),
    cursor_{0, 0},
    textBuffer_{nullptr}
{
    Painter p(this);
    glyphWidth_ = p.glyphWidth();
    glyphHeight_ = p.glyphHeight();
    ResizeEvent e{width(), height()};
    resizeEvent(e);
}

void Screen::paintEvent(PaintEvent &)
{
    Painter p(this);
    for (size_t y = 0; y < ch_.size(); ++y)
        for (size_t x = 0; x < ch_[y].size(); ++x)
        {
            const auto &c = ch_[y][x];
            p.renderGlyph(c.ch, x * glyphWidth_, y * glyphHeight_, c.fg, c.bg);
        }
    p.setColor(Gray);
    p.drawLine(cursor_.x * glyphWidth_, cursor_.y * glyphHeight_,
               cursor_.x * glyphWidth_, (cursor_.y + 1) * glyphHeight_);
}

bool Screen::keyPressEvent(KeyEvent &)
{
    setVScroll(vScroll() + 1);
    return true;
}

void Screen::resizeEvent(ResizeEvent &e)
{
    ch_.resize(heightCh());
    for (auto &r: ch_)
        r.resize(widthCh());
}

int Screen::widthCh() const
{
    return width() / glyphWidth_;
}

int Screen::heightCh() const
{
    return height() / glyphHeight_;
}

Screen::Char &Screen::ch(int x, int y)
{
    return ch_[y][x];
}

const Screen::Char &Screen::ch(int x, int y) const
{
    return ch_[y][x];
}

Coord Screen::cursor() const
{
    return cursor_;
}

void Screen::setCursor(Coord value)
{
    cursor_ = value;
}

void Screen::setCursor(int x, int y)
{
    cursor_ = { x, y };
}

BaseTextBuffer *Screen::textBuffer() const
{
    return textBuffer_;
}

void Screen::setTextBuffer(BaseTextBuffer *value)
{
    if (value != textBuffer_)
    {
        textBuffer_ = value;
        if (textBuffer_)
            textBuffer_->render(this);
        else
        {
            for (size_t y = 0; y < ch_.size(); ++y)
                for (size_t x = 0; x < ch_[y].size(); ++x)
                    ch_[y][x] = '\0';
            update();
        }
    }
}

int Screen::hScroll() const
{
    return hScroll_;
}

void Screen::setHScroll(int value)
{
    hScroll_ = value;
    if (textBuffer_)
        textBuffer_->render(this);
}

int Screen::vScroll() const
{
    return vScroll_;
}

void Screen::setVScroll(int value)
{
    vScroll_ = value;
    if (textBuffer_)
        textBuffer_->render(this);
}
