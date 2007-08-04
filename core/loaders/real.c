/*
 * Copyright (c) 2003-2007 Hypertriton, Inc. <http://hypertriton.com/>
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Serialization functions for IEEE.754 floating-point types.
 */

#include <core/core.h>

float
AG_ReadFloat(AG_Netbuf *buf)
{
	float f;

	AG_NetbufRead(&f, sizeof(f), 1, buf);
	return (f);
}

void
AG_WriteFloat(AG_Netbuf *buf, float f)
{
	AG_NetbufWrite(&f, sizeof(f), 1, buf);
}

void
AG_PwriteFloat(AG_Netbuf *buf, float f, off_t offs)
{
	AG_NetbufPwrite(&f, sizeof(f), 1, offs, buf);
}

double
AG_ReadDouble(AG_Netbuf *buf)
{
	double f;

	AG_NetbufRead(&f, sizeof(f), 1, buf);
	return (f);
}

void
AG_WriteDouble(AG_Netbuf *buf, double f)
{
	AG_NetbufWrite(&f, sizeof(f), 1, buf);
}

void
AG_PwriteDouble(AG_Netbuf *buf, double f, off_t offs)
{
	AG_NetbufPwrite(&f, sizeof(f), 1, offs, buf);
}


#ifdef HAVE_LONG_DOUBLE
long double
AG_ReadLongDouble(AG_Netbuf *buf)
{
	long double f;

	AG_NetbufRead(&f, sizeof(f), 1, buf);
	return (f);
}

void
AG_WriteLongDouble(AG_Netbuf *buf, long double f)
{
	AG_NetbufWrite(&f, sizeof(f), 1, buf);
}

void
AG_PwriteLongDouble(AG_Netbuf *buf, long double f, off_t offs)
{
	AG_NetbufPwrite(&f, sizeof(f), 1, offs, buf);
}
#endif /* HAVE_LONG_DOUBLE */
