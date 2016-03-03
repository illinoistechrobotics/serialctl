#include <util/crc16.h>
/* http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html
 * uint8_t serno[] = { 0x02, 0x1c, 0xb8, 0x01, 0, 0, 0, 0xa2 };
 * int
 * checkcrc(void)
 * {
 * uint8_t crc = 0, i;
 * for (i = 0; i < sizeof serno / sizeof serno[0]; i++)
 * crc = _crc_ibutton_update(crc, serno[i]);
 * return crc; // must be 0
 * 
 */
// C implemtation for computer
uint16_t compute_crc(char *data, int len){
        uint16_t crc = 0xffff;
        int i;
        for(i=0; i<len; i++){
                crc=_crc16_update(crc,data[i]);
        }
        return crc;
}
