#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <libserialport.h>
#include "config.h"
#include "deh_str.h"
#include "doomtype.h"
#include "doomkeys.h"
#include "i_joystick.h"
#include "i_system.h"
#include "i_swap.h"
#include "i_timer.h"
#include "i_video.h"
#include "i_scale.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "tables.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

typedef struct {
  uint8_t magic;
  uint8_t sequence_number;
  uint8_t unk;
  uint8_t message_type;
  uint16_t data_length;
  uint8_t crc;
} ipp_header;

typedef struct {
  char tag_id[7];
  char doom_keycode;
  char is_end;
} tag_info;

tag_info all_tags[] = {
  { .tag_id = { 0x04, 0x04, 0xcb, 0x65, 0x5f, 0x61, 0x80 }, .doom_keycode = 'w' },
  { .tag_id = { 0x04, 0x51, 0xcd, 0x65, 0x5f, 0x61, 0x80 }, .doom_keycode = 's' },
  { .is_end = 1 },
};

#define HEADER_LENGTH 7

int vanilla_keyboard_mapping = 1;

struct sp_port *nfc_port = NULL;

char current_message[65536 + 4 + 8] = { 0 };

char prev_key = 0;

int offset = 0;
/* Helper function for error handling. */
int check(enum sp_return result)
{
        /* For this example we'll just exit on any error by calling abort(). */
        char *error_message;
        switch (result) {
        case SP_ERR_ARG:
                printf("Error: Invalid argument.\n");
                abort();
        case SP_ERR_FAIL:
                error_message = sp_last_error_message();
                printf("Error: Failed: %s\n", error_message);
                sp_free_error_message(error_message);
                abort();
        case SP_ERR_SUPP:
                printf("Error: Not supported.\n");
                abort();
        case SP_ERR_MEM:
                printf("Error: Couldn't allocate memory.\n");
                abort();
        case SP_OK:
        default:
                return result;
        }
}

// returns whether we should keep trying to read
boolean nfc_read(void)
{
  if (nfc_port == NULL)
    return false;

  if (offset < HEADER_LENGTH) {
    int ret = check(sp_nonblocking_read(nfc_port, current_message + offset, HEADER_LENGTH - offset));
    if (ret == 0)
      return false;
    offset += ret;
  }
  if (offset >= HEADER_LENGTH) {
    ipp_header head;

    memcpy(current_message, &head, HEADER_LENGTH);

    int full_length = HEADER_LENGTH + head.data_length;

    int ret = check(sp_nonblocking_read(nfc_port, current_message + offset, full_length - offset));
    if (ret == 0)
      return false;
    offset += ret;

    if (offset >= full_length) {
      fprintf(stderr, "Got ipp message\n");
      offset = 0;
      // 0xbe = PICC Read
      if (head.message_type == 0xbe) {
        if (head.data_length != 21) {
          fprintf(stderr, "dunno how to read length %d", (int)head.data_length);
          return true;
        }
        char tag_id[7];
        for (int i=0; all_tags[i].is_end == 0; i++) {
          tag_info this_tag = all_tags[i];
          if (this_tag.tag_id == tag_id) {
            event_t ev;
            if (prev_key != 0) {
              ev.type = ev_keyup;
              ev.data1 = prev_key;
              ev.data2 = 0;
              D_PostEvent(&ev);
            }
            char this_keycode = this_tag.doom_keycode;
            prev_key = this_keycode;
            if (this_keycode != 0) {
              ev.type = ev_keydown;
              ev.data1 = this_tag.doom_keycode;
              ev.data2 = 0;
              D_PostEvent(&ev);
            }
            break;
          }
        }
        memcpy(current_message + HEADER_LENGTH + 11, tag_id, 7);
      }
      // TODO: do stuff with the message
      return true;
    }
  }
  return true;
}

void I_GetEvent(void)
{
  while (nfc_read()) {}
}

void I_InitInput(void)
{
  fprintf(stderr, "get port\n");
  check(sp_get_port_by_name("/dev/ttyS3", &nfc_port));
  fprintf(stderr, "open port\n");
  check(sp_open(nfc_port, SP_MODE_READ));
  fprintf(stderr, "setting stuffs\n");
  check(sp_set_baudrate(nfc_port, 115200));
  check(sp_set_bits(nfc_port, 8));
  check(sp_set_parity(nfc_port, SP_PARITY_NONE));
  check(sp_set_stopbits(nfc_port, 1));
  check(sp_set_flowcontrol(nfc_port, SP_FLOWCONTROL_NONE));
}

