cmd_arch/arm/boot/dts/sun8iw12p1-soc.dtb := mkdir -p arch/arm/boot/dts/ ; arm-linux-gnueabi-gcc -E -Wp,-MD,arch/arm/boot/dts/.sun8iw12p1-soc.dtb.d.pre.tmp -nostdinc -I./arch/arm/boot/dts -I./arch/arm/boot/dts/include -I./drivers/of/testcase-data -undef -D__DTS__ -x assembler-with-cpp -o arch/arm/boot/dts/.sun8iw12p1-soc.dtb.dts.tmp arch/arm/boot/dts/sun8iw12p1-soc.dts ; ./scripts/dtc/dtc -O dtb -o arch/arm/boot/dts/sun8iw12p1-soc.dtb -b 0 -i arch/arm/boot/dts/  -d arch/arm/boot/dts/.sun8iw12p1-soc.dtb.d.dtc.tmp arch/arm/boot/dts/.sun8iw12p1-soc.dtb.dts.tmp ; cat arch/arm/boot/dts/.sun8iw12p1-soc.dtb.d.pre.tmp arch/arm/boot/dts/.sun8iw12p1-soc.dtb.d.dtc.tmp > arch/arm/boot/dts/.sun8iw12p1-soc.dtb.d

source_arch/arm/boot/dts/sun8iw12p1-soc.dtb := arch/arm/boot/dts/sun8iw12p1-soc.dts

deps_arch/arm/boot/dts/sun8iw12p1-soc.dtb := \
  arch/arm/boot/dts/sun8iw12p1.dtsi \
  arch/arm/boot/dts/include/dt-bindings/interrupt-controller/arm-gic.h \
  arch/arm/boot/dts/include/dt-bindings/interrupt-controller/irq.h \
  arch/arm/boot/dts/include/dt-bindings/gpio/gpio.h \
  arch/arm/boot/dts/sun8iw12p1-clk.dtsi \
  arch/arm/boot/dts/sun8iw12p1-pinctrl.dtsi \

arch/arm/boot/dts/sun8iw12p1-soc.dtb: $(deps_arch/arm/boot/dts/sun8iw12p1-soc.dtb)

$(deps_arch/arm/boot/dts/sun8iw12p1-soc.dtb):
