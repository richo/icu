#include "icu.h"

#include "unicode/ures.h"

VALUE cLocale;

/* Returns the default locale 
 *  call-seq:
 *     ICU::Locale.default => locale
 */
VALUE rb_ICU_Locale_singleton_default(VALUE self)
{
	return rb_funcall(self, rb_intern("get"), 1, rb_str_new2(uloc_getDefault()));
}

/**
 *  call-seq:
 *     ICU::Locale.set(locale) => true
 *
 * Sets default locale.
 * 
 * By default (without calling this function), default locale will be based on information obtained from the underlying system environment. 
 * A value of NULL will try to get the system's default locale.
 * 
 */
VALUE rb_ICU_Locale_singleton_default_setter(VALUE self, VALUE locale)
{
	char *cpLocale = NULL;
	UErrorCode status = U_ZERO_ERROR;
	
	if (locale != Qnil) {
		cpLocale = StringValuePtr(locale);
		CheckLocaleID(cpLocale);
	}
	uloc_setDefault(cpLocale, &status);
	RAISE_ON_ERROR(status);
	
	return Qnil;
}

/**
 *  call-seq:
 *     ICU::Locale.get => locale
 */
VALUE rb_ICU_Locale_singleton_get(VALUE self, VALUE locale_id)
{
	VALUE locales, locale;
	
	Check_Type(locale_id, T_STRING);
	locales = rb_iv_get(self, "@@locales");
	locale = rb_hash_aref(locales, locale_id);
	if (locale == Qnil) {
		VALUE args[1];
		
		args[0] = locale_id;
		locale = rb_class_new_instance(1, args, self);
		rb_hash_aset(locales, locale_id, locale);
	}
	return locale;
}

/**
 *  call-seq:
 *     ICU::Locale.availables([locale]) => Array
 */
VALUE rb_ICU_Locale_singleton_available(int argc, VALUE *argv, VALUE self)
{
	VALUE options;
	char *locale = NULL;
	VALUE allowed = Qfalse, languages = Qfalse, countries = Qfalse, group = Qfalse;
	
	long i;
	int32_t length = uloc_countAvailable();
	UErrorCode status = U_ZERO_ERROR;
	VALUE select = rb_ary_new2(length);
	
	UCollator *collator;
	
	rb_scan_args(argc, argv, "01", &options);
	if (options != Qnil) {
		VALUE rb_locale;
		
		Check_Type(options, T_HASH);
		rb_locale = rb_hash_aref(options, ID2SYM(rb_intern("locale")));
		if (rb_locale != Qnil) {
			locale = StringValuePtr(rb_locale);
		}
		allowed = rb_hash_aref(options, ID2SYM(rb_intern("allowed")));
		if (allowed == Qnil) {
			allowed = Qfalse;
		} else {
			Check_Type(allowed, T_ARRAY);
		}
		languages = rb_hash_aref(options, ID2SYM(rb_intern("languages")));
		if (languages == Qnil) {
			languages = Qfalse;
		} else {
			Check_Type(languages, T_ARRAY);
		}
		countries = rb_hash_aref(options, ID2SYM(rb_intern("countries")));
		if (countries == Qnil) {
			countries = Qfalse;
		} else {
			Check_Type(countries, T_ARRAY);
		}
		group = rb_hash_aref(options, ID2SYM(rb_intern("group")));
		group = group && group != Qnil ? Qtrue : Qfalse;
    }

	for (i = 0; i < length; i++)	{
		const char *availableLocale = uloc_getAvailable(i);
		
		UChar *displayName = ALLOC_N(UChar, 64);
		int32_t displayNameLen = 64;
		VALUE option;
		
		if (allowed && !rb_ary_includes(allowed, rb_str_new2(availableLocale)))
			continue;
		if (languages) {
			char language[3];
			int32_t languageLen = 3;
			
			languageLen = uloc_getLanguage(availableLocale, language, languageLen, &status);
			RAISE_ON_ERROR(status);
			if (!rb_ary_includes(languages, rb_str_new(language, languageLen)))
				continue;
		}
		if (countries) {
			char country[3];
			int32_t countryLen = 3;
			
			countryLen = uloc_getCountry(availableLocale, country, countryLen, &status);
			RAISE_ON_ERROR(status);
			if (!rb_ary_includes(countries, rb_str_new(country, countryLen)))
				continue;
		}
	    displayNameLen = uloc_getDisplayName(availableLocale, locale, displayName, 64, &status);
		if (status == U_BUFFER_OVERFLOW_ERROR) {
			status = U_ZERO_ERROR;
			REALLOC_N(displayName, UChar, displayNameLen);
			uloc_getDisplayName(availableLocale, locale, displayName, displayNameLen, &status);
		}
	    RAISE_ON_ERROR(status);
	    
    	option = rb_ary_new2(2);
    	rb_ary_push(option, u_strToRString(displayName, displayNameLen));
    	rb_ary_push(option, rb_str_new2(availableLocale));
    	rb_ary_push(select, option);
	}
	/* */

	collator = ucol_open(locale, &status);
	RAISE_ON_ERROR(status);
	if (group) {
		char groupLanguageCode[3] = "";
		VALUE tmpSelect = rb_ary_new();
		VALUE options;
		
		//ruby_qsort(RARRAY(select)->ptr, RARRAY(select)->len, sizeof(VALUE), collateByLocaleID, collator);
		for (i = 0; i < RARRAY(select)->len; i++) {
			char languageCode[4];
			int32_t languageCodeLen = 4;
			
			languageCodeLen = uloc_getLanguage(RSTRING(RARRAY(RARRAY(select)->ptr[i])->ptr[1])->ptr, languageCode, languageCodeLen, &status);
			RAISE_ON_ERROR(status);
			
			if (strcmp(groupLanguageCode, languageCode)) {
				UChar *languageName = ALLOC_N(UChar, 32);
				int32_t languageNameLen = 32;
				VALUE group = rb_ary_new2(2);
				
				//get display name
				languageNameLen = uloc_getDisplayLanguage(RSTRING(RARRAY(RARRAY(select)->ptr[i])->ptr[1])->ptr, locale, languageName, languageNameLen, &status);
				if (status == U_BUFFER_OVERFLOW_ERROR) {
					status = U_ZERO_ERROR;
					REALLOC_N(languageName, UChar, languageNameLen);
					uloc_getDisplayLanguage(RSTRING(RARRAY(RARRAY(select)->ptr[i])->ptr[1])->ptr, locale, languageName, languageNameLen, &status);
				}
				RAISE_ON_ERROR(status);
				//create group
				rb_ary_push(group, u_strToRString(languageName, languageNameLen));
				options = rb_ary_new();
				rb_ary_push(group, options);
				rb_ary_push(tmpSelect, group);
				
				strcpy(groupLanguageCode, languageCode);
			}
			//add option
			rb_ary_push(options, RARRAY(select)->ptr[i]);
		}
		select = tmpSelect;
		for (i = 0; i < RARRAY(select)->len; i++) {
			ruby_qsort(RARRAY(RARRAY(RARRAY(select)->ptr[i])->ptr[1])->ptr, RARRAY(RARRAY(RARRAY(select)->ptr[i])->ptr[1])->len, sizeof(VALUE), collateByDisplayName, collator);
		}
	}
	ruby_qsort(RARRAY(select)->ptr, RARRAY(select)->len, sizeof(VALUE), collateByDisplayName, collator);
	ucol_close(collator);
	
	return select;
}

/**
 *  call-seq:
 *     ICU::Locale.determine_from_http(accept_language_header) => locale
 */
VALUE rb_ICU_Locale_singleton_determine_from_http(VALUE self, VALUE acceptLanguageHeader)
{
	UErrorCode status = U_ZERO_ERROR;
	UEnumeration *availableLocales;
	char *locale = ALLOC_N(char, 16);
	int32_t localeLen;
	UAcceptResult outResult;
	
	availableLocales = ures_openAvailableLocales(NULL, &status);
	RAISE_ON_ERROR(status);
	localeLen = uloc_acceptLanguageFromHTTP(locale, 16, &outResult, RSTRING(acceptLanguageHeader)->ptr, availableLocales, &status);
	if (status == U_BUFFER_OVERFLOW_ERROR) {
		status = U_ZERO_ERROR;
		REALLOC_N(locale, char, localeLen);
		uloc_acceptLanguageFromHTTP(locale, localeLen, &outResult, RSTRING(acceptLanguageHeader)->ptr, availableLocales, &status);
	}
	RAISE_ON_ERROR(status);
	
	return rb_str_new(locale, localeLen);
}

VALUE rb_ICU_Locale_initialize(VALUE self, VALUE locale_id)
{
	Check_Type(locale_id, T_STRING);
	CheckLocaleID(StringValuePtr(locale_id));
	rb_iv_set(self, "@locale_id", locale_id);
	
	return self;
}

VALUE rb_ICU_Locale_parent(VALUE self)
{
	char parent[16];
	int32_t parentLen = 16;
	UErrorCode status = U_ZERO_ERROR;
	
	parentLen = uloc_getParent(RSTRING(rb_iv_get(self, "@locale_id"))->ptr, parent, parentLen, &status);

	return parentLen ? rb_funcall(cLocale, rb_intern("get"), 1, rb_str_new(parent, parentLen)) : Qnil;
}

VALUE rb_ICU_Locale_country(VALUE self)
{
	extern VALUE cCountry;
	
	VALUE country;
	
	country = rb_iv_get(self, "@country");
	if (country == Qnil) {
		char country[3];
		int32_t countryLen = 3;
		UErrorCode status = U_ZERO_ERROR;
		
		countryLen = uloc_getCountry(RSTRING(rb_iv_get(self, "@locale_id"))->ptr, country, countryLen, &status);
		RAISE_ON_ERROR(status);
		
		return rb_iv_set(self, "@country", countryLen ? rb_funcall(cCountry, rb_intern("get"), 1, rb_str_new(country, countryLen)) : Qnil);
	} else {
		return country;
	}
}

VALUE rb_ICU_Locale_language(VALUE self)
{
	extern VALUE cLanguage;
	
	VALUE language;
	
	language = rb_iv_get(self, "@language");
	if (language == Qnil) {
		char language[3];
		int32_t languageLen = 3;
		UErrorCode status = U_ZERO_ERROR;
		
		languageLen = uloc_getLanguage(RSTRING(rb_iv_get(self, "@locale_id"))->ptr, language, languageLen, &status);
		RAISE_ON_ERROR(status);
		
		return rb_iv_set(self, "@language", languageLen ? rb_funcall(cLanguage, rb_intern("get"), 1, rb_str_new(language, languageLen)) : Qnil);
	} else {
		return language;
	}
}

void Init_ICU_Locale(void)
{
	extern VALUE mICU;
	extern void Init_ICU_Locale_Country(void);
	extern void Init_ICU_Locale_Language(void);
	
	cLocale = rb_define_class_under(mICU, "Locale", rb_cObject);
	rb_define_singleton_method(cLocale, "default", rb_ICU_Locale_singleton_default, 0);
	rb_define_singleton_method(cLocale, "default=", rb_ICU_Locale_singleton_default_setter, 1);
	rb_define_singleton_method(cLocale, "get", rb_ICU_Locale_singleton_get, 1);
	rb_define_singleton_method(cLocale, "available", rb_ICU_Locale_singleton_available, -1);
	rb_define_singleton_method(cLocale, "determine_from_http", rb_ICU_Locale_singleton_determine_from_http, 1);
	rb_funcall(cLocale, rb_intern("private_class_method"), 1, ID2SYM(rb_intern("new")));
	rb_iv_set(cLocale, "@@locales", rb_hash_new());
	rb_define_private_method(cLocale, "initialize", rb_ICU_Locale_initialize, 1);
	rb_define_attr(cLocale, "locale_id", 1, 0);
	rb_define_alias(cLocale, "to_s", "locale_id");
	rb_define_alias(cLocale, "to_str", "locale_id");
	rb_define_method(cLocale, "parent", rb_ICU_Locale_parent, 0);
	rb_define_method(cLocale, "country", rb_ICU_Locale_country, 0);
	rb_define_method(cLocale, "language", rb_ICU_Locale_language, 0);
	
	Init_ICU_Locale_Country();
	Init_ICU_Locale_Language();
}
