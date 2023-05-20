#if !defined(RS232UTIL_HPP)
#define RS232UTIL_HPP

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h> //ioctl() call defenitions

#define PORT_TTY ("/dev/tty.usbserial-0001ED15")
#define PORT_CU ("/dev/cu.usbserial-0001ED15")

// Other header files

class RS232Util
{
public:
    RS232Util() : flagRTS_(TIOCM_RTS),flagDTR_(TIOCM_DTR){;}
    ~RS232Util(){close_pin();}
    void set_pin();
    void clear_pin();
private:
    void open_pin();
    void close_pin();
    int fd_;
    int lock_;
    int flagRTS_;
    int flagDTR_;
};

#endif // RS232UTIL_HPP
