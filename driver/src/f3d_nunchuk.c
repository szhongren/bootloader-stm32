/*
 * f3d_nunchuk.c
 *
 * Part Of: C335 Lab 9
 *
 * Last Edited By: Erin Leonhard (eeleonha), Zhongren Shao (shaoz)
 * Last Edited Date: 10/31/14
 * 
 * The definitions below read from the nunchuk and check for different
 * variables like the buttons being pressed or the joystick being moved.
*/


#include <f3d_i2c.h>
#include <f3d_nunchuk.h>
#include <stdio.h>

#define NUNCHUK_ADDRESS 0xA4

#define NUN_X_MID 140
#define NUN_Y_MID 137

void f3d_nunchuk_init(void) {
  uint8_t realb = 0x00;
  uint8_t buf = 0x55;
  uint8_t buf2 = 0x00;


  // Use for factory Wii Nunchuk controllers
  // f3d_i2c1_write(NUNCHUK_ADDRESS,0x40,&realb);
  // Use for 3 party nunchuks like we have in the lab
  f3d_i2c1_write(NUNCHUK_ADDRESS,0xf0,&buf); 
  f3d_i2c1_write(NUNCHUK_ADDRESS,0xfb,&buf2);
}

void f3d_nunchuk_read(nunchuk_t *n) {
  uint8_t data[6] = {0,0,0,0,0,0};
  int i;
  unsigned short tmp;

  f3d_i2c1_read_nunchuk(0xA5, data, 6);
  n->jx = data[0];
  n->jy = data[1];

  tmp = data[2];
  tmp = tmp << 2;
  n->ax = tmp | ((data[5] & 0x0C) >> 2);

  tmp = data[3];
  tmp = tmp << 2;
  n->ay = tmp | ((data[5] & 0x30) >> 4);

  tmp = data[4];
  tmp = tmp << 2;
  n->az = tmp | ((data[5] & 0xC0) >> 6);

  switch (data[5] & 0x03) {
  case 2:
    n->z = 1;
    n->c = 0;
    break;
  case 1:
    n->z = 0;
    n->c = 1;
    break;
  case 0:
    n->z = 1;
    n->c = 1;
    break;
  case 3:
    n->z = 0;
    n->c = 0;
    break;
  }
  //  printf("n->c=%d n->z=%d n->jx=%d n->jy=%d n->ax=%d n->ay=%d n->az=%d\n",n->c,n->z,n->jx,n->jy,n->ax,n->ay,n->az);
}

// prints out the data from the nunchuk
void f3d_nunchuk_display_data(nunchuk_t *nun_data) {
  printf("jx: %u\n", nun_data->jx);
  printf("jy: %u\n", nun_data->jy);
  printf("ax: %u\n", nun_data->ax);
  printf("ay: %u\n", nun_data->ay);
  printf("az: %u\n", nun_data->az);
  printf("z: %u\n", nun_data->z);
  printf("c: %u\n", nun_data->c);
}

// is the c button pressed
int f3d_c_pressed(nunchuk_t *nun_data) {
  return nun_data->c;
}

// is the z button pressed
int f3d_z_pressed(nunchuk_t *nun_data) {
  return nun_data->z;
}

// has the joystick been moved right
int f3d_j_right(nunchuk_t *nun_data) {
  return nun_data->jx > NUN_X_MID + 30;
}

// has the joystick been moved left
int f3d_j_left(nunchuk_t *nun_data) {
  return nun_data->jx < NUN_X_MID - 30;
}

// has the joystick been moved up
int f3d_j_up(nunchuk_t *nun_data) {
  return nun_data->jy > NUN_Y_MID + 70;
}

// has the joystick been moved down
int f3d_j_down(nunchuk_t *nun_data) {
  return nun_data->jy < NUN_Y_MID - 70;
}

// checks for the presence of certain variables with the
// nunchuk to know if the mode should change
int f3d_nunchuk_change_mode(nunchuk_t *nun_data) {
  if (f3d_c_pressed(nun_data) || f3d_j_right(nun_data) || f3d_j_down(nun_data))
    return 1;
  else if (f3d_z_pressed(nun_data) || f3d_j_left(nun_data) || f3d_j_up(nun_data))
    return -1;
  else
    return 0;
}
/* f3d_nunchuk.c ends here */
