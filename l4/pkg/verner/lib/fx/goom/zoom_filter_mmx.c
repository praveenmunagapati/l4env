#define BUFFPOINTNB 16
#define BUFFPOINTMASK 0xffff
#define BUFFINCR 0xff

#include "mmx.h"

#define sqrtperte 16
// faire : a % sqrtperte <=> a & pertemask
#define PERTEMASK 0xf
// faire : a / sqrtperte <=> a >> PERTEDEC
#define PERTEDEC 4


int zoom_filter_mmx_supported (void);
void zoom_filter_mmx (int prevX, int prevY,
		      unsigned int *expix1, unsigned int *expix2,
		      int *brutS, int *brutD, int buffratio,
		      int precalCoef[16][16]);



int
zoom_filter_mmx_supported (void)
{
  return (mm_support () & 0x1);
}



void
zoom_filter_mmx (int prevX, int prevY,
		 unsigned int *expix1, unsigned int *expix2,
		 int *brutS, int *brutD, int buffratio,
		 int precalCoef[16][16])
{
  unsigned int ax = (prevX - 1) << PERTEDEC, ay = (prevY - 1) << PERTEDEC;

  int bufsize = prevX * prevY;
  int loop;

  pxor_r2r (mm7, mm7);

  for (loop = 0; loop < bufsize; loop++)
  {
    int px, py;
    int pos;
    int coeffs;

    int posplusprevX;

    int myPos = loop << 1, myPos2 = myPos + 1;
    int brutSmypos = brutS[myPos];

    px =
      brutSmypos + (((brutD[myPos] - brutSmypos) * buffratio) >> BUFFPOINTNB);
    brutSmypos = brutS[myPos2];
    py =
      brutSmypos +
      (((brutD[myPos2] - brutSmypos) * buffratio) >> BUFFPOINTNB);

    if ((py >= (int) ay) || (px >= (int) ax))
    {
      pos = coeffs = 0;
    }
    else
    {
      pos = ((px >> PERTEDEC) + prevX * (py >> PERTEDEC));
      /* coef en modulo 15 */
      coeffs = precalCoef[px & PERTEMASK][py & PERTEMASK];
    }

    posplusprevX = pos + prevX;

    movd_m2r (coeffs, mm6);
    ;				/* recuperation des deux premiers pixels dans mm0 et mm1 */
    movq_m2r (expix1[pos], mm0);	/* b1-v1-r1-a1-b2-v2-r2-a2 */
    movq_r2r (mm0, mm1);	/* b1-v1-r1-a1-b2-v2-r2-a2 */
    ;				/* depackage du premier pixel */
    punpcklbw_r2r (mm7, mm0);	/* 00-b2-00-v2-00-r2-00-a2 */
    movq_r2r (mm6, mm5);	/* xx-xx-xx-xx-c4-c3-c2-c1 */
    ;				/* depackage du 2ieme pixel */
    punpckhbw_r2r (mm7, mm1);	/* 00-b1-00-v1-00-r1-00-a1 */
    ;				/* extraction des coefficients... */
    punpcklbw_r2r (mm5, mm6);	/* c4-c4-c3-c3-c2-c2-c1-c1 */
    movq_r2r (mm6, mm4);	/* c4-c4-c3-c3-c2-c2-c1-c1 */
    movq_r2r (mm6, mm5);	/* c4-c4-c3-c3-c2-c2-c1-c1 */
    punpcklbw_r2r (mm5, mm6);	/* c2-c2-c2-c2-c1-c1-c1-c1 */
    punpckhbw_r2r (mm5, mm4);	/* c4-c4-c4-c4-c3-c3-c3-c3 */

    movq_r2r (mm6, mm3);	/* c2-c2-c2-c2-c1-c1-c1-c1 */
    punpcklbw_r2r (mm7, mm6);	/* 00-c1-00-c1-00-c1-00-c1 */
    punpckhbw_r2r (mm7, mm3);	/* 00-c2-00-c2-00-c2-00-c2 */
    ;				/* multiplication des pixels par les coefficients */
    pmullw_r2r (mm6, mm0);	/* c1*b2-c1*v2-c1*r2-c1*a2 */
    pmullw_r2r (mm3, mm1);	/* c2*b1-c2*v1-c2*r1-c2*a1 */
    paddw_r2r (mm1, mm0);

    ;				/* ...extraction des 2 derniers coefficients */
    movq_r2r (mm4, mm5);	/* c4-c4-c4-c4-c3-c3-c3-c3 */
    punpcklbw_r2r (mm7, mm4);	/* 00-c3-00-c3-00-c3-00-c3 */
    punpckhbw_r2r (mm7, mm5);	/* 00-c4-00-c4-00-c4-00-c4 */

    /* ajouter la longueur de ligne a esi */
    ;				/* recuperation des 2 derniers pixels */
    movq_m2r (expix1[posplusprevX], mm1);
    movq_r2r (mm1, mm2);

    ;				/* depackage des pixels */
    punpcklbw_r2r (mm7, mm1);
    punpckhbw_r2r (mm7, mm2);

    ;				/* multiplication pas les coeffs */
    pmullw_r2r (mm4, mm1);
    pmullw_r2r (mm5, mm2);

    ;				/* ajout des valeurs obtenues %/1��iso8859-15� la valeur finale */
    paddw_r2r (mm1, mm0);
    paddw_r2r (mm2, mm0);

    ;				/* division par 256 = 16+16+16+16, puis repackage du pixel final */
    psrlw_i2r (8, mm0);
    packuswb_r2r (mm7, mm0);
    movd_r2m (mm0, expix2[loop]);
  }
  emms ();
}
