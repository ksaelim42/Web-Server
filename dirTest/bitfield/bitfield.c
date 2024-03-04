#include <stdio.h>
#include <stdint.h>

uint64_t	arrBit;

#define SET_BIT(BF, N) BF |= ((uint64_t)1 << N)
#define CLR_BIT(BF, N) BF &= ~((uint64_t)1 << N)
#define IS_BIT_SET(BF, N) ((BF >> N) & 1)

int	main(void) {
	printf("sizeof uint64_t: %lu\n", sizeof(arrBit));
	SET_BIT(arrBit, 2);
	SET_BIT(arrBit, 7);
	SET_BIT(arrBit, 32);
	SET_BIT(arrBit, 1);

	CLR_BIT(arrBit, 32);
	for (int i = 63; i >= 0; i--) {
		if (IS_BIT_SET(arrBit, i))
			printf("1");
		else
			printf("0");
	}
	printf("\n");

	printf("value: %lu\n", arrBit);
	
}