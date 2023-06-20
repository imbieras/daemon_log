include $(TOPDIR)/rules.mk

PKG_NAME:=tuya-iot-core-sdk
PKG_RELEASE:=1
PKG_VERSION:=0.0.1
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/tuya/tuya-iot-core-sdk
PKG_SOURCE_VERSION:=0208badefb2003d30e70b5b875395e3187c850d9
PKG_HASH:=f55ef9b585350849b796cb95694be69aa122c86f09e8b6bc3e2c68ec2cd266e1
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_BINARY_SUBDIR:=build
CMAKE_INSTALL:=1

define Package/tuya-iot-core-sdk
	CATEGORY:=Libraries
	TITLE:=tuya-iot-core-sdk
endef

define Package/tuya-iot-core-sdk/description
	Tuya IoT Core SDK
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/*.so $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/utils/*.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/include/*.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/libraries/coreJSON/source/include/*.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/interface/*.h $(1)/usr/include/
endef

define Package/tuya-iot-core-sdk/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/*.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,tuya-iot-core-sdk))
