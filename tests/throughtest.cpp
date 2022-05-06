#include "../Headers/awfulcurl.hpp"
#include "../Headers/time.hpp"

void curlspam(std::string target, std::string filepath, bool& quit)
{
	//find a way to get the response code
	//with normal curl, I would use the -w option, then some filtering or something
	while (!quit)
	{
		curlPUT(target_url, message);
	}
}

//the goal here is to keep increasing the number of threads attempting to upload a file until the server is no longer able to keep up
//comparing the concurrently uploading thread count to the time taken to complete one request and the size of the source file should allow calculation of data throughput of the server.
//repeating the test with different size source files would be worthwhile, and would validate the conclusion if the calculated througputs were very similar for multiple different test file sizes
void throughputtest()
{
	std::string target_url = "https://76.121.52.252:8080/content";
	std::string message = "testdata.txt";
	int testdata_size = 10;	//MB
	int strikes = 0;
	std::vector<std::thread> threads;
	//std::time_t start_time = GetTime_t();
	int elapsed_time;

	bool quit = false;

	while (strikes < 3)	//keep adding threads spamming PUT requests until the test request fails three times in a row. might want a different failure indication, but this is what I have for now
	{
		time_machine.Start();
		if (curlPUT(target_url, message) != 200)	//this *might* work
		{
			strikes++;
		}
		else
		{
			elapsed_time = time_machine::Read();
			strikes = 0;
			threads.push_back(std::thread(&curlspam, target_url, message, quit));
		}
	}

	quit = true;	//this will allow the threads looping in curlspam to stop looping and reach the end of the function

	for (auto i = threads.begin(); i != threads.end(); i++)
	{
		threads[i].join();
	}

	elapsed_time = end_time - start_time;	//elapsed time = time required to upload the file(I hope)	//elapsed_time is in milliseconds
	float throughput = (static_cast<float>(testdata_size) / static_cast<float>(elapsed_time)) * static_cast<float>(threads.size());	//result in MB/ms
	std::cout << "approximate data throughput of " << throughput << " MB/ms" << std::endl;
}
int main()
{
	throughputtest();
	return 0;
}

