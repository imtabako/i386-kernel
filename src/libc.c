#include <kernel.h>
#include <libc.h>

size_t
strlen(const char *s)
{
	size_t n;

	for (n = 0; *s++ != '\0'; n++)
		;

	return n;
}

void *
memcpy(void *dst, const void *src, size_t len)
{
	unsigned char *p1, *p2, *end;

	p1 = (unsigned char *)dst;
	p2 = (unsigned char *)src;
	end = p1 + len;

	while (p1 != end)
		*p1++ = *p2++;

	return dst;
}

void *
memset(void *s, int c, size_t n)
{
	unsigned char *p, *end;

	if (n == 0)
		return s;

	p = s;
	end = p + n;

	while (p != end)
		*p++ = c;

	return s;
}

void
swap(void *a, void *b, size_t size)
{
	char c;

	while (size--) {
		c = *(char *)a;
		*(char *)a++ = *(char *)b;
		*(char *)b++ = c;

	}
}

void
qsort(void *base, size_t nmemb, size_t size, int (*cmp)(const void *, const void *))
{
	unsigned char *ptr;
	size_t i, j, idx;

	ptr = base;
	for (i = 0, j = nmemb-1; i < j;) {
		for (idx = i; idx < j; idx++) {
			if ((*cmp)(ptr + size*(idx+1), ptr + size*idx) < 0)
				swap(ptr + size*idx, ptr + size*(idx+1), size);
		}
		j--;

		for (idx = j; idx > i; idx--) {
			if ((*cmp)(ptr + size*(idx-1), ptr + size*idx) > 0)
				swap(ptr + size*(idx-1), ptr + size*idx, size);
		}
		i++;
	}
}

