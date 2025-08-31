#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <functional>
#include <unistd.h>
#include <cmath>
#include "ran.h"

#define N 10000
#define NUMV 8
#define WIDTH  500
#define HEIGHT 500

void V(int i, double &x, double &y);
void fractalFlame(int iter, int seed, SDL_Renderer *ren, SDL_Window *win, int (*surfaceArray)[HEIGHT][3]);

int main(int argc, char* argv[]) {

	SDL_Renderer *ren;
	SDL_Window *win;

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &win, &ren);
	int (*surfaceArray)[HEIGHT][3] = new int[WIDTH][HEIGHT][3]();

	int iter = 10000000;
	int seed = 0;
	int nsamples = 0;

	int option;
    while ((option = getopt(argc, argv, "s:i:n:")) != -1) {
        switch (option) {
            case 's':
	            seed = atoi(optarg);
				break;
            case 'i':
	            iter = atoi(optarg);
				break;
            case 'n':
	            nsamples = atoi(optarg);
				break;
        }
    }
	
	if (nsamples > 1) printf("Making %d images starting from seed %d\n\n", nsamples, seed); 
	for (int i=0; i<nsamples; ++i) {
		if (nsamples > 1) printf("Making image %d/%d\r", i, nsamples); 
		fractalFlame(iter, i + seed, ren, win, surfaceArray);
	}

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    IMG_Quit();
    SDL_Quit();
	delete[] surfaceArray;
    return 0;
}

void fractalFlame(int iter, int seed, SDL_Renderer *ren, SDL_Window *win, int (*surfaceArray)[HEIGHT][3]) {
	std::function<void(double&, double&)> F[N];

	Ran ran(seed);
	{
		double a, b, c, d, e, f;
		for (int i=0; i<N; i++) {

			a = ran.doub();
			b = ran.doub();
			c = ran.doub();
			d = ran.doub();
			e = ran.doub();
			f = ran.doub();

			int case0 = ran.int64() % NUMV;
			F[i] = [a, b, c, d, e, f, case0](double &x, double &y) {
				double x0 = a*x + b*y + c;
				double y0 = d*x + e*y + f;
				x = x0;
				y = y0;

				V(case0, x, y);
			};
		}
	}

	int fval_v = ran.int64() % NUMV;
	auto F_final = [F, fval_v ](double& x, double& y) { F[fval_v](x, y); };
	
	double x, y, c;
	int x_f, y_f;
	x = ran.doub()*2 - 1;
	y = ran.doub()*2 - 1;
	c = ran.doub();

	for (int i=0; i<WIDTH; i++) {
		for (int j=0; j<WIDTH; j++) {
			for (int k=0; k<3; k++) {
				surfaceArray[i][j][k] = 0;
			}
		}
	}

    for (int k=0; k<iter; ++k) {

		int i = ran.int64() % NUMV;
		x = x + 1e-7;
		y = y + 1e-7;

		(F[i])(x, y);
		F_final(x, y);

		x_f = (int)((x + 1.0) * ((double)WIDTH/2.0));
		y_f = (int)((y + 1.0) * ((double)HEIGHT/2.0));

		if ((0 <= x_f) && (x_f < WIDTH) && (0 <= y_f) && (y_f < HEIGHT)) {
			for (int j=0; j<3; j++) {
				surfaceArray[x_f][y_f][j] = 255;
			}
		}
	}

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
    SDL_RenderClear(ren);

	for (int i=0; i<WIDTH; i++) {
		for (int j=0; j<HEIGHT; j++) {
			SDL_SetRenderDrawColor(ren,
				surfaceArray[i][j][0],
				surfaceArray[i][j][1],
				surfaceArray[i][j][2],
				0);
			SDL_RenderDrawPoint(ren, i, j);
		}
	}

	SDL_Surface *surface;
    surface = SDL_CreateRGBSurfaceWithFormat(0, WIDTH, HEIGHT, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_RenderReadPixels(ren, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch);

	char fname[100];
    sprintf(fname, "./data/%d.png", seed);
    IMG_SavePNG(surface, fname);

    SDL_FreeSurface(surface);
}



void V(int i, double &x, double &y) {
	double r2 = x*x + y*y;
	double r = sqrt(r2);
	double theta = atan2(x, y);
	double phi = atan2(y, x);

	switch (i) {
		case 0:
			x = x;
			y = y;
			break;
		case 1:
			x = sin(x);
			y = sin(y);
			break;
		case 2:
			if (r2 < 1e-6) {
				x = 1;
				y = -1;
			} else {
				x = (x / r2);
				y = (y / r2);
			}
			break;
		case 3:
			x = x*sin(r2) + y*cos(r2); 
			y = x*cos(r2) + y*sin(r2); 
			break;
		case 4:
			x = (x - y)*(x + y);
			y = 2*x*y;
			break;
		case 5:
			x = theta / M_PI;
			y = r - 1;
			break;
		case 6:
			x = r*sin(theta + r);
			y = r*cos(theta - r);
			break;
		case 7:
			x = r*sin(theta*r);
			y = - r*cos(theta*r);
			break;
		case 8:
			x = (theta/M_PI)*sin(M_PI*r);
			y = (theta/M_PI)*cos(M_PI*r);
			break;
	}
}


