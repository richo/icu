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

VALUE cLanguage;

/******************* ICU::Locale::Language */

/**
 *  call-seq:
 *     ICU::Locale.get => locale
 */
VALUE rb_Language_class_get(VALUE self, VALUE code)
{
	VALUE languages, language;
	
	code = StringValue(code);
	languages = rb_cv_get(self, "@@languages");
	language = rb_hash_aref(languages, code);
	if (language == Qnil) {
		VALUE args[1];
		
		args[0] = code;
		language = rb_class_new_instance(1, args, self);
		rb_hash_aset(languages, code, language);
		return language;
	} else {
		return language;
	}
}

VALUE rb_Language_initialize(VALUE self, VALUE code)
{
	code = StringValue(code);
	CheckLanguageCode(RSTRING(code)->ptr);
	rb_iv_set(self, "@code", code);
	
	return self;
}

VALUE rb_Language_name(int argc, VALUE *argv, VALUE self)
{
	char *fakeLocale;
	VALUE code = rb_iv_get(self, "@code");
	
	VALUE rb_locale;
	char *locale = NULL;
	
	UChar *language = ALLOC_N(UChar, 32);
	int32_t languageLen = 32;
	UErrorCode status = U_ZERO_ERROR;
	
	fakeLocale = RSTRING(code)->ptr;

	rb_scan_args(argc, argv, "01", &rb_locale);
	if (rb_locale != Qnil) {
		locale = StringValuePtr(rb_locale);
	}
	
	languageLen = uloc_getDisplayLanguage(fakeLocale, locale, language, languageLen, &status);
	if (status == U_BUFFER_OVERFLOW_ERROR) {
		status = U_ZERO_ERROR;
		REALLOC_N(language, UChar, languageLen);
		uloc_getDisplayLanguage(fakeLocale, locale, language, languageLen, &status);
	}
	RAISE_ON_ERROR(status);
	
	return u_strToRString(language, languageLen);
}

void Init_ICU_Locale_Language()
{
	extern VALUE cLocale;
	
	cLanguage = rb_define_class_under(cLocale, "Language", rb_cObject);
	rb_funcall(cLanguage, rb_intern("private_class_method"), 1, ID2SYM(rb_intern("new")));
	rb_cv_set(cLanguage, "@@languages", rb_hash_new());
	rb_define_singleton_method(cLanguage, "get", rb_Language_class_get, 1);
	rb_define_private_method(cLanguage, "initialize", rb_Language_initialize, 1);
	rb_define_attr(cLanguage, "code", 1, 0);
	rb_define_alias(cLanguage, "to_str", "code");
	rb_define_alias(cLanguage, "to_s", "code");
	rb_define_method(cLanguage, "name", rb_Language_name, -1);
}
