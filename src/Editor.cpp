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
    if (mText.size() < mCursor.row + 1)
        mText.push_back("");
    mText[mCursor.row].insert(mCursor.col, text);
    mCursor.col++;
}

void Editor::handleBackSpace()
{
    bool move = mCursor.col == 0;
    if (!mText[mCursor.row].empty() && mCursor.col > 0)
    {
        mText[mCursor.row].erase(mCursor.col - 1, 1);
        mCursor.col--;
    }
    if (move && mCursor.row > 0)
    {
        mCursor.row--;
        mCursor.col = mText[mCursor.row].size();
    }
}

void Editor::handleReturn()
{
    mCursor.row++;
    mCursor.col = 0;
    if (mText.size() < mCursor.row + 1)
    {
        mText.push_back("");
    }
}

void Editor::handleLeft()
{
    if (mCursor.col > 0)
    {
        mCursor.col--;
    }
    else if (mCursor.col == 0 && mCursor.row > 0)
    {
        mCursor.row--;
        mCursor.col = mText[mCursor.row].size();
    }
}

void Editor::handleRight()
{
    if (mCursor.col < mText[mCursor.row].size())
    {
        mCursor.col++;
    }
    else if (mCursor.col == mText[mCursor.row].size() && mCursor.row < mText.size()-1)
    {
        mCursor.row++;
        mCursor.col = 0;
    }
}

Cursor Editor::getCursor() const
{
    return mCursor;
}

const std::string& Editor::getLineString(int i) const
{
    return mText[i];
}

const std::vector<std::string>& Editor::getText() const
{
    return mText;
}
