#include <fstream>
#include <iostream>

#include "Editor.h"

Editor::Editor()
{
    mCursor = Cursor{0, 0};
}

Editor::~Editor()
{
}

void Editor::handleTextInput(const std::string &text)
{
    mBuffer.insert(mCursor.row, mCursor.col, text);
    mCursor.col += text.size();
}

void Editor::handleBackSpace()
{
    if (mCursor.col > 0)
    {
        mBuffer.erase(mCursor.row, mCursor.col - 1);
        mCursor.col--;
    }
    else if (mCursor.col == 0 && mCursor.row > 0)
    {
        mCursor.row--;
        mCursor.col = mBuffer.getLineSize(mCursor.row);
        mBuffer.mergeWithNext(mCursor.row);
    }
    markActivity();
}

void Editor::handleReturn()
{
    mBuffer.splitLine(mCursor.row, mCursor.col);
    mCursor.col = 0;
    mCursor.row++;
    markActivity();
}

void Editor::handleLeft(SDL_Keymod mod)
{
    if (mod & SDL_KMOD_SHIFT)
    {
        if (mSelection.end_col > 0)
        {
            mSelection.end_col--;
        }
        else if (mSelection.end_col == 0 && mSelection.end_row > 0)
        {
            mSelection.end_row--;
            mSelection.end_col = mBuffer.getLineSize(mSelection.end_row);
        }
    }

    if (mCursor.col > 0)
    {
        mCursor.col--;
    }
    else if (mCursor.col == 0 && mCursor.row > 0)
    {
        mCursor.row--;
        mCursor.col = mBuffer.getLineSize(mCursor.row);
    }
    markActivity();
}

void Editor::handleRight(SDL_Keymod mod)
{
    if (mod & SDL_KMOD_SHIFT)
    {
        std::cout << "Selection Right arrow \n";
        if (mSelection.end_col < mBuffer.getLineSize(mCursor.row))
        {
            mSelection.end_col++;
        }
        else if (mSelection.end_col == mBuffer.getLineSize(mCursor.row) && mSelection.end_row < mBuffer.getLineCount() - 1)
        {
            mSelection.end_row++;
            mSelection.end_col = 0;
        }
    }

    if (mCursor.col < mBuffer.getLineSize(mCursor.row))
    {
        mCursor.col++;
    }
    else if (mCursor.col == mBuffer.getLineSize(mCursor.row) && mCursor.row < mBuffer.getLineCount() - 1)
    {
        mCursor.row++;
        mCursor.col = 0;
    }
    markActivity();
}

void Editor::handleUp()
{
    if (mCursor.row > 0)
    {
        mCursor.row--;
        bool isUpperShorter = mBuffer.getLineSize(mCursor.row) < mBuffer.getLineSize(mCursor.row + 1);
        bool colGreaterThanLineAbove = mCursor.col >= mBuffer.getLineSize(mCursor.row);
        if (isUpperShorter && colGreaterThanLineAbove)
        {
            mCursor.col = mBuffer.getLineSize(mCursor.row);
        }
    }
    markActivity();
}

void Editor::handleDown()
{
    if (mCursor.row < mBuffer.getLineCount() - 1)
    {
        mCursor.row++;
        bool isUpperLonger = mBuffer.getLineSize(mCursor.row - 1) > mBuffer.getLineSize(mCursor.row);
        bool colGreaterThanLineBelow = mCursor.col >= mBuffer.getLineSize(mCursor.row);
        if (isUpperLonger && colGreaterThanLineBelow)
        {
            mCursor.col = mBuffer.getLineSize(mCursor.row);
        }
    }
    markActivity();
}

void Editor::handleTab()
{
    handleTextInput("\t");
    markActivity();
}

void Editor::handleShift(Uint32 type)
{
    if (type == SDL_EVENT_KEY_DOWN)
    {
        mSelection.begin_row = mCursor.row;
        mSelection.begin_col = mCursor.col;
        mSelection.end_row = mCursor.row;
        mSelection.end_col = mCursor.col;
        setSelectionActive(true);

        std::cout << "Shift pressed! (Row,Col): " << mSelection.begin_row << "," << mSelection.begin_col << "\n";
    }
    else if (type == SDL_EVENT_KEY_UP)
    {
        mSelection.end_row = mCursor.row;
        mSelection.end_col = mCursor.col;
        setSelectionActive(false);
        std::cout << "Shift released! (Row,Col): " << mSelection.end_row << "," << mSelection.end_col << "\n";
    }
}

void Editor::loadFile(const std::filesystem::path &path)
{
    std::ifstream file{path};

    if (!file.is_open())
    {
        std::cerr << "ERROR: Could not open file " << path << "\n";
        return;
    }
    std::vector<std::string> lines;
    std::string currentLine;
    while (std::getline(file, currentLine))
    {
        lines.push_back(currentLine);
    }

    mCurrentFilePath = path;
    mBuffer.setLines(std::move(lines));
    mCursor.row = mBuffer.getLineCount() - 1;
    mCursor.col = mBuffer.getLineSize(mCursor.row);
    std::cout << path << " was loaded! \n";
}

void Editor::saveFileAs(const std::filesystem::path &path)
{
    std::ofstream file{path};

    if (!file.is_open())
    {
        std::cerr << "ERROR: Could not open file " << path << "\n";
        return;
    }

    std::vector<std::string> lines = mBuffer.getText();

    for (size_t i = 0; i < lines.size(); ++i)
    {
        file << lines[i];

        if (i < lines.size() - 1)
        {
            file << "\n";
        }
    }
    std::cout << path << " was saved! \n";
}

void Editor::saveFile()
{
    saveFileAs(mCurrentFilePath);
}

void Editor::markActivity()
{
    mActivity = true;
}

bool Editor::consumeActivity()
{
    if (!mActivity)
    {
        return false;
    }

    mActivity = false;
    return true;
}

const Selection &Editor::getSelection() const
{
    return mSelection;
}

void Editor::setSelectionActive(bool b)
{
    mSelectionActive = b;
}

bool Editor::getSelectionActive() const
{
    return mSelectionActive;
}

Cursor Editor::getCursor() const
{
    return mCursor;
}

const std::string &Editor::getLineString(int i) const
{
    return mBuffer.getLine(i);
}

const std::vector<std::string> &Editor::getText() const
{
    return mBuffer.getText();
}
