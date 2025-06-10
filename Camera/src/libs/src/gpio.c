#include "../gpio.h"

//#define ENABLE_DEBUG_MSG = y

struct gpiod_line_request* gpio_request_to_output(const char  *chip_path,
                                                 unsigned int *offset,
                                                 int           num_lines,
                                                 const char   *consumer)
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
#ifdef ENABLE_DEBUG_MSG
    else
        log_debug("OUTPUT: Chip opened sucessfully.");
#endif
    
    settings = gpiod_line_settings_new();
    if (!settings)
        goto close_chip;
#ifdef ENABLE_DEBUG_MSG
    else
        log_debug("OUTPUT: Line setting created successfully.");
#endif
    
    // Set Direction
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);

    line_cfg = gpiod_line_config_new();
	if (!line_cfg)
		goto free_settings;
#ifdef ENABLE_DEBUG_MSG
    else
        log_debug("OUTPUT: Line Config successfully.");
#endif
    
    for(int i = 0; i < num_lines; i++)
    {
        ret = gpiod_line_config_add_line_settings(line_cfg, &offset[i], 1, settings);
        if(ret)
            goto free_line_config;
#ifdef ENABLE_DEBUG_MSG
        else
            log_debug("OUTPUT: Line settings added successfully.");
#endif
    }

    if(consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg)
			goto free_line_config;
#ifdef ENABLE_DEBUG_MSG
        else
            log_debug("OUTPUT: Request config consumer successfully.");
#endif

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

struct gpiod_line_request* gpio_request_to_input(const char   *chip_path,
                                                 unsigned int *offset,
                                                 int           num_lines,
                                                 const char   *consumer,
                                                 enum gpiod_line_edge edgeEvent,
                                                 size_t        event_buffer_size,
                                                 bool          setDebounce)
{
    struct gpiod_request_config *req_cfg  = NULL;
	struct gpiod_line_request   *request  = NULL;
	struct gpiod_line_settings  *settings;
	struct gpiod_line_config    *line_cfg;
	struct gpiod_chip           *chip;
    int ret;
    unsigned long debounce_period = 50000; // 50ms = 50000 us 

    chip = gpiod_chip_open(chip_path);
	if (!chip)
		return NULL;
#ifdef ENABLE_DEBUG_MSG
    else
        log_debug("INPUT: Chip opened sucessfully.");
#endif
    
    settings = gpiod_line_settings_new();
        if (!settings)
            goto close_chip;
#ifdef ENABLE_DEBUG_MSG
        else
            log_debug("INPUT: Line setting created successfully.");
#endif
    // Set Direction
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    // Set Edge event
    gpiod_line_settings_set_edge_detection(settings, edgeEvent);
    // Set Debounce
    if(setDebounce)
        gpiod_line_settings_set_debounce_period_us(settings, debounce_period); 

    line_cfg = gpiod_line_config_new();
	if (!line_cfg)
		goto free_settings;
#ifdef ENABLE_DEBUG_MSG
    else
        log_debug("INPUT: Line Config successfully.");
#endif
    
    for(int i = 0; i < num_lines; i++)
    {
        ret = gpiod_line_config_add_line_settings(line_cfg, &offset[i], 1, settings);
        if(ret)
            goto free_line_config;
#ifdef ENABLE_DEBUG_MSG
        else
            log_debug("INPUT: Line settings added successfully.");
#endif
    }

    if(consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg)
			goto free_line_config;
#ifdef ENABLE_DEBUG_MSG
        else
            log_debug("INPUT: Request config consumer successfully.");
#endif

		gpiod_request_config_set_consumer(req_cfg, consumer);

        gpiod_request_config_set_event_buffer_size(req_cfg, event_buffer_size);
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
        log_error("Error reading line value: %d", offset);
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