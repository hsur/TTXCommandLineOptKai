#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "tt_res.h"
#include <stdlib.h>
#include <stdio.h>

#include "compat_w95.h"

#define ORDER 5800

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct {
  PTTSet ts;
  PComVar cv;
  PParseParam origParseParam;
} TInstVar;

static TInstVar FAR * pvar;
static TInstVar InstVar;

static void PASCAL FAR TTXInit(PTTSet ts, PComVar cv) {
  pvar->ts = ts;
  pvar->cv = cv;
}

BOOL ColorStr2ColorRef(COLORREF *color, PCHAR Str) {
	static enum ColorPreset {
		R = 64,
		G = 16384,
		B = 6291456,
		M = 6291520,
		C = 6307840,
		Y = 16448,
		K = 0,
		S = 4210752
	} COLOR_PRESET;
  int TmpColor[3];
  int i, result;
  PCHAR cur, next, code;

  cur = Str;

  for (i=0; i<3; i++) {
    if (!cur)
      return FALSE;

    if ((next = strchr(cur, ',')) != NULL)
      *next = 0;

    result = sscanf_s(cur, "%d", &TmpColor[i]);

    if (next)
      *next++ = ',';

	if (result != 1 || TmpColor[i] < 0 || TmpColor[i] > 255) {
		result = sscanf_s(cur, "%c", &code, (unsigned)sizeof(code));
		if (result == 1) {
			if (_strnicmp(code, "R", 1) == 0) {
				*color = R;
				return TRUE;
			}
			else if (_strnicmp(code, "G", 1) == 0) {
				*color = G;
				return TRUE;
			}
			else if (_strnicmp(code, "B", 1) == 0) {
				*color = B;
				return TRUE;
			}
			else if (_strnicmp(code, "C", 1) == 0) {
				*color = C;
				return TRUE;
			}
			else if (_strnicmp(code, "M", 1) == 0) {
				*color = M;
				return TRUE;
			}
			else if (_strnicmp(code, "Y", 1) == 0) {
				*color = Y;
				return TRUE;
			}
			else if (_strnicmp(code, "K", 1) == 0) {
				*color = K;
				return TRUE;
			}
			else if (_strnicmp(code, "S", 1) == 0) {
				*color = S;
				return TRUE;
			}
			else {
				return FALSE;
			}
		}
	}

    cur = next;
  }

  *color = RGB((BYTE)TmpColor[0], (BYTE)TmpColor[1], (BYTE)TmpColor[2]);
  return TRUE;
}

static void PASCAL FAR TTXParseParam(PCHAR Param, PTTSet ts, PCHAR DDETopic) {
  char buff[1024];
  PCHAR start, cur, next;
  int x, y;

  /* the first term shuld be executable filename of Tera Term */
  start = GetParam(buff, sizeof(buff), Param);

  cur = start;
  while (next = GetParam(buff, sizeof(buff), cur)) {
    if (_strnicmp(buff, "/FG=", 4) == 0) {
      ColorStr2ColorRef(&(ts->VTColor[0]), &buff[4]);
	  memset(cur, ' ', next - cur);
    }
    else if (_strnicmp(buff, "/BG=", 4) == 0) {
      ColorStr2ColorRef(&(ts->VTColor[1]), &buff[4]);
	  ColorStr2ColorRef(&(ts->VTBoldColor[1]), &buff[4]);
	  ColorStr2ColorRef(&(ts->VTBlinkColor[1]), &buff[4]);
	  ColorStr2ColorRef(&(ts->URLColor[1]), &buff[4]);
	  ColorStr2ColorRef(&(ts->VTReverseColor[0]), &buff[4]);
	  ts->VTReverseColor[1] = ts->VTColor[0];
      memset(cur, ' ', next - cur);
    }
    else if (_strnicmp(buff, "/SIZE=", 6) == 0) {
      if (sscanf_s(buff+6, "%dx%d", &x, &y) == 2 || sscanf_s(buff+6, "%d,%d", &x, &y) == 2) {
	ts->TerminalWidth = x;
	ts->TerminalHeight = y;
      }
      memset(cur, ' ', next - cur);
    }
    cur = next;
  }

  pvar->origParseParam(Param, ts, DDETopic);
}

static void PASCAL FAR TTXGetSetupHooks(TTXSetupHooks FAR *hooks) {
  pvar->origParseParam = *hooks->ParseParam;
  *hooks->ParseParam = TTXParseParam;
}

static TTXExports Exports = {
  sizeof(TTXExports),
  ORDER,

  TTXInit,
  NULL, // TTXGetUIHooks,
  TTXGetSetupHooks,
  NULL, // TTXOpenTCP,
  NULL, // TTXCloseTCP,
  NULL, // TTXSetWinSize,
  NULL, // TTXModifyMenu,
  NULL, // TTXModifyPopupMenu,
  NULL, // TTXProcessCommand,
  NULL, // TTXEnd
};

BOOL __declspec(dllexport) PASCAL FAR TTXBind(WORD Version, TTXExports FAR * exports) {
  int size = sizeof(Exports) - sizeof(exports->size);

  if (size > exports->size) {
    size = exports->size;
  }
  memcpy((char FAR *)exports + sizeof(exports->size),
    (char FAR *)&Exports + sizeof(exports->size),
    size);
  return TRUE;
}

BOOL WINAPI DllMain(HANDLE hInstance,
		    ULONG ul_reason_for_call,
		    LPVOID lpReserved)
{
  switch( ul_reason_for_call ) {
    case DLL_THREAD_ATTACH:
      /* do thread initialization */
      break;
    case DLL_THREAD_DETACH:
      /* do thread cleanup */
      break;
    case DLL_PROCESS_ATTACH:
      /* do process initialization */
      DoCover_IsDebuggerPresent();
      hInst = hInstance;
      pvar = &InstVar;
      break;
    case DLL_PROCESS_DETACH:
      /* do process cleanup */
      break;
  }
  return TRUE;
}
