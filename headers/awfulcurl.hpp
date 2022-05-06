#ifndef AWFUL_CURL_H
#define AWFUL_CURL_H

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <curl/curl.h>
#include <cstring>


size_t write_curled(char* ptr, size_t size, size_t nmemb, void* userdata)
{

	char* thing = new char[nmemb + 1];
	//memcpy(thing, ptr, nmemb);
	memmove(thing, ptr, nmemb);
	thing[nmemb] = '\0';
	
//	size_t placeholder = 0;
	std::ofstream output("trashdata.txt", std::ios::app);
/*
	while(placeholder < nmemb)
	{
		std::cout << ptr[placeholder];
		output << ptr[placeholder];
		placeholder++;
	}
	output << thing;
*/
	output.close();
//	std::cout << thing;
	
return nmemb;
}

void curlGET(std::string target, long* response_code)
{
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, target.c_str());

//#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who is not using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//#endif
 
//#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you are connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
//#endif


//**************  MY OPTIONS  **********************

	res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_curled);
 if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response_code);
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
}

//POST is not (necessarily) idempotent
void curlPOST(std::string target, std::string message)
{
  CURL *curl;
  CURLcode res;
/* get a curl handle */
  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
    curl_easy_setopt(curl, CURLOPT_URL, target.c_str());
    /* Now specify the POST data */
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=AngryOldMan&project=curl");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message.c_str());
 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
std::cout << '\n' << res << std::endl;
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* always cleanup */
    curl_easy_cleanup(curl);
 }
}

size_t curl_readed(char *buffer, size_t size, size_t nitems, void *userdata)
{
    return 0;
}

size_t curl_readed2(char *buffer, size_t size, size_t nitems, void *userdata)
{
	int max_chars = nitems * size;
	int count = 0;
	char tempchar;
	while(count < max_chars)
	{
		tempchar = static_cast<char>(static_cast<std::ifstream*>(userdata)->get());
		buffer[count] = tempchar;
		count++;
		std::cout << tempchar;
	}
	std::cout << std::endl;
return count;


}
size_t curl_readed3(char *buffer, size_t size, size_t nitems, void *userdata)
{
	int max_chars = nitems * size;
	int count = 0;
	char tempchar;

	std::string* filepoint = static_cast<std::string*>(userdata);
	std::string filepath = filepoint->data();
	std::ifstream input(filepath);
	while ((count < max_chars) && (!input.eof()))
	{
		tempchar = input.get();
		buffer[count] = tempchar;
		count++;
		std::cout << tempchar;	
	}
input.close();
return count;


}

//PUT is idempotent (in theory)


int curlPUT(std::string target, std::string filepath)
{
    int response_code = 500;
  CURL *curl;
  CURLcode res;
 
  /* get a curl handle */
  curl = curl_easy_init();
  if(curl) {
   
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);


    /* we want to use our own read function */
   curl_easy_setopt(curl, CURLOPT_READFUNCTION, curl_readed);


    /* enable uploading (implies PUT over HTTP) */
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
 
    /* specify target URL, and note that this URL should include a file
       name, not only a directory */
    
    curl_easy_setopt(curl, CURLOPT_URL, target.c_str());
 
    /* now specify which file to upload */
	std::ifstream input(filepath);
	input.seekg(0, input.end);
	int inputsize = input.tellg();
	input.clear();
	input.seekg(0, input.beg);
   // curl_easy_setopt(curl, CURLOPT_READDATA, &input);
	 curl_easy_setopt(curl, CURLOPT_READDATA, filepath.c_str());
    /* provide the size of the upload, we specicially typecast the value
       to curl_off_t since we must be sure to use the correct data size */
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)inputsize);
 
    /* Now run off and do what you have been told! */
    res = curl_easy_perform(curl);      //"Perform the request, res will get the return code" ?!?!?
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    response_code = static_cast<int>(res);  //there's no way this actually works
    /* always cleanup */
    curl_easy_cleanup(curl);
    input.close();
  }
  return response_code;
}

void curlPUT(std::string target, std::string filepath, int& response_code)
{
    response_code = curlPUT(target, filepath);
}

/*
int DoACurl(std::string type, std::string target_url, std::string message = "")
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
  //std::string target_url = "https://76.121.52.252:8080/content";
if(type == "GET")
{
  curlGET(target_url);
}
else if(type == "POST")
{
  curlPOST(target_url, message);
 }
else if(type == "PUT")
{
	curlPUT(target_url, message);
}

  curl_global_cleanup();
 
  return 0;
}
*/
#endif