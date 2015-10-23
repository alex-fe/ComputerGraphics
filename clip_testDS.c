#include <FPT.h>

// Bounding Box For Line Segment
//(x1,y1) = start of the line
//(x2, y2) = the end of the line
int pointincontainer(double x1, double y1, double x2, double y2,
			 double xprime, double yprime){
    double left, top, right, bottom; 
    if(x1 < x2){
        left = x1;
        right = x2;
    } else {
        left = x2;
        right = x1;
    }
    if(y1 > y2){
        top = y1;
        bottom = y2;
    } else {
        top = y2;
        bottom = y1;
    }
    //printf("l:%lf, r:%lf, t:%lf, b:%lf\n", left, right, top, bottom);
    //printf("x': %lf, y': %lf\n", xprime, yprime);
    if ((xprime+0.01) >= left && (xprime-0.01) <= right && 
	(yprime+0.01) <= top && (yprime-0.01) >= bottom) {
        return 1;
    } else {
        return 0;
    }
}


//gets the slope of all the lines; returns them via modified array
void getslope(double *x, double *y, int z, double *slope){
  int i;
  for (i = 0; i < z; i++){
    slope[i] = (y[i+1] - y[i]) / (x[i + 1] - x[i]);
  }
  slope[i] = (y[0] - y[i]) / (x[0] - x[i]);
}

int looper(int i, int n){
  if (i == n - 1){
    return 0;
  } else {
    return i + 1;
  }
}

//finds the intersection of the two lines
void f_intersect(double *px, double *py, int pn,
		double *wx, double *wy, int wn, double *c){
  int i, j;
  double yprime, xprime, wslope[100], pslope[100], c1, c2;
  getslope(px, py, pn, pslope);
  getslope(wx, wy, wn, wslope);

  for (i = 0; i < pn; i++){
    for(j = 0; j < wn; j++){
      c1 = py[i] - pslope[i] * px[i]; //y = mx + c -> c = y -mx
      c2 = wy[j] - wslope[j] * wx[j];

      if ((pslope[i] - wslope[j]) == 0){ //prevents divison by 0
	printf("Slope division = 0; ERROR!\n"); 
      } else {
	xprime = (c2 - c1) / (pslope[i] - wslope[j]); //intersection of x
	yprime = pslope[i] * xprime + c1; //intersection of y


	
	if(pointincontainer(px[i], py[i], px[looper(i, pn)], py[looper(i, pn)],
			    xprime, yprime) == 1 &&
	   pointincontainer(wx[j], wy[j],wx[looper(j, wn)], wy[looper(j, wn)],
			    xprime, yprime ) == 1){

	  printf("\nPolygon:\nstart: (%.2lf,%.2lf), end: (%.2lf,%.2lf), slope:%.2lf\n",px[i], py[i], px[looper(i, pn)], py[looper(i, pn)], pslope[i]);
	  printf("Window:\nstart: (%.2lf,%.2lf), end: (%.2lf,%.2lf), slope:%.2lf\n\n",wx[j], wy[j], wx[looper(j, wn)], wy[looper(i, wn)], wslope[j]);
	  c[0] = xprime; c[1] = yprime;
	  G_rgb(1,1,1);
	  G_fill_circle(xprime, yprime, 2);
	}
      }
    }
  }
}


int Clip_Polygon_Against_Convex_Window(double *px, double *py, int pn,
				       double *wx, double *wy, int wn){
  int j, i;
  double cd[100];

  f_intersect(px, py, pn, wx, wy, wn, cd);

  return 1;
}


int main()
// this tests clipping of polygon to convex window
//prefix -p == polygon; prefix -w == window
{
  int pn, wn ;
  double pt[2],u,v,q ;

  double px[100] = {  70,460,400} ;
  double py[100] = { 350, 25,550} ;
  pn = 3 ;

  double wx[100] = { 100,600,550,150} ;
  double wy[100] = { 150,200,450,500} ;
  wn = 4 ;

  srand48(100) ;

  G_init_graphics (700, 700) ;
  G_rgb (0,0,0) ;
  G_clear() ;

  G_rgb (1,0,0) ;
  G_polygon(wx,wy,wn) ;

  G_rgb (0,0,1) ;
  G_polygon(px,py,pn) ;


  q = G_wait_key() ;


  pn = Clip_Polygon_Against_Convex_Window (px, py, pn, wx, wy, wn) ;

  G_rgb (1,1,0) ;
  G_fill_polygon(px,py,pn) ;
  q = G_wait_key() ;
}




