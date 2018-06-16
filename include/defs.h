#ifndef __DEFS_H__
#define __DEFS_H__

#ifndef	NULL
	#define	NULL	(void *)0
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#define SWAP(a, b)	\
do {			\
	typeof(a) c;	\
	c = a;		\
	a = b;		\
	b = c;		\
} while(0)

#define NELEMS(x)	(sizeof(x)/sizeof(x[0]))

#define MAX(a, b)	((a) > (b) ? (a) : (b))
#define MIN(a, b)	((a) < (b) ? (a) : (b))

#include <stdint.h>
typedef uint32_t size_t;

#endif	/* _DEFS_H_ */

