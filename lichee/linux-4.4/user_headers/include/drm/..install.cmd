cmd_/home/shane/Downloads/action/New_SDK/Vecoo-SDK-v5-hdmi.Linux-x86_64/out/v5-hdmi/compile_dir/target/linux-v5-hdmi/linux-4.4.55/user_headers/include/drm/.install := bash scripts/headers_install.sh /home/shane/Downloads/action/New_SDK/Vecoo-SDK-v5-hdmi.Linux-x86_64/out/v5-hdmi/compile_dir/target/linux-v5-hdmi/linux-4.4.55/user_headers/include/drm ./include/uapi/drm drm.h drm_fourcc.h drm_mode.h drm_sarea.h exynos_drm.h i810_drm.h i915_drm.h mga_drm.h msm_drm.h nouveau_drm.h qxl_drm.h r128_drm.h radeon_drm.h savage_drm.h sis_drm.h tegra_drm.h via_drm.h virtgpu_drm.h vmwgfx_drm.h; bash scripts/headers_install.sh /home/shane/Downloads/action/New_SDK/Vecoo-SDK-v5-hdmi.Linux-x86_64/out/v5-hdmi/compile_dir/target/linux-v5-hdmi/linux-4.4.55/user_headers/include/drm ./include/drm ; bash scripts/headers_install.sh /home/shane/Downloads/action/New_SDK/Vecoo-SDK-v5-hdmi.Linux-x86_64/out/v5-hdmi/compile_dir/target/linux-v5-hdmi/linux-4.4.55/user_headers/include/drm ./include/generated/uapi/drm ; for F in ; do echo "\#include <asm-generic/$$F>" > /home/shane/Downloads/action/New_SDK/Vecoo-SDK-v5-hdmi.Linux-x86_64/out/v5-hdmi/compile_dir/target/linux-v5-hdmi/linux-4.4.55/user_headers/include/drm/$$F; done; touch /home/shane/Downloads/action/New_SDK/Vecoo-SDK-v5-hdmi.Linux-x86_64/out/v5-hdmi/compile_dir/target/linux-v5-hdmi/linux-4.4.55/user_headers/include/drm/.install