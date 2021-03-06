#include <FPT.h>

int clickAndSave(double x[100], double y[100]){
  double ca[2];
  int i = 0;
  int going = 1;
  while (going == 1){
    G_wait_click(ca);
    // printf("%lf, %lf", ca[0], ca[1]);
    if (ca[0] < 100 && ca[1] < 100){
      going = 0;
      printf("done\n");
    }else{ 
      x[i] = ca[0];
      y[i] = ca[1];
      G_circle(x[i], y[i], 2);
      i++;
      // printf(" %d\n", i);
    }
  }
  return i;
}

void myPolygon(double x[100], double y[100], int n){
  int i = 0;
  while (i <= n){
    if (i != n){
      G_line(x[i], y[i], x[i+1], y[i+1]);
    }else {
      G_line(x[i], x[i], x[0], y[0]);
    } 
    i++;
  }

  }


int main()
{

  double ax[100], ay[100], bx[100], by[100];
  int anom, bnom;

  G_init_graphics(600,600);
  G_rgb(0,0,0);
  G_clear();
  G_rgb(1,1,1);

  anom = clickAndSave(ax,ay);
  // printf("%d\n",anom);
  myPolygon(ax,ay,anom);

  bnom = clickAndSave(ax, ay);
  myPolygon(ax,ay,anom);

  G_wait_key();

}
