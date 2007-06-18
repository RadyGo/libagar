/*	Public domain	*/

#ifndef _AGAR_WIDGET_LABEL_H_
#define _AGAR_WIDGET_LABEL_H_

#ifdef _AGAR_INTERNAL
#include <gui/widget.h>
#else
#include <agar/gui/widget.h>
#endif

#include "begin_code.h"

#define AG_LABEL_MAX		128
#define AG_LABEL_MAX_POLLPTRS	32

enum ag_label_type {
	AG_LABEL_STATIC,		/* Static text */
	AG_LABEL_POLLED,		/* Polling (thread unsafe) */
	AG_LABEL_POLLED_MT		/* Polling (thread safe) */
};

typedef struct ag_label {
	struct ag_widget wid;
	enum ag_label_type type;
	AG_Mutex lock;
	int surface;
	int wPre, hPre;				/* Prescale dimensions */
	int lPad, rPad, tPad, bPad;		/* Label padding */
	struct {
		char fmt[AG_LABEL_MAX];
		void *ptrs[AG_LABEL_MAX_POLLPTRS];
		int nptrs;
		AG_Mutex *lock;
	} poll;
} AG_Label;

__BEGIN_DECLS
AG_Label *AG_LabelNew(void *, enum ag_label_type, const char *, ...);
AG_Label *AG_LabelNewFmt(void *, const char *, ...);
__inline__ AG_Label *AG_LabelNewStatic(void *, const char *);

void	 AG_LabelInit(AG_Label *, enum ag_label_type, const char *);
void 	 AG_LabelDestroy(void *);
void	 AG_LabelDraw(void *);
void	 AG_LabelScale(void *, int, int);
void	 AG_LabelPrintf(AG_Label *, const char *, ...)
		        FORMAT_ATTRIBUTE(printf, 2, 3)
		        NONNULL_ATTRIBUTE(2);
void	 AG_LabelSetSurface(AG_Label *, SDL_Surface *);
void	 AG_LabelSetPadding(AG_Label *, int, int, int, int);
#define	 AG_LabelSetPaddingLeft(lbl,v)   AG_LabelSetPadding(lbl,(v),-1,-1,-1)
#define	 AG_LabelSetPaddingRight(lbl,v)  AG_LabelSetPadding(lbl,-1,(v),-1,-1)
#define	 AG_LabelSetPaddingTop(lbl,v)    AG_LabelSetPadding(lbl,-1,-1,(v),-1)
#define	 AG_LabelSetPaddingBottom(lbl,v) AG_LabelSetPadding(lbl,-1,-1,-1,(v))
void	 AG_LabelPrescale(AG_Label *, const char *);
__END_DECLS

#include "close_code.h"
#endif /* _AGAR_WIDGET_LABEL_H_ */
