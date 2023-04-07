add_executable(touchLCD
        main.cpp
        pinDefinitions/pinDefinitions.cpp
        touchController/touchController.cpp
        lcdController/lcdDriver.cpp
        graphics/utils/utils.cpp
        graphics/bitmap/bitmap12.cpp
        graphics/bitmap/bitmap16.cpp
        graphics/bitmap/monochromaticBitmap.cpp
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(touchLCD 1)
pico_enable_stdio_uart(touchLCD 0)
# pull in common dependencies
target_link_libraries(touchLCD pico_stdlib hardware_spi)
# create map/bin/hex file etc.
pico_add_extra_outputs(touchLCD)
