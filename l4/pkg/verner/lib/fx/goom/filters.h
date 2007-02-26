#ifndef FILTERS_H
#define FILTERS_H

#include "config.h"

#include "graphic.h"
#include "math.h"

typedef struct
{
  int vitesse;			/* 128 = vitesse nule... * * 256 = en arriere 
				 * hyper vite.. * * 0 = en avant hype vite. */
  unsigned char pertedec;
  unsigned char sqrtperte;
  int middleX, middleY;		/* milieu de l'effet */
  char reverse;			/* inverse la vitesse */
  char mode;			/* type d'effet � appliquer (cf les #define)
				 * * * * * * @since June 2001 */
  int hPlaneEffect;		/* deviation horitontale */
  int vPlaneEffect;		/* deviation verticale */
	/** @since April 2002 */
  int waveEffect;		/* applique une "surcouche" de wave effect */
  int hypercosEffect;		/* applique une "surcouche de hypercos effect 
				 */
  char noisify;			/* ajoute un bruit a la transformation */
}
ZoomFilterData;


#define NORMAL_MODE 0
#define WAVE_MODE 1
#define CRYSTAL_BALL_MODE 2
#define SCRUNCH_MODE 3
#define AMULETTE_MODE 4
#define WATER_MODE 5
#define HYPERCOS1_MODE 6
#define HYPERCOS2_MODE 7
#define YONLY_MODE 8
#define SPEEDWAY_MODE 9

void pointFilter (guint32 * pix1, Color c,
		  float t1, float t2, float t3, float t4, guint32 cycle);

/* filtre de zoom :
 * le contenu de pix1 est copie dans pix2.
 * zf : si non NULL, configure l'effet.
 * resx,resy : taille des buffers.
 */

void zoomFilterFastRGB (guint32 * pix1,
			guint32 * pix2,
			ZoomFilterData * zf,
			guint32 resx, guint32 resy,
			int switchIncr, float switchMult);


void zoom_filter_free(void);

/* filtre sin :
 le contenu de pix1 est copie dans pix2, avec l'effet appliqu�
 cycle est la variable de temps.
 mode vaut SIN_MUL ou SIN_ADD
 rate est le pourcentage de l'effet appliqu�
 lenght : la longueur d'onde (1..10) [5]
 speed : la vitesse (1..100) [10]
*/
/*
void sinFilter(Uint *pix1,Uint *pix2,
			   Uint cycle,
			   Uint mode,
			   Uint rate,
			   char lenght,
			   Uint speed);
*/

#define SIN_MUL 1
#define SIN_ADD 2

//#ifdef USE_ASM
//void setAsmUse (int useIt);
//#endif

#endif