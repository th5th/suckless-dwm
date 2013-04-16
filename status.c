#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "status.h"

#define STATUS_BUF_SIZE 64

int fetch_int_from_file(const char *file_name);
float get_battery_percent();
void get_date_time(char *buf);

Bool
get_custom_status_text(char *text) {
    // Fetch time, battery percentage, wireless stuff?
    // Peek at Conky source?
    // Already have a battery info parser (in C++ but still)
    char *buf;
	if((buf = malloc(sizeof(char) * STATUS_BUF_SIZE)) == NULL) {
		fprintf(stderr, "Cannot allocate memory for date and time buffer.\n");
        return False;
	}

    get_date_time(buf);

    if(buf == NULL)
    {
		fprintf(stderr, "Error fetching date and time.\n");
        return False;
    }

    float batt_perc = get_battery_percent();
    if(batt_perc < 0.0)
        return False;

    sprintf(text, "Battery at %.0f%%  |  %s", batt_perc, buf);
    free(buf);
    return True;
}

float get_battery_percent() {
	float energy_now, energy_full;

	energy_now = (float)fetch_int_from_file("/sys/class/power_supply/BAT0/energy_now");
    energy_full	= (float)fetch_int_from_file("/sys/class/power_supply/BAT0/energy_full");

    if(energy_now < 0.0 || energy_full < 0.0)
        return -1.0;
    else
        return 100 * energy_now / energy_full;
}

void get_date_time(char *buf)
{
	time_t result;
	struct tm *resulttm;

	result = time(NULL);
	resulttm = localtime(&result);
	if(resulttm == NULL) {
		fprintf(stderr, "Error getting localtime.\n");
        buf = NULL;
        return;
	}
	if(!strftime(buf, sizeof(char)*STATUS_BUF_SIZE - 1, "%A %d %B %R", resulttm)) {
		fprintf(stderr, "strftime is 0.\n");
        buf = NULL;
        return;
	}
}

int fetch_int_from_file(const char *file_name)
{
    int return_val;
    FILE *fd = fopen(file_name, "r");
	if(fd == NULL) {
		fprintf(stderr, "Error opening file %s.\n", file_name);
        return_val = -1;
    } else {
        fscanf(fd, "%d", &return_val);
        fclose(fd);
	}
    return return_val;
}
