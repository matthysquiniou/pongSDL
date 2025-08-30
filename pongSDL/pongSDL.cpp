#include "pong.hpp"

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO) || !TTF_Init()) {
        std::cerr << "Erreur SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Pong SDL",
        800, 600, 0);

    if (!window) {
        std::cerr << "Erreur fenetre: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Erreur renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cerr << "Erreur chargement font " << std::endl;
    }

    bool running = true;
    SDL_Event event;

    Paddle player1 = {{50, 250, 20, 100}, 300.0f};
    Paddle player2 = {{730, 250, 20, 100}, 300.0f};

    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<int> distrib(0, 1);
    int random = distrib(gen);
    float ballBegin = random ? 1.0f : -1.0f;

    Ball ball = {{390,290,20,20},200.0f,10.0f,1.0f*ballBegin,0.0f};

    Uint64 lastTime = SDL_GetTicks();

    while (running) {

        Uint64 currentTime = SDL_GetTicks();
        float delta = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        SDL_PumpEvents();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        int numkeys = 0;
        const bool* state = SDL_GetKeyboardState(&numkeys);

        updatePlayer1(player1, delta, state);

        updatePlayer2(player2, delta, state);

        updateBall(ball,player1,player2,delta);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        displayScore(player1, player2, font, renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &player1.rect);
        SDL_RenderFillRect(renderer, &player2.rect);
        SDL_RenderFillRect(renderer, &ball.rect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    TTF_CloseFont(font);
    TTF_Quit();

    return 0;
}

void updatePlayer1(Paddle &p1, float delta, const bool* state) {
    if (state[SDL_SCANCODE_UP]) {
        p1.rect.y -= p1.speed * delta;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        p1.rect.y += p1.speed * delta;
    }

    float p1yMin = p1.rect.y;
    float p1yMax = p1.rect.y + p1.rect.h;

    float top = 0.0f;

    if (p1yMin < top)
    {
        p1.rect.y = top;
    }

    float bottom = 600.0f;

    if (p1yMax > bottom)
    {
        p1.rect.y = bottom - p1.rect.h;
    }

}

void updatePlayer2(Paddle &p2, float delta, const bool* state) {
    if (state[SDL_SCANCODE_W]) {
        p2.rect.y -= p2.speed * delta;
    }
    if (state[SDL_SCANCODE_S]) {
        p2.rect.y += p2.speed * delta;
    }

    float p2yMin = p2.rect.y;
    float p2yMax = p2.rect.y + p2.rect.h;

    float top = 0.0f;

    if (p2yMin < top)
    {
        p2.rect.y = top;
    }

    float bottom = 600.0f;

    if (p2yMax > bottom)
    {
        p2.rect.y = bottom - p2.rect.h;
    }
}

void updateBall(Ball& ball, Paddle& p1, Paddle& p2, float delta) {
    ball.rect.x += ball.speed * delta * ball.dx;
    ball.rect.y += ball.speed * delta * ball.dy;
    ball.speed += ball.accel * delta;

    float ballxMin = ball.rect.x;
    float ballxMax = ball.rect.x + ball.rect.w;
    float ballyMin = ball.rect.y;
    float ballyMax = ball.rect.y + ball.rect.h;

    float p1x = p1.rect.x + p1.rect.w;
    float p1xPast = p1.rect.x;
    float p1yMin = p1.rect.y;
    float p1yMax = p1.rect.y + p1.rect.h;

    if (ballxMin < p1x && ballxMin > p1xPast && ((ballyMin<p1yMax && ballyMin>p1yMin)||(ballyMax>p1yMin && ballyMax<p1yMax)))
    {
        ball.rect.x = p1x;
        ball.dy = ((ball.rect.y + (ball.rect.h/2)) - (p1.rect.y + (p1.rect.h / 2))) / p1.rect.h;
        ball.dx = sqrt(1.0f - ball.dy * ball.dy);
    }

    float p2x = p2.rect.x;
    float p2xPast = p2.rect.x + p2.rect.w;
    float p2yMin = p2.rect.y;
    float p2yMax = p2.rect.y + p2.rect.h;

    if (ballxMax > p2x && ballxMax < p2xPast && ((ballyMin<p2yMax && ballyMin>p2yMin) || (ballyMax>p2yMin && ballyMax<p2yMax)))
    {
        ball.rect.x = p2x - ball.rect.w;
        ball.dy = ((ball.rect.y + (ball.rect.h/2)) - (p2.rect.y + (p2.rect.h/2))) / p2.rect.h;
        ball.dx = -sqrt(1.0f - ball.dy * ball.dy);
    }

    float top = 0.0f;

    if (ballyMin < top)
    {
        ball.rect.y = top;
        ball.dy *= -1.0f;
    }

    float bottom = 600.0f;

    if (ballyMax > bottom)
    {
        ball.rect.y = bottom - ball.rect.h;
        ball.dy *= -1.0f;
    }

    float left = 0.0f;

    if (ballxMin < left)
    {
        ball.rect.x = 390.0f;
        ball.rect.y = 290.0f;
        ball.speed = 200.0f;
        p2.score++;
    }

    float right = 800.0f;

    if (ballxMax > right)
    {
        ball.rect.x = 390.0f;
        ball.rect.y = 290.0f;
        ball.speed = 200.0f;
        p1.score++;
    }
}

void displayScore(Paddle p1, Paddle p2, TTF_Font* font, SDL_Renderer* renderer) {
    SDL_Color white = { 255, 255, 255, 255 };

    std::string scoreStrP1 = std::to_string(p1.score);
    std::string scoreStrP2 = std::to_string(p2.score);

    SDL_Surface* surfaceP1 = TTF_RenderText_Blended(font, scoreStrP1.c_str(), scoreStrP1.length(), white);
    SDL_Texture* textureP1 = SDL_CreateTextureFromSurface(renderer, surfaceP1);
    SDL_FRect dstP1 = { 50, 50, (float)surfaceP1->w, (float)surfaceP1->h };
    SDL_DestroySurface(surfaceP1);

    SDL_Surface* surfaceP2 = TTF_RenderText_Blended(font, scoreStrP2.c_str(), scoreStrP2.length(), white);
    SDL_Texture* textureP2 = SDL_CreateTextureFromSurface(renderer, surfaceP2);
    SDL_FRect dstP2 = { 800 - 50 - (float)surfaceP2->w, 50, (float)surfaceP2->w, (float)surfaceP2->h };
    SDL_DestroySurface(surfaceP2);

    SDL_RenderTexture(renderer, textureP1, nullptr, &dstP1);
    SDL_RenderTexture(renderer, textureP2, nullptr, &dstP2);

    SDL_DestroyTexture(textureP1);
    SDL_DestroyTexture(textureP2);
}
