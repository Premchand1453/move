#ifndef __INTERSECT_TRIANGLE_H__
#define __INTERSECT_TRIANGLE_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int
intersect_triangle(double orig[3], double dir[3],
                   double vert0[3], double vert1[3], double vert2[3],
                   double *t, double *u, double *v);

#ifdef __cplusplus
}
#endif

#endif /* !__INTERSECT_TRIANGLE_H__ */
