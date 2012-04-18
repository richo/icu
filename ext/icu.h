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

#include "ruby.h"

#include "unicode/uloc.h"
#include "unicode/ucol.h"

#define RAISE_ON_ERROR(e) if (U_FAILURE(e)) rb_raise(rb_eRuntimeError, u_errorName(e));
#define SYMBOL2CSTR(s) rb_id2name(SYM2ID(s)));
#define CheckLocaleID(x) if (!validLocale(x)) rb_raise(rb_eArgError, "Invalid locale id '%s'", x);
#define CheckLanguageCode(x) if (!validLanguage(x)) rb_raise(rb_eArgError, "Invalid language code '%s'. Must be ISO639 2/3-letter.", x);
#define CheckCountryCode(x) if (!validCountry(x)) rb_raise(rb_eArgError, "Invalid country code '%s'. Must be ISO3166 2-letter.", x);

int validLocale(const char *locale_id);
int validLanguage(const char *code);
int validCountry(const char *code);
UChar *u_strFromRString(VALUE rStr, int32_t *resultLen);
VALUE u_strToRString(const UChar *uStr, int32_t uStrLen);
VALUE collateByDisplayName(VALUE *a, VALUE *b, UCollator *collator);
