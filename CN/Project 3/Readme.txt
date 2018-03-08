**************************************************************************************************************
									Project 3 : CS528 / Computer Network 
**************************************************************************************************************
==4369== Conditional jump or move depends on uninitialised value(s)
==4369==    at 0x401F94: getContentLength(char**, int) (http_proxy_server.cpp:171)
==4369==    by 0x402C56: callback_method(void*) (http_proxy_server.cpp:460)
==4369==    by 0x4E416B9: start_thread (pthread_create.c:333)
==4369==    by 0x59FF3DC: clone (clone.S:109)
==4369== 
**************************************************************************************************************

implementation details:
	client server model between proxy and actual client , another client server model in proxy and actual server.
	cache implemeted as array of structure , structure contains following elements .....

			struct cache_object
			{
				vector<string> my_req;
				vector<int> track_char;
				vector<string> my_resp;
				int contentlength;
				int size;
			};
	before connecting to actual server first cache is verified if data for respective web page is already cached in , it will be 
	servered form cache only , this increases performance.

Sample output :

remote00:~/CN/Project3> make
g++ -lpthread -g -o proxy http_proxy_server.cpp -lm
./proxy
admin: started proxy server on 'remote00.cs.binghamton.edu' at '45454'
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/cs528.html | CACHE_MISS | 2131 | 6
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/cs528.html | CACHE_HIT | 2131 | 0
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/cs528.html | CACHE_HIT | 2131 | 0
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/cs528.html | CACHE_HIT | 2131 | 0
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/cs528.html | CACHE_HIT | 2131 | 0
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/cs528.html | CACHE_HIT | 2131 | 0
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/course.css | CACHE_MISS | 1841 | 5
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/syllabus.pdf | CACHE_MISS | 106317 | 15018
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/lena_std.tif | CACHE_MISS | 786572 | 15024
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/course.css | CACHE_HIT | 1841 | 0
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/course.css | CACHE_HIT | 1841 | 0
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/course.css | CACHE_HIT | 1841 | 0
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/course.css | CACHE_HIT | 1841 | 0
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/course.css | CACHE_HIT | 1841 | 0
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/skype-ubuntu-precise_4.3.0.37-1_i386.deb | CACHE_MISS | 20112698 | 15166
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/syllabus.pdf | CACHE_HIT | 106317 | 2
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/syllabus.pdf | CACHE_HIT | 106317 | 2
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/syllabus.pdf | CACHE_HIT | 106317 | 2
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/syllabus.pdf | CACHE_HIT | 106317 | 2
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/syllabus.pdf | CACHE_HIT | 106317 | 2
128.226.180.162 |  http://www.binghamton.edu:8080/asf-logo.png | CACHE_MISS | 17811 | 20043
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/lena_std.tif | CACHE_HIT | 786572 | 18
128.226.180.162 |  http://www.binghamton.edu:8080/docs/setup.html | CACHE_MISS | 12880 | 20994
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/lena_std.tif | CACHE_HIT | 786572 | 17
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/lena_std.tif | CACHE_HIT | 786572 | 19
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/lena_std.tif | CACHE_HIT | 786572 | 17
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/lena_std.tif | CACHE_HIT | 786572 | 17
128.226.180.162 |  http://portquiz.net:10000/portquizm.png | CACHE_MISS | 2450 | 273
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/skype-ubuntu-precise_4.3.0.37-1_i386.deb | CACHE_HIT | 20112698 | 536
128.226.180.162 |  http://httpbin.org/html | CACHE_MISS | 3741 | 139
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/skype-ubuntu-precise_4.3.0.37-1_i386.deb | CACHE_HIT | 20112698 | 532
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/skype-ubuntu-precise_4.3.0.37-1_i386.deb | CACHE_HIT | 20112698 | 531
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/skype-ubuntu-precise_4.3.0.37-1_i386.deb | CACHE_HIT | 20112698 | 532
128.226.180.162 |  http://www.binghamton.edu:8080/asf-logo.png | CACHE_HIT | 17811 | 0
128.226.180.162 |  http://www.cs.binghamton.edu/~yaoliu/courses/cs528/skype-ubuntu-precise_4.3.0.37-1_i386.deb | CACHE_HIT | 20112698 | 532
128.226.180.162 |  http://httpbin.org/image/png | CACHE_MISS | 8090 | 101
128.226.180.162 |  http://www.binghamton.edu:8080/asf-logo.png | CACHE_HIT | 17811 | 0
128.226.180.162 |  http://www.binghamton.edu:8080/docs/setup.html | CACHE_HIT | 12880 | 0
128.226.180.162 |  http://www.binghamton.edu:8080/docs/setup.html | CACHE_HIT | 12880 | 0
128.226.180.162 |  http://portquiz.net:10000/portquizm.png | CACHE_HIT | 2450 | 0
128.226.180.162 |  http://portquiz.net:10000/portquizm.png | CACHE_HIT | 2450 | 0
128.226.180.162 |  http://httpbin.org/html | CACHE_HIT | 3741 | 0
128.226.180.162 |  http://httpbin.org/html | CACHE_HIT | 3741 | 0
128.226.180.162 |  http://httpbin.org/image/png | CACHE_HIT | 8090 | 1
128.226.180.162 |  http://httpbin.org/image/png | CACHE_HIT | 8090 | 1
128.226.180.162 |  http://httpbin.org/image/jpeg | CACHE_MISS | 35588 | 65140
128.226.180.162 |  http://httpbin.org/image/jpeg | CACHE_HIT | 35588 | 3
128.226.180.162 |  http://httpbin.org/image/jpeg | CACHE_HIT | 35588 | 3

