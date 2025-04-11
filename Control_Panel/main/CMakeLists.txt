idf_component_register(SRCS "fileSystem.c" "packageProcessor.c" "MQTT.c" "project_Timers.c" "displayOled_controller.c" "displayOled.c" "m_key.c" "buttons.c" "main.c"
                    INCLUDE_DIRS ".")
# Note: you must have a partition named the first argument (here it's "littlefs")
# in your partition table csv file.
littlefs_create_partition_image(storage ../flash_data FLASH_IN_PROJECT)
