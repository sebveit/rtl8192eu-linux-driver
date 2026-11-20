################################################################################
# RTL8192EU USB WiFi driver (Linux only)
#
# Supported kernels: 5.4, 5.10, 5.15, 6.1
#
# Usage:
#   make                       build against the running kernel
#   make KSRC=/path/to/kernel  build against a prepared kernel tree
#   make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- KSRC=... (cross build)
#   make install / uninstall   (native builds only)
#   make test                  build against all supported kernel trees
#
# Feature options below may be overridden on the command line,
# e.g. "make CONFIG_RTW_DEBUG=n" or "make CONFIG_POWER_SAVING=y".
################################################################################

MODULE_NAME ?= 8192eu

########################## Features ###########################
CONFIG_MP_INCLUDED ?= n
CONFIG_POWER_SAVING ?= n
CONFIG_USB_AUTOSUSPEND ?= n
CONFIG_BT_COEXIST ?= y
CONFIG_EFUSE_CONFIG_FILE ?= y
CONFIG_LOAD_PHY_PARA_FROM_FILE ?= y
CONFIG_TXPWR_BY_RATE ?= y
# n, y or auto
CONFIG_TXPWR_BY_RATE_EN ?= n
CONFIG_TXPWR_LIMIT ?= y
# n, y or auto
CONFIG_TXPWR_LIMIT_EN ?= n
# 0xFF: use the channel plan stored in efuse
CONFIG_RTW_CHPLAN ?= 0xFF
# disable or enable
CONFIG_RTW_ADAPTIVITY_EN ?= disable
# normal or carrier_sense
CONFIG_RTW_ADAPTIVITY_MODE ?= normal
CONFIG_80211W ?= y
CONFIG_BR_EXT ?= y
CONFIG_BR_EXT_BRNAME ?= br0
CONFIG_TDLS ?= n
CONFIG_WIFI_MONITOR ?= n
CONFIG_RTW_NAPI ?= y
CONFIG_RTW_GRO ?= y
CONFIG_RTW_NETIF_SG ?= y
CONFIG_TX_CSUM_OFFLOAD ?= n
CONFIG_WOWLAN ?= n
CONFIG_WAKEUP_TYPE ?= 0x7
CONFIG_SUSPEND_TYPE ?= 0

# Files the driver loads at runtime (via the firmware loader, so they must
# be below /lib/firmware/)
EFUSE_MAP_PATH ?= /lib/firmware/rtl8192eu/wifi_efuse_8192eu.map
WIFIMAC_PATH ?= /lib/firmware/rtl8192eu/wifimac.txt
REALTEK_CONFIG_PATH ?= /lib/firmware/

########################## Debug ###########################
CONFIG_RTW_DEBUG ?= y
# Log level for RTW_PRINT/RTW_ERR/RTW_WARN and hex dumps (0=none, 1=always,
# 2=error, 3=warning, 4=info, 5=debug). RTW_INFO/RTW_DBG messages use
# dynamic debug: "modprobe 8192eu dyndbg=+p".

CONFIG_RTW_LOG_LEVEL ?= 4

ifneq ($(KERNELRELEASE),)
################################################################################
# Kbuild part
################################################################################

ccflags-y += $(USER_EXTRA_CFLAGS)
ccflags-y += -Wformat -Wformat-security

ccflags-y += -I$(src)/include -I$(src)/platform -I$(src)/hal/btc -I$(src)/hal/phydm

ccflags-y += -DCONFIG_RTL8192E -DDM_ODM_SUPPORT_TYPE=0x04
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y += -DHIGH_ACTIVE_HST2DEV=0

ifdef CONFIG_CPU_BIG_ENDIAN
ccflags-y += -DCONFIG_BIG_ENDIAN
else
ccflags-y += -DCONFIG_LITTLE_ENDIAN
endif

bool_flag = $(if $(filter y,$(strip $(1))),$(2))
tri_val = $(if $(filter y,$(strip $(1))),1,$(if $(filter auto,$(strip $(1))),2,0))

ccflags-y += $(call bool_flag,$(CONFIG_MP_INCLUDED),-DCONFIG_MP_INCLUDED)
ccflags-y += $(call bool_flag,$(CONFIG_POWER_SAVING),-DCONFIG_POWER_SAVING)
ccflags-y += $(call bool_flag,$(CONFIG_USB_AUTOSUSPEND),-DCONFIG_USB_AUTOSUSPEND)
ccflags-y += $(call bool_flag,$(CONFIG_BT_COEXIST),-DCONFIG_BT_COEXIST)
ccflags-y += $(call bool_flag,$(CONFIG_EFUSE_CONFIG_FILE),-DCONFIG_EFUSE_CONFIG_FILE \
	-DEFUSE_MAP_PATH=\"$(EFUSE_MAP_PATH)\" -DWIFIMAC_PATH=\"$(WIFIMAC_PATH)\")
ccflags-y += $(call bool_flag,$(CONFIG_LOAD_PHY_PARA_FROM_FILE),-DCONFIG_LOAD_PHY_PARA_FROM_FILE \
	-DREALTEK_CONFIG_PATH=\"$(REALTEK_CONFIG_PATH)\")
ccflags-y += -DCONFIG_TXPWR_BY_RATE=$(call tri_val,$(CONFIG_TXPWR_BY_RATE))
ccflags-y += -DCONFIG_TXPWR_BY_RATE_EN=$(call tri_val,$(CONFIG_TXPWR_BY_RATE_EN))
ccflags-y += -DCONFIG_TXPWR_LIMIT=$(call tri_val,$(CONFIG_TXPWR_LIMIT))
ccflags-y += -DCONFIG_TXPWR_LIMIT_EN=$(call tri_val,$(CONFIG_TXPWR_LIMIT_EN))
ifneq ($(strip $(CONFIG_RTW_CHPLAN)),0xFF)
ccflags-y += -DCONFIG_RTW_CHPLAN=$(strip $(CONFIG_RTW_CHPLAN))
endif
ccflags-y += -DCONFIG_RTW_ADAPTIVITY_EN=$(if $(filter enable,$(strip $(CONFIG_RTW_ADAPTIVITY_EN))),1,0)
ccflags-y += -DCONFIG_RTW_ADAPTIVITY_MODE=$(if $(filter carrier_sense,$(strip $(CONFIG_RTW_ADAPTIVITY_MODE))),1,0)
ccflags-y += $(call bool_flag,$(CONFIG_80211W),-DCONFIG_IEEE80211W)
ccflags-y += $(call bool_flag,$(CONFIG_BR_EXT),-DCONFIG_BR_EXT \
	'-DCONFIG_BR_EXT_BRNAME="'$(CONFIG_BR_EXT_BRNAME)'"')
ccflags-y += $(call bool_flag,$(CONFIG_TDLS),-DCONFIG_TDLS)
ccflags-y += $(call bool_flag,$(CONFIG_WIFI_MONITOR),-DCONFIG_WIFI_MONITOR)
ccflags-y += $(call bool_flag,$(CONFIG_RTW_NAPI),-DCONFIG_RTW_NAPI)
ccflags-y += $(call bool_flag,$(CONFIG_RTW_GRO),-DCONFIG_RTW_GRO)
ccflags-y += $(call bool_flag,$(CONFIG_RTW_NETIF_SG),-DCONFIG_RTW_NETIF_SG)
ccflags-y += $(call bool_flag,$(CONFIG_TX_CSUM_OFFLOAD),-DCONFIG_TX_CSUM_OFFLOAD)
ccflags-y += $(call bool_flag,$(CONFIG_WOWLAN),-DCONFIG_WOWLAN \
	-DRTW_WAKEUP_EVENT=$(CONFIG_WAKEUP_TYPE) -DRTW_SUSPEND_TYPE=$(CONFIG_SUSPEND_TYPE))
ccflags-y += $(call bool_flag,$(CONFIG_RTW_DEBUG),-DCONFIG_RTW_DEBUG \
	-DRTW_LOG_LEVEL=$(CONFIG_RTW_LOG_LEVEL))

ldflags-y += --strip-debug

######################## Objects ###########################
rtw_core := \
	core/rtw_cmd.o core/rtw_security.o core/rtw_debug.o core/rtw_io.o \
	core/rtw_ioctl_set.o core/rtw_ieee80211.o \
	core/rtw_mlme.o core/rtw_mlme_ext.o core/rtw_mi.o core/rtw_wlan_util.o \
	core/rtw_vht.o core/rtw_pwrctrl.o core/rtw_rf.o core/rtw_chplan.o \
	core/rtw_recv.o core/rtw_sta_mgt.o core/rtw_ap.o \
	core/rtw_xmit.o core/rtw_p2p.o core/rtw_tdls.o \
	core/rtw_br_ext.o core/rtw_iol.o core/rtw_sreset.o \
	core/rtw_btcoex_wifionly.o core/rtw_btcoex.o core/rtw_beamforming.o \
	core/rtw_odm.o core/rtw_rm.o core/rtw_rm_fsm.o core/efuse/rtw_efuse.o

rtw_os := \
	os_dep/osdep_service.o os_dep/linux/os_intfs.o os_dep/linux/usb_intf.o \
	os_dep/linux/usb_ops_linux.o os_dep/linux/ioctl_linux.o \
	os_dep/linux/xmit_linux.o os_dep/linux/mlme_linux.o \
	os_dep/linux/recv_linux.o os_dep/linux/ioctl_cfg80211.o \
	os_dep/linux/rtw_cfgvendor.o os_dep/linux/wifi_regd.o \
	os_dep/linux/rtw_android.o os_dep/linux/rtw_proc.o


rtw_hal := \
	hal/hal_intf.o hal/hal_com.o hal/hal_com_phycfg.o hal/hal_phy.o \
	hal/hal_dm.o hal/hal_dm_acs.o hal/hal_btcoex_wifionly.o hal/hal_btcoex.o \
	hal/hal_mp.o hal/hal_hci/hal_usb.o \
	hal/led/hal_led.o hal/led/hal_usb_led.o hal/HalPwrSeqCmd.o \
	hal/rtl8192e/Hal8192EPwrSeq.o hal/rtl8192e/rtl8192e_xmit.o \
	hal/rtl8192e/rtl8192e_sreset.o hal/rtl8192e/rtl8192e_hal_init.o \
	hal/rtl8192e/rtl8192e_phycfg.o hal/rtl8192e/rtl8192e_rf6052.o \
	hal/rtl8192e/rtl8192e_dm.o hal/rtl8192e/rtl8192e_rxdesc.o \
	hal/rtl8192e/rtl8192e_cmd.o hal/rtl8192e/hal8192e_fw.o \
	hal/rtl8192e/usb/usb_halinit.o hal/rtl8192e/usb/rtl8192eu_led.o \
	hal/rtl8192e/usb/rtl8192eu_xmit.o hal/rtl8192e/usb/rtl8192eu_recv.o \
	hal/rtl8192e/usb/usb_ops_linux.o hal/efuse/rtl8192e/HalEfuseMask8192E_USB.o

rtw_phydm := \
	hal/phydm/phydm_debug.o hal/phydm/phydm_antdiv.o hal/phydm/phydm_soml.o \
	hal/phydm/phydm_smt_ant.o hal/phydm/phydm_antdect.o \
	hal/phydm/phydm_interface.o hal/phydm/phydm_phystatus.o \
	hal/phydm/phydm_hwconfig.o hal/phydm/phydm.o hal/phydm/phydm_dig.o \
	hal/phydm/phydm_pathdiv.o hal/phydm/phydm_rainfo.o \
	hal/phydm/phydm_dynamictxpower.o hal/phydm/phydm_adaptivity.o \
	hal/phydm/phydm_cfotracking.o hal/phydm/phydm_noisemonitor.o \
	hal/phydm/phydm_beamforming.o hal/phydm/phydm_dfs.o \
	hal/phydm/txbf/halcomtxbf.o hal/phydm/txbf/haltxbfinterface.o \
	hal/phydm/txbf/phydm_hal_txbf_api.o hal/phydm/phydm_adc_sampling.o \
	hal/phydm/phydm_ccx.o hal/phydm/phydm_psd.o hal/phydm/phydm_primary_cca.o \
	hal/phydm/phydm_cck_pd.o hal/phydm/phydm_rssi_monitor.o \
	hal/phydm/phydm_auto_dbg.o hal/phydm/phydm_math_lib.o \
	hal/phydm/phydm_api.o hal/phydm/phydm_pow_train.o \
	hal/phydm/phydm_lna_sat.o hal/phydm/phydm_pmac_tx_setting.o \
	hal/phydm/phydm_mp.o hal/phydm/halrf/halrf.o hal/phydm/halrf/halrf_debug.o \
	hal/phydm/halrf/halphyrf_ce.o hal/phydm/halrf/halrf_powertracking_ce.o \
	hal/phydm/halrf/halrf_powertracking.o hal/phydm/halrf/halrf_kfree.o \
	hal/phydm/rtl8192e/halhwimg8192e_mac.o hal/phydm/rtl8192e/halhwimg8192e_bb.o \
	hal/phydm/rtl8192e/halhwimg8192e_rf.o hal/phydm/halrf/rtl8192e/halrf_8192e_ce.o \
	hal/phydm/rtl8192e/phydm_regconfig8192e.o hal/phydm/rtl8192e/phydm_rtl8192e.o

$(MODULE_NAME)-y += $(rtw_core) $(rtw_os) $(rtw_hal) $(rtw_phydm)
$(MODULE_NAME)-y += platform/platform_ops.o
$(MODULE_NAME)-$(CONFIG_BT_COEXIST) += hal/btc/halbtc8192e1ant.o hal/btc/halbtc8192e2ant.o
$(MODULE_NAME)-$(CONFIG_MP_INCLUDED) += core/rtw_mp.o os_dep/linux/ioctl_mp.o

obj-m := $(MODULE_NAME).o

else
################################################################################
# External part
################################################################################

KVER ?= $(shell uname -r)
KERNEL_SRC ?= /lib/modules/$(KVER)/build
KSRC ?= $(KERNEL_SRC)
# Yocto's module.bbclass passes INSTALL_MOD_PATH
DESTDIR ?= $(INSTALL_MOD_PATH)
MODDESTDIR ?= /lib/modules/$(KVER)/kernel/drivers/net/wireless/realtek/rtl8192eu
PWD := $(shell pwd)

KBUILD_ARGS := -C $(KSRC) M=$(PWD)
ifneq ($(ARCH),)
KBUILD_ARGS += ARCH=$(ARCH)
endif
ifneq ($(CROSS_COMPILE),)
KBUILD_ARGS += CROSS_COMPILE=$(CROSS_COMPILE)
endif

.PHONY: all modules clean install uninstall strip test

all: modules

modules:
	$(MAKE) $(KBUILD_ARGS) modules

clean:
	$(MAKE) $(KBUILD_ARGS) clean

strip:
	$(CROSS_COMPILE)strip --strip-unneeded $(MODULE_NAME).ko

install:
	install -D -p -m 644 $(MODULE_NAME).ko $(DESTDIR)$(MODDESTDIR)/$(MODULE_NAME).ko
ifeq ($(DESTDIR),)
	/sbin/depmod -a $(KVER)
endif

uninstall:
	rm -f $(DESTDIR)$(MODDESTDIR)/$(MODULE_NAME).ko
ifeq ($(DESTDIR),)
	/sbin/depmod -a $(KVER)
endif

test:
	./kernel-tests/simple-test-all.sh

endif
