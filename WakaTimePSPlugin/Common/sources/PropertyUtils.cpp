#include "PropertyUtils.h"
#include "PIUSuites.h"

static Fixed DoubleToFixed(double in);
static double FixedToDouble(Fixed in);

OSErr HandleToString(const Handle h, string & outString)
{
	OSErr error = kNoErr;

	const string sEmptyString;

	outString = sEmptyString;

	int32 s = sPSHandle->GetSize(h);
	
	if (s > 0)
	{
		Boolean oldLock = FALSE;
		Ptr p = NULL;
		sPSHandle->SetLock(h, true, &p, &oldLock);
		if (p != NULL)
		{
			outString.assign(p, p + s);
			sPSHandle->SetLock(h, false, &p, &oldLock);
		}
		else
		{
			error = errPlugInHostInsufficient;
		}
	}
	else
	{
		error = errPlugInHostInsufficient;
	}

	return error;
}

OSErr HandleToString(const Handle h, wstring & outString)
{
	OSErr error = kNoErr;

	const wstring sEmptyWString;

	outString = sEmptyWString;

	int32 s = sPSHandle->GetSize(h);
	
	if (s > 0)
	{
		Boolean oldLock = FALSE;
		uint16 * p = NULL;
		sPSHandle->SetLock(h, true, reinterpret_cast<char**>(&p), &oldLock);
		if (p != NULL)
		{
			outString.assign(p, p + ( s / sizeof(uint16) ));
			sPSHandle->SetLock(h, false, reinterpret_cast<char**>(&p), &oldLock);
		}
		else
		{
			error = errPlugInHostInsufficient;
		}
	}
	else
	{
		error = errPlugInHostInsufficient;
	}

	return error;
}

OSErr HandleToString(const Handle h, Str255 & outString)
{
	OSErr error = kNoErr;

	outString[0] = 0;
	outString[1] = 0;

	int32 s = sPSHandle->GetSize(h);
	
	if (s > 0)
	{
		outString[0] = static_cast<unsigned char>(s);
		Boolean oldLock = FALSE;
		Ptr address = NULL;
		sPSHandle->SetLock(h, true, &address, &oldLock);
		if (address != NULL)
		{
			memcpy(outString, &address, s+1);
			sPSHandle->SetLock(h, false, &address, &oldLock);
		}
		else
		{
			error = errPlugInHostInsufficient;
		}
	}
	else
	{
		error = errPlugInHostInsufficient;
	}

	return error;
}

OSErr StringToHandle(const string & inString, Handle & outHandle)
{
	OSErr error = kNoErr;

	outHandle = NULL;

	size_t s = inString.length();

	if (s) 
	{
		outHandle = sPSHandle->New((int32)s);
		if (outHandle != NULL)
		{
			Boolean oldLock = FALSE;
			Ptr p = NULL;
			sPSHandle->SetLock(outHandle, true, &p, &oldLock);
			if (p != NULL)
			{
				Ptr originalP = p;
				for(size_t a = 0; a < s; a++, p++)
					*p = inString.at(a);
				sPSHandle->SetLock(outHandle, false, &originalP, &oldLock);
			}
			else
			{
				sPSHandle->Dispose(outHandle);
				outHandle = NULL;
				error = errPlugInHostInsufficient;
			}
		}
		else
		{
			error = errPlugInHostInsufficient;
		}
	}
	else
	{
		error = errPlugInHostInsufficient;
	}

	return error;
}

OSErr StringToHandle(const wstring & inString, Handle & outHandle)
{
	OSErr error = kNoErr;

	outHandle = NULL;

	size_t s = inString.length();

	if (s) 
	{
		outHandle = sPSHandle->New((int32)s);
		if (outHandle != NULL)
		{
			Boolean oldLock = FALSE;
			uint16 * p = NULL;
			sPSHandle->SetLock(outHandle, true, reinterpret_cast<char**>(&p), &oldLock);
			if (p != NULL)
			{
				Ptr originalP = (Ptr)p;
				for(size_t a = 0; a < s; a++, p++)
					*p = inString.at(a);
				sPSHandle->SetLock(outHandle, false, &originalP, &oldLock);
			}
			else
			{
				sPSHandle->Dispose(outHandle);
				outHandle = NULL;
				error = errPlugInHostInsufficient;
			}
		}
		else
		{
			error = errPlugInHostInsufficient;
		}
	}
	else
	{
		error = errPlugInHostInsufficient;
	}

	return error;
}

OSErr PIGetNumberChannels(int32 & numChannels)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propNumberOfChannels, 
										0, (intptr_t*)&numChannels, NULL);
}

OSErr PIGetChannelName(const int32 index, string & name)
{
	Handle h = NULL;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propChannelName, 
											   index, NULL, &h);
	
	if (!error && h != NULL)
	{
		error = HandleToString(h, name);
		sPSHandle->Dispose(h);
	}
	
	return error;
}

OSErr PIGetChannelName(const int32 index, wstring & name)
{
	Handle h = NULL;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propUnicodeChannelName, 
											   index, NULL, &h);

	if (!error && h != NULL)
	{
		error = HandleToString(h, name);
        sPSHandle->Dispose(h);
	}

	return error;
}

OSErr PIGetImageMode(int32 & mode)
{ 
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propImageMode, 
										0, (intptr_t*)&mode, NULL);
}

OSErr PIGetTargetLayerLock(bool & transparency, bool & composite, bool & position)
{
	int32 simple = 0;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propTargetLayerLock, 
											   0, (intptr_t*)&simple, NULL);
	
	if (!error)
	{
		transparency = (simple & propTargetLayerLockTransparency) != 0;
		composite = (simple & propTargetLayerLockComposite) != 0;
		position = (simple & propTargetLayerLockPosition) != 0;
	}
	
	return error;
}

OSErr PIGetNumberLayers(int32 & layers)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature,
		                                propNumberOfLayers,
										0, (intptr_t*)&layers, NULL);
}

OSErr PIGetTargetLayerIndex(int32 & index)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature,
										propTargetLayerIndex,
										0, (intptr_t*)&index, NULL);
}

OSErr PIGetLayerName(const int32 index, string & layerName)
{
	Handle h = NULL;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
						                       propLayerName,
											   index, NULL, &h);
	
	if (!error && h != NULL)
	{
		error = HandleToString(h, layerName);
		sPSHandle->Dispose(h);
	}
	
	return error;
}

OSErr PIGetLayerName(const int32 index, wstring & layerName)
{
	Handle h = NULL;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
						                       propUnicodeLayerName,
											   index, NULL, &h);
	
	if (!error && h != NULL)
	{
		error = HandleToString(h, layerName);
		sPSHandle->Dispose(h);
	}
	
	return error;
}

OSErr PIGetNumberPaths(int32 & paths)
{ 
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propNumberOfPaths, 
										0, (intptr_t*)&paths, NULL);
}

OSErr PIGetPathName(const int32 index, string & pathName)
{
	Handle h = NULL;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
						                       propPathName, 
											   index, NULL, &h);
	
	if (!error && h != NULL)
	{
		error = HandleToString(h, pathName);
		sPSHandle->Dispose(h);
	}
	
	return error;
}

OSErr PIGetPathName(const int32 index, wstring & pathName)
{
	Handle h = NULL;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propUnicodePathName, 
											   index, NULL, &h);
	
	if (!error && h != NULL)
	{
		error = HandleToString(h, pathName);
		sPSHandle->Dispose(h);
	}
	
	return error;
}

OSErr PIGetWorkPathIndex(int32 & pathIndex)
{ 
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propWorkPathIndex, 
										0, (intptr_t*)&pathIndex, NULL);
}

OSErr PIGetClippingPathIndex(int32 & pathIndex)
{ 
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propClippingPathIndex, 
										0, (intptr_t*)&pathIndex, NULL);
}

OSErr PIGetTargetPathIndex(int32 & pathIndex)
{ 
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propTargetPathIndex, 
										0, (intptr_t*)&pathIndex, NULL);
}

OSErr PIGetPathContents(const int32 index, Handle & contents)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propPathContents, 
										index, NULL, &contents);
}

OSErr PIGetPathContentsAI(const int32 index, Handle & contents)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propPathContentsAI, 
										index, NULL, &contents);
}

OSErr PIGetCaption(Handle & caption)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propCaption, 
										0, NULL, &caption);
}

OSErr PISetCaption(const Handle inCaption)
{
	return sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                propCaption, 
										0, NULL, inCaption);
}

OSErr PIGetBigNudge(double & horizontal, double & vertical)
{
	int32 t;
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propBigNudgeH, 
											   0, (intptr_t*)&t, NULL);
	Fixed f = (Fixed)(t);
	
	if (!error)
	{
		horizontal = FixedToDouble(f);

		error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
			                                 propBigNudgeV,
											 0, (intptr_t*)&t, NULL);
	}
	
	f = (Fixed)(t);

	if (!error)
		vertical = FixedToDouble(f);

	return error;
}

OSErr PISetBigNudge(const double horizontal, const double vertical)
{
	OSErr error = sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                       propBigNudgeH, 
											   0, DoubleToFixed(horizontal), NULL);
	
	if (!error)
		error = sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                     propBigNudgeV, 
											 0, DoubleToFixed(vertical), NULL);

	return error;
}

OSErr PIGetInterpolationMethod(InterpolationMethod & method)
{
	int32 simple = 0;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propInterpolationMethod, 
											   0, (intptr_t*)&simple, NULL);
	
	if (!error)
		method = static_cast<InterpolationMethod>(simple);

	return error;
}

OSErr PIGetRulerUnits(RulerUnits & units)
{
	int32 simple = 0;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propRulerUnits, 
											   0, (intptr_t*)&simple, NULL);

	if (!error)
		units = static_cast<RulerUnits>(simple);

	return error;
}

OSErr PIGetRulerOrigin(double & horizontal, double & vertical)
{
	int32 t;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propRulerOriginH, 
											   0, (intptr_t*)&t, NULL);
	
	Fixed f = (Fixed)(t);

	if (!error)
	{
		horizontal = FixedToDouble(f);

		error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
			                                 propRulerOriginV,
											 0, (intptr_t*)&t, NULL);
	}

	f = (Fixed)(t);

	if (!error)
		vertical = FixedToDouble(f);

	return error;
}

OSErr PISetRulerOrigin(const double horizontal, const double vertical)
{
	OSErr error = sPSProperty->setPropertyProc(kPhotoshopSignature,
		                                       propRulerOriginH, 
											   0, DoubleToFixed(horizontal), NULL);
	
	if (!error)
		error = sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                     propRulerOriginV, 
											 0, DoubleToFixed(vertical), NULL);

	return error;
}
	
OSErr PIGetGrid(double & major, int32 & minor)
{
	int32 t;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propGridMajor, 
											   0, (intptr_t*)&t, NULL);
	
	Fixed f = (Fixed)(t);

	if (!error)
	{
		major = FixedToDouble(f);

		error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
			                                 propGridMinor, 
											 0, (intptr_t*)&minor, NULL);
	}

	return error;
}

OSErr PISetGrid(const double major, const int32 minor)
{
	OSErr error = sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                       propGridMajor, 
											   0, DoubleToFixed(major), NULL);

	if (!error)
		error = sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                     propGridMinor, 
											 0, minor, NULL);

	return error;
}
	
OSErr PIGetSerialStringOld(string & serialStr)
{
	Handle h = NULL;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propSerialString, 
											   0, NULL, &h);
	
	if (!error && h != NULL)
	{
		error = HandleToString(h, serialStr);
		sPSHandle->Dispose(h);
	}
	
	return error;
}

OSErr PIGetSerialString(string & serialStr )
{
	Handle h = NULL;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propSerialString2, 
											   0, NULL, &h);
	
	if (!error && h != NULL)
	{
		error = HandleToString(h, serialStr);
		sPSHandle->Dispose(h);
	}
	
	return error;
}

#if __PIWin__
OSErr PIGetHardwareGammaTable(Handle & gammaTable)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propHardwareGammaTable, 
										0, NULL, &gammaTable);
}
#endif

OSErr PIGetInterfaceColor(const int8 index, PIInterfaceColor & color)
{
	Handle h = NULL;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propInterfaceColor, 
											   index, NULL, &h);

	if (!error && h != NULL)
	{
		Boolean oldLock = FALSE;
		PIInterfaceColor * p = NULL;
		sPSHandle->SetLock(h, true, reinterpret_cast<char**>(&p), &oldLock);
		if (p != NULL)
		{
			color = *p;
			sPSHandle->SetLock(h, false, reinterpret_cast<char**>(&p), &oldLock);
			sPSHandle->Dispose(h);
		}
		else
		{
			sPSHandle->Dispose(h);
			h = NULL;
			error = errPlugInHostInsufficient;
		}
	}
	else
	{
		error = errPlugInHostInsufficient;
	}

	return error;
}

OSErr PIGetWatchSuspension(int32 & watch)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propWatchSuspension, 
										0, (intptr_t*)&watch, NULL);
}

OSErr PISetWatchSuspension(const int32 inWatchSuspension)
{
	return sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                propWatchSuspension, 
										0, inWatchSuspension, NULL);
}

OSErr PIGetCopyright(bool & copyright)
{
	int32 simple = 0;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propCopyright, 
											   0, (intptr_t*)&simple, NULL);
	
	if (!error)
		copyright = simple != 0;

	return error;
}
	
OSErr PISetCopyright(const bool inCopyright)
{
	return sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                propCopyright, 
										0, inCopyright, NULL);
}

OSErr PIGetURL(string & url)
{
	Handle h = NULL;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propURL, 
											   0, NULL, &h);
	
	if (!error && h != NULL)
	{
		error = HandleToString(h, url);
		sPSHandle->Dispose(h);
	}
	
	return error;
}

OSErr PISetURL(const string & inURL)
{
	Handle h = NULL;

	OSErr error = StringToHandle(inURL, h);
	
	if (!error && h != NULL)
	{
		error = sPSProperty->setPropertyProc(kPhotoshopSignature, 
			                                 propURL, 
											 0, NULL, h);
		sPSHandle->Dispose(h);
	}
	else
	{
		error = errPlugInHostInsufficient;
	}
	
	return error;
}

OSErr PIGetDocumentName(string & name)
{
	Handle h = NULL;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propTitle, 
											   0, NULL, &h);
	
	if (!error && h != NULL)
	{
		error = HandleToString(h, name);
		sPSHandle->Dispose(h);
	}
	
	return error;
}


OSErr PIGetWatermark(bool & watermark)
{
	int32 simple = 0;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propWatermark, 
											   0, (intptr_t*)&simple, NULL);
	
	if (!error)
		watermark = (simple != 0);

	return error;
}

OSErr PISetWatermark(void)
{
	return sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                propWatermark, 
										0, 1, NULL);
}

OSErr PIGetDocumentHeight(int32 & height)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propDocumentHeight, 
										0, (intptr_t*)&height, NULL);
}

OSErr PIGetDocumentWidth(int32 & width)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propDocumentWidth, 
										0, (intptr_t*)&width, NULL);
}

OSErr PIGetToolTips(bool & toolTips)
{
	int32 simple = 0;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propToolTips, 
											   0, (intptr_t*)&simple, NULL);
	
	if (!error)
		toolTips = simple != 0;

	return error;
}

OSErr PIGetPaintCursorKind(PaintCursorKind & cursorKind)
{
	int32 simple = 0;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propPaintCursorKind, 
											   0, (intptr_t*)&simple, NULL);

	if (!error)
		cursorKind = static_cast<PaintCursorKind>(simple);

	return error;
}

OSErr PIGetSlices(Handle & slices)
{	
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propSlices, 
										0, NULL, &slices);
}

OSErr PISetSlices(const Handle inSlices)
{
	return sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                propSlices, 
										0, NULL, inSlices);
}

OSErr PIGetSelectedSliceID(int32 & sliceID)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propSelectedSliceID, 
										0, (intptr_t*)&sliceID, 0);
}

OSErr PISetSelectedSliceID(int32 sliceID)
{
	return sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                propSelectedSliceID, 
										0, sliceID, 0);
}

OSErr PIGetSelectedSliceIndex(int32 & sliceIndex)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propSelectedSliceIndex, 
										0, (intptr_t*)&sliceIndex, NULL);
}

OSErr PISetSelectedSliceIndex(int32 sliceIndex)
{
	return sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                propSelectedSliceIndex, 
										0, sliceIndex, NULL);
}

OSErr PIGetShowSliceNumbers(bool & showSliceNumbers)
{
	int32 simple = 0;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propShowSliceNumbers, 
											   0, (intptr_t*)&simple, NULL);
	
	if (!error)
		showSliceNumbers = simple != 0;

	return error;
}

OSErr PIGetSliceLineColor(RGBColor & lineColor)
{
	Handle h = NULL;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propSliceLineColor, 
											   0, NULL, &h);

	if (!error && h != NULL)
	{
		Boolean oldLock = FALSE;
		RGBColor * c = NULL;
		sPSHandle->SetLock(h, true, reinterpret_cast<char**>(&c), &oldLock);
		if (c != NULL)
		{
			lineColor.red = c->red;
			lineColor.green = c->green;
			lineColor.blue = c->blue;
			sPSHandle->SetLock(h, false, reinterpret_cast<char**>(&c), &oldLock);
		}
		else
		{
			error = errPlugInHostInsufficient;
		}

		sPSHandle->Dispose(h);

	}
	return error;
}

OSErr PIGetEXIFData(Handle & exif)
{
	return sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                propEXIFData, 
										0, NULL, &exif);
}

OSErr PISetEXIFData(const Handle inEXIFData)
{
	return sPSProperty->setPropertyProc(kPhotoshopSignature, 
		                                propEXIFData, 
										0, NULL, inEXIFData);
}

OSErr PIGetPlayInProgress(bool & inProgress)
{
	int32 simple = 0;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propPlayInProgress, 
											   0, (intptr_t*)&simple, NULL);
	
	if (!error)
		inProgress = simple != 0;

	return error;
}

OSErr PIGetLayerIsSmart(bool & inSmart)
{
	int32 simple = 0;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propLayerIsSmart, 
											   0, (intptr_t*)&simple, NULL);
	
	if (!error)
		inSmart = simple != 0;

	return error;
}

OSErr PIGetHostVersion(int32 & major, int32 & minor, int32 & fix)
{
	int32 s = 0;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propVersion, 
											   0, (intptr_t*)&s, NULL);

	int32 simple = (int32)s;
	if (!error)
	{
		major = (simple & 0xffff0000) >> 16;
		minor = (simple & 0x0000ff00) >> 8;
		fix =	(simple & 0x000000ff);
	}

	return error;
}

OSErr PIGetXMP(string & xmp)
{
	Handle h = NULL;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propXMP, 
											   0, NULL, &h);
	
	if (!error && h != NULL)
	{
		error = HandleToString(h, xmp);
		sPSHandle->Dispose(h);
	}
	
	return error;
}

OSErr PISetXMP(const string & inXMP)
{
	Handle h = NULL;

	OSErr error = StringToHandle(inXMP, h);
	
	if (h != NULL)
	{
		error = sPSProperty->setPropertyProc(kPhotoshopSignature, 
			                                 propXMP, 
											 0, NULL, h);
		sPSHandle->Dispose(h);
	}
	else
	{
		error = errPlugInHostInsufficient;
	}
	
	return error;
}

OSErr PIGetDocumentName(wstring & name, bool withExtension)
{
	Handle h = NULL;
	
	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       withExtension ? propUnicodeName : propUnicodeNameWithoutExtension, 
											   0, NULL, &h);

	if (!error && h != NULL)
	{
		error = HandleToString(h, name);
		sPSHandle->Dispose(h);
	}

	return error;
}

OSErr PIGetUnicodeStr255Name(Str255 & name)
{
	Handle h = NULL;

	OSErr error = sPSProperty->getPropertyProc(kPhotoshopSignature, 
		                                       propUniStr255Name, 
											   0, NULL, &h);

	if (!error && h != NULL)
	{
		error = HandleToString(h, name);
		sPSHandle->Dispose(h);
	}

	return error;
}

static Fixed DoubleToFixed(double in)
{	
	uint16 n = static_cast<uint16>(in);
	double t = in - n;
	t *= 65536.0;
	uint16 r = static_cast<uint16>(t);
	Fixed f = n << 16;
	f += r;
	return (f);
}

static double FixedToDouble(Fixed in)
{
	uint16 * n = reinterpret_cast<uint16*>(&in);
	uint16 * r = n++;
	return ((double)(*n) + ((double)(*r) / 65536.0));
}
// end PropertyUtils.cpp