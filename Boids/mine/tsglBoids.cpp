#include <tsgl.h>
#include "boids.hpp"
#include "misc.h"

using namespace tsgl;

class boid {
private:
    Arrow* arrow;
    Canvas* can;
public:
    int index;
    /**
     * @brief Construct a new boid object, which is only ever stored on the CPU.
     * We use pointer arrays so we don't have to send my boid objects to the GPU.
     * 
     * @param x 
     * @param y 
     * @param can 
     */
    boid(float x, float y, int index, Canvas* canvasP) : can(canvasP) {
        this->index = index;
        arrow = new Arrow(
            x, y, 0, 
            20, 20, 
            0, 0, 0, 
            CYAN
        );
        can->add(arrow);
    }

    /**
     * @brief Destroy the boid object
     * 
     */
    ~boid() {
        can->remove(arrow);
        delete arrow;
    }

    void setColor(tsgl::ColorFloat color) {
        arrow->setColor(color);
    }

    void updatePosition(float x, float y) {
        arrow->setCenter(x, y, 0);
    }

    void updateDirection(float velx, float vely) {
        float yaw = atan(vely / velx) * 180. / PI;
        if (velx > 0) {
            yaw += 180;
        }
        arrow->setYaw(yaw);
    }
};

struct boids::Params p;

struct boids::Params defaultParams = 
    {
		.width = 1024, .height = 1024, 
		.num = 1024, .len = 20, 
		.mag = 1, .seed = 0, 
		.invert = 0, .steps = 100000000, 
		.psdump = 0, .angle = 270.0, 
		.vangle = 90, .minv = 0.5, 
		.ddt = 0.95, .dt = 3.0, 
		.rcopy = 80, .rcent = 30, 
		.rviso = 40, .rvoid = 15, 
		.wcopy = 0.2, .wcent = 0.4, 
		.wviso = 0.8, .wvoid = 1.0, .
		threads = 1, .term = NULL
    };

float* xp;
float* yp;
float* xv;
float* yv;
float* xnv;
float* ynv;

void initiateBoidArrays(
    struct boids::Params p, 
    float* xp, float* yp,
    float* xv, float* yv
) 
{
    for (int i = 0; i < p.num; ++i) {
        xp[i] = random_range(-p.width/2, p.width/2);
        yp[i] = random_range(-p.height/2, p.height/2);
        xv[i] = random_range(-1., 1.);
        yv[i] = random_range(-1., 1.);
        boids::norm(&xv[i], &yv[i]);
    }
}

void initiateBoidDraw(
    struct boids::Params p,
    std::vector<std::unique_ptr<boid>>& boidDraw,
    float* xp, float* yp,
    float* xv, float* yv,
    Canvas* canvasP
)
{
    for (int i = 0; i < p.num; ++i) {
        boidDraw[i] = std::make_unique<boid>(xp[i], yp[i], i, canvasP);
        boidDraw[i]->updateDirection(xv[i], yv[i]);
    }
}

void tsglScreen(Canvas& canvas) {
    initiateBoidArrays(p, xp, yp, xv, yv);

    std::vector<std::unique_ptr<boid>> boidDraw(p.num);
    initiateBoidDraw(p, boidDraw, xp, yp, xv, yv, &canvas);

    while (canvas.isOpen()) {
        canvas.sleep();

        boids::compute_new_headings(p, xp, yp, xv, yv, xnv, ynv);


        for (int i = 0; i < p.num; ++i) {
            xv[i] = xnv[i];
            yv[i] = ynv[i];
            xp[i] += xv[i] * p.dt;
            yp[i] += yv[i] * p.dt;
            
            
            // Wrap around screen coordinates
            if (xp[i] < -p.width / 2) {
                xp[i] += p.width;
            }
            else if (xp[i] >= p.width / 2) {
                xp[i] -= p.width;
            }

            if (yp[i] < -p.height / 2) {
                yp[i] += p.height;
            }
            else if (yp[i] >= p.height / 2) {
                yp[i] -= p.height;
            }

            boidDraw[i]->updatePosition(xp[i], yp[i]);
            boidDraw[i]->updateDirection(xv[i], yv[i]);
        }

    }
}

int main(int argc, char* argv[]) {
    
    p = defaultParams;

    xp  = new float[p.num];
    yp  = new float[p.num];
    xv  = new float[p.num];
    yv  = new float[p.num];
    xnv = new float[p.num];
    ynv = new float[p.num];



    

    Canvas can(-1, -1, p.width, p.height, "Test Screen", BLACK);
    can.run(tsglScreen);


    delete [] xp;
    delete [] yp;
    delete [] xv;
    delete [] yv;
    delete [] xnv;
    delete [] ynv;
}