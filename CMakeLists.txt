if(CONFIG_ZMK_DISPLAY AND CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM)
    target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/boards/arm/eyelash_sofle/custom_status_screen.c)
endif()
