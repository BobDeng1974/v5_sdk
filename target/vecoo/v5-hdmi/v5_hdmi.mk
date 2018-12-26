$(call inherit-product-if-exists, target/vecoo/v5-common/v5-common.mk)

PRODUCT_PACKAGES +=

PRODUCT_COPY_FILES +=

PRODUCT_AAPT_CONFIG := large xlarge hdpi xhdpi
PRODUCT_AAPT_PERF_CONFIG := xhdpi
PRODUCT_CHARACTERISTICS := musicbox

PRODUCT_BRAND := vyagoo
PRODUCT_NAME := v5_hdmi
PRODUCT_DEVICE := v5-hdmi
PRODUCT_MODEL := V5 AIWORLD
