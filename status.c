#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "status.h"

#define STATUS_BUF_SIZE 64

int fetch_int_from_file(const char *file_name);
Bool get_batt_info(char *buf);
Bool get_date_time(char *buf);

Bool get_custom_status_text(char *text) {
    // Fetch time, battery percentage, wireless stuff?
    char batt_buf[STATUS_BUF_SIZE];
    char time_buf[STATUS_BUF_SIZE];

    if(!get_date_time(time_buf) || !get_batt_info(batt_buf))
    {
		fprintf(stderr, "Error fetching status info.\n");
        return False;
    }

    sprintf(text, "%s  |  %s", batt_buf, time_buf);
    return True;
}

Bool get_batt_info(char *buf) {
    char batt_state[STATUS_BUF_SIZE];
	float energy_now, energy_full;

	energy_now = (float)fetch_int_from_file("/sys/class/power_supply/BAT0/energy_now");
    energy_full	= (float)fetch_int_from_file("/sys/class/power_supply/BAT0/energy_full");

    if(energy_now < 0.0 || energy_full < 0.0)
        return False;

    FILE *fd = fopen("/sys/class/power_supply/BAT0/status", "r");
	if(fd == NULL) {
		fprintf(stderr, "Error opening status file.\n");
        return False;
    } else {
        fscanf(fd, "%s", batt_state);
        fclose(fd);
	}

    sprintf(buf, "Battery %s (%.0f%%)", batt_state, 100*energy_now/energy_full);
    return True;
}

Bool get_date_time(char *buf)
{
	time_t result;
	struct tm *resulttm;

	result = time(NULL);
	resulttm = localtime(&result);
	if(resulttm == NULL) {
		fprintf(stderr, "Error getting localtime.\n");
        return False;
	}
	if(!strftime(buf, sizeof(char)*STATUS_BUF_SIZE - 1, "%A %d %B %R", resulttm)) {
		fprintf(stderr, "strftime is 0.\n");
        return False;
	}

    return True;
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
