#if !defined(AUDIOUTIL_HPP)
#define AUDIOUTIL_HPP

#define DEFAULT_FILESIZE 262144 // ~2MB
#include <stdio.h>
#include <string.h>
#include <util.h>
#include <memory>
#include <stdlib.h>
#include <curl/curl.h>
#include <out123.h>
#include <mpg123.h>
#include <stdio.h>

#include "decodetable.h"

class AudioUtil
{
public:
    AudioUtil(){}
    ~AudioUtil(){}
    void download_file(const char *url, const char *filename);
    void play_mpeg(const char *filename);
    void decode_and_write(const char *data, const char *filename);
private:
    void cleanup(mpg123_handle *mh, out123_handle *ao);

};

#endif // AUDIOUTIL_HPP
