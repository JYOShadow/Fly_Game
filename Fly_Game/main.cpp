#include "UPDATEMAP.h"

int main()
{
	UPDATEMAP fg = UPDATEMAP::UPDATEMAP(40,40);
	fg.Start();
	fg.~UPDATEMAP();
	return 0;
}