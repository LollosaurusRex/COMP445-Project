#include <iostream>
#include <stdlib.h>

#include <tsgl.h>
#include "myVector.cpp"


using namespace tsgl;

int numThreads = 8;
/*
    Arrays I need:
        boids x's
        boids y's
        
        boids velocity x
        boids velocity y
        
        boids new velocity x
        boids new velocity y

        
*/

class boid {
private:
    Arrow* arrow;
    Canvas& can;
public:
    Vector2 pos, vel, acc;
    boid(float x, float y, Canvas& can) : can(can) {
        pos = Vector2(x, y);
        arrow = new Arrow(
            x, y, 0, 
            20, 12, 
            0, 0, 0, 
            CYAN
        );
        can.add(arrow);
    }

    ~boid() {
        can.remove(arrow);
        delete arrow;
    }
};

void testScreen(Canvas& canvas) {
    const int WW = canvas.getWindowWidth(),
              WH = canvas.getWindowHeight();

    boid* b = new boid(0, 0, canvas);

    Background* back = canvas.getBackground();
    back->drawSquare(0, 0, 0, 50, 0, 0, 0, WHITE, true);

    back->drawPixel(0, 0, ColorInt(120, 120, 120));




    /* NOTES:
        Background has (0, 0) in the center for some reason

        Whenever I want to flush the updated arrays to the background, 
        I should do this on the CPU. 
    */
    // #define TEST
    #ifdef TEST
    int w = WW / 2;
    int h = WH / 2;
    double startT = omp_get_wtime();
    int i, j;
    #ifdef GPU
    #pragma acc kernels
    #pragma acc loop independent collapse(2) private(i, j)
    #else
    #pragma acc parallel loop independent collapse(2) private(i, j) num_gangs(numThreads)
    #endif
    for (i = -h; i < h; ++i) {
        for (j = -w; j < w; ++j) {
            int color = 240;

            #ifndef GPU
            color = omp_get_thread_num() * 25 + 25;
            #endif

            back->drawPixel(j, i, ColorInt(color));
            // canvas.sleepFor(0.00005);
        }
    }
    printf("Duration: %lf\n", omp_get_wtime() - startT);
    #endif


    while (canvas.isOpen()) {
        canvas.sleep();

        
        // for (int iter = 0; iter < 1000; ++iter) {
            /*
            pragma compiler directives
            for each boid i
                for each boid j
                    if j is i continue

                    do the boid calculation stuff, new position and velocity stored in new arrays

            
            update position prev position to new position

            have CPU update screen
            */

        // }
    }

    
} 

int vec(int a) {
    return a + 1;
}

int main (int argc, char* argv[]) {
    

    std::cout << "Hello world!" <<  std::endl;

    numThreads = atoi(argv[1]);
    printf("NumThreads: %d\n", numThreads);

    Vector2 a(5, 5);
    Vector2 b(10, 11);

    Vector2 c = a + b;

    Canvas can(-1, -1, 1600, 900, "Test Screen", BLACK);
    can.run(testScreen);

    

    #ifdef GPU
    #pragma acc kernels
    #pragma acc loop independent
    #else
    #pragma acc parallel loop independent
    #endif
    for (int i = 0; i < 100; ++i) {
        // printf("Hello! Thread %d on %d\n", omp_get_thread_num(), i);
        // printf("Hello there! I'm hopefully threaded lol %d\n", i);
    }

    return 0;
}