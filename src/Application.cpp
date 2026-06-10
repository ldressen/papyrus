#include "Application.h"
#include "Argparser.h"
#include "logger.h"
#include "util.h"

Application::Application(int argc, char *argv[])
{
    ArgParser parser;

    parser.parse(argc, argv);
    std::string filename = "";
    Argument filenameArg = Argument(ArgumentType::POSITIONAL, "positional");
    if (parser.hasArgument(filenameArg))
    {
        filename = parser.getArgumentValue(filenameArg);
    }
    Argument helpArg = Argument(ArgumentType::BOOL_FLAG, "h");
    if (parser.hasArgument(helpArg))
    {
        std::cout << "usage: papyrus [filename] [option] \n"
                  << "-h: displays this help message \n";
    }

    SDL_Init(SDL_INIT_VIDEO);

    mWindow = SDL_CreateWindow("papyrus", 800, 600, SDL_WINDOW_RESIZABLE);
    CSF(SDL_StartTextInput(mWindow));

    mRenderer = std::make_unique<Renderer>(mWindow);

    if (!filename.empty())
    {
        mEditor.loadFile(filename);
        SDL_SetWindowTitle(mWindow, std::format("papyrus [{}]", filename).c_str());
    }
        
}

Application::~Application()
{
    CSF(SDL_StopTextInput(mWindow));
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Application::run()
{
    bool running = true;

    Uint64 lastTime = SDL_GetTicksNS();
    float fps = 0.0f;

    while (running)
    {
        Uint64 currentTime = SDL_GetTicksNS();
        Uint64 deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        if (deltaTime > 0)
        {
            fps = 1000000000.0f / static_cast<float>(deltaTime);
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_TEXT_INPUT)
            {
                mEditor.handleTextInput(event.text.text);
            }
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                SDL_Keycode key = event.key.key;
                SDL_Keymod mod = event.key.mod;
                switch (key)
                {
                case SDLK_BACKSPACE:
                    mEditor.handleBackSpace();
                    break;
                case SDLK_RETURN:
                    mEditor.handleReturn();
                    break;
                case SDLK_LEFT:
                    mEditor.handleLeft(mod);
                    break;
                case SDLK_RIGHT:
                    mEditor.handleRight(mod);
                    break;
                case SDLK_UP:
                    mEditor.handleUp(mod);
                    break;
                case SDLK_DOWN:
                    mEditor.handleDown(mod);
                    break;
                case SDLK_TAB:
                    mEditor.handleTab();
                    break;
                case SDLK_HOME:
                    mEditor.handleHome(mod);
                    break;
                case SDLK_END:
                    mEditor.handleEnd(mod);
                    break;
                case SDLK_DELETE:
                    mEditor.handleDelete(mod);
                    break;
                case SDLK_A:
                    mEditor.handleA(mod);
                    break;
                case SDLK_C:
                    mEditor.handleC(mod);
                    break;
                case SDLK_V:
                    mEditor.handleV(mod);
                    break;

                // IO
                case SDLK_F1:
                    mEditor.loadFile("test.txt");
                    break;
                case SDLK_F2:
                    mEditor.saveFileAs("./test2.txt");
                    break;
                case SDLK_F3:
                    mCurrentScreen = Screen::FileBrowser;
                    break;
                case SDLK_F4:
                    mCurrentScreen = Screen::Editor;
                    break;
                default:
                    break;
                }
            }
            if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                uint32_t w, h;
                SDL_GetWindowSize(mWindow, (int *)&w, (int *)&h);
                mRenderer->onResize(w, h);
            }
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
        update();
    }
}

void Application::update()
{
    switch (mCurrentScreen)
    {
    case Screen::Editor:
        mRenderer->updateEditor(mEditor);
        break;
    case Screen::FileBrowser:
        mRenderer->updateFileBrowser(mFileBrowser);
        break;
    default:
        LOG_ERROR() << "Unknown Screen!";
        break;
    }
}
