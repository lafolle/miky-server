#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char const *argv[])
{

	const char *name = "karan chaudhary";
	char firstname[6], lastname[10];

	strncpy(firstname, name, 5);
	firstname[5] = (char)0;

	strncpy(lastname, &name[6], 9);

	cout << firstname << endl;
	cout << lastname << endl;

	return 0;
}