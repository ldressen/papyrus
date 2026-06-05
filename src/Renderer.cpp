#include <stdexcept>
#include <iostream>

#include "Renderer.h"
#include "Editor.h"
#include "util.h"

Renderer::Renderer(SDL_Window *window)
{
    mRenderer = SDL_CreateRenderer(window, nullptr);

    if (!mRenderer)
    {
        throw std::runtime_error("Failed to create renderer");
    }
    if (!TTF_Init())
    {
        throw std::runtime_error("Failed to initialize SDL_ttf");
    }
    mFont = TTF_OpenFont("assets/JetBrainsMono-Regular.ttf", 20);

    if (!mFont)
    {
        throw std::runtime_error("Failed to load font");
    }
}

Renderer::~Renderer()
{
    TTF_CloseFont(mFont);
    TTF_Quit();

    SDL_DestroyRenderer(mRenderer);
}

void Renderer::clear()
{
    CSF(SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255));
    CSF(SDL_RenderClear(mRenderer));
}

int Renderer::measureTextWidth(const std::string &text)
{
    if (text.empty())
    {
        return 0;
    }
    int w = 0;
    int h = 0;

    CSF(TTF_GetStringSize(mFont, text.c_str(), 0, &w, &h));

    return w;
}

int Renderer::getLineHeight() const
{
    return TTF_GetFontHeight(mFont);
}

std::string Renderer::expandTabs(const std::string &text)
{
    std::string result = "";

    for (char c : text)
    {
        if (c == '\t')
        {
            result += "    ";
        }
        else
        {
            result += c;
        }
    }
    return result;
}

void Renderer::drawText(const std::string &text, int x, int y)
{
    if (text.empty())
        return;

    SDL_Color color = {255, 255, 255, 255};

    SDL_Surface *surface =
        TTF_RenderText_Blended(
            mFont,
            text.c_str(),
            text.size(),
            color);

    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(
            mRenderer,
            surface);

    SDL_FRect dst;
    dst.x = static_cast<float>(x);
    dst.y = static_cast<float>(y);
    dst.w = static_cast<float>(surface->w);
    dst.h = static_cast<float>(surface->h);

    CSF(SDL_RenderTexture(mRenderer, texture, nullptr, &dst));

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

void Renderer::drawRect(int x, int y, int w, int h, SDL_Color color)
{
    SDL_FRect rect;
    rect.x = static_cast<float>(x);
    rect.y = static_cast<float>(y);
    rect.w = static_cast<float>(w);
    rect.h = static_cast<float>(h);

    CSF(SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a));

    CSF(SDL_RenderFillRect(mRenderer, &rect));
}

void Renderer::resetCursorBlink()
{
    mCursorVisible = true;
    mLastBlink = SDL_GetTicks();
}

void Renderer::renderCursor(const Cursor &cursor, const std::string &text)
{
    if (mCursorVisible)
    {
        int x = 20 + measureTextWidth(expandTabs(text.substr(0, cursor.col)));
        int y = 20 + cursor.row * getLineHeight();

        drawRect(x, y, 2, getLineHeight(), SDL_Color{255, 255, 255, 255});
    }
}

void Renderer::renderText(const std::vector<std::string> &text)
{
    for (size_t i = 0; i < text.size(); ++i)
    {
        drawText(expandTabs(text[i]), 20, 20 + getLineHeight() * i);
    }
}

void Renderer::renderSelection(const Editor &editor)
{
    Selection selection = editor.getSelection();

    int br = selection.begin_row;
    int bc = selection.begin_col;
    int er = selection.end_row;
    int ec = selection.end_col;

    int start_row, start_col, end_row, end_col;

    if (br < er) {
        // Normal top-to-bottom selection across lines
        start_row = br; start_col = bc;
        end_row = er;   end_col = ec;
    } 
    else if (br > er) {
        // Reverse bottom-to-top selection across lines
        start_row = er; start_col = ec;
        end_row = br;   end_col = bc;
    } 
    else {
        // Selection is on the exact same line (br == er)
        start_row = end_row = br;
        if (bc <= ec) {
            // Left-to-right on single line
            start_col = bc;
            end_col = ec;
        } else {
            // Right-to-left on single line
            start_col = ec;
            end_col = bc;
        }
    }

    for(int i=start_row; i <= end_row; ++i){
        int beg, end;
        if(i == start_row){
            
            beg = start_col;
            // if only one line selected
            if(start_row == end_row){
                end = end_col;
            }
            else{
                end = editor.getLineString(i).size();
            }
        }
        // in between line -> should be fully selected
        else if(i < end_row){
            beg = 0;
            end = editor.getLineString(i).size();
        }
        else{
            beg = 0;
            end = end_col;
        }
        std::string selectedText = expandTabs(editor.getLineString(i).substr(beg, end-beg));
        int x = 20 + measureTextWidth(expandTabs(editor.getLineString(i).substr(0, beg)));
        int y = 20 + i * getLineHeight();
        int w = measureTextWidth(selectedText);
        int h = getLineHeight();
        std::cout << selectedText << "\n";
        drawRect(x, y, w, h, SDL_Color{46, 47, 48, 255});
    }
}

void Renderer::renderEditor(const Editor &editor)
{
    if (editor.getSelectionActive())
    {
        renderSelection(editor);
    }

    Cursor cursor = editor.getCursor();
    std::string currentLineText = editor.getLineString(cursor.row);
    renderCursor(cursor, currentLineText);
    renderText(editor.getText());
}

void Renderer::updateCursor()
{
    Uint64 now = SDL_GetTicks();
    if (now - mLastBlink > 500)
    {
        mCursorVisible = !mCursorVisible;
        mLastBlink = now;
    }
}

void Renderer::update(Editor &editor)
{
    if (editor.consumeActivity())
    {
        resetCursorBlink();
    }
    updateCursor();
    clear();
    renderEditor(editor);
    present();
}

void Renderer::present()
{
    CSF(SDL_RenderPresent(mRenderer));
}
