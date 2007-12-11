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

VALUE cCountry;

/******************* ICU::Locale::Country */

/**
 *  call-seq:
 *     ICU::Locale.get => locale
 */
VALUE rb_Country_class_get(VALUE self, VALUE code)
{
	VALUE countries, country;
	
	code = StringValue(code);
	countries = rb_cv_get(self, "@@countries");
	country = rb_hash_aref(countries, code);
	if (country == Qnil) {
		VALUE args[1];
		
		args[0] = code;
		country = rb_class_new_instance(1, args, self);
		rb_hash_aset(countries, code, country);
		return country;
	} else {
		return country;
	}
}

VALUE rb_Country_initialize(VALUE self, VALUE code)
{
	code = StringValue(code);
	CheckCountryCode(RSTRING(code)->ptr);
	rb_iv_set(self, "@code", code);
	
	return self;
}

VALUE rb_Country_name(int argc, VALUE *argv, VALUE self)
{
	char fakeLocale[4] = "_";
	VALUE code = rb_iv_get(self, "@code");
	
	VALUE rb_locale;
	char * locale = NULL;
	
	UChar * country = ALLOC_N(UChar, 32);
	int32_t countryLen = 32;
	UErrorCode status = U_ZERO_ERROR;
	
	strcat(fakeLocale, RSTRING(code)->ptr);
	
	rb_scan_args(argc, argv, "01", &rb_locale);
	if (rb_locale != Qnil) {
		locale = StringValuePtr(rb_locale);
	}
	
	countryLen = uloc_getDisplayCountry(fakeLocale, locale, country, countryLen, &status);
	if (status == U_BUFFER_OVERFLOW_ERROR) {
		status = U_ZERO_ERROR;
		REALLOC_N(country, UChar, countryLen);
		uloc_getDisplayCountry(fakeLocale, locale, country, countryLen, &status);
	}
	RAISE_ON_ERROR(status);
	
	return u_strToRString(country, countryLen);
}

void Init_ICU_Locale_Country()
{
	extern VALUE cLocale;
	
	cCountry = rb_define_class_under(cLocale, "Country", rb_cObject);
	rb_funcall(cCountry, rb_intern("private_class_method"), 1, ID2SYM(rb_intern("new")));
	rb_cv_set(cCountry, "@@countries", rb_hash_new());
	rb_define_singleton_method(cCountry, "get", rb_Country_class_get, 1);
	rb_define_private_method(cCountry, "initialize", rb_Country_initialize, 1);
	rb_define_attr(cCountry, "code", 1, 0);
	rb_define_alias(cCountry, "to_str", "code");
	rb_define_alias(cCountry, "to_s", "code");
	rb_define_method(cCountry, "name", rb_Country_name, -1);
}
