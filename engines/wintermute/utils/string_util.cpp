/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#include "common/language.h"
#include "common/tokenizer.h"
#include "common/str-enc.h"
#include "engines/wintermute/base/base_engine.h"
#include "engines/wintermute/utils/string_util.h"

namespace Wintermute {

//////////////////////////////////////////////////////////////////////////
bool StringUtil::compareNoCase(const AnsiString &str1, const AnsiString &str2) {
	return (str1.compareToIgnoreCase(str2) == 0);
}

//////////////////////////////////////////////////////////////////////////
/*bool StringUtil::CompareNoCase(const WideString &str1, const WideString &str2) {
	WideString str1lc = str1;
	WideString str2lc = str2;

	ToLowerCase(str1lc);
	ToLowerCase(str2lc);

	return (str1lc == str2lc);
}*/

//////////////////////////////////////////////////////////////////////////
WideString StringUtil::utf8ToWide(const Utf8String &Utf8Str) {
	return Common::convertToU32String(Utf8Str.c_str(), Common::kUtf8);
}

//////////////////////////////////////////////////////////////////////////
Utf8String StringUtil::wideToUtf8(const WideString &WideStr) {
	return Common::convertFromU32String(WideStr, Common::kUtf8);
}

//////////////////////////////////////////////////////////////////////////
Common::CodePage StringUtil::mapCodePage(TTextCharset charset) {
	switch (charset) {
	case CHARSET_EASTEUROPE:
		return Common::kWindows1250;

	case CHARSET_RUSSIAN:
		return Common::kWindows1251;

	case CHARSET_ANSI:
		return Common::kWindows1252;

	case CHARSET_GREEK:
		return Common::kWindows1253;

	case CHARSET_TURKISH:
		return Common::kWindows1254;

	case CHARSET_HEBREW:
		return Common::kWindows1255;

	case CHARSET_ARABIC:
		return Common::kWindows1256;

	case CHARSET_BALTIC:
		return Common::kWindows1257;

	case CHARSET_DEFAULT:
		switch (BaseEngine::instance().getLanguage()) {

		//cp1250: Central Europe
		case Common::CS_CZE:
		case Common::HR_HRV:
		case Common::HU_HUN:
		case Common::PL_POL:
		case Common::SK_SVK:
			return Common::kWindows1250;

		//cp1251: Cyrillic
		case Common::RU_RUS:
		case Common::UA_UKR:
			return Common::kWindows1251;

		//cp1252: Western Europe
		case Common::DA_DNK:
		case Common::DE_DEU:
		case Common::EN_ANY:
		case Common::EN_GRB:
		case Common::EN_USA:
		case Common::ES_ESP:
		case Common::FI_FIN:
		case Common::FR_FRA:
		case Common::IT_ITA:
		case Common::NB_NOR:
		case Common::NL_NLD:
		case Common::PT_BRA:
		case Common::PT_PRT:
		case Common::SV_SWE:
		case Common::UNK_LANG:
			return Common::kWindows1252;

		//cp1253: Greek
		case Common::EL_GRC:
			return Common::kWindows1253;

		//cp1254: Turkish
		case Common::TR_TUR:
			return Common::kWindows1254;

		//cp1255: Hebrew
		case Common::HE_ISR:
			return Common::kWindows1255;

		//cp1256: Arabic
		case Common::FA_IRN:
			return Common::kWindows1256;

		//cp1257: Baltic
		case Common::ET_EST:
		case Common::LV_LVA:
			return Common::kWindows1257;

		case Common::JA_JPN:
		case Common::KO_KOR:
		case Common::ZH_CHN:
		case Common::ZH_TWN:
		default:
			warning("Unsupported charset: %d", charset);
			return Common::kWindows1252;
		}

	case CHARSET_OEM:
	case CHARSET_CHINESEBIG5:
	case CHARSET_GB2312:
	case CHARSET_HANGUL:
	case CHARSET_MAC:
	case CHARSET_SHIFTJIS:
	case CHARSET_SYMBOL:
	case CHARSET_VIETNAMESE:
	case CHARSET_JOHAB:
	case CHARSET_THAI:
	default:
		warning("Unsupported charset: %d", charset);
		return Common::kWindows1252;
	}
}

//////////////////////////////////////////////////////////////////////////
WideString StringUtil::ansiToWide(const AnsiString &str, TTextCharset charset) {
	return Common::convertToU32String(str.c_str(), mapCodePage(charset));
}

//////////////////////////////////////////////////////////////////////////
AnsiString StringUtil::wideToAnsi(const WideString &wstr, TTextCharset charset) {
	return Common::convertFromU32String(wstr, mapCodePage(charset));
}

//////////////////////////////////////////////////////////////////////////
bool StringUtil::isUtf8BOM(const byte *buffer, uint32 bufferSize) {
	if (bufferSize > 3 && buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF) {
		return true;
	} else {
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
int StringUtil::indexOf(const WideString &str, const WideString &toFind, size_t startFrom) {
	return str.find(toFind, startFrom);
}

Common::String StringUtil::encodeSetting(const Common::String &str) {
	for (uint32 i = 0; i < str.size(); i++) {
		if ((str[i] < 33) || (str[i] == '=') || (str[i] > 126)) {
			error("Setting contains illegal characters: %s", str.c_str());
		}
	}
	return str;
}

Common::String StringUtil::decodeSetting(const Common::String &str) {
	return str;
}

//////////////////////////////////////////////////////////////////////////
AnsiString StringUtil::toString(int val) {
	return Common::String::format("%d", val);
}

} // End of namespace Wintermute
