BOARD ?= nrf52_pca10040
CONF_FILE ?= prj_cs7ns2.conf

include $(ZEPHYR_BASE)/Makefile.inc

ifeq ($(BOARD), qemu_x86)
	include $(ZEPHYR_BASE)/samples/net/common/Makefile.ipstack
endif
