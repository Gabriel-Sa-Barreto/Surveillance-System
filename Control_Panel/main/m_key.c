#include "headers/m_key.h"

enum MAPPING_KEY
{
    M_KEY_1,    /* (i,j) = (0,0) = pos 0  */
    M_KEY_2,    /* (i,j) = (0,1) = pos 1  */
    M_KEY_3,    /* (i,j) = (0,2) = pos 2  */ 
    M_KEY_4,    /* (i,j) = (1,0) = pos 3  */
    M_KEY_5,    /* (i,j) = (1,1) = pos 4  */
    M_KEY_6,    /* (i,j) = (1,2) = pos 5  */
    M_KEY_7,    /* (i,j) = (2,0) = pos 6  */
    M_KEY_8,    /* (i,j) = (2,1) = pos 7  */
    M_KEY_9,    /* (i,j) = (2,2) = pos 8  */
    M_KEY_STAR, /* (i,j) = (3,0) = pos 9  */
    M_KEY_0,    /* (i,j) = (3,1) = pos 10 */
    M_KEY_GRID  /* (i,j) = (3,2) = pos 11 */
};

char char_keys[] = {'1','2','3','4','5','6','7','8','9','*','0','#'};

static void set_col(uint8_t n_col, uint8_t level)
{
    switch (n_col)
    {
        case 0: gpio_set_level(M_KEY_COL_0_PIN, level); break;
        case 1: gpio_set_level(M_KEY_COL_1_PIN, level); break;
        case 2: gpio_set_level(M_KEY_COL_2_PIN, level); break;
        default:
            ESP_LOGE(M_KEY_TAG, "Column don't recognized.");
            break;
    }
}

static uint8_t get_line(uint8_t n_line)
{
    uint8_t value;
    switch (n_line)
    {
        case 0: value = gpio_get_level(M_KEY_LINE_0_PIN); break;
        case 1: value = gpio_get_level(M_KEY_LINE_1_PIN); break;
        case 2: value = gpio_get_level(M_KEY_LINE_2_PIN); break;
        case 3: value = gpio_get_level(M_KEY_LINE_3_PIN); break;
        default:
            ESP_LOGE(M_KEY_TAG, "Line don't recognized.");
            value = 0;
            break;
    }
    return value;
}

char key_selected(uint8_t position)
{
    switch (position)
    {
        case M_KEY_1:    return char_keys[position]; break;
        case M_KEY_2:    return char_keys[position]; break;
        case M_KEY_3:    return char_keys[position]; break;
        case M_KEY_4:    return char_keys[position]; break;
        case M_KEY_5:    return char_keys[position]; break;
        case M_KEY_6:    return char_keys[position]; break;
        case M_KEY_7:    return char_keys[position]; break;
        case M_KEY_8:    return char_keys[position]; break;
        case M_KEY_9:    return char_keys[position]; break;
        case M_KEY_STAR: return char_keys[position]; break;
        case M_KEY_0:    return char_keys[position]; break;
        case M_KEY_GRID: return char_keys[position]; break;
        default: 
            ESP_LOGE(M_KEY_TAG,"Key don't recognized.");
            return NULL;
            break;
    }
}

int scan_keyboard()
{
    uint8_t isPressed = 1; 
    uint8_t col       = 0;
    uint8_t line      = 0; 
    for(col = 0; col < M_KEY_NUMBER_COL; col++)
    {
        set_col(col,0);
        for(line = 0; line < M_KEY_NUMBER_LINE; line++)
        {
            isPressed = get_line(line);
            if(!isPressed)
            {
                while(!get_line(line)) // Loop for wait the button to be released.
                {
                    vTaskDelay(pdMS_TO_TICKS(50));
                }
                break;
            }
        }
        set_col(col,1);
        if(!isPressed)
            break;
    }
    if(!isPressed)
    {
        return (line * M_KEY_NUMBER_COL + col);
    }
    else
    {
        return -1;
    }
}

void init_m_key()
{
    gpio_set_direction(M_KEY_COL_0_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(M_KEY_COL_1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(M_KEY_COL_2_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(M_KEY_LINE_0_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(M_KEY_LINE_1_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(M_KEY_LINE_2_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(M_KEY_LINE_3_PIN, GPIO_MODE_INPUT);

    gpio_pulldown_dis(M_KEY_LINE_0_PIN);
    gpio_pulldown_dis(M_KEY_LINE_1_PIN);
    gpio_pulldown_dis(M_KEY_LINE_2_PIN);
    gpio_pulldown_dis(M_KEY_LINE_3_PIN);

    gpio_pullup_en(M_KEY_LINE_0_PIN);
    gpio_pullup_en(M_KEY_LINE_1_PIN);
    gpio_pullup_en(M_KEY_LINE_2_PIN);
    gpio_pullup_en(M_KEY_LINE_3_PIN);

    /* Scan performed with low logic level */
    gpio_set_level(M_KEY_COL_0_PIN, 1);
    gpio_set_level(M_KEY_COL_1_PIN, 1);
    gpio_set_level(M_KEY_COL_2_PIN, 1);
}