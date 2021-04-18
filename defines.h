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

#define printf pspDebugScreenPrintf

#define MODULE_NAME "WifiController"

#define PORT1 20001
#define PORT2 20002
#define PORT3 20003
#define PORT4 20004

#define SCR_WIDTH  96
#define SCR_HEIGHT 27

// #define MAXPATHLEN 512

#define WHITE    0xffffff
#define RED      0x6666ff
#define BLUE     0xff6666

#define ANALOGX  "Analog X = "
#define ANALOGY  "Analog Y = "
#define LTRIGGER "Çk"
#define RTRIGGER "                  Çq"
#define UP       "Å™"
#define LEFT     "Å©"
#define RIGHT    "  Å®"
#define DOWN     "Å´"
#define TRIANGLE "                  Å¢"
#define SQUARE   "              Å†"
#define CIRCLE   "  Åõ"
#define CROSS    "                  Å~"
#define SELECT   "SELECT"
#define START    "   START"
#define NSCIRCLE "Åõ"
#define NSCROSS  "Å~"

#define INVERTY  "Analog Y is now inverted!"
#define INVERTN  "Analog Y is no longer inverted!"
