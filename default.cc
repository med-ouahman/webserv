

#include <iostream>
using namespace std;
int __default( int a = 1, int b = 1 ) {
	b = 1;
	for ( int i = 0; i < 1; i++) {
		continue;
	}
	cout << "main\n";return 1;
}


int main()
{

	__default(1);
}
