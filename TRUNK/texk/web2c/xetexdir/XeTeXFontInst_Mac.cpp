/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2005 by SIL International
 written by Jonathan Kew

 This software is distributed under the terms of the Common Public License,
 version 1.0.
 For details, see <http://www.opensource.org/licenses/cpl1.0.php> or the file
 cpl1.0.txt included with the software.
\****************************************************************************/

/*
 *   file name:  XeTeXFontInst_Mac.cpp
 *
 *   created on: 2005-10-22
 *   created by: Jonathan Kew
 */


#include "XeTeXFontInst_Mac.h"
#include "XeTeX_ext.h"

XeTeXFontInst_Mac::XeTeXFontInst_Mac(ATSFontRef atsFont, float pointSize, LEErrorCode &status)
    : XeTeXFontInst(atsFont, pointSize, status)
    , fStyle(0)
{
    if (LE_FAILURE(status)) {
        return;
    }

	initialize(status);
}

XeTeXFontInst_Mac::~XeTeXFontInst_Mac()
{
	if (fStyle != 0)
		ATSUDisposeStyle(fStyle);
}

void XeTeXFontInst_Mac::initialize(LEErrorCode &status)
{
    if (fFontRef == 0) {
        status = LE_FONT_FILE_NOT_FOUND_ERROR;
        return;
    }

	XeTeXFontInst::initialize(status);

	if (status != LE_NO_ERROR)
		fFontRef = 0;

	if (ATSUCreateStyle(&fStyle) == noErr) {
		ATSUFontID	font = FMGetFontFromATSFontRef(fFontRef);
		Fixed		size = X2Fix(fPointSize * 72.0 / 72.27); /* convert TeX to Quartz points */
		ATSStyleRenderingOptions	options = kATSStyleNoHinting;
		ATSUAttributeTag		tags[3] = { kATSUFontTag, kATSUSizeTag, kATSUStyleRenderingOptionsTag };
		ByteCount				valueSizes[3] = { sizeof(ATSUFontID), sizeof(Fixed), sizeof(ATSStyleRenderingOptions) };
		ATSUAttributeValuePtr	values[3] = { &font, &size, &options };
		ATSUSetAttributes(fStyle, 3, tags, valueSizes, values);
	}
	else {
		status = LE_FONT_FILE_NOT_FOUND_ERROR;
		fFontRef = 0;
	}
	
    return;
}

const void *XeTeXFontInst_Mac::readTable(LETag tag, le_uint32 *length) const
{
	OSStatus status = ATSFontGetTable(fFontRef, tag, 0, 0, 0, (ByteCount*)length);
	if (status != noErr) {
		*length = 0;
		return NULL;
	}
	void*	table = LE_NEW_ARRAY(char, *length);
	if (table != NULL) {
		status = ATSFontGetTable(fFontRef, tag, 0, *length, table, (ByteCount*)length);
		if (status != noErr) {
			*length = 0;
			LE_DELETE_ARRAY(table);
			return NULL;
		}
	}

    return table;
}

void XeTeXFontInst_Mac::getGlyphBounds(LEGlyphID gid, GlyphBBox* bbox)
{
	GetGlyphBBox_AAT(fStyle, gid, bbox);
}
