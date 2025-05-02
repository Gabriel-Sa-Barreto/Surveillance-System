#include "../gpio.h"

struct gpiod_line_request* gpio_request_to_lines(const char *chip_path, unsigned int *offset, int num_lines, const char *consumer, char *in_out)
{
    struct gpiod_request_config *req_cfg  = NULL;
	struct gpiod_line_request   *request  = NULL;
	struct gpiod_line_settings  *settings;
	struct gpiod_line_config    *line_cfg;
	struct gpiod_chip           *chip;
    int ret;

    chip = gpiod_chip_open(chip_path);
	if (!chip)
		return NULL;
    else
        printf("%s Chip opened successfully.\n", TAG);
    
    settings = gpiod_line_settings_new();
        if (!settings)
            goto close_chip;
        else
            printf("%s Line setting created successfully.\n", TAG);
    
    if(strcmp(in_out, GPIO_DIRECTION_IN) == 0)
    {
        gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    }else if(strcmp(in_out, GPIO_DIRECTION_OUT) == 0)
    {
        gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    }else
        goto free_settings;

    line_cfg = gpiod_line_config_new();
	if (!line_cfg)
		goto free_settings;
    else
        printf("%s Line Config successfully.\n", TAG);
    
    for(int i = 0; i < num_lines; i++)
    {
        ret = gpiod_line_config_add_line_settings(line_cfg, &offset[i], 1, settings);
        if(ret)
            goto free_line_config;
        else
            printf("%s Line settings added successfully.\n", TAG);
    }

    if(consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg)
			goto free_line_config;
        else
            printf("%s Request config consumer successfully.\n", TAG);

		gpiod_request_config_set_consumer(req_cfg, consumer);
	}

	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	gpiod_request_config_free(req_cfg);

free_line_config:
	gpiod_line_config_free(line_cfg);

free_settings:
	gpiod_line_settings_free(settings);

close_chip:
	gpiod_chip_close(chip);

	return request;
}

int gpio_read(struct gpiod_line_request *request, const unsigned int offset)
{
    enum gpiod_line_value value = gpiod_line_request_get_value(request, offset);
    if(value == GPIOD_LINE_VALUE_ACTIVE)
		return GPIOD_LINE_VALUE_ACTIVE;
	else if(value == GPIOD_LINE_VALUE_INACTIVE)
		return GPIOD_LINE_VALUE_INACTIVE;
	else
		printf("%s Error reading line value: %s\n", TAG, offset);
    return -1;
}

void gpio_write(struct gpiod_line_request *request, const unsigned int offset, enum gpiod_line_value value)
{
    gpiod_line_request_set_value(request, offset, value);
}

void gpio_release_request(struct gpiod_line_request *request)
{
    gpiod_line_request_release(request);
}