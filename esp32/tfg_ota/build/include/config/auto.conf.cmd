deps_config := \
	/home/javicano8/esp/esp-idf/components/app_trace/Kconfig \
	/home/javicano8/esp/esp-idf/components/aws_iot/Kconfig \
	/home/javicano8/esp/esp-idf/components/bt/Kconfig \
	/home/javicano8/esp/esp-idf/components/driver/Kconfig \
	/home/javicano8/esp/esp-idf/components/esp32/Kconfig \
	/home/javicano8/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/home/javicano8/esp/esp-idf/components/esp_http_client/Kconfig \
	/home/javicano8/esp/esp-idf/components/ethernet/Kconfig \
	/home/javicano8/esp/esp-idf/components/fatfs/Kconfig \
	/home/javicano8/esp/esp-idf/components/freertos/Kconfig \
	/home/javicano8/esp/esp-idf/components/heap/Kconfig \
	/home/javicano8/esp/esp-idf/components/libsodium/Kconfig \
	/home/javicano8/esp/esp-idf/components/log/Kconfig \
	/home/javicano8/esp/esp-idf/components/lwip/Kconfig \
	/home/javicano8/esp/esp-idf/components/mbedtls/Kconfig \
	/home/javicano8/esp/esp-idf/components/nvs_flash/Kconfig \
	/home/javicano8/esp/esp-idf/components/openssl/Kconfig \
	/home/javicano8/esp/esp-idf/components/pthread/Kconfig \
	/home/javicano8/esp/esp-idf/components/spi_flash/Kconfig \
	/home/javicano8/esp/esp-idf/components/spiffs/Kconfig \
	/home/javicano8/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/javicano8/esp/esp-idf/components/vfs/Kconfig \
	/home/javicano8/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/javicano8/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/javicano8/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/javicano8/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/javicano8/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
