#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	char *a = (char *) dest;
	const char *b = (const char *) src;
	size_t i;

	for (i = 0; i < n; i++)
		a[i] = b[i];
}

void *memset(void *dest, int c, size_t n)
{
	char *a = (char *) dest;
	char b = (char) c;
	size_t i;

	for (i = 0; i < n; i++)
		a[i] = b;
}
