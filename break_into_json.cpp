#include <stack>
#include <iostream>
#include <cstring>
#include <cstdio>

using namespace std;

stack<int> si;
/*
	Why is this needed?
	json objects from server come like this:
		{"coord_y":1090,"coord_x":556,"mtype":"ACTION"}{"coord_y":1035,"coord_x":579,"mtype":"ACTION"}
	This function breaks this string into balanced json strings objects.
	Somehow btoutputstream.flush() is not working on Android.

	It assumes:
	1. max of 10 objects are concatenated in buf.
	2. length of json string per object is <= 512.
	   

*/
int break_into_json(const char *buf, char objects[][512]) 
{
	int i, j, k, l=0, len = strlen(buf);
	cout << strlen(buf) << endl;
	bool isnew = true;

	// fine
	for( i = 0; i < len; i++ ) {
		if ( buf[i] == '{' ) {
			si.push(i);
			if ( isnew ) j = i;
		} else if ( buf[i] == '}' ) {
			si.pop();
			if ( si.empty() ) {
				k = i;
				if ( l < 10 ){
					strncpy( objects[l++], &buf[j], k-j+1 );
				}
			}
		}
		isnew = si.empty();
	}

	return l;
}

int main()
{
	const char *rawjson = "{\"coord_y\":1045,\"coord_x\":579,\"mtype\":\"ACTION\"}\"coord_y\":867,\"coord_x\":741,\"mtype\":\"ACTION\"}{\"coord_y\":867,\"coord_x\":743,\"mtype\":\"ACTION\"}{\"coord_y\":867,\"coord_x\":744,\"mtype\":\"ACTION\"}{\"coord_y\":452,\"coord_x\":324,\"mtype\":\"ACTION\"}{\"coord_y\":474,\"coord_x\":689,\"mtype\":\"ACTION\"}";
	char objects[][512] = {'\0'};
	int c = break_into_json( rawjson, objects );
	printf("%d\n", c);
	for (int i = 0; i < c; ++i)
	{
		cout << objects[i] << endl;
	}
}