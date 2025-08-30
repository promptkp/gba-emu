#include <SDL3/SDL_render.h>
#include <iostream>
#include <SDL3/SDL.h>

int main() {
  std::cout << "Hello, World!" << std::endl;
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Init failed: %s", SDL_GetError());
    return -1;
  }
  SDL_Window *window;
  SDL_Renderer *renderer;
  if (!SDL_CreateWindowAndRenderer("GBA Emu", 480, 320, 0, &window, &renderer)) {
    SDL_Log("Window and renderer init failed: %s", SDL_GetError());
    return -1;
  }

  const double now = 0;
  const float red = (float) (0.5 + 0.5 * SDL_sin(now));
  const float green = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
  const float blue = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
  SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */

  /* clear the window to the draw color. */
  SDL_RenderClear(renderer);

  /* put the newly-cleared rendering on the screen. */
  SDL_RenderPresent(renderer);

  SDL_Event event;
  while (true) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) return 0;
    }
    SDL_Delay(16);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  return 0;
}
