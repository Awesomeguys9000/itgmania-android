/**
 * ITGMania Android Entry Point
 * 
 * This is the minimal entry point for the Android build.
 * It initializes SDL2 and delegates to the main ITGMania code.
 * 
 * This file will be expanded as more of the engine is ported.
 */

#include <SDL.h>
#include <GLES2/gl2.h>
#include <android/log.h>

#define LOG_TAG "ITGMania"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// SDL requires this signature for the main function
extern "C" int SDL_main(int argc, char* argv[])
{
    LOGI("ITGMania Android starting...");
    
    // Initialize SDL with video and audio subsystems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        LOGE("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    
    LOGI("SDL initialized successfully");
    
    // Set OpenGL ES 2.0 attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    // Create window (fullscreen on Android)
    SDL_Window* window = SDL_CreateWindow(
        "ITGMania",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        0, 0,  // Size will be determined by fullscreen
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN
    );
    
    if (!window) {
        LOGE("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    LOGI("Window created successfully");
    
    // Create OpenGL ES context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        LOGE("SDL_GL_CreateContext failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    LOGI("OpenGL ES context created");
    LOGI("GL Vendor: %s", glGetString(GL_VENDOR));
    LOGI("GL Renderer: %s", glGetString(GL_RENDERER));
    LOGI("GL Version: %s", glGetString(GL_VERSION));
    
    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    
    // Get window size
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    LOGI("Window size: %dx%d", width, height);
    
    // Main loop - placeholder for now
    // TODO: Initialize ITGMania engine and run game loop
    bool running = true;
    SDL_Event event;
    
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                    
                case SDL_KEYDOWN:
                    LOGI("Key pressed: %d", event.key.keysym.scancode);
                    if (event.key.keysym.scancode == SDL_SCANCODE_AC_BACK) {
                        running = false;
                    }
                    break;
                    
                case SDL_CONTROLLERBUTTONDOWN:
                    LOGI("Controller button pressed: %d", event.cbutton.button);
                    break;
                    
                case SDL_CONTROLLERDEVICEADDED:
                    LOGI("Controller connected: %d", event.cdevice.which);
                    SDL_GameControllerOpen(event.cdevice.which);
                    break;
                    
                case SDL_CONTROLLERDEVICEREMOVED:
                    LOGI("Controller disconnected: %d", event.cdevice.which);
                    break;
            }
        }
        
        // Clear screen with a test color
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // TODO: Render ITGMania frame here
        
        // Swap buffers
        SDL_GL_SwapWindow(window);
    }
    
    LOGI("ITGMania shutting down...");
    
    // Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
