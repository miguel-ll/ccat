#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#define uint uint32_t

extern char *optarg;

static uint color_count = 50;		// N hues
static uint start_count = 0;		// Starting hue
static uint shift_c = 1;

static uint hue_to_ansiNum(double hue);
static void color(uint color);
static void help(int code);
static double dmod(double x, double y);

static void init_rand();
static long long time_millis();
static FILE *Fdat;

static long long time_millis()
{
 	struct timeval te;
 	gettimeofday(&te, NULL);
 	long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
 	return milliseconds;
}

static void init_rand() {
 	Fdat = fopen("/dev/urandom", "r");
 	srand(time_millis(NULL));
}

double dmod(double x, double y) {
 	return x - (int)(x / y) * y;
}

static void strip_ansi(char * str) {
 	const size_t len = strlen(str);
 	char *ptr = str;
 	char *ptr2;
 	while(1) {
 		ptr = strchr(ptr,'\033');
 		if (!ptr)
 			return;
 		ptr2 = strchr(ptr,'m');
 		char chk[] ="hldABCDEFGHJKSTfinsu0123456789";
 		for (size_t i = 0; (!ptr2 || (ptr2-ptr) > 10) && i < ((sizeof chk) - 1); i++)
 		    ptr2 = strchr(ptr,chk[i]);
 		if (!ptr2 || (ptr2-ptr) > 10) {
 			ptr++;
 			continue;
 		}
 		memcpy(ptr, ptr2 + 1, len - (ptr2 - str));
 	}
}

static uint hue_to_ansiNum(double hue) {
 	const double sat = 1.0;
 	const double gam = 0.75;
 	double p, q, t, ff;
 	double r, g, b;
 	long i;
 	hue = dmod(hue, 360.0);
 	hue /= 60.0;
 	i = (long)hue;
 	ff = hue - i;
 	p = gam * (1.0 - sat);
 	q = gam * (1.0 - (sat * ff));
 	t = gam * (1.0 - (sat * (1.0 - ff)));
 	switch (i) {
 	case 0:
 		r = gam;
 		g = t;
 		b = p;
 		break;
 	case 1:
 		r = q;
 		g = gam;
 		b = p;
 		break;
 	case 2:
 		r = p;
 		g = gam;
 		b = t;
 		break;
 	case 3:
 		r = p;
 		g = q;
 		b = gam;
 		break;
 	case 4:
 		r = t;
 		g = p;
 		b = gam;
 		break;
 	case 5:
 	default:
 		r = gam;
 		g = p;
 		b = q;
 		break;
 	}
 	r *= 6;
 	g *= 6;
 	b *= 6;
 	return 16 + (36 * (int)r) + (6 * (int)g) + (int)b;
}

static void color(uint color) {
 		printf("\033[38;5;%um", (hue_to_ansiNum(((color * 360) / color_count) + start_count)));
}

static void help(int code) {
 	puts("| -h - This message.                     |");
 	puts("| -s - Starting color in hue 360*.       |");
 	exit(code);
}

int main(int argc, char **argv) {
 	FILE *fp = stdin;
 	int opt;
 	while ((opt = getopt(argc, argv, "h:s:")) != -1)
 	{
 		switch (opt) {
 		case 'h':
 			help(0); break;
 		case 's':
 			start_count = strtol(optarg, NULL, 10);
 			break;
 		default:
 			help(EXIT_FAILURE);
 		}
 	}
 	if (*argv[argc - 1] != '-' && argc > 1) {
 		char var_opts[] = "asA";
 		int file = 1;
 		for (size_t i = 0; i < sizeof(var_opts) - 1; i++) {
 			char opts[] = "-a";
 			opts[1] = var_opts[i];
 			if (!strcmp(argv[argc - 2], opts))
 				file = 0;
 		}
 		if (file) {
 			fp = fopen(argv[argc - 1], "r");
 			if (!fp) {
 				fprintf(stderr, "Error opening file '%s'\n", argv[argc - 1]);
 				return EXIT_FAILURE;
 			}
 		}
 	}
 	init_rand();
 	char *line = NULL;
 	size_t len = 0;
 	ssize_t lineSize = 0;
 	int curent_color = 0;
 	while ((lineSize = getline(&line, &len, fp)) != -1) {
 		strip_ansi(line);
 		color(curent_color);
		curent_color++;
 		if (curent_color >= color_count)
 			curent_color = 0;

 		printf("%s", line);
 		curent_color += shift_c;
 		if (curent_color <= 0)
 			curent_color = color_count - 1;
 		if (curent_color >= color_count)
 			curent_color = 0;
 	}
 	free(line);
 	fclose(Fdat);
 	return 0;
}

