add_executable(pisca-led-dois-botoes
        main.c
)

target_link_libraries(pisca-led-dois-botoes
                      pico_stdlib
                hardware_adc
                hardware_gpio
                hardware_timer )

pico_add_extra_outputs(pisca-led-dois-botoes)
