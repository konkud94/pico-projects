add_executable(door_phone
        main.cpp
        ca_signal_processor/ca_signal_processor_impl.cpp
        rp2040_adc/rp2040_adc.cpp
        hardware_clock/hardware_clock.cpp
        time_aware/cyw43_blinker.cpp
        time_aware/ca_signal_observer.cpp
        time_aware/door_electric_lock.cpp
        repeating_timer/repeating_timer.cpp
        consent_provider/consent_provider_impl.cpp
        relay_controller/relay_controller_impl.cpp
        )

target_link_libraries(door_phone
        pico_stdlib              # for core functionality
        pico_cyw43_arch_none     # we need Wifi to access the GPIO, but we don't need anything else
        pico_time
        hardware_adc
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(door_phone)

# add url via pico_set_program_url
#example_auto_set_url(door_phone)

pico_enable_stdio_usb(door_phone 1)
pico_enable_stdio_uart(door_phone 0)
