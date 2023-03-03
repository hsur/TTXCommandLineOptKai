#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "tt_res.h"
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#define ORDER 5800

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct {
  PTTSet ts;
  PComVar cv;
  PParseParam origParseParam;
} TInstVar;

static TInstVar *pvar;
static TInstVar InstVar;

static void PASCAL TTXInit(PTTSet ts, PComVar cv) {
  pvar->ts = ts;
  pvar->cv = cv;
}

static BOOL ColorStr2ColorRef(COLORREF *color, wchar_t *Str) {
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
  wchar_t *cur, *next, code[2];

  cur = Str;

  for (i=0; i<3; i++) {
    if (!cur)
      return FALSE;

    if ((next = wcsrchr(cur, L',')) != NULL)
      *next = 0;

    result = swscanf_s(cur, L"%d", &TmpColor[i]);

    if (next)
      *next++ = ',';

	if (result != 1 || TmpColor[i] < 0 || TmpColor[i] > 255) {
		result = swscanf_s(cur, L"%s", &code, (unsigned)sizeof(code));
		if (result == 1) {
			if (_wcsnicmp(code, L"R", 1) == 0) {
				*color = R;
				return TRUE;
			}
			else if (_wcsnicmp(code, L"G", 1) == 0) {
				*color = G;
				return TRUE;
			}
			else if (_wcsnicmp(code, L"B", 1) == 0) {
				*color = B;
				return TRUE;
			}
			else if (_wcsnicmp(code, L"C", 1) == 0) {
				*color = C;
				return TRUE;
			}
			else if (_wcsnicmp(code, L"M", 1) == 0) {
				*color = M;
				return TRUE;
			}
			else if (_wcsnicmp(code, L"Y", 1) == 0) {
				*color = Y;
				return TRUE;
			}
			else if (_wcsnicmp(code, L"K", 1) == 0) {
				*color = K;
				return TRUE;
			}
			else if (_wcsnicmp(code, L"S", 1) == 0) {
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

static void PASCAL TTXParseParam(wchar_t *Param, PTTSet ts, PCHAR DDETopic) {
  wchar_t buff[1024];
  wchar_t *start, *cur, *next;
  int x, y;

  /* the first term shuld be executable filename of Tera Term */
  start = GetParam(buff, sizeof(buff), Param);

  cur = start;
  while (next = GetParam(buff, sizeof(buff), cur)) {
    if (_wcsnicmp(buff, L"/FG=", 4) == 0) {
      ColorStr2ColorRef(&(ts->VTColor[0]), &buff[4]);
      wmemset(cur, ' ', next - cur);
    }
    else if (_wcsnicmp(buff, L"/BG=", 4) == 0) {
      ColorStr2ColorRef(&(ts->VTColor[1]), &buff[4]);
      wmemset(cur, ' ', next - cur);
    }
    else if (_wcsnicmp(buff, L"/SIZE=", 6) == 0) {
      if (swscanf_s(buff+6, L"%dx%d", &x, &y) == 2 || swscanf_s(buff+6, L"%d,%d", &x, &y) == 2) {
        ts->TerminalWidth = x;
        ts->TerminalHeight = y;
      }
      wmemset(cur, ' ', next - cur);
    }
    cur = next;
  }

  pvar->origParseParam(Param, ts, DDETopic);
}

static void PASCAL TTXGetSetupHooks(TTXSetupHooks *hooks) {
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

BOOL __declspec(dllexport) PASCAL TTXBind(WORD Version, TTXExports *exports) {
  int size = sizeof(Exports) - sizeof(exports->size);

  if (size > exports->size) {
    size = exports->size;
  }
  memcpy((char *)exports + sizeof(exports->size),
    (char *)&Exports + sizeof(exports->size),
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
      hInst = hInstance;
      pvar = &InstVar;
      break;
    case DLL_PROCESS_DETACH:
      /* do process cleanup */
      break;
  }
  return TRUE;
}
