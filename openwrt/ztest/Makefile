
include $(TOPDIR)/rules.mk

PKG_NAME:=ztest
PKG_VERSION:=1.0
PKG_RELEASE:=20190531

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)_$(PKG_VERSION)
include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=ZIHOME
  CATEGORY:=ZIHOME
  TITLE:=ZTEST
  PKGARCH:=all
  SUBMENU:=utils
  DEPENDS:= +libubox +libuci +libubus +libiwinfo
endef

define Package/$(PKG_NAME)/description
  ZIHOME libbox and ubus test
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ztest.init $(1)/etc/init.d/ztest
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ztest $(1)/usr/bin/ztest
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
