#include "audioutil.hpp"

void AudioUtil::play_mpeg(const char *filename)
{
    printf("OPENING FILE %s\n",filename);
    int err = MPG123_OK;
    mpg123_handle *mh = mpg123_new(NULL,&err);
    out123_handle *ao = out123_new();
    unsigned char *buffer = NULL;
    const char *encName;
    size_t bufferSize = 0;
    size_t done = 0;
    int frameSize = 1;
    long rate = 0;
    off_t samples = 0;

    mpg123_open(mh,filename);

    int channels = 0;
    int encoding = 0;
    mpg123_getformat(mh,&rate,&channels,&encoding);
    
    char *driver = NULL;
    char *outfile = NULL;
    out123_open(ao,driver,outfile);
    out123_driver_info(ao,&driver,&outfile);
    mpg123_format_none(mh);
    mpg123_format2(mh,rate,channels,encoding);
    encName = out123_enc_name(encoding);

    printf("Playing with %i channels and %li Hz, encoding %s.\n", channels, rate, encName ? encName : "???");

    out123_start(ao,rate,channels,encoding);
    out123_getformat(ao,NULL,NULL,NULL,&frameSize);

    bufferSize = mpg123_outblock(mh);
    buffer = (unsigned char*)malloc(bufferSize);

    do
    {
        size_t played;
        err = mpg123_read(mh,buffer,bufferSize,&done);
        played = out123_play(ao,buffer,done);
        if(played != done)
            fprintf(stderr, "Warning: written less than gotten from libmpg123: %li != %li\n", (long)played, (long)done);
        samples += played / frameSize;
    } while (done && err == MPG123_OK);
    
    free(buffer);    

    if(err != MPG123_DONE)
        fprintf(stderr, "Warning: Decoding ended prematurely because: %s\n",
                err == MPG123_ERR ? mpg123_strerror(mh) : mpg123_plain_strerror(err));
    printf("%li samples written.\n", (long)samples);
    cleanup(mh, ao);
}


void AudioUtil::cleanup(mpg123_handle *mh, out123_handle *ao)
{
    out123_del(ao);
    /* It's really to late for error checks here;-) */
    mpg123_close(mh);
    mpg123_delete(mh);
}

void AudioUtil::decode_and_write(const char *data, const char *filename)
{
    const unsigned char *input = (const unsigned char *)data;
    int len = strlen (data);
    size_t outputSize = len*3/4;
    printf("len = %lu outputSize = %lu\n",len,outputSize);
    char *out = (char*)malloc(outputSize + 5);
    char *result = out;
    signed char vals[4];

    while(len > 0) 
    {
        if(len < 4)
        {
            free(result);
            printf("INCALD");
            return; /* Invalid Base64 data */
        }

        vals[0] = base64decode[*input++];
        vals[1] = base64decode[*input++];
        vals[2] = base64decode[*input++];
        vals[3] = base64decode[*input++];

        if(vals[0] < 0 || vals[1] < 0 || vals[2] < -1 || vals[3] < -1) {
            len -= 4;
            continue;
        }

        *out++ = vals[0]<<2 | vals[1]>>4;
        /* vals[3] and (if that is) vals[2] can be '=' as padding, which is
           looked up in the base64decode table as '-1'. Check for this case,
           and output zero-terminators instead of characters if we've got
           padding. */
        if(vals[2] >= 0)
            *out++ = ((vals[1]&0x0F)<<4) | (vals[2]>>2);
        else
            *out++ = 0;

        if(vals[3] >= 0)
            *out++ = ((vals[2]&0x03)<<6) | (vals[3]);
        else
            *out++ = 0;

        len -= 4;
    }
    // printf("out = %s\n",out);
    FILE *fp = fopen(filename,"wb");
    fwrite(result,1,outputSize,fp);
    fclose(fp);
    free(result);
}


void AudioUtil::download_file(const char *url, const char *filename)
{

  CURL *curl;
  CURLcode res;
  FILE *fp;

  curl = curl_easy_init();
  if (curl) {
    fp = fopen(filename,"wb");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    res = curl_easy_perform(curl);

    /* always cleanup */
    curl_easy_cleanup(curl);

    fclose(fp);
    if (res == CURLE_OK)
      printf("File downloaded successfully\n");
    else
      printf("Error downloading file: %s\n", curl_easy_strerror(res));
  }
}
