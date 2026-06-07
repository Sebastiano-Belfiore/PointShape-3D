#include <SDL2/SDL.h>
#include <stdint.h>
#include <math.h>
#define PI 3.14159265358979323846
#define NUMPOINTS 1000
typedef struct  {
    int r,g,b;
}Color;
typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    Vertex v;
    Color c;    
} Point;

typedef struct {
    int v0, v1, v2; //Verticies
    Point points[NUMPOINTS];       
} Triangle;

typedef struct {
    Vertex   *vertices;
    int       num_vertices;
    Triangle *triangles;
    int       num_triangles;
} Model;
Vertex randomPointInTriangle(Vertex A,Vertex B,Vertex C) {
    float r1 = (float)rand() / RAND_MAX;
    float r2 = (float)rand() / RAND_MAX;

    if (r1 + r2 > 1) {
        r1 = 1 - r1;
        r2 = 1 - r2;
    }

    Vertex P;
    P.x = A.x + r1 * (B.x - A.x) + r2 * (C.x - A.x);
    P.y = A.y + r1 * (B.y - A.y) + r2 * (C.y - A.y);
    P.z = A.z + r1 * (B.z - A.z) + r2 * (C.z - A.z);
    return P;
}
void setPointsToModel(Model *model){
    int tri = model->num_triangles;
    for(int j = 0; j < tri; j++){
        for(int i = 0; i < NUMPOINTS; i++){
            Vertex A = model->vertices[model->triangles[j].v0];
            Vertex B = model->vertices[model->triangles[j].v1];
            Vertex C = model->vertices[model->triangles[j].v2];
            model->triangles[j].points[i].v = randomPointInTriangle(A,B,C);
            model->triangles[j].points[i].c = (Color){255,255,255};
        }    
    }
}

void create_circle(Model *model, int segments){
    
    float step = (2 * PI) / segments;
    float radius = 5;
    // 0 is center vertex
    model->num_vertices = segments + 1;
    model->vertices = malloc(sizeof(Vertex) * (segments + 1));
    model->vertices[0] = (Vertex) {0,0,0};
    for(int i = 1; i < segments + 1; i++){
        float angle = step * i;
        
        model->vertices[i] = (Vertex){
            cosf(angle) * radius, //Togliere radius da qui e creare modello cerchio 
            sinf(angle) * radius,
            0
        };
    }

    model->triangles = malloc(sizeof(Triangle) * (segments + 1));
    model->num_triangles = segments;
    int verticies_index = 0;
    for(int i = 0; i < segments; i++){
        model->triangles[i].v0 = 0; //center
        model->triangles[i].v1 = i+ 1;
        model->triangles[i].v2 = ((i+1) % segments) + 1;

        printf("tri[%d]: v0=%d v1=%d v2=%d (num_vertices=%d)\n",
            i,
            model->triangles[i].v0,
            model->triangles[i].v1,
            model->triangles[i].v2,
            model->num_vertices);
    }
    
    setPointsToModel(model);
    
}

void clear(SDL_Surface *surface) {
    int height = surface->h;
    int pitch = surface->pitch;
    uint8_t *fb = (uint8_t*)surface->pixels;
    memset(fb, 0, pitch * height);
}


Model *create_model(void) {
    Model *model = malloc(sizeof(Model));

    model->num_vertices = 3;
    model->vertices = malloc(sizeof(Vertex) * 3);
    model->vertices[0] = (Vertex){ 0,  1, 0};
    model->vertices[1] = (Vertex){-1, -1, 0};
    model->vertices[2] = (Vertex){ 1, -1, 0};

    model->num_triangles = 1;
    model->triangles = malloc(sizeof(Triangle) * 1);
    model->triangles[0].v0 = 0;
    model->triangles[0].v1 = 1;
    model->triangles[0].v2 = 2;
    model->num_triangles = 1;
    setPointsToModel(model);
    return model;
}
void pixel(SDL_Surface *surface, int x, int y, int r, int g, int b) {
    // Write pixels – fill with a horizontal RGB gradient
    int width = surface->w;
    int height = surface->h;
    if (x < 0 || x >= width) return;
    if (y < 0 || y >= height) return;

    int pitch = surface->pitch;               // bytes per row
    uint8_t *fb = (uint8_t*)surface->pixels;

    fb[y * pitch + x * 4 + 0] = b;
    fb[y * pitch + x * 4 + 1] = g;
    fb[y * pitch + x * 4 + 2] = r;
    fb[y * pitch + x * 4 + 3] = 255;
}

void draw(SDL_Surface *surface, float time, Model *model) {
    int width = surface->w;
    int height = surface->h;
    float cx = (float)width / 2;
    float cy = (float)height / 2;

    clear(surface);
    for (int t = 0; t < model->num_triangles; t++){
        for (int j = 0; j < NUMPOINTS; j++) {
            float zfactor = 1 + model->triangles[t].points[j].v.z;
            float x = (model->triangles[t].points[j].v.x / zfactor) * 50;
            float y = (model->triangles[t].points[j].v.y / zfactor) * 50;
            pixel(surface, round(cx+x), round(cy+y), 255, 255, 255);
        }
}
}




int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Initialize SDL video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "Pixel Framebuffer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Get the window's surface (the framebuffer)
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    if (!surface) {
        SDL_Log("SDL_GetWindowSurface failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    clear(surface);
    Model *model = malloc(sizeof(Model));
    create_circle(model,20);
    int running = 1;
    float time = 0;
    while(running) {
        draw(surface,time,model);
        SDL_UpdateWindowSurface(window);
        time += 1;

        // Main loop – wait for quit event
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
        SDL_Delay(16);
    }

    // Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}