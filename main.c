#include <SDL2/SDL.h>
#include <stdint.h>
#include <math.h>
#define PI 3.14159265358979323846
#define NUMPOINTS 500
typedef struct  {
    int r,g,b;
}Color;
typedef struct {
    float x, y, z;
} Vertex;


// r1, r2 sono i pesi del Point rispetto al Triangle
// Indicano dove si trova il Point all'interno del Triangle
// Per animare il modello, invece di rigenerare i punti con randomPointInTriangle(),
// si usa recalc_point() che ricalcola la posizione del Point
// seguendo i vertici del Triangle aggiornati.
typedef struct {
    int r1,r2;
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
void setPointsToModel(Model *model) {
    for (int j = 0; j < model->num_triangles; j++) {
        for (int i = 0; i < NUMPOINTS; i++) {
            float r1 = (float)rand() / RAND_MAX;
            float r2 = (float)rand() / RAND_MAX;
            if (r1 + r2 > 1) { r1 = 1 - r1; r2 = 1 - r2; }

            model->triangles[j].points[i].r1 = r1;
            model->triangles[j].points[i].r2 = r2;

            Vertex A = model->vertices[model->triangles[j].v0];
            Vertex B = model->vertices[model->triangles[j].v1];
            Vertex C = model->vertices[model->triangles[j].v2];
            model->triangles[j].points[i].v = randomPointInTriangle(A, B, C);
            model->triangles[j].points[i].c = (Color){255, 255, 255};
        }
    }
}
void create_cube(Model *model){
    model->num_vertices = 8;
    model->num_triangles = 12; //6 faces
    float h = 4;
    float half_h = h /2; 
    //center 0,0,0
    //half_h distanza dal centro verso ogni vertice
    model->vertices = malloc(sizeof(Vertex) * model->num_vertices);
    model->triangles = malloc(sizeof(Triangle) * model->num_triangles);
    //z -- front(+half_h), back(-half_h)
    //x -- left(-half_h), right(+half_h)
    //y -- up(+half_h), bottom(-half_h)
    model->vertices[0] = (Vertex){-half_h,+half_h,+half_h};  //left-up-front -A 
    model->vertices[1] = (Vertex){+half_h,+half_h,+half_h};  //right-up-front - B
    model->vertices[2] = (Vertex){+half_h,-half_h,+half_h}; //right-bottom-front - C
    model->vertices[3] = (Vertex){-half_h,-half_h,+half_h};  //left-bottom-front - D
    model->vertices[4] = (Vertex){-half_h,+half_h,-half_h}; //left-up-back - E
    model->vertices[5] = (Vertex){+half_h,+half_h,-half_h}; //right-up-back - F
    model->vertices[6] = (Vertex){+half_h,-half_h,-half_h}; //right-bottom-back - G
    model->vertices[7] = (Vertex){-half_h,-half_h,-half_h}; //left-bottom-back - H


    //front
    model->triangles[0] = (Triangle){0,1,2}; //A-B-C
    model->triangles[1] = (Triangle){0,2,3}; // A-C-D

    //back

    model->triangles[2] = (Triangle){4,5,6}; //E-F-G
    model->triangles[3] = (Triangle){4,6,7}; // E-G-H

    //left

    model->triangles[4] = (Triangle){0,4,7}; //A-E-H
    model->triangles[5] = (Triangle){0,7,3}; //A-H-D

    //right

    model->triangles[6] = (Triangle){1,5,6}; //B-F-G
    model->triangles[7] = (Triangle){1,6,2}; //B-G-C

    //up

    model->triangles[8] = (Triangle){0,4,5}; //A-E-F
    model->triangles[9] = (Triangle){0,5,1}; //A-F-B

    //down

    model->triangles[10] = (Triangle){3,7,6}; //D-H-G
    model->triangles[11] = (Triangle){3,6,2}; //D-G-C

    setPointsToModel(model);


    
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
            cosf(angle) * radius, 
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
            float zfactor = 1 + model->triangles[t].points[j].v.z+3;
            float x = (model->triangles[t].points[j].v.x / zfactor) * 100;
            float y = (model->triangles[t].points[j].v.y / zfactor) * 100;
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
    //reate_circle(model,20);
    create_cube(model);
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