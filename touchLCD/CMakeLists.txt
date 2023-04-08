include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)
add_executable(touchLCD
        main.cpp
        pinDefinitions/pinDefinitions.cpp
        touchController/touchController.cpp
        lcdController/lcdDriver.cpp
        graphics/utils/utils.cpp
        graphics/bitmap/bitmap12.cpp
        graphics/bitmap/bitmap16.cpp
        graphics/bitmap/monochromaticBitmap.cpp
        spiDmaDriver/spiDmaDriver.cpp
        tasks/LcdTask.cpp
        tasks/spi1DmaTask.cpp
        )
target_include_directories(touchLCD PRIVATE ${CMAKE_CURRENT_LIST_DIR})
# enable usb output, disable uart output
pico_enable_stdio_usb(touchLCD 1)
pico_enable_stdio_uart(touchLCD 0)
# pull in common dependencies
target_link_libraries(touchLCD pico_stdlib hardware_spi FreeRTOS-Kernel FreeRTOS-Kernel-Heap4 hardware_dma)
# create map/bin/hex file etc.
pico_add_extra_outputs(touchLCD)
