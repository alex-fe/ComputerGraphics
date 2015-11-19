#include <FPT.h>
#include <D3d_matrix.h>

double halfangle = 40;
double radians = 3 * (M_PI / 180);
int WIDTH = 600; int HEIGHT = 600; int DEPTH = 600;

typedef struct {
  double centerx, centery, centerz;
  double *x, *y, *z;
  FILE *file;
  int numpolys; //the number of subpolygons that make up the object
  int points; //number of points in whatever
  int shapeorder[100][100]; //Where things connect to
  int shapes[1000]; //the shapes [which shape]
} Polygon;

typedef struct {
  double avg_depth;
  double plane_x[100];
  double plane_y[100];
  double plane_z[100];
  double r, g, b;
  int size;
} Plane;

void printarray(Plane *a, int size) {
  int i;
  for (i = 0; i < size; i++) {
    printf("%.2lf\n", a[i].avg_depth);
  }
  printf("\n");
}

int size(double *a){

}

//reads in file
//Modified from lab2; does not read in colors
int readobject1(FILE *g) {
  int p;
  fscanf(g, "%d", &poly->points);
  printf("there are %d points\n", poly->points);
  return 
}

void readobject2(FILE *g, Polygon* poly) {
  for (k = 0; k < poly->points; k++) {
        fscanf(g, "%lf  %lf  %lf", &poly->x[k], &poly->y[k],
           &poly->z[k]);
    // printf("(%lf,%lf,%lf)\n", poly->x[k], poly->y[k], poly->z[k]);
  }

  // fscanf(g, "%d", &poly->numpolys);
  // // printf("there are %d polygons in this whatever\n", poly->numpolys);
  // for (k = 0; k < poly->numpolys; k++) {
  //   fscanf(g, "%d", &poly->shapes[k]);
  //   // printf("%d: ", poly->shapes[k]);
  //   for (j = 0; j < poly->shapes[k]; j++) {
  //     fscanf(g, "%d", &poly->shapeorder[k][j]);
  //     // printf("%d ", poly->shapeorder[k][j]);
  //   }
  //   G_wait_key();
  //   // printf("\n");
  // }
}

int compare (const void *p, const void *q) {
  Plane *a, *b ;

  a = (Plane*)p ; b = (Plane*)q ;

  return ((*a).avg_depth) < ((*b).avg_depth) ? 1 :
         ((*a).avg_depth) > ((*b).avg_depth) ? -1 : 0;
}

void drawit(Polygon poly) {
  int k, j;
  double mod = (HEIGHT / 2) / tan(halfangle * (M_PI / 180));
  Plane planecontainer[poly.numpolys];
  for (k = 0; k < poly.numpolys; k++) {
    j = 0;
    planecontainer[k].avg_depth = 0.0;
    while (j < poly.shapes[k]) {
      if (fabs(poly.z[poly.shapeorder[k][j]]) > 10e-7) { //Checks if z == 0
        planecontainer[k].plane_x[j] = mod * (poly.x[poly.shapeorder[k][j]]
                                              / poly.z[poly.shapeorder[k][j]])
                                       + (WIDTH / 2);
        planecontainer[k].plane_y[j] = mod * (poly.y[poly.shapeorder[k][j]]
                                              / poly.z[poly.shapeorder[k][j]])
                                       + (WIDTH / 2);
        printf("%lf, %lf\n", planecontainer[k].plane_x[j], planecontainer[k].plane_y[j]);
        planecontainer[k].plane_z[j] = poly.z[poly.shapeorder[k][j]];
        planecontainer[k].avg_depth += poly.z[poly.shapeorder[k][j]];
        j++;
      } else {
        j++;
      }
    }
    planecontainer[k].size = j;
    planecontainer[k].avg_depth = planecontainer[k].avg_depth / j;
    planecontainer[k].r = (double)(rand() % 100) / 100;
    planecontainer[k].g = (double)(rand() % 100) / 100;
    planecontainer[k].b = (double)(rand() % 100) / 100;
  }
  // printarray(planecontainer, poly.numpolys);
  qsort (planecontainer, poly.numpolys, sizeof(Plane), compare);
  // printarray(planecontainer, poly.numpolys);

  for (k = 0; k < poly.numpolys; k++) {
    G_rgb(planecontainer[k].r, planecontainer[k].g, planecontainer[k].b);
    G_fill_polygon(planecontainer[k].plane_x,
                   planecontainer[k].plane_y, planecontainer[k].size);
  }

}

//finds the largest/smallest value in array
int findextrema(double *y, int z, int swatch) {
  int position = 0; int i = 0;
  double temp = 0 + (swatch * HEIGHT);
  for (i = 0; i < z; i++) {
    if (swatch == 0 && y[i] > temp) { //finds largest
      position = i;
      temp = y[i];
    } else if (swatch == 1 && y[i] < temp) { //finds smallest
      position = i;
      temp = y[i];
    }
  }
  return position;
}

//finds the scale factor of the object
//also reveals the center of the object
double scale_n_fit(Polygon *poly) {
  int k;
  double smallx, bigx, smally, bigy, smallz, bigz;
  bigx = poly->x[findextrema(poly->x, poly->points, 0)];
  smallx = poly->x[findextrema(poly->x, poly->points, 1)];
  bigy = poly->y[findextrema(poly->y, poly->points, 0)];
  smally = poly->y[findextrema(poly->y, poly->points, 1)];
  bigz = poly->z[findextrema(poly->z, poly->points, 0)];
  smallz = poly->z[findextrema(poly->z, poly->points, 1)];

  double boxheight = bigy - smally;
  double boxwidth =  bigx - smallx;
  double boxdepth = bigz - smallz;

  poly->centerx = (bigx + smallx) / 2;
  poly->centery = (bigy + smally) / 2;
  poly->centerz = (bigz + smallz) / 2;
  double centers[3] = {poly->centerx, poly->centery,
                       poly->centerz
                      };
  return (findextrema(centers, 3, 1) == 1) ? WIDTH / boxwidth :
         (findextrema(centers, 3, 1) == 2) ? HEIGHT / boxheight :
         DEPTH / boxdepth;
}

//changes the message based on how many objects are inputted via command line
void welcome(int i) {
  printf((i == 1) ? "Press 1 to move the object: " :
         (i == 2) ? "Press 1 or 2 to the first or second object: " :
         "Press 1 through %d to move the respective object: ", i);
}

int main (int argc, char **argv)
{
  char q, action;
  double mat[4][4], minv[4][4], scaleFactor;
  int cc, sign, currentObj, k, h;
  int increment = 20;
  int xcounter, ycounter, zcounter = 0;
  Polygon polygon[argc];

  for (cc = 1; cc < argc; cc++) {
    polygon[cc].file = fopen(argv[cc], "r"); //opens a file; r = read only
    if (polygon[cc].file == NULL) { //if the file is empty
      printf("can't open (1)\n");
      exit(1);
    } else {
      readobject(polygon[cc].file, &polygon[cc]);
      printf("%d\n", sizeof(polygon[cc].x)/ sizeof(polygon[cc].x[0]));

      // printf("%d\n", polygon[cc].points);
      // D3d_make_identity(mat); D3d_make_identity(minv);
      // scaleFactor = scale_n_fit(&polygon[cc]);
      // D3d_translate(mat, minv, -polygon[cc].centerx, -polygon[cc].centery,
      //               -polygon[cc].centerz);
      // D3d_scale(mat, minv, scaleFactor, scaleFactor, scaleFactor);
      // D3d_mat_mult_points(polygon[cc].x, polygon[cc].y, polygon[cc].z,
      //                     mat, polygon[cc].x, polygon[cc].y,
      //                     polygon[cc].z, polygon[cc].points);
    }
  }

  exit(1);
  printf("Which object (between 1 and %d) would you like to start with? \n", argc - 1);
  scanf("%c", &q);
  currentObj = q - '0';
  sign = 1 ;
  action = 't' ;

  if (currentObj < argc && currentObj > 0) {
    G_init_graphics(WIDTH, HEIGHT);

    while (1) {
      G_rgb(0, 0, 0);
      G_clear();
      drawit(polygon[currentObj]);


      D3d_make_identity (mat) ;
      D3d_make_identity (minv) ;

      q = G_wait_key() ;

      if (q == 'q') {
        exit(0) ;
      } else if (q == 'c') {
        sign = -sign ;
      } else if (q == 't') {
        action = q ;
      } else if (q == 'S') {
        action = q ;
      } else if (q == 'r') {
        action = q ;
      } else if (('0' <= q) && (q <= '9')) {
        k = q - '0' ;
        if (h != currentObj) {
          currentObj = k;
        }
      } else if ((q == 'x') && (action == 't')) {
        D3d_translate (mat, minv, sign * increment, 0, 0);
        xcounter = xcounter + (sign * increment);

      } else if ((q == 'y') && (action == 't')) {
        D3d_translate (mat, minv, 0, sign * increment, 0);
        ycounter = ycounter + (sign * increment);

      } else if ((q == 'z') && (action == 't')) {
        D3d_translate(mat, minv, 0, 0, sign * increment);
        zcounter = zcounter + (sign * increment);

      } else if ((q == 'x') && (action == 'r')) {
        D3d_translate(mat, minv, -xcounter, -ycounter, -zcounter);
        D3d_rotate_x(mat, minv, sign * radians);
        D3d_translate(mat, minv, xcounter, ycounter, zcounter);

      } else if ((q == 'y') && (action == 'r')) {
        D3d_translate(mat, minv, -xcounter, -ycounter, -zcounter);
        D3d_rotate_y(mat, minv, sign * radians);
        D3d_translate(mat, minv, xcounter, ycounter, zcounter);

      } else if ((q == 'z') && (action == 'r')) {
        D3d_translate(mat, minv, -xcounter, -ycounter, -zcounter);
        D3d_rotate_z(mat, minv, sign * radians);
        D3d_translate(mat, minv, xcounter, ycounter, zcounter);

      } else {
        printf("no action\n") ;
      }


      D3d_mat_mult_points(polygon[currentObj].x, polygon[currentObj].y,
                          polygon[currentObj].z, mat,
                          polygon[currentObj].x, polygon[currentObj].y,
                          polygon[currentObj].z,
                          polygon[currentObj].points + 1) ;
      //the numpoints[currentObj]+1 is because we have stored
      //the center of the object at the arrays' end
    }
  }
}

