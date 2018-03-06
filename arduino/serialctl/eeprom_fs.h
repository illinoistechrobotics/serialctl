//Nomenclature is FS_<parameter_name>_<number of bytes>
#define EEPROM_BASE 0
#define EEPROM_OFFSET_D(x) (EEPROM_BASE + (x)*sizeof(double))
#define FS_0_P_4 EEPROM_OFFSET_D(0)
#define FS_0_I_4 EEPROM_OFFSET_D(1)
#define FS_0_D_4 EEPROM_OFFSET_D(2)
#define FS_120_P_4 EEPROM_OFFSET_D(3)
#define FS_120_I_4 EEPROM_OFFSET_D(4)
#define FS_120_D_4 EEPROM_OFFSET_D(5)
#define FS_240_P_4 EEPROM_OFFSET_D(6)
#define FS_240_I_4 EEPROM_OFFSET_D(7)
#define FS_240_D_4 EEPROM_OFFSET_D(8)
