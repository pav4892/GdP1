#include <stdio.h>

int main() {
	
	char name[] = "Paul";
	int day = 17;
	char month[] = "01";
	int year = 2006;

	printf("Mein Name ist %s\n",name);
	printf("Mein Geburtsdatum ist der %02d.%2s.%04d\n",day,month,year);
	
	return 0;
}
