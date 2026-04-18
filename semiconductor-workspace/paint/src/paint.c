#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct
{
    int x, y, width, height;
    Uint8 r, g, b, a;
} colours;

typedef struct
{
    SDL_Rect background;
    colours colours[8];
    SDL_Rect save_btn;
} palette;

typedef struct
{
    // Define the right side of the screen
    palette p;
    Uint8 selected_color;
    SDL_Rect grid_rect;
    Uint8 grid[24][24];

} paint_state;

void init_palette(paint_state *paint_state)
{
    // Initialize palette background
    paint_state->p.background = (SDL_Rect){480, 0, 160, 480};

    // Initialize colours
    paint_state->p.colours[0] = (colours){490, 90, 60, 60, 255, 0, 0, 255};
    paint_state->p.colours[1] = (colours){570, 90, 60, 60, 0, 255, 0, 255};

    paint_state->p.colours[2] = (colours){490, 170, 60, 60, 0, 0, 255, 255};
    paint_state->p.colours[3] = (colours){570, 170, 60, 60, 255, 255, 0, 255};

    paint_state->p.colours[4] = (colours){490, 250, 60, 60, 255, 0, 255, 255};
    paint_state->p.colours[5] = (colours){570, 250, 60, 60, 0, 255, 255, 255};

    paint_state->p.colours[6] = (colours){490, 330, 60, 60, 0, 0, 0, 255};
    paint_state->p.colours[7] = (colours){570, 330, 60, 60, 255, 255, 255, 255};

    // Initialize save button
    paint_state->p.save_btn = (SDL_Rect){490, 10, 140, 60};
}



void draw_palette(SDL_Renderer *renderer, palette *palette)
{
    // Render the palette rectangle
    SDL_SetRenderDrawColor(renderer, 228, 228, 228, 255);
    SDL_RenderFillRect(renderer, &palette->background);

    // Render save button. Will use pink for now as a replacement for text
    SDL_SetRenderDrawColor(renderer, 255, 50, 255, 255);
    SDL_RenderFillRect(renderer, &palette->save_btn);

    // Render colour boxes
    for (int i  = 0; i < 8; i++)
    {
        SDL_Rect current_colour = {palette->colours[i].x, palette->colours[i].y,
                                   palette->colours[i].width, palette->colours[i].height};
        SDL_SetRenderDrawColor(renderer, palette->colours[i].r, palette->colours[i].g,
                               palette->colours[i].b, palette->colours[i].a);
        SDL_RenderFillRect(renderer, &current_colour);
    }
}

void draw_grid(SDL_Renderer *renderer, paint_state *paint_state, int size)
{
    SDL_SetRenderDrawColor(renderer, 61, 61, 61, 255);

    int x = paint_state->grid_rect.x;
    int y = paint_state->grid_rect.y;
    int w = paint_state->grid_rect.w;
    int h = paint_state->grid_rect.h;

    for (int i = x + size; i < x + w; i += size)
    {
        SDL_RenderDrawLine(renderer, i, y, i, y + h);
    }

    for (int i = y + size; i < y + h; i += size)
    {
        SDL_RenderDrawLine(renderer, x, i, x + w, i);
    }

    for (int row = 0; row < 24; ++row)
    {
        for (int col = 0; col < 24; ++col)
        {
            if (paint_state->grid[row][col] != 255) // 255 is the default value for an uninitialized cell
            {
                int colorIndex = paint_state->grid[row][col];
                colours cell_colour = paint_state->p.colours[colorIndex];
                SDL_Rect cell_rect = {paint_state->grid_rect.x + col * (paint_state->grid_rect.w / 24),
                                     paint_state->grid_rect.y + row * (paint_state->grid_rect.h / 24),
                                     paint_state->grid_rect.w / 24,
                                     paint_state->grid_rect.h / 24};

                SDL_SetRenderDrawColor(renderer, cell_colour.r, cell_colour.g, cell_colour.b, cell_colour.a);
                SDL_RenderFillRect(renderer, &cell_rect);
            }
        }
    }
}

void save_to_bmp_file(paint_state *paint_state)
{
    char * file_name = "drawing";

    SDL_Surface *surface = SDL_CreateRGBSurface(0, 480, 480, 32, 0, 0, 0, 0);

    for (int row = 0; row < 24; row++)
    {
        for (int col = 0; col < 24; col++)
        {
            if (paint_state->grid[row][col] != 255)
            {
                int colour_index = paint_state->grid[row][col];
                colours cell_colour = paint_state->p.colours[colour_index];

                Uint32 pixelColor = SDL_MapRGBA(surface->format, cell_colour.r, cell_colour.g, cell_colour.b, cell_colour.a);
                int x = col * (paint_state->grid_rect.w / 24);
                int y = row * (paint_state->grid_rect.h / 24);

                SDL_Rect destRect = {x, y, paint_state->grid_rect.w / 24, paint_state->grid_rect.h / 24};

                SDL_FillRect(surface, &destRect, pixelColor);
            }
        }
    }
    SDL_SaveBMP(surface, file_name);
    SDL_FreeSurface(surface);
}

bool mouse_down = false;

void handle_click(SDL_Event *mouse_e, paint_state *paint_state)
{
    int mouse_x = mouse_e->button.x;
    int mouse_y = mouse_e->button.y;

    switch (mouse_e->type)
    {
        case SDL_MOUSEBUTTONDOWN:
        {
            mouse_down = true;

            if ( (mouse_x >= 490 && mouse_x <= 630) && (mouse_y >= 10 && mouse_y <= 80))
            {
                save_to_bmp_file(paint_state);
            }
            else if ( (mouse_x >= 480 && mouse_x <= 640) && (mouse_y >= 80 && mouse_y <= 480)) {
                int col = (mouse_x - 490) / (60 + 10);
                int row = (mouse_y - 90) / (60 + 10);
                paint_state->selected_color = col + row * 2;
            }
            else if ( (mouse_x >= 0 && mouse_x <= 480) && (mouse_y >= 0 && mouse_y <= 480))
            {
                int cell_x = (mouse_x - paint_state->grid_rect.x) / (paint_state->grid_rect.w / 24);
                int cell_y = (mouse_y - paint_state->grid_rect.y) / (paint_state->grid_rect.h / 24);

                paint_state->grid[cell_y][cell_x] = paint_state->selected_color;
            }
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            mouse_down = false;
            break;
        }
        case SDL_MOUSEMOTION:
        {
            if ( mouse_down && (mouse_x >= 0 && mouse_x <= 480) && (mouse_y >= 0 && mouse_y <= 480) )
            {
                int cell_x = (mouse_x - paint_state->grid_rect.x) / (paint_state->grid_rect.w / 24);
                int cell_y = (mouse_y - paint_state->grid_rect.y) / (paint_state->grid_rect.h / 24);

                paint_state->grid[cell_y][cell_x] = paint_state->selected_color;
            }
            break;
        }
    }


}

void render_screen(SDL_Renderer *renderer, paint_state *paint_state)
{
    // Render a white screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderClear(renderer);

    // Draw palette on right side of screen
    draw_palette(renderer, &paint_state->p);

    // Draw grid on left side of screen
    draw_grid(renderer, paint_state, 20);

    SDL_RenderPresent(renderer);
}

short process_events(SDL_Window *window, paint_state *state)
{
    SDL_Event event;

    short done = 0;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_WINDOWEVENT_CLOSE:
            {
                if (window)
                {
                    SDL_DestroyWindow(window);
                    window = NULL;
                    done = 1;
                }
            }
            break;
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    {
                        done = 1;
                        break;
                    }
                }
            }
            break;
            case SDL_QUIT:
            {
                done = 1;
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEMOTION:
                handle_click(&event, state);
                break;
        }
    }
    return done;
}

int main(int argc, char *argv[])
{
    paint_state paint_state;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    init_palette(&paint_state);

    paint_state.grid_rect = (SDL_Rect){0, 0, 480, 480};
    memset(paint_state.grid, 255, sizeof(paint_state.grid));

    // Initialize default drawing colour to black
    paint_state.selected_color = 6;

    window = SDL_CreateWindow("Paint Window",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640,
                              480,
                              0
                              );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    short done = 0;

    while (!done)
    {
        done = process_events(window, &paint_state);

        render_screen(renderer, &paint_state);
    }
    return 0;
}
