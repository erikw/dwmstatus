/* made by profil 2011-12-29.
 *
 * Compile with:
 * gcc -Wall -pedantic -std=c99 -lX11 status.c
 */
#define _BSD_SOURCE	/* To get loadavg function form stdlib.h. */
#include <stdlib.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include <X11/Xlib.h>

static Display *dpy;

static void set_status(char *str)
{
	XStoreName(dpy, DefaultRootWindow(dpy), str);
	XSync(dpy, False);
}

char * smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	ret = malloc(++len);
	if (ret == NULL) {
		perror("malloc");
		exit(1);
	}

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}

static float get_freq(char *file)
{
	FILE *fd;
	char *freq;
	float ret;

	freq = malloc(10);
	fd = fopen(file, "r");
	if(fd == NULL) {
		fprintf(stderr, "Cannot open '%s' for reading.\n", file);
		exit(EXIT_FAILURE);
	}

	fgets(freq, 10, fd);
	fclose(fd);

	ret = atof(freq)/1000000;
	free(freq);
	return ret;
}

#define DATE_MAX_LEN (65)
static char *datetime_buf;
static void get_datetime()
{
	time_t result;
	struct tm *resulttm;

	result = time(NULL);
	resulttm = localtime(&result);
	if (resulttm == NULL) {
		fprintf(stderr, "Error getting localtime.\n");
		exit(EXIT_FAILURE);
	}
	/*if (!strftime(datetime_buf, (DATE_MAX_LEN - 1), "%a %F %H:%M:%S", resulttm)) {*/	// 24 hour.
	if (!strftime(datetime_buf, (DATE_MAX_LEN - 1), "%a %F %I:%M:%S %p", resulttm)) {	// 12 hour.
		fprintf(stderr, "strftime is 0.\n");
		exit(EXIT_FAILURE);
	}
}

static int get_battery()
{
	FILE *fd;
	int energy_now, energy_full, voltage_now;

	fd = fopen("/sys/class/power_supply/BAT0/energy_now", "r");
	if (fd == NULL) {
		fprintf(stderr, "Error opening energy_now.\n");
		return -1;
	}
	fscanf(fd, "%d", &energy_now);
	fclose(fd);


	fd = fopen("/sys/class/power_supply/BAT0/energy_full", "r");
	if(fd == NULL) {
		fprintf(stderr, "Error opening energy_full.\n");
		return -1;
	}
	fscanf(fd, "%d", &energy_full);
	fclose(fd);


	fd = fopen("/sys/class/power_supply/BAT0/voltage_now", "r");
	if(fd == NULL) {
		fprintf(stderr, "Error opening voltage_now.\n");
		return -1;
	}
	fscanf(fd, "%d", &voltage_now);
	fclose(fd);


	return ((float)energy_now * 1000 / (float)voltage_now) * 100 / ((float)energy_full * 1000 / (float)voltage_now);
}


// TODO get from tmux-powerline.
static char *mpd_no_playing = "";
static const char *get_mpd_np()
{
	return mpd_no_playing;
}


static char *loadavg(void)
{
	double avgs[3];

	if (getloadavg(avgs, 3) < 0) {
		perror("getloadavg");
		exit(1);
	}

	return smprintf("%.2f %.2f %.2f", avgs[0], avgs[1], avgs[2]);
}

static char*maildir_no_mail = "0m |";
static char *get_maildir_counts()
{
	return maildir_no_mail;
}

int main(void)
{
	char *status;
	char *avgs;
	float cpu0, cpu1, cpu2, cpu3;
	int bat0;
	const char *mpd_np;
	char *maildir_counts;


	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "Cannot open display.\n");
		return EXIT_FAILURE;
	}

	if ((status = malloc(200)) == NULL) {
		return EXIT_FAILURE;
	}

	if ((datetime_buf = malloc(DATE_MAX_LEN)) == NULL) {
		fprintf(stderr, "Cannot allocate memory for datetime_buf.\n");
		exit(EXIT_FAILURE);
	}

	//for (size_t i = 0; i < 10; ++i) { // For leak-checking.
	while (true) {
		mpd_np = get_mpd_np();
		maildir_counts = get_maildir_counts();
		avgs = loadavg();
		cpu0 = get_freq("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
		cpu1 = get_freq("/sys/devices/system/cpu/cpu1/cpufreq/scaling_cur_freq");
		cpu2 = get_freq("/sys/devices/system/cpu/cpu2/cpufreq/scaling_cur_freq");
		cpu3 = get_freq("/sys/devices/system/cpu/cpu3/cpufreq/scaling_cur_freq");
		get_datetime();
		bat0 = get_battery();
		snprintf(status, 200, "%s %s %s | %0.2f, %0.2f, %0.2f, %0.2f | %d%% | %s", mpd_np, maildir_counts, avgs, cpu0, cpu1, cpu2, cpu3, bat0, datetime_buf);

		set_status(status);
		sleep(1);
	}

	free(datetime_buf);
	free(status);
	XCloseDisplay(dpy);

	return 0;
}
