#include "nanbox_shortstring.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
	printf("Enter short strings of up to 6 chars to dump, q to quit.\n");
	while (1) {
		char buf[7];
		size_t len;
		printf("Short string --> ");
		if (scanf("%6s", buf) == EOF)
			break;
		if (!strcmp(buf, "q"))
			break;
		len = strlen(buf);
		nanbox_t val = nanbox_shortstring_create(buf, len);
		printf("%p \"%.6s\" (length %d)\n",
		       (void*)val.as_int64,
		       nanbox_shortstring_chars(&val),
		       nanbox_shortstring_length(val));
	}
	printf("\n");
	return 0;
}
