idf_component_register(SRCS "AK8975.cpp"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES driver esp_timer
                       REQUIRES I2Cdev)
