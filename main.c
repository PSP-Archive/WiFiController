/*
 * Copyright (c) 2007 David Rudie
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <pspwlan.h>
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <psppower.h>
#include <arpa/inet.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <psputility.h>
#include <psputility_netparam.h>

#include "defines.h"

# ifdef PSPFW3
PSP_MODULE_INFO(MODULE_NAME, 0x0, 0, 4);
PSP_HEAP_SIZE_KB(1024*16);
# else
PSP_MODULE_INFO(MODULE_NAME, 0x1000, 0, 4);
# endif
//PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_MAIN_THREAD_ATTR(0);
PSP_MAIN_THREAD_STACK_SIZE_KB(32);

int done = 0;
char confname[128];
extern int connect_to_apctl(int config);

// --------------------------------------------------------------------------


int ExitCallback(int arg1, int arg2, void *common) {
  done = 1;
  sceKernelExitGame();
  return 0;
}

// --------------------------------------------------------------------------

int CallbackThread(SceSize args, void *argp) {
  int cbid;

  cbid = sceKernelCreateCallback("Exit Callback", ExitCallback, NULL);
  sceKernelRegisterExitCallback(cbid);
  sceKernelSleepThreadCB();

  return 0;
}

// --------------------------------------------------------------------------

int SetupCallbacks(void) {
  int thid = 0;

  //thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
  thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
  if (thid >= 0) {
    sceKernelStartThread(thid, 0, 0);
  }

  return thid;
}

// --------------------------------------------------------------------------

void ColorPrintF(int color, char *fmt, ...) {
  va_list ap;
  char buffer[256];
  int size;

  va_start(ap, fmt);
  size = vsnprintf(buffer, 256, fmt, ap);
  pspDebugScreenSetTextColor(color);
  pspDebugScreenPrintData(buffer, size);
  pspDebugScreenSetTextColor(WHITE);
  va_end(ap);
}

// --------------------------------------------------------------------------

void CenterPrintF(int color, int offset, char *fmt, ...) {
  va_list ap;
  char buffer[256];
  int size;

  int dx = SCR_WIDTH;
  int dy = SCR_HEIGHT;

  va_start(ap, fmt);
  size = vsnprintf(buffer, 256, fmt, ap);
  pspDebugScreenSetXY((dx - strlen(buffer)) / 2, (dy / 2) - offset);
  pspDebugScreenSetTextColor(color);
  pspDebugScreenPrintData(buffer, size);
  pspDebugScreenSetTextColor(WHITE);
  va_end(ap);
}

// --------------------------------------------------------------------------

int ConnectToClient(int controller) {
  int sock;
  struct sockaddr_in addr;
  int err;
  char buf[MAXPATHLEN];
  char file_path[128];
  char Buffer[512];
  char *Scan;
  FILE *CfgFile;
  char host[32];
  unsigned short port;
  unsigned short port1 = PORT1;
  unsigned short port2 = PORT2;
  unsigned short port3 = PORT3;
  unsigned short port4 = PORT4;
  int found = 0;
  // char temp[10];

  switch (controller) {
    case 1:
      port = port1;
      break;
    case 2:
      port = port2;
      break;
    case 3:
      port = port3;
      break;
    case 4:
      port = port4;
      break;
    default:
      port = port1;
  }

  pspDebugScreenClear();

  strcpy(file_path, getcwd(buf, MAXPATHLEN));
  strcat(file_path, "/wifi.cfg");

  printf("Reading configuration file: %s \n", file_path);

  CfgFile = fopen(file_path, "r");

  if (CfgFile != (FILE*)0) {
    while (fgets(Buffer, 512, CfgFile) != (char *)0) {
      Scan = strchr(Buffer, '\n');
      if (Scan) *Scan = '\0';
      Scan = strchr(Buffer, '\r');
      if (Scan) *Scan = '\0';

      if (Buffer[0] == '#') continue;

      if (!strncasecmp(Buffer, "address=", 8)) {
        printf("Found IP address!\n");
        found = 1;
        strcpy(host, Buffer + 8);
        printf("Setting IP address to connect to as %s.\n", host);
      }
    }
    fclose(CfgFile);
    if (found == 0) {
      ColorPrintF(RED, "Could not find IP address in configuration file!");
      done = 1;
    }
  }

  printf("Controller selection is %d, we will be connecting to port %d.\n", controller, port);

  sceKernelDelayThread(3000 * 1000);

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  sceNetInetInetAton(host, &addr.sin_addr);

  sock = sceNetInetSocket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    return -1;
  }

  err = sceNetInetConnect(sock, &addr, sizeof(addr));
  if (err) {
    return -1;
  }

  return sock;
}

// --------------------------------------------------------------------------

void read_pad(int controller) {
  int sock;
  int batteryLifeTime = 0;

  int dx = SCR_WIDTH;
  int dy = SCR_HEIGHT;

  int inverted = 0;

  SceCtrlData pad;

  unsigned int packetNum = 1;
  unsigned char packet[11];
  memset(packet, 0, sizeof(packet));

  sock = ConnectToClient(controller);
  if (sock < 0) {
    CenterPrintF(RED, 0, "Error opening socket!\n");
    return;
  }

  sceCtrlSetSamplingCycle(0);
  sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

  pspDebugScreenClear();

  while (!done) {
    if (sceWlanGetSwitchState() != 1) {
      connect_to_apctl(1);
      read_pad(controller);
    }
    sceCtrlReadBufferPositive(&pad, 1);

    if ((pad.Buttons & PSP_CTRL_LTRIGGER) && (pad.Buttons & PSP_CTRL_RTRIGGER) && (pad.Buttons & PSP_CTRL_SELECT)) {
      pspDebugScreenClear();
      if (inverted == 0) {
        pspDebugScreenSetXY( (dx - strlen(INVERTY)) / 2, dy / 2);
        ColorPrintF(RED, INVERTY);
        inverted = 1;
      } else if (inverted == 1) {
        pspDebugScreenSetXY( (dx - strlen(INVERTN)) / 2, dy / 2);
        ColorPrintF(RED, INVERTN);
        inverted = 0;
      }
      sceKernelDelayThread(2000 * 1000);
      pspDebugScreenClear();
    }

    pspDebugScreenSetXY((dx - (strlen(ANALOGX) + 3)) / 2, ((dy / 2) - 4));
    printf(ANALOGX "%d  \n", pad.Lx);
    pspDebugScreenSetXY((dx - (strlen(ANALOGY) + 3)) / 2, ((dy / 2) - 3));
    printf(ANALOGY "%d  \n", (inverted == 1 ? 255 - pad.Ly : pad.Ly));

    pspDebugScreenSetXY((dx - (strlen(LTRIGGER) + strlen(RTRIGGER))) / 2, ((dy / 2) - 2));
    ColorPrintF(pad.Buttons & PSP_CTRL_LTRIGGER ? BLUE : WHITE, LTRIGGER);
    ColorPrintF(pad.Buttons & PSP_CTRL_RTRIGGER ? BLUE : WHITE, RTRIGGER);

    pspDebugScreenSetXY((dx - (strlen(UP) + strlen(TRIANGLE))) / 2, ((dy / 2) - 1));
    ColorPrintF(pad.Buttons & PSP_CTRL_UP ? BLUE : WHITE, UP);
    ColorPrintF(pad.Buttons & PSP_CTRL_TRIANGLE ? BLUE : WHITE, TRIANGLE);

    pspDebugScreenSetXY((dx - (strlen(LEFT) + strlen(RIGHT) + strlen(SQUARE) + strlen(CIRCLE))) / 2, (dy / 2));
    ColorPrintF(pad.Buttons & PSP_CTRL_LEFT ? BLUE : WHITE, LEFT);
    ColorPrintF(pad.Buttons & PSP_CTRL_RIGHT ? BLUE : WHITE, RIGHT);
    ColorPrintF(pad.Buttons & PSP_CTRL_SQUARE ? BLUE : WHITE, SQUARE);
    ColorPrintF(pad.Buttons & PSP_CTRL_CIRCLE ? BLUE : WHITE, CIRCLE);

    pspDebugScreenSetXY((dx - (strlen(DOWN) + strlen(CROSS))) / 2, ((dy / 2) + 1));
    ColorPrintF(pad.Buttons & PSP_CTRL_DOWN ? BLUE : WHITE, DOWN);
    ColorPrintF(pad.Buttons & PSP_CTRL_CROSS ? BLUE : WHITE, CROSS);

    pspDebugScreenSetXY((dx - (strlen(SELECT) + strlen(START))) / 2, ((dy / 2) + 2));
    ColorPrintF(pad.Buttons & PSP_CTRL_SELECT ? BLUE : WHITE, SELECT);
    ColorPrintF(pad.Buttons & PSP_CTRL_START ? BLUE : WHITE, START);

    if (scePowerIsBatteryExist()) {
      batteryLifeTime = scePowerGetBatteryLifeTime();
      pspDebugScreenSetXY(0, (dy - 1));

      if (scePowerIsPowerOnline()) {
        printf("Running on external power...                                            ");
      } else {
        printf("Battery - Life: ");
        ColorPrintF(scePowerIsLowBattery() ? RED : WHITE, "%d", scePowerGetBatteryLifePercent());
        printf("%% (%02dh%02dm) / Temperature: %d C / Voltage: %0.3fV        ",
          batteryLifeTime / 60,
          batteryLifeTime - (batteryLifeTime / 60 * 60),
          scePowerGetBatteryTemp(),
          (float) scePowerGetBatteryVolt() / 1000.0);
      }
    }

    /* The code below was written by Mikko Monone. */
    unsigned char packet[11];
    packet[0] = '!';
    packet[1] = 'K';
    packet[2] = (packetNum) & 0xff;
    packet[3] = (packetNum >> 8) & 0xff;
    packet[4] = (packetNum >> 16) & 0xff;
    packet[5] = (packetNum >> 24) & 0xff;
    packet[6] = pad.Lx;
    packet[7] = (inverted == 1 ? 255 - pad.Ly : pad.Ly);
    packet[8] = (pad.Buttons) & 0xff;
    packet[9] = (pad.Buttons >> 8) & 0xff;
    packet[10] = (pad.Buttons >> 16) & 0xff;
    packetNum++;
    /* The code above was written by Mikko Monone. */
    sceNetInetSend(sock, packet, sizeof(packet), 0);
    sceDisplayWaitVblankStart();
  }
}

// --------------------------------------------------------------------------

int connect_to_apctl(int config) {
  int err;
  int stateLast = -1;
  char *stateinfo;

  if (sceWlanGetSwitchState() != 1)
    pspDebugScreenClear();

  while (sceWlanGetSwitchState() != 1) {
    pspDebugScreenSetXY(0, 0);
    ColorPrintF(RED, "Please enable WLAN to continue.\n");
    sceKernelDelayThread(1000 * 1000);
  }

  err = sceNetApctlConnect(config);
  if (err != 0) {
    CenterPrintF(RED, 0, MODULE_NAME ": sceNetApctlConnect returns %08X\n", err);
    return 0;
  }

  printf(MODULE_NAME ": Connecting to Access Point.\n");
  while (1) {
    int state;
    err = sceNetApctlGetState(&state);
    if (err != 0) {
      CenterPrintF(RED, 0, MODULE_NAME ": sceNetApctlGetState returns $%x\n", err);
      break;
    }
    if (state > stateLast) {
      switch (state) {
        case 0:
          stateinfo = "Idle";
          break;
        case 1:
          stateinfo = "Starting up";
          break;
        case 2:
          stateinfo = "Starting up";
          break;
        case 3:
          stateinfo = "Obtaining IP address";
          break;
        case 4:
          stateinfo = "IP address obtained";
          break;
        default:
          stateinfo = "Idle";
      }
      printf("  Connection state %d of 4: %s.\n", state, stateinfo);
      stateLast = state;
    }
    if (state == 4) {
      break;
    }

    sceKernelDelayThread(50 * 1000);
  }
  printf(MODULE_NAME ": Connected to Access Point!\n");
  sceKernelDelayThread(2000 * 1000);

  if (err != 0) {
    return 0;
  }

  return 1;
}

// --------------------------------------------------------------------------

char *getconfname(int confnum) {
  sceUtilityGetNetParam(confnum, PSP_NETPARAM_NAME, (netData *)confname);
  return confname;
}

// --------------------------------------------------------------------------

int net_thread(SceSize args, void *argp) {
  int err;
  int i;
  int start = 1;
  int selComponent = 1;
  int selController = 1;
  int select = 1;
  int last = 0;
  int lastC = 0;

  SceCtrlData pad;
  sceCtrlSetSamplingCycle(0);
  sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
  
  pspDebugScreenClear();

  do {
    if ((err = pspSdkInetInit())) {
      CenterPrintF(RED, 0, MODULE_NAME ": Error, could not initialize the network.  Error Code: %08X\n", err);
      break;
    }

    while (sceUtilityCheckNetParam(start) == 0) {
      start++;
    }
    start--;

    pspDebugScreenSetXY(0, 0);
    printf("%d internet configuration(s) found.\n\n", start);
    printf("Select which configuration to use from\n");
    printf("the list and press either %s or %s.\n", NSCROSS, NSCIRCLE);
    i = 1;
    while (i <= start) {
      printf("  Configuration %d: %s\n", i, getconfname(i));
      i++;
    }

    while (select) {
      sceCtrlReadBufferPositive(&pad, 1);
      if (pad.Buttons & PSP_CTRL_UP) {
        if (selComponent > 1) {
          selComponent--;
        }
        sceKernelDelayThread(100 * 1000);
      } else if (pad.Buttons & PSP_CTRL_DOWN) {
        if (selComponent < start) {
          selComponent++;
        }
        sceKernelDelayThread(100 * 1000);
      }
      if ((pad.Buttons & PSP_CTRL_CIRCLE) || (pad.Buttons & PSP_CTRL_CROSS)) {
        select = 0;
      }
      if (last != selComponent) {
        if (last == 0) { last = 1; }
        pspDebugScreenSetXY(0, 3 + last);
        ColorPrintF(WHITE, "  Configuration %d: %s", last, getconfname(last));

        pspDebugScreenSetXY(0, 3 + selComponent);
        ColorPrintF(RED, "  Configuration %d: %s", selComponent, getconfname(selComponent));
      }
      last = selComponent;
      sceDisplayWaitVblankStart();
    }

    pspDebugScreenSetXY(0, 4 + start);

    printf("\nUsing connection %d (%s) to connect...\n\n", selComponent, getconfname(selComponent));

    sceKernelDelayThread(1000 * 1000);

    pspDebugScreenClear();

    select = 1;

    pspDebugScreenSetXY(0, 0);
    printf("Select which controller you will be from\n");
    printf("the list and press either %s or %s.\n", NSCROSS, NSCIRCLE);
    i = 1;
    while (i <= 4) {
      printf("  Controller %d\n", i);
      i++;
    }

    while (select) {
      sceCtrlReadBufferPositive(&pad, 1);
      if (pad.Buttons & PSP_CTRL_UP) {
        if (selController > 1) {
          selController--;
        }
        sceKernelDelayThread(100 * 1000);
      } else if (pad.Buttons & PSP_CTRL_DOWN) {
        if (selController < 4) {
          selController++;
        }
        sceKernelDelayThread(100 * 1000);
      }
      if ((pad.Buttons & PSP_CTRL_CIRCLE) || (pad.Buttons & PSP_CTRL_CROSS)) {
        select = 0;
      }
      if (lastC != selController) {
        if (lastC == 0) { lastC = 1; }
        pspDebugScreenSetXY(0, 1 + lastC);
        ColorPrintF(WHITE, "  Controller %d", lastC);

        pspDebugScreenSetXY(0, 1 + selController);
        ColorPrintF(RED, "  Controller %d", selController);
      }
      lastC = selController;
      sceDisplayWaitVblankStart();
    }

    pspDebugScreenSetXY(0, 6);

    printf("\nActing as controller %d\n\n", selController);

    sceKernelDelayThread(1000 * 1000);

    pspDebugScreenClear();

    if (connect_to_apctl(selComponent)) {
      char szMyIPAddr[32];
      if (sceNetApctlGetInfo(8, szMyIPAddr) != 0)
        strcpy(szMyIPAddr, "unknown IP address");

      read_pad(selController);
    }
  }
  while (0);

  return 0;
}

// --------------------------------------------------------------------------

int main(void) {
  SceUID thid;

  SetupCallbacks();

  pspDebugScreenInit();

  CenterPrintF(WHITE, 1, "WiFiController v0.4.4");
  CenterPrintF(WHITE, 0, "Copyright (c)2007 David Rudie");
  sceKernelDelayThread(2000 * 1000);
  pspDebugScreenClear();

# ifdef PSPFW3
  sceUtilityLoadNetModule(1);
  sceUtilityLoadNetModule(3);
# else
  if (pspSdkLoadInetModules() < 0) {
    CenterPrintF(RED, 0, "Error!  inet modules could not be loaded!\n");
    sceKernelSleepThread();
  }
# endif

  thid = sceKernelCreateThread("net_thread", net_thread, 0x18, 0x10000, PSP_THREAD_ATTR_USER, NULL);
  if (thid < 0) {
    CenterPrintF(RED, 0, "Error! Thread could not be created!\n");
    sceKernelSleepThread();
  }

  sceKernelStartThread(thid, 0, NULL);

  sceKernelExitDeleteThread(0);

  return 0;
}
