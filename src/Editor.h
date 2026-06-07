#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include <SDL3/SDL_stdinc.h>

#include "Renderer.h"
#include "TextBuffer.h"

struct Cursor
{
    size_t row, col;
};

struct Position
{
    size_t row, col;

    bool operator==(const Position &other) const
    {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Position &other) const
    {
        return !(*this == other);
    }
    bool operator<(const Position &other) const
    {
        if (row != other.row)
        {
            return row < other.row;
        }

        return col < other.col;
    }
    bool operator>(const Position &other) const
    {
        return other < *this;
    }
};

struct Selection
{
    Position begin;
    Position end;

    bool empty() const
    {
        return begin == end;
    }

    Selection normalized() const
    {
        if (begin < end)
        {
            return *this;
        }
        return Selection{
            .begin = end,
            .end = begin};
    }
};

class Editor
{

public:
    Editor();
    ~Editor();

    void handleTextInput(const std::string &text);
    void handleBackSpace();
    void handleReturn();
    void handleLeft(SDL_Keymod mod);
    void handleRight(SDL_Keymod mod);
    void handleUp();
    void handleDown();
    void handleTab();
    void handleShift(Uint32 type);

    void loadFile(const std::filesystem::path &path);
    void saveFileAs(const std::filesystem::path &path);
    void saveFile();

    void markActivity();
    bool consumeActivity();
    void markSelectionVisible();
    bool consumeSelectionVisible();
    bool getSelectionVisible() const;

    const Selection &getSelection() const;
    void setSelectionActive(bool b);
    bool getSelectionActive() const;

    Cursor getCursor() const;
    const std::string &getLineString(int i) const;
    const std::vector<std::string> &getText() const;

private:
    Selection mSelection{0};
    Cursor mCursor;
    TextBuffer mBuffer;
    std::filesystem::path mCurrentFilePath;
    bool mActivity;
    bool mSelectionActive = false;
    bool mSelectionVisible = false;
};