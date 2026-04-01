import shutil

# src = "F:\projects\CSEE\L_2025_Stm32H723ZGT6_Cmake\STM32H723XG_FLASH.ld"       # file you want to use
# dst = "F:\projects\CSEE\L_2025_Stm32H723ZGT6_Cmake\STM32H723XG_FLASH copy.ld"    # file to be replaced
src = "F:\projects\CSEE\L_2025_Stm32H723ZGT6_Cmake\STM32H723XG_FLASH.ld"       # file you want to use
dst = "F:\projects\CSEE\L_2025_Stm32H723ZGT6_Cmake\STM32H723XG_FLASH copy.ld"    # file to be replaced
temp = src
src = dst
dst = temp
shutil.copyfile(src, dst)

print("Linker script replaced successfully.")