#ifndef __PropertyUtils_H__
#define __PropertyUtils_H__

#include "PIGeneral.h"
#include "PIProperties.h"
#include <string>

using namespace std;

enum InterpolationMethod { pointSample = 1, bilinear, bicubic };
enum PaintCursorKind { standard = 1, precise, brushSize };

OSErr HandleToString(const Handle h, string & outString);
OSErr HandleToString(const Handle h, wstring & outString);
OSErr HandleToString(const Handle h, Str255 & outString);

OSErr StringToHandle(const string & inString, Handle & outHandle);
OSErr StringToHandle(const wstring & inString, Handle & outHandle);

OSErr PIGetNumberChannels(int32 & numChannels);
OSErr PIGetChannelName(const int32 index, string & name);
OSErr PIGetChannelName(const int32 index, wstring & name);
	
OSErr PIGetImageMode(int32 & mode);

OSErr PIGetTargetLayerLock(bool & transparency, bool & composite, bool & position);
	
OSErr PIGetNumberLayers(int32 & layers);
OSErr PIGetTargetLayerIndex(int32 & index);
OSErr PIGetLayerName(const int32 index, string & layerName);
OSErr PIGetLayerName(const int32 index, wstring & layerName);

OSErr PIGetNumberPaths(int32 & numPaths);
OSErr PIGetPathName(const int32 index, string & name);
OSErr PIGetPathName(const int32 index, wstring & name);
OSErr PIGetWorkPathIndex(int32 & index);
OSErr PIGetClippingPathIndex(int32 & index);
OSErr PIGetTargetPathIndex(int32 & index);
OSErr PIGetPathContents(const int32 index, Handle & contents);
OSErr PIGetPathContentsAI(const int32 index, Handle & contents);
OSErr PIGetCaption(Handle & caption);
OSErr PISetCaption(const Handle h);

OSErr PIGetBigNudge(double & horizontal, double & vertical);
OSErr PISetBigNudge(const double horizontal, const double vertical);

OSErr PIGetInterpolationMethod(InterpolationMethod & method);

OSErr PIGetRulerUnits(RulerUnits & rulerUnits);

OSErr PIGetRulerOrigin(double & horizontal, double & vertical);
OSErr PISetRulerOrigin(const double horizontal, const double vertical);

OSErr PIGetGrid(double & major, int32 & minor);
OSErr PISetGrid(const double major, const int32 minor);

OSErr PIGetSerialString(string & serial);
OSErr PIGetSerialStringOld(string & serial);

#if __PIWin__
	OSErr PIGetHardwareGammaTable(Handle & table);
#endif

OSErr PIGetInterfaceColor(const int8 index, PIInterfaceColor & color);

OSErr PIGetWatchSuspension(int32 & watch);
OSErr PISetWatchSuspension(const int32 inWatchSuspension);

OSErr PIGetCopyright(bool & copyright);
OSErr PISetCopyright(bool inCopyright);

OSErr PIGetURL(string & url);
OSErr PISetURL(const string & inURL);

OSErr PIGetDocumentName(string & name);
OSErr PIGetDocumentName(wstring & name, bool withExtension = true);
OSErr PIGetUnicodeStr255Name(Str255 & name);

OSErr PIGetWatermark(bool & watermark);
OSErr PISetWatermark(void);

OSErr PIGetDocumentHeight(int32 & height);
OSErr PIGetDocumentWidth(int32 & width);

OSErr PIGetToolTips(bool & toopTips);

OSErr PIGetPaintCursorKind(PaintCursorKind & cursorKind);

OSErr PIGetSlices(Handle & slices);
OSErr PISetSlices(const Handle inSlices);

OSErr PIGetSelectedSliceID(int32 & sliceID);
OSErr PISetSelectedSliceID(const int32 sliceID);

OSErr PIGetSelectedSliceIndex(int32 & sliceIndex);
OSErr PISetSelectedSliceIndex(const int32 sliceIndex);

OSErr PIGetShowSliceNumbers(bool & showSliceNumbers);

OSErr PIGetSliceLineColor(RGBColor & lineColor);

OSErr PIGetPlayInProgress(bool & inProgress);

OSErr PIGetLayerIsSmart(bool & inSmart);

OSErr PIGetEXIFData(Handle & exifData);
OSErr PISetEXIFData(const Handle inEXIFData);

OSErr PIGetHostVersion(int32 & major, int32 & minor, int32 & fix);

OSErr PIGetXMP(string & xmp);
OSErr PISetXMP(const string & inXMP);

#endif
// end PropertyUtils.h