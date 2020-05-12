#ifndef _PECCode_h
#define _PECCode_h

   //Code to Caculate the PEC code - Coppied from the Solo Code
  
  byte SMBUS_PEC_POLYNOME = 0x07;

  /// get_PEC - calculate packet error correction code of buffer
  byte get_PEC(uint8_t i2c_addr, uint8_t cmd, bool reading, const uint8_t buff[],
        uint8_t len) {
    // exit immediately if no data
    if (len <= 0) {
        return 0;
    }

    // prepare temp buffer for calcing crc
    uint8_t tmp_buff[len + 3];
    tmp_buff[0] = i2c_addr << 1;
    tmp_buff[1] = cmd;
    tmp_buff[2] = tmp_buff[0] | (uint8_t) reading;
    memcpy(&tmp_buff[3], buff, len);

    // initialise crc to zero
    uint8_t crc = 0;
    uint8_t shift_reg = 0;
    bool do_invert;

    // for each byte in the stream
    for (uint8_t i = 0; i < sizeof(tmp_buff); i++) {
        // load next data byte into the shift register
        shift_reg = tmp_buff[i];
        // for each bit in the current byte
        for (uint8_t j = 0; j < 8; j++) {
            do_invert = (crc ^ shift_reg) & 0x80;
            crc <<= 1;
            shift_reg <<= 1;
            if (do_invert) {
                crc ^= SMBUS_PEC_POLYNOME;
            }
        }
    }

    // return result
    return crc;
  }

   #endif
