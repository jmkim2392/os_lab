

Clear-Host;

$exec=$args[0]
$ErrorActionPreference = "Stop"

& "C:\Program Files (x86)\GNU Tools ARM Embedded\8 2018-q4-major\bin\arm-none-eabi-gcc.exe" `
	-mcpu=cortex-a7 `
	-fpic `
	-ffreestanding `
  --specs=nosys.specs `
	-std=gnu99 `
	-T linker.ld  `
	src/myApp/myApp.c `
  -o output/myApp.elf `
	-O0	`
	-lc -lm -lg -lgcc