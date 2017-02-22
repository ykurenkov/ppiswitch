#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <dev/ppbus/ppi.h>
#include <dev/ppbus/ppbconf.h>
#include <sys/types.h>
#include <machine/sysarch.h>
#include <machine/cpufunc.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sysexits.h>
#include <math.h>
#include <getopt.h>
#include <err.h>
extern void circuit(void);
static int ppi_fd;

static void do_init(char port[]) {
    ppi_fd = open(port, O_RDWR);

    if ( ppi_fd < 0 ) {
        perror(port);
        exit(1);
    }
}

static void do_out(unsigned long outval) {
    int val = outval, n;

    n = ioctl(ppi_fd, PPISDATA, &val);

    if ( n < 0 ) {
        perror("ioctl PPISDATA");
        exit(1);
    }
}

static unsigned long do_in(void) {
    int val, n;

    n = ioctl(ppi_fd, PPIGDATA, &val);

    if ( n < 0 ) {
        perror("ioctl PPIGDATA");
        exit(1);
    }

    //val = val - 3217025024;
    return val;
}

static unsigned long do_status(void) {
    int val, n;

    /* n = ioctl(ppi_fd, PPIGCTRL, &val); */
    n = ioctl(ppi_fd, PPIGSTATUS, &val);

    if ( n < 0 ) {
        /* perror("ioctl PPIGCTRL"); */
        perror("ioctl PPIGSTATUS");
        exit(1);
    }

    return val;
}

void usage() {

#define HELP_TARGET stdout;
    (void)fprintf(stderr, "Parallel port switcher. (c)2007 Yuri Kurenkov\n");
    (void)fprintf(stderr, "Usage:\n");
    (void)fprintf(stderr, "\tppiswitch [--device /dev/ppi0] --on <channels>\n");
    (void)fprintf(stderr, "\tppiswitch [--device /dev/ppi0] --off <channels>\n");
    (void)fprintf(stderr, "\tppiswitch [--device /dev/ppi0] [--time <sec>] --kick <channels>\n");
    (void)fprintf(stderr, "\tppiswitch [--device /dev/ppi0] [--time <sec>] --kick <channels> --on <channels> --off <channels> --status\n");
    (void)fprintf(stderr, "\tppiswitch [--device /dev/ppi0] --allon\n");
    (void)fprintf(stderr, "\tppiswitch [--device /dev/ppi0] --alloff\n");
    (void)fprintf(stderr, "\tppiswitch [--device /dev/ppi0] --quiet --status\n");
    (void)fprintf(stderr, "\tppiswitch [--device /dev/ppi0] --value <value>\n");
    (void)fprintf(stderr, "Options:\n");
    (void)fprintf(stderr, "\t--device device,-d device\t- parallel port device, default /dev/ppi0.\n");
    (void)fprintf(stderr, "\t--quiet,-q\t\t- supress output.\n");
    (void)fprintf(stderr, "\t--on <channels>,-i <channels>\t- turn on <channels>.\n");
    (void)fprintf(stderr, "\t--off <channels>,-o <channels>\t- turn off <channels>.\n");
    (void)fprintf(stderr, "\t--kick <channels>,-k <channels>\t- kick on/off or off/on <channel>\n\t\t\t  according current status.\n");
    (void)fprintf(stderr, "\t--time <sec>,-t <sec>\t- kick timeout in sec (1-86400), 1 sec default.\n");
    (void)fprintf(stderr, "\t--alloff,-r\t\t- turn off all channels.\n");
    (void)fprintf(stderr, "\t--allon,-s\t\t- turn on all channels.\n");
    (void)fprintf(stderr, "\t--status,-c\t\t- show turned on channels.\n");
    (void)fprintf(stderr, "\t--circuit,-e\t\t- print examples electrical circuits.\n");
    (void)fprintf(stderr, "\t--value <value>,\n\t-v <value>\t- set value at port.\n");
    (void)fprintf(stderr, "\t<value>\t- number from 0 to 255.\n");
    (void)fprintf(stderr, "\t<channels>\t- digits from 0 to 7 (ex. '1,3,4,6').\n");
    exit(EX_USAGE);
}


int main(int argc, char* argv[]) {
    FILE* dummy;
    int  ch;
    u_long result;
    u_long port_status;
    int param;
    int bit;
    int setbit;
    int need;
    int quiet=1;
    u_int time;
    int value;
    int tempresult;
    char *ep;
    u_long ultmp;
    u_long valtmp;
    char *port="/dev/ppi0";
    time = 1;

    if ( argc > 1 ) {

        static struct option longopts[] = {
            { "help",   no_argument,  NULL, 'h' },
            { "quiet",  no_argument,  NULL, 'q' }, 
            { "allon",  no_argument,  NULL, 's' }, 
            { "alloff", no_argument,  NULL, 'r' }, 
            { "status", no_argument,  NULL, 'c' }, 
            { "gstatus", no_argument,  NULL, 'g' }, 
            { "on",  required_argument, NULL, 'i' }, 
            { "off",  required_argument, NULL, 'o' }, 
            { "kick",  required_argument, NULL, 'k' }, 
            { "device", required_argument, NULL, 'd' }, 
            { "timeout", required_argument, NULL, 't' }, 
            { "value", required_argument, NULL, 'v' }, 
            { "circuit", no_argument, NULL, 'e' }, 
            { NULL,  0,   NULL, 0 }
        };

        /* getsubopt begin */
        char *tokens[] = {
            "0","1","2","3","4","5","6","7",
            NULL
        };

        char *options, *value;

        /* getsubopt end */

        while ( (ch=getopt_long(argc,argv,"qd:t:i:k:o:v:rscghe", longopts, NULL)) != EOF)
            switch (ch) {

                case 'q': {
                        quiet = 0;
                    }

                    break;

                case 'd': {
                        port = optarg;
                    }

                    break;

                case 't': {
                        ultmp = strtoul(optarg, &ep, 0);

                        if (*ep || ep == optarg || ultmp > 86400 || ultmp < 0 || ! ultmp)
                            errx(EX_USAGE, "invalid time: `%s'", optarg);

                        time = (u_int)ultmp;

                        if (quiet) printf("Timout set %d sec\n", time);
                    }

                    break;

                case 'i': {
                        options = optarg;

                        while (*options) {
                            bit=getsubopt(&options, tokens, &value);
                            do_init(port);
                            port_status = do_in();

                            tempresult = port_status;

                            need = 1;

                            while ( tempresult > 0 ) {
                                setbit = log10(tempresult)/log10(2);

                                if (bit == setbit ) {
                                    need = 0;
                                }

                                tempresult = tempresult-pow(2,setbit);
                            }

                            if ( need ) {
                                param = pow(2, bit);

                                if (quiet) printf ("Turn on channel %d\n", bit);

                                do_out(port_status + param);
                            } else {
                                if (quiet) printf("Channel %d already on\n", bit);
                            }
                        }
                    }

                    break;

                case 'v': {
                        valtmp = strtoul(optarg, &ep, 0);

                        if (*ep || ep == optarg || valtmp > 255 || valtmp < 0 )
                            errx(EX_USAGE, "invalid value: `%s'", optarg);

                        int value = (int) valtmp;

                        if (quiet) printf ("Set port value to:\n%d\n", value);

                        do_init(port);

                        do_out(value);
                    }

                    break;

                case 'o': {
                        options = optarg;

                        while (*options) {
                            bit=getsubopt(&options, tokens, &value);
                            do_init(port);
                            port_status = do_in();

                            tempresult = port_status;

                            need = 0;

                            while ( tempresult > 0 ) {
                                setbit = log10(tempresult)/log10(2);

                                if (bit == setbit ) {
                                    need = 1;
                                }

                                tempresult = tempresult-pow(2,setbit);
                            }

                            if ( need ) {
                                param = pow(2, bit);

                                if (quiet) printf ("Turn off channel %d\n", bit);

                                do_out(port_status - param);
                            } else {
                                if (quiet) printf("Channel %d already off\n", bit);
                            }
                        }
                    }

                    break;

                case 'k': {
                        options = optarg;

                        while (*options) {
                            bit=getsubopt(&options, tokens, &value);
                            do_init(port);
                            port_status = do_in();

                            tempresult = port_status;

                            need = 1;

                            while ( tempresult > 0 ) {
                                setbit = log10(tempresult)/log10(2);

                                if (bit == setbit ) {
                                    need = 0;
                                }

                                tempresult = tempresult-pow(2,setbit);
                            }

                            param = pow(2, bit);

                            if (!time) time=1;

                            if (quiet) printf ("Kick channel");

                            if ( need ) {
                                if (quiet) printf (" %d on/off\n", bit);

                                do_out(port_status + param);

                                sleep(time);

                                do_out(port_status);
                            } else {
                                if (quiet) printf (" %d off/on\n", bit);

                                do_out(port_status - param);

                                sleep(time);

                                do_out(port_status);
                            }
                        }
                    }

                    break;

                case 's': {
                        if (quiet) printf ("Turn on all channels\n");

                        do_init(port);

                        do_out(255);
                    }

                    break;

                case 'r': {
                        if (quiet) printf ("Turn off all channels\n");

                        do_init(port);

                        do_out(0);
                    }

                    break;

                case 'c': {
                        do_init(port);

                        result = do_in();

                        if (quiet) {
                            printf ("Show turned on channels:\n");

                            tempresult = result;

                            while ( tempresult > 0 ) {
                                bit = log10(tempresult)/log10(2);
                                printf ("%d ", bit);
                                tempresult = tempresult-pow(2,bit);
                            }
                        } else {
                            printf ("%d ", result);
                        }

                        printf ("\n");
                    }

                    break;
                case 'g': {
                        do_init(port);

                        result = do_status();

                        if (quiet) {
                            printf ("Show turned on status channels:\n");

                            tempresult = result;

                            while ( tempresult > 0 ) {
                                bit = log10(tempresult)/log10(2);
                                printf ("%d ", bit);
                                tempresult = tempresult-pow(2,bit);
                            }
                        } else {
                            printf ("%d ", result);
                        }

                        printf ("\n");
                    }

                    break;

                case 'e': {
                        circuit();
                    }
                    break;

                case 'h':

                default:  {
                        usage();
                    }

                    break;
            }

        return(0);
    } else {
        usage();
    }
}

