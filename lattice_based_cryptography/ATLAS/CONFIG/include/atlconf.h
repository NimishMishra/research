#ifndef ATLCONF_H
   #define ATLCONF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NOS 13
static char *osnam[NOS] =
   {"UNKNOWN", "Linux", "SunOS", "SunOS4", "OSF1", "IRIX", "AIX",
    "Win9x", "WinNT", "Win64", "HPUX", "FreeBSD", "OSX"};
enum OSTYPE {OSOther=0, OSLinux, OSSunOS, OSSunOS4, OSOSF1, OSIRIX, OSAIX,
             OSWin9x, OSWinNT, OSWin64, OSHPUX, OSFreeBSD, OSOSX};
#define OSIsWin(OS_) ( ((OS_) == OSWinNT) || ((OS_) == OSWin9x) || \
                       ((OS_) == OSWin64) )

enum ARCHFAM {AFOther=0, AFPPC, AFSPARC, AFALPHA, AFX86, AFIA64, AFMIPS,
              AFARM, AFS390};

/*
 * Corei1: Nahalem / Westmere
 * Corei2: ivy bridge, sandy bridge: AVX
 * Corei3: haswell: AVXMAC
 * Corei3EP: v3 Haswell, E5-26XX
 * Corei4: skylake
 */
#define NMACH 62
static char *machnam[NMACH] =
   {"UNKNOWN", "PPCG4", "PPCG5", "POWER3", "POWER4", "POWER5",
    "POWER6", "POWER7", "POWER8", "POWERe6500",
    "IBMz9", "IBMz10", "IBMz196", "IBMz12", "IBMz13",
    "x86x87", "x86SSE1", "x86SSE2", "x86SSE3",
    "P5", "P5MMX", "PPRO", "PII", "PIII", "PM", "CoreSolo",
    "CoreDuo", "Core2Solo", "Core2", "Corei1", "Corei2", "Corei3",
    "Corei4", "Atom", "P4", "P4E",
    "Efficeon", "K7", "HAMMER", "AMD64K10h", "AMDLLANO", "AMDDOZER","AMDDRIVER",
    "UNKNOWNx86", "IA64Itan", "IA64Itan2",
    "USI", "USII", "USIII", "USIV", "UST1", "UST2", "UnknownUS",
    "MIPSR1xK", "MIPSICE9",
    "ARMa7", "ARMa9", "ARMa15", "ARMa17",
    "ARM64xgene1", "ARM64a53", "ARM64a57"};
enum MACHTYPE {MACHOther, PPCG4, PPCG5, IbmPwr3, IbmPwr4, IbmPwr5,
               IbmPwr6, IbmPwr7, IbmPwr8, Pwre6500,
               IbmZ9, IbmZ10, IbmZ196, IbmZ12, IbmZ13, /* s390(x) in Linux */
               x86x87, x86SSE1, x86SSE2, x86SSE3, /* generic targets */
               IntP5, IntP5MMX, IntPPRO, IntPII, IntPIII, IntPM, IntCoreS,
               IntCoreDuo, IntCore2Solo, IntCore2, IntCorei1, IntCorei2,
               IntCorei3, IntCorei4, IntAtom, IntP4, IntP4E, TMEff,
               AmdAthlon, AmdHammer, Amd64K10h, AmdLlano, AmdDozer, AmdDriver,
               x86X, IA64Itan, IA64Itan2,
               SunUSI, SunUSII, SunUSIII, SunUSIV, SunUST1, SunUST2, SunUSX,
               MIPSR1xK, /* includes R10K, R12K, R14K, R16K */
               MIPSICE9,  /* SiCortex ICE9 -- like MIPS5K */
               ARM7,      /* odroid-little */
               ARM9,      /* pandaboard */
               ARM15,     /* tegra, odroid-big */
               ARM17,     /* lots of tablets */
               ARM64xg,     /* includes ARMv8 */
               ARM64a53,
               ARM64a57
               };
#define MachIsX86(mach_) \
   ( (mach_) >= x86x87 && (mach_) <= x86X )
#define MachIsPWR(mach_) \
   ( (mach_) >= IbmPwr3 && (mach_) <= Pwre6500 )
#define MachIsIA64(mach_) \
   ( (mach_) >= IA64Itan && (mach_) <= IA64Itan2 )
#define MachIsUS(mach_) \
   ( (mach_) >= SunUSI && (mach_) <= SunUSX )
#ifdef __mips__
   #define MachIsMIPS(mach_) \
      ( (__mips__) || (mach_) >= MIPSR1xK && (mach_) <= MIPSICE9 )
#else
   #define MachIsMIPS(mach_) \
      ( (mach_) >= MIPSR1xK && (mach_) <= MIPSICE9 )
#endif
#define MachIsPPC(mach_) \
   ( (mach_) >= PPCG4 && (mach_) <= PPCG5 )
#define MachIsARM(mach_) \
   ( (mach_) >= ARM7 && (mach_) <= ARM17 )
#define MachIsARM64(mach_) \
   ( (mach_) >= ARM64xg && || (mach_) <= ARM64a57)
#define MachIsS390(mach_) \
   ( (mach_) >= IbmZ9 && (mach_) <= IbmZ13 )


static char *f2c_namestr[5] = {"UNKNOWN","Add_", "Add__", "NoChange", "UpCase"};
static char *f2c_intstr[5] =
       {"UNKNOWN", "F77_INTEGER=int", "F77_INTEGER=long",
        "F77_INTEGER=\"long long\"", "F77_INTEGER=short"};
static char *f2c_strstr[5]=
       {"UNKNOWN", "SunStyle", "CrayStyle", "StructVal", "StructPtr"};

enum F2CNAME {f2c_NamErr=0, f2c_Add_, f2c_Add__, f2c_NoChange, f2c_UpCase};
enum F2CINT {f2c_IntErr=0, FintCint, FintClong, FintClonglong, FintCshort};
enum F2CSTRING {f2c_StrErr=0, fstrSun, fstrCray, fstrStructVal, fstrStructPtr};

#define NISA 15
static char *ISAXNAM[NISA] =
   {"", "VSX", "VXZ", "AltiVec",
    "AVXMAC", "AVXFMA4", "AVX", "SSE3", "SSE2", "SSE1", "3DNow",
    "FPV3D2MACNEON", "FPV3D16MACNEON", "FPV3D32MAC", "FPV3D16MAC"};
enum ISAEXT
   {ISA_None=0, ISA_VSX, ISA_VXZ, ISA_AV,
    ISA_AVXMAC, ISA_AVXFMA4, ISA_AVX, ISA_SSE3, ISA_SSE2, ISA_SSE1, ISA_3DNow,
    ISA_NEON, ISA_NEON16, ISA_VFP3D32MAC, ISA_VFP3D16MAC};

#define NASMD 11
enum ASMDIA
   {ASM_None=0, gas_x86_32, gas_x86_64, gas_sparc, gas_ppc, gas_parisc,
    gas_mips, gas_arm, gas_arm64, gas_wow64, gas_s390};
static char *ASMNAM[NASMD] =
   {"",     "GAS_x8632", "GAS_x8664", "GAS_SPARC", "GAS_PPC", "GAS_PARISC",
    "GAS_MIPS", "GAS_ARM", "GAS_ARM64", "GAS_WOW64", "GAS_S390"};

/*
 * Used for archinfo probes (can pack in bitfield)
 */
enum WHATPROBE{Parch=1, P64=2, Pncpu=4, Pverb=8, Pncache=16, PCacheSize=32,
               PMhz=64, Pthrottle=128};

#define NARDEF 4
enum ARDEF{ADsk=0, ADdk, ADsm, ADdm};  /* m = matmul kernel, k = non-mm kern */
/*
 * Used for all the compilers ATLAS needs
 */
#define NCOMP 8
static char *COMPNAME[NCOMP]={"ICC","SMC","DMC","SKC","DKC","XCC","GCC","F77"};
#define ICC_ 0   /* Compiles non-computation routines, and all I/O */
#define SMC_ 1   /* single prec matmul compiler */
#define DMC_ 2   /* double prec matmul compiler */
#define SKC_ 3   /* single prec computation compiler (non-mm kernels) */
#define DKC_ 4   /* double prec computation compiler */
#define XCC_ 5   /* Compiler for frontend of cross-compilation */
#define GCC_ 6
#define F77_ 7   /* Valid fixed-format Fortran77 compiler */

typedef struct CompNode COMPNODE;
struct CompNode
{
   int priority;              /* priority of this definition */
   int comps[1];              /* bitfield: (1<<ICC)|...|(1<<F77) */
   int OS[(NOS+31)/32];       /* bitfield for OS */
   int arch[(NMACH+31)/32];   /* bitfields for architecture */
   char *comp, *flags;        /* compiler & flags as strings */
   COMPNODE *next;
};
#include "atlconf_misc.h"

#endif
