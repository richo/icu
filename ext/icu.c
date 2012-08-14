// Copyright (c) 2007 Joshua Llorach
//
// This file is part of ICU Ruby extension.
//
// ICU is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ICU is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with ICU.  If not, see <http://www.gnu.org/licenses/>.

#include "icu.h"

VALUE mICU;

int validLocale(const char *locale_id)
{
	const char *language, *country;

	language = uloc_getISO3Language(locale_id);
	country = uloc_getISO3Country(locale_id);

	return language[0] == 0x00 && country[0] == 0x00 ? 0 : 1;
}

int validLanguage(const char *code)
{
	const char *language;

	if (strlen(code) > 3)
		return 0;
	language = uloc_getISO3Language(code);

	return language[0] == 0x00 ? 0 : 1;
}

int validCountry(const char *code)
{
	const char *country;
	char fakeLocale[4] = "_";

	if (strlen(code) > 2)
		return 0;
	strcat(fakeLocale, code);
	country = uloc_getISO3Country(fakeLocale);

	return country[0] == 0x00 ? 0 : 1;
}

UChar *u_strFromRString(VALUE rStr, int32_t *resultLen)
{
	UErrorCode errorCode;
	UChar *uStr = ALLOC_N(UChar, 64);

	if (resultLen == NULL) {
		resultLen = ALLOC_N(int32_t, 1);
	}
	errorCode = U_ZERO_ERROR;
	u_strFromUTF8(uStr, 64, resultLen, RSTRING_PTR(rStr), RSTRING_LEN(rStr), &errorCode);
	if (errorCode == U_STRING_NOT_TERMINATED_WARNING || errorCode == U_BUFFER_OVERFLOW_ERROR) {
		uStr = REALLOC_N(uStr, UChar, *resultLen + 1);
		errorCode = U_ZERO_ERROR;
		u_strFromUTF8(uStr, *resultLen + 1, resultLen, RSTRING_PTR(rStr), RSTRING_LEN(rStr), &errorCode);
	}
	RAISE_ON_ERROR(errorCode);

	return uStr;
}

VALUE u_strToRString(const UChar *uStr, int32_t uStrLen)
{
	UErrorCode errorCode;
	char *result = ALLOC_N(char, 64);
	int32_t resultLen;

    rb_encoding* encoding = rb_enc_find("UTF-8");

	errorCode = U_ZERO_ERROR;
	u_strToUTF8(result, 64, &resultLen, uStr, uStrLen, &errorCode);
	if (errorCode == U_STRING_NOT_TERMINATED_WARNING || errorCode == U_BUFFER_OVERFLOW_ERROR) {
		REALLOC_N(result, char, resultLen + 1);
		errorCode = U_ZERO_ERROR;
		u_strToUTF8(result, resultLen + 1, NULL, uStr, uStrLen, &errorCode);
	}
	RAISE_ON_ERROR(errorCode);

	return rb_enc_str_new(result, resultLen, encoding);
}

VALUE collateByDisplayName(VALUE *a, VALUE *b, UCollator *collator)
{
	UCharIterator sourceIter, targetIter;
	UErrorCode status = U_ZERO_ERROR;
	UCollationResult result;

	if (TYPE(*a) == T_ARRAY) {
    	uiter_setUTF8(&sourceIter, RSTRING_PTR(RARRAY_PTR(*a)[0]), RSTRING_LEN(RARRAY_PTR(*a)[0]));
    	uiter_setUTF8(&targetIter, RSTRING_PTR(RARRAY_PTR(*b)[0]), RSTRING_LEN(RARRAY_PTR(*b)[0]));
	} else {
    	uiter_setUTF8(&sourceIter, RSTRING_PTR(*a), RSTRING_LEN(*a));
    	uiter_setUTF8(&targetIter, RSTRING_PTR(*b), RSTRING_LEN(*b));
	}
    result = ucol_strcollIter(collator, &sourceIter, &targetIter, &status);
    RAISE_ON_ERROR(status);

    return result;
}

void Init_icu(void)
{
	extern void Init_ICU_Locale(void);
	extern void Init_ICU_Numeric(void);
	extern void Init_ICU_Time(void);
	extern VALUE rb_numeric_localize(int argc, VALUE *argv, VALUE self);

	/* ICU */
	mICU = rb_define_module("ICU");
	/* ICU::Locale */
	Init_ICU_Locale();
	/* Numeric */
	Init_ICU_Numeric();
	/* Time */
	Init_ICU_Time();
}
