include $(TOPDIR)/rules.mk

PKG_NAME:=daemon_log
PKG_RELEASE:=1
PKG_VERSION:=0.1.0
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/daemon_log
	CATEGORY:=Logging program
	TITLE:=daemon_log
	DEPENDS:= +tuya-iot-core-sdk
endef

define Package/daemon_log/description
	Simple daemon program that logs command output to Tuya IoT core
endef

define Package/daemon_log/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/daemon_log $(1)/usr/bin
	$(INSTALL_BIN) ./files/daemon_log.init $(1)/etc/init.d/daemon_log
	$(INSTALL_CONF) ./files/daemong_log.conf $(1)/etc/config/daemon_log
endef

$(eval $(call BuildPackage,daemon_log))
