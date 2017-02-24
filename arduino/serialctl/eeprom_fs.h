//Nomenclature is FS_<parameter_name>_<number of bytes>
#define EEPROM_BASE 0
#define EEPROM_OFFSET_D(x) (EEPROM_BASE + (x)*sizeof(double))
#define FS_LEFT_P_4 EEPROM_OFFSET_D(0)
#define FS_LEFT_I_4 EEPROM_OFFSET_D(1)
#define FS_LEFT_D_4 EEPROM_OFFSET_D(2)
#define FS_RIGHT_P_4 EEPROM_OFFSET_D(3)
#define FS_RIGHT_I_4 EEPROM_OFFSET_D(4)
#define FS_RIGHT_D_4 EEPROM_OFFSET_D(5)
