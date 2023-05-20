#include "client.hpp"
#include "xmlhandler.hpp"
#include "audioutil.hpp"
#include "rs232util.hpp"
#include "geoutil.hpp"

#define SERVER1 "streaming1.naad-adna.pelmorex.com" 
#define SERVER2 "streaming2.naad-adna.pelmorex.com" 
#define PORT "8080"
int main(int argc, char *argv[])
{
    if(argc < 5)
    {
        printf("USAGE:   ./capster <MODE> <PORT> <LATITUDE> <LONGITUDE>\n");
        printf("EXAMPLE: ./capster PROD 8080 42.384 -81.836\n");
        printf("EXAMPLE: ./capster TEST 8000 42.384 -81.836\n");
        return EXIT_FAILURE;
    }
    boost::asio::io_context ioContext;

    Client c1(ioContext);
    Client c2(ioContext);
    XmlHandler x1;
    AudioUtil a1;
    RS232Util r1;
    GeoUtil g1;
    
    // g1.set_station(43.3197,-81.7633);
    g1.set_station(atof(argv[3]), atof(argv[4]));
    x1.set_audioutil(&a1);
    x1.set_rs232util(&r1);
    x1.set_geoutil(&g1);
    c1.set_xmlhandler(&x1);
    c2.set_xmlhandler(&x1);
    if(strcmp("PROD",argv[1]) == 0)
    {
        c1.set_endpoint(SERVER1,argv[2]);
        c2.set_endpoint(SERVER2,argv[2]);
    }
    else if(strcmp("TEST",argv[1]) == 0)
    {
        c1.set_endpoint("0.0.0.0",argv[2]);
        c2.set_endpoint("127.0.0.1",argv[2]);
    }
    else
        {
            printf("Unknown startup configuration: %s\n",argv[1]);
            return EXIT_FAILURE;
        }
    c2.start();
    c1.start();
    std::thread t1([&c1,&ioContext](){
        ioContext.run();
    });    
    // std::thread t2([&c2,&ioContext](){
    //     ioContext.run();
    // });    
    t1.join();
    // t2.join();
    return 0;
}