#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 875
#define BALL_SPEED 8
#define FPS 30
#define MAX_FRUITS 1
#define FRUIT_SIZE 10

SDL_Color fontColor = {205, 102, 77};
SDL_Event event;
SDL_Renderer *renderer;
SDL_Rect ball, fruit, field;
SDL_Surface *message;
SDL_Texture *messageTexture;
SDL_Window *window;
TTF_Font *Font;

int is_running, fruits_on, afterReset, nightMode, ball_size = 20, firstLaunch = 1;

float fruitX, fruitY;

void afterResetAwait(void)
{
    TTF_SetFontSize(Font, 30);
    message = TTF_RenderText_Solid(Font, "[Press any key to start]", fontColor);
    if (message == NULL)
    {
        fprintf(stderr, "Failed at creating message.\n");
        is_running = 0;
        return;
    }
    messageTexture = SDL_CreateTextureFromSurface(renderer, message);
    if (messageTexture == NULL)
    {
        fprintf(stderr, "Failed at creating texture.\n");
        is_running = 0;
        return;
    }
    int texW = 0;
    int texH = 0;
    if (SDL_QueryTexture(messageTexture, NULL, NULL, &texW, &texH) < 0)
        fprintf(stderr, "Failed at setting attributes to texture.\n");
    SDL_Rect dstrect = {SCREEN_WIDTH / 2 - message->w / 2, ball.y - 100, texW, texH};
    if (SDL_RenderCopy(renderer, messageTexture, NULL, &dstrect) < 0)
        fprintf(stderr, "Failed at setting texture to current rendering.\n");
    SDL_RenderPresent(renderer);
    while (1)
    {
        if (SDL_WaitEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                is_running = 0;
                return;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                afterReset = 0;
                break;
            }
        }
    }
    SDL_Delay(200);
    SDL_DestroyTexture(messageTexture);
    SDL_FreeSurface(message);
    return;
}

void resetGame(void)
{
    ball_size = 20;
    ball.w = ball.h = ball_size;
    ball.x = SCREEN_WIDTH / 2 - ball_size / 2;
    ball.y = SCREEN_HEIGHT / 2 - ball_size / 2;
    afterReset = 1;
    return;
}

void endScreen(void)
{
    SDL_SetRenderDrawColor(renderer, 15, 13, 12, 255);
    SDL_RenderClear(renderer);
    TTF_SetFontSize(Font, 100);
    message = TTF_RenderText_Solid(Font, "[!YOU WIN!]", fontColor);
    if (message == NULL)
    {
        fprintf(stderr, "Failed at creating message.\n");
        is_running = 0;
        return;
    }
    messageTexture = SDL_CreateTextureFromSurface(renderer, message);
    if (messageTexture == NULL)
    {
        fprintf(stderr, "Failed at creating texture.\n");
        is_running = 0;
        return;
    }
    int texW2 = 0;
    int texH2 = 0;
    if (SDL_QueryTexture(messageTexture, NULL, NULL, &texW2, &texH2) < 0)
        fprintf(stderr, "Failed at setting attributes to texture.\n");
    SDL_Rect dstrect2 = {SCREEN_WIDTH / 2 - message->w / 2, SCREEN_HEIGHT / 2 - message->h / 2, texW2, texH2};
    if (SDL_RenderCopy(renderer, messageTexture, NULL, &dstrect2) < 0)
        fprintf(stderr, "Failed at setting texture to current rendering.\n");
    SDL_RenderPresent(renderer);
    SDL_Delay(2000);
    SDL_FreeSurface(message);
    SDL_DestroyTexture(messageTexture);
    resetGame();
    return;
}

void ballResize(void)
{
    ball_size += fruit.w;
    fruits_on--;
    return;
}

void prepare(void)
{
    if (SDL_HasIntersection(&ball, &fruit))
        ballResize();
    if (ball.x <= 0)
        ball.x = 0;
    if (ball.x + ball.w >= SCREEN_WIDTH)
        ball.x = SCREEN_WIDTH - ball.w;
    if (ball.y <= 0)
        ball.y = 0;
    if (ball.y + ball.h >= SCREEN_HEIGHT)
        ball.y = SCREEN_HEIGHT - ball.h;
    ball.h = ball.w = ball_size;
    return;
}

void input(void)
{
    const unsigned char *input = SDL_GetKeyboardState(NULL);
    if (input[SDL_SCANCODE_RIGHT] || input[SDL_SCANCODE_D])
        ball.x += BALL_SPEED;
    if (input[SDL_SCANCODE_LEFT] || input[SDL_SCANCODE_A])
        ball.x -= BALL_SPEED;
    if (input[SDL_SCANCODE_UP] || input[SDL_SCANCODE_W])
        ball.y -= BALL_SPEED;
    if (input[SDL_SCANCODE_DOWN] || input[SDL_SCANCODE_S])
        ball.y += BALL_SPEED;
    if (input[SDL_SCANCODE_R])
        resetGame();
    if (input[SDL_SCANCODE_E])
    {
        nightMode = -nightMode;
        SDL_Delay(200);
    }
    if (input[SDL_SCANCODE_Q])
        is_running = 0;
    if (ball_size == fmax(SCREEN_HEIGHT, SCREEN_WIDTH))
        endScreen();
    return;
}

void draw(void)
{
    if (nightMode < 1)
    {
        SDL_SetRenderDrawColor(renderer, 205, 200, 176, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 205, 102, 77, 255);
        SDL_RenderFillRect(renderer, &ball);
        SDL_SetRenderDrawColor(renderer, 87, 84, 74, 255);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 15, 13, 12, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 205, 102, 77, 255);
        SDL_RenderFillRect(renderer, &ball);
        SDL_SetRenderDrawColor(renderer, 61, 48, 42, 255);
    }
    if (fruits_on < MAX_FRUITS)
    {
        fruitX = rand() % (SCREEN_WIDTH - FRUIT_SIZE);
        fruitY = rand() % (SCREEN_HEIGHT - FRUIT_SIZE);
        fruits_on++;
    }
    fruit.x = fruitX;
    fruit.y = fruitY;
    fruit.w = fruit.h = FRUIT_SIZE;
    SDL_RenderFillRect(renderer, &fruit);
    SDL_RenderPresent(renderer);
    return;
}

int main(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr, "Failed Initialization\n");
        return 0;
    }
    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC, &window, &renderer) < 0)
    {
        fprintf(stderr, "Failed at Creating Window\n");
        return 0;
    }
    if (TTF_Init() < 0)
    {
        fprintf(stderr, "Failed at initializing font.\n");
        return 0;
    }
    Font = TTF_OpenFont("./include/Retro_Gaming.ttf", 30);
    if (Font == NULL)
    {
        fprintf(stderr, "Failed at opening font.\n");
        return 0;
    }
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"))
        fprintf(stderr, "SetHint did not work properly.\n");
    srand(time(NULL));
    resetGame();
    is_running = 1;
    nightMode = -1;
    unsigned int ticks;
    while (is_running != 0)
    {
        ticks = SDL_GetTicks();
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                is_running = 0;
        prepare();
        input();
        draw();
        if (afterReset)
            afterResetAwait();
        if (1000 / FPS > (SDL_GetTicks() - ticks))
            SDL_Delay(1000 / FPS - (SDL_GetTicks() - ticks));
    }
    TTF_CloseFont(Font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}