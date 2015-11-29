#include <FPT.h>
#include <D3d_matrix.h>

double halfangle = 40;
double radians = 3 * (M_PI / 180);
int WIDTH = 600; int HEIGHT = 600; int DEPTH = 600;


typedef struct {
  double centerx, centery, centerz;
  double r, g, b;
  double x[10000], y[10000], z[10000];
  FILE *file;
  int numpolys; //the number of subobjects that make up the object
  int points; //number of points in whatever
  int shapeorder[1000][8]; //Where things connect to
  int shapes[10000]; //the shapes [which shape]
  int xcounter, ycounter, zcounter;
} Object;

typedef struct {
  double avg_depth;
  double color[3];
  double x[100];
  double y[100];
  double z[100];
  int size;
} Plane;

typedef struct {
  int counter;
  Plane *plane;
} Final_plane;

Final_plane total; //Global collection of planes

//reads in file
//Modified from lab2; does not read in colors
void readobject(FILE *g, Object* poly) {
  int j, k, p, np, shp[1000];
  // int temp_numpolys, temp_shapes[1000], temp_shapeorder[100][100];
  fscanf(g, "%d", &p);
  poly->points = p;
  // printf("there are %d points\n", poly->points);
  for (k = 0; k < poly->points; k++) {
    fscanf(g, "%lf  %lf  %lf", &poly->x[k], &poly->y[k],
           &poly->z[k]);
    // printf("(%lf,%lf,%lf)\n", poly->x[k], poly->y[k], poly->z[k]);
  }
  // printf("%d\n", sizeof(poly->x) / sizeof(poly->x[0]));
  fscanf(g, "%d", &np);
  poly->numpolys = np;
  // printf("there are %d objects in this whatever\n", poly->numpolys); //works up til here
  for (k = 0; k < np; k++) {
    fscanf(g, "%d", &shp[k]);
    poly->shapes[k] = shp[k];
    // printf("%d: ", poly->shapes[k]);
    for (j = 0; j < shp[k]; j++) {
      fscanf(g, "%d", &poly->shapeorder[k][j]);
      // printf("%d ", poly->shapeorder[k][j]);
    }
    // printf("\n");
  }
}

//Compare method; auxillary for qsort
int compare (const void *p, const void *q) {
  Plane *a, *b ;

  a = (Plane*)p ; b = (Plane*)q ;

  return ((*a).avg_depth) < ((*b).avg_depth) ? 1 :
         ((*a).avg_depth) > ((*b).avg_depth) ? -1 : 0;
}

//puts all the planes into collection
void predraw(Object poly, int in) {
  double mod = (HEIGHT / 2) / tan(halfangle * (M_PI / 180));
  int size = 0;
  int i, k, j;
  Plane plane[poly.numpolys];
  for (k = 0; k < poly.numpolys; k++) {
    j = 0;
    plane[k].avg_depth = 0.0;
    while (j < poly.shapes[k]) {
      if (fabs(poly.z[poly.shapeorder[k][j]]) > 10e-7) { //Checks if z == 0
        plane[k].x[j] = mod * (poly.x[poly.shapeorder[k][j]]
                               / poly.z[poly.shapeorder[k][j]])
                        + (WIDTH / 2);
        plane[k].y[j] = mod * (poly.y[poly.shapeorder[k][j]]
                               / poly.z[poly.shapeorder[k][j]])
                        + (WIDTH / 2);
        // printf("%lf, %lf\n", plane[k].x[j], plane[k].y[j]);
        plane[k].z[j] = poly.z[poly.shapeorder[k][j]];
        plane[k].avg_depth += poly.z[poly.shapeorder[k][j]];
        j++;
      } else {
        j++;
      }
    }
    plane[k].size = j;
    plane[k].color[0] = poly.r;
    plane[k].color[1] = poly.g;
    plane[k].color[2] = poly.b;
    plane[k].avg_depth = plane[k].avg_depth / j;
  }

  for (k = 0; k < poly.numpolys; k++) {
    total.plane[k + total.counter] = plane[k];
    // printf("%lf\n",  total->plane[k + total->counter].avg_depth);
  }
  total.counter += poly.numpolys;
  // printf("-----------------------\n");
  // printf("%d\n", total.counter);
}

//draws the all the planes based on average depth
void draw() {
  int i;
  qsort (total.plane, total.counter, sizeof(Plane), compare);
  for (i = 0; i < total.counter; i++) {
    G_rgb(total.plane[i].color[0], total.plane[i].color[1],
          total.plane[i].color[2]);
    G_fill_polygon(total.plane[i].x, total.plane[i].y,
                   total.plane[i].size);
    G_rgb(1, 1, 1);
    G_polygon(total.plane[i].x, total.plane[i].y,
              total.plane[i].size);
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
double scale_n_fit(Object* poly) {
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
int welcome(int i) {
  char q;
  if (i == 1) {
    printf("Press 1 to move the object\n");
    q = '1';
  } else if (i == 2) {
    printf("Press 1 or 2 to move the first or second object: ");
    scanf("%c", &q);
  } else {
    printf("Press 1 through %d to move that object: ", i);
    scanf("%c", &q);
  }
  return q - '0';
}

int main (int argc, char **argv)
{
  char q, action;
  double mat[4][4], minv[4][4], scaleFactor;
  int cc, sign, curObj, k, h;
  int increment = 20; int temp = 0;
  Object object[argc];



  for (cc = 1; cc < argc; cc++) {
    object[cc].file = fopen(argv[cc], "r"); //opens a file; r = read only
    if (object[cc].file == NULL) { //if the file is empty
      printf("can't open (1)\n");
      exit(1);
    } else {
      readobject(object[cc].file, &object[cc]);

      D3d_make_identity(mat); D3d_make_identity(minv);
      scaleFactor = scale_n_fit(&object[cc]);
      D3d_translate(mat, minv, -object[cc].centerx, -object[cc].centery,
                    -object[cc].centerz);
      D3d_scale(mat, minv, scaleFactor, scaleFactor, scaleFactor);
      D3d_mat_mult_points(object[cc].x, object[cc].y, object[cc].z,
                          mat, object[cc].x, object[cc].y,
                          object[cc].z, object[cc].points);
      object[cc].r = (double)(rand() % 100) / 100;
      object[cc].g = (double)(rand() % 100) / 100;
      object[cc].b = (double)(rand() % 100) / 100;
      temp += object[cc].numpolys;
    }
  }

  total.plane = malloc(temp  * sizeof(Plane));


  curObj = welcome(argc - 1);
  sign = 1 ;
  action = 't' ;


  if (curObj < argc && curObj > 0) {
    G_init_graphics(WIDTH, HEIGHT);
    object[curObj].xcounter, object[curObj].ycounter,
           object[curObj].zcounter = 0;

    while (1) {
      G_rgb(0, 0, 0);
      G_clear();
      total.counter = 0;

      for (cc = 1; cc < argc; cc++) {
        predraw(object[cc], cc);
        // printf("-----------------------\n");
      }
      draw(total);

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
        if (h != curObj) {
          curObj = k;
        }
      } else if ((q == 'x') && (action == 't')) {
        D3d_translate (mat, minv, sign * increment, 0, 0);
        object[curObj].xcounter = object[curObj].xcounter + (sign * increment);

      } else if ((q == 'y') && (action == 't')) {
        D3d_translate (mat, minv, 0, sign * increment, 0);
        object[curObj].ycounter = object[curObj].ycounter + (sign * increment);

      } else if ((q == 'z') && (action == 't')) {
        D3d_translate(mat, minv, 0, 0, sign * increment);
        object[curObj].zcounter = object[curObj].zcounter + (sign * increment);

      } else if ((q == 'x') && (action == 'r')) {
        D3d_translate(mat, minv, -object[curObj].xcounter, -object[curObj].ycounter, -object[curObj].zcounter);
        D3d_rotate_x(mat, minv, sign * radians);
        D3d_translate(mat, minv, object[curObj].xcounter, object[curObj].ycounter, object[curObj].zcounter);

      } else if ((q == 'y') && (action == 'r')) {
        D3d_translate(mat, minv, -object[curObj].xcounter, -object[curObj].ycounter, -object[curObj].zcounter);
        D3d_rotate_y(mat, minv, sign * radians);
        D3d_translate(mat, minv, object[curObj].xcounter, object[curObj].ycounter, object[curObj].zcounter);

      } else if ((q == 'z') && (action == 'r')) {
        D3d_translate(mat, minv, -object[curObj].xcounter, -object[curObj].ycounter, -object[curObj].zcounter);
        D3d_rotate_z(mat, minv, sign * radians);
        D3d_translate(mat, minv, object[curObj].xcounter, object[curObj].ycounter, object[curObj].zcounter);

      } else {
        printf("no action\n") ;
      }

      D3d_mat_mult_points(object[curObj].x, object[curObj].y,
                          object[curObj].z, mat,
                          object[curObj].x, object[curObj].y,
                          object[curObj].z,
                          object[curObj].points + 1) ;
      //the numpoints[curObj]+1 is because we have stored
      //the center of the object at the arrays' end
    }
  }
}

