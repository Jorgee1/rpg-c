#include "sdl.h"

int check_collition(SDL_Rect A, SDL_Rect B)
{
    /// PARA A
    int A_IZQ = A.x;
    int A_DER = A.x + A.w;
    int A_ARR = A.y;
    int A_ABJ = A.y + A.h;
    /// PARA B
    int B_IZQ = B.x;
    int B_DER = B.x + B.w;
    int B_ARR = B.y;
    int B_ABJ = B.y + B.h;
    /// Restriccones
    if(
        (A_ABJ >= B_ARR) &&
        (A_ARR <= B_ABJ) &&
        (A_DER >= B_IZQ) &&
        (A_IZQ <= B_DER) 
    ){
        return 1;
    }
    return 0;
}

void set_render_draw_color(SDL_Renderer* renderer, SDL_Color* color)
{
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
}