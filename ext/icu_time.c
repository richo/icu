// Copyright (c) 2007 Joshua Llorach
//  
// This file is part of ICU Ruby extension.
// 
// ICU is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Foobar is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public License
// along with ICU.  If not, see <http://www.gnu.org/licenses/>.

#include "icu.h"

#include "unicode/ucal.h"
#include "unicode/udat.h"

/******************* Time */

/**
 *  call-seq:
 *     23874.localize(:date => :short) => 23.874
 */
VALUE rb_Time_localize(int argc, VALUE *argv, VALUE self)
{
	VALUE options;
	UDateFormatStyle dateStyle = UDAT_DEFAULT, timeStyle = UDAT_DEFAULT;
	char *locale = NULL;
	const UChar *timeZone = 0;
	int32_t timeZoneLen = -1;
	
	UDateFormat *format;
	UErrorCode status;
	UChar result[256];
	/* arguments */
	rb_scan_args(argc, argv, "01", &options);
	if (options != Qnil) {
		VALUE rb_locale, rb_timeZone;
		
		Check_Type(options, T_HASH);
		/* date and time style */
		dateStyle = getStyle(options, "date");
		timeStyle = getStyle(options, "time");
		/* locale */
		rb_locale = rb_hash_aref(options, ID2SYM(rb_intern("locale")));
		if (rb_locale != Qnil) {
			locale = StringValuePtr(rb_locale);
		}
		/* time zone */
		rb_timeZone = rb_hash_aref(options, ID2SYM(rb_intern("zone")));
		if (rb_timeZone != Qnil) {
			Check_Type(rb_timeZone, T_STRING);
			timeZone = u_strFromRString(rb_timeZone, &timeZoneLen);
		}
	}
	
	/* formatter */
	status = U_ZERO_ERROR;
    format = udat_open(timeStyle, dateStyle, locale, timeZone, timeZoneLen, NULL, -1, &status);
    RAISE_ON_ERROR(status);
	/* format */
	status = U_ZERO_ERROR;
	{
		extern struct timeval rb_time_timeval(VALUE time);
		struct timeval tv;
		UDate dateToFormat;
		UChar result[256];
		
		tv = rb_time_timeval(self);
    	dateToFormat = ((double)tv.tv_sec + (double)tv.tv_usec / 1e6) * 1e3;
    	udat_format(format, dateToFormat, result, 256, NULL, &status);
	    RAISE_ON_ERROR(status);
	    /* free resources */
	    unum_close(format);
	   
	    return u_strToRString(result, -1);
	}
}

UDateFormatStyle getStyle(VALUE options, const char * key)
{
	UDateFormatStyle formatStyle = UDAT_DEFAULT;
	VALUE style;
	
	style = rb_hash_aref(options, ID2SYM(rb_intern(key)));
	if (style != Qnil) {
		if (style == Qfalse) {
			formatStyle = UDAT_NONE;
		} else {
			ID style_ID;
	
			Check_Type(style, T_SYMBOL);
			style_ID = SYM2ID(style);
			if (style_ID == rb_intern("full")) {
				formatStyle = UDAT_FULL;
			} else if (style_ID == rb_intern("long")) {
				formatStyle = UDAT_LONG;
			} else if (style_ID == rb_intern("medium")) {
				formatStyle = UDAT_MEDIUM;
			} else if (style_ID == rb_intern("short")) {
				formatStyle = UDAT_SHORT;
			} else {
				rb_raise(rb_eArgError, "unsupported format style %s", rb_id2name(style_ID));
			}
		}
	}
	
	return formatStyle;
}

VALUE enumToOptions(UEnumeration *enumeration, VALUE options, UCalendar *calendar, UCalendarDisplayNameType type, char *locale)
{
	const UChar *zoneID;
	int32_t zoneIDLen;
	UErrorCode status = U_ZERO_ERROR;
	
	while (zoneID = uenum_unext(enumeration, &zoneIDLen, &status)) {
		RAISE_ON_ERROR(status);
		if (type == -1) {
			rb_ary_push(options, u_strToRString(zoneID, zoneIDLen));
		} else {
			VALUE option = rb_ary_new2(2);
			UChar *zoneName = ALLOC_N(UChar, 64);
			int32_t zoneNameLen = 64;
			
			ucal_setTimeZone(calendar, zoneID, zoneIDLen, &status);
			RAISE_ON_ERROR(status);
			zoneNameLen = ucal_getTimeZoneDisplayName(calendar, UCAL_STANDARD, locale, zoneName, zoneNameLen, &status);
			if (status == U_BUFFER_OVERFLOW_ERROR) {
				status = U_ZERO_ERROR;
				REALLOC_N(zoneName, UChar, zoneNameLen);
				ucal_getTimeZoneDisplayName(calendar, UCAL_STANDARD, locale, zoneName, zoneNameLen, &status);
			}
			RAISE_ON_ERROR(status);
			rb_ary_push(option, u_strToRString(zoneName, zoneNameLen));
			rb_ary_push(option, u_strToRString(zoneID, zoneIDLen));
			rb_ary_push(options, option);
		}
	}
	
	return options;
}

/**
 *  call-seq:
 *     Time.zones(:countries => ['US'], :locale => 'es_ES', :type => :short) => 23.874
 * 	   Time.zones(:countries => :all, :group => true) => 23.874
 */
VALUE rb_Time_singleton_zones(int argc, VALUE *argv, VALUE self)
{
	VALUE options, countries = Qfalse, group = Qfalse;
	char *locale = NULL;
	UCalendarDisplayNameType type = -1;
	
	UCalendar *calendar;
	UErrorCode status = U_ZERO_ERROR;
	UEnumeration *enumeration;
	VALUE select = rb_ary_new();
	UCollator *collator;
	
	//arguments
	rb_scan_args(argc, argv, "01", &options);
	if (options != Qnil) {
		VALUE rb_locale, rb_type;
		
		Check_Type(options, T_HASH);
		//locale
		rb_locale = rb_hash_aref(options, ID2SYM(rb_intern("locale")));
		if (rb_locale != Qnil) {
			locale = StringValuePtr(rb_locale);
		}
		//countries
		countries = rb_hash_aref(options, ID2SYM(rb_intern("countries")));
		if (countries == Qnil) {
			countries = Qfalse;
		} else {
			
		}
		//group
		group = rb_hash_aref(options, ID2SYM(rb_intern("group")));
		group = group && group != Qnil ? Qtrue : Qfalse;
		//type
		rb_type = rb_hash_aref(options, ID2SYM(rb_intern("type")));
		if (rb_type != Qnil) {
			ID rb_type_ID;
	
			Check_Type(rb_type, T_SYMBOL);
			rb_type_ID = SYM2ID(rb_type);
			if (rb_type_ID == rb_intern("standard")) {
				type = UCAL_STANDARD;
			} else if (rb_type_ID == rb_intern("short_standard")) {
				type = UCAL_SHORT_STANDARD;
			} else if (rb_type_ID == rb_intern("dst")) {
				type = UCAL_DST;
			} else if (rb_type_ID == rb_intern("short_dst")) {
				type = UCAL_SHORT_DST;
			} else {
				rb_raise(rb_eArgError, "unsupported time zone type %s", rb_id2name(rb_type_ID));
			}
		}
	}
	//calendar
	calendar = ucal_open(NULL, -1, locale, UCAL_GREGORIAN, &status);
	RAISE_ON_ERROR(status);
	
	collator = ucol_open(locale, &status);
	RAISE_ON_ERROR(status);
	
	if (countries) {
		const char* const* isoCountries = uloc_getISOCountries();
		
		while (*isoCountries != NULL) {
			if (TYPE(countries) == T_SYMBOL || (TYPE(countries) == T_ARRAY && rb_ary_includes(countries, rb_str_new2(*isoCountries)))) {
				if (group) {
					char fakeLocale[4] = "_";
					UChar *country = ALLOC_N(UChar, 32);
					int32_t countryLen = 32;
					VALUE optgroup = rb_ary_new2(2);
					
					strcat(fakeLocale, *isoCountries);
					countryLen = uloc_getDisplayCountry(fakeLocale, locale, country, countryLen, &status);
					if (status == U_BUFFER_OVERFLOW_ERROR) {
						status = U_ZERO_ERROR;
						REALLOC_N(country, UChar, countryLen);
						uloc_getDisplayCountry(fakeLocale, locale, country, countryLen, &status);
					}
					rb_ary_push(optgroup, u_strToRString(country, countryLen));
					enumeration = ucal_openCountryTimeZones(*isoCountries, &status);
					RAISE_ON_ERROR(status);
					rb_ary_push(optgroup, enumToOptions(enumeration, rb_ary_new(), calendar, type, locale));
					uenum_close(enumeration);
					if (RARRAY(RARRAY(optgroup)->ptr[1])->len) {
						ruby_qsort(RARRAY(RARRAY(optgroup)->ptr[1])->ptr, RARRAY(RARRAY(optgroup)->ptr[1])->len, sizeof(VALUE), collateByDisplayName, collator);
						rb_ary_push(select, optgroup);
					}
				} else {
					enumeration = ucal_openCountryTimeZones(*isoCountries, &status);
					RAISE_ON_ERROR(status);
					enumToOptions(enumeration, select, calendar, type, locale);
					uenum_close(enumeration);
				}
			}
			*isoCountries++;
		}
	} else {
		enumeration = ucal_openTimeZones(&status);
		RAISE_ON_ERROR(status);
		enumToOptions(enumeration, select, calendar, type, locale);
		uenum_close(enumeration);
	}
	//sort
	ruby_qsort(RARRAY(select)->ptr, RARRAY(select)->len, sizeof(VALUE), collateByDisplayName, collator);
	//free
	ucal_close(calendar);
	ucol_close(collator);
	
	return select;
}

/**
 *  call-seq:
 *     Time.default_zone = 'GMT'
 */
VALUE rb_Time_singleton_set_default_zone(VALUE self, VALUE zone)
{
	UErrorCode status = U_ZERO_ERROR;

	ucal_setDefaultTimeZone((const UChar *)u_strFromRString(zone, NULL), &status);
	RAISE_ON_ERROR(status);
	
	return zone;
}

/**
 *  call-seq:
 *     Time.default_zone -> 'GMT'
 */
VALUE rb_Time_singleton_get_default_zone(VALUE self)
{
	UChar * timeZone = ALLOC_N(UChar, 64);
	int32_t timeZoneLen;
	UErrorCode status = U_ZERO_ERROR;
	
	timeZoneLen = ucal_getDefaultTimeZone(timeZone, 2, &status);
	if (status == U_BUFFER_OVERFLOW_ERROR) {
		status = U_ZERO_ERROR;
		REALLOC_N(timeZone, UChar, timeZoneLen);
		ucal_getDefaultTimeZone(timeZone, timeZoneLen, &status);
	}
	RAISE_ON_ERROR(status);
	
	return u_strToRString(timeZone, timeZoneLen);
}

/**
 *  call-seq:
 *     23874.localize(:date => :short) => 23.874
 */
VALUE rb_Time_singleton_pattern(int argc, VALUE *argv, VALUE self)
{
	VALUE options;
	UDateFormatStyle dateStyle = UDAT_DEFAULT, timeStyle = UDAT_DEFAULT;
	char *locale = NULL;
	
	UDateFormat *format;
	UErrorCode status = U_ZERO_ERROR;
	
	UChar * pattern = ALLOC_N(UChar, 64);
	int32_t patternLen;

	/* arguments */
	rb_scan_args(argc, argv, "01", &options);
	if (options != Qnil) {
		VALUE rb_locale;
		
		Check_Type(options, T_HASH);
		/* date and time style */
		dateStyle = getStyle(options, "date");
		timeStyle = getStyle(options, "time");
		/* locale */
		rb_locale = rb_hash_aref(options, ID2SYM(rb_intern("locale")));
		if (rb_locale != Qnil) {
			locale = StringValuePtr(rb_locale);
		}
	}
	/* formatter */
	status = U_ZERO_ERROR;
    format = udat_open(timeStyle, dateStyle, locale, NULL, 0, NULL, 0, &status);
    RAISE_ON_ERROR(status);
    /* pattern */
	patternLen = udat_toPattern(format, 0, pattern, 64, &status);
	if (status == U_BUFFER_OVERFLOW_ERROR) {
		status = U_ZERO_ERROR;
		REALLOC_N(pattern, UChar, patternLen);
		udat_toPattern(format, 0, pattern, 64, &status);
	}
	RAISE_ON_ERROR(status);
	/* free */
	udat_close(format);
    
    return u_strToRString(pattern, patternLen);
}

/**
 *  call-seq:
 *     23874.localize(:date => :short) => 23.874
 */
VALUE rb_Time_singleton_week_start(int argc, VALUE *argv, VALUE self)
{
	VALUE options;
	char * locale = NULL;
	UErrorCode status = U_ZERO_ERROR;
	UCalendar * calendar;
	int32_t result;
	
	/* arguments */
	rb_scan_args(argc, argv, "01", &options);
	if (options != Qnil) {
		VALUE rb_locale;
		
		Check_Type(options, T_HASH);
		/* locale */
		rb_locale = rb_hash_aref(options, ID2SYM(rb_intern("locale")));
		if (rb_locale != Qnil) {
			locale = StringValuePtr(rb_locale);
		}
	}
	/* calendar */
	calendar = ucal_open(NULL, 0, locale, UCAL_GREGORIAN, &status);
	RAISE_ON_ERROR(status);
	/* attribute */
	result = ucal_getAttribute(calendar, UCAL_FIRST_DAY_OF_WEEK);
	ucal_close(calendar);
	
	return INT2NUM(result);
}

void Init_ICU_Time(void)
{
	rb_define_method(rb_cTime, "localize", rb_Time_localize, -1);
	rb_define_singleton_method(rb_cTime, "zones", rb_Time_singleton_zones, -1);
	rb_define_singleton_method(rb_cTime, "default_zone=", rb_Time_singleton_set_default_zone, 1);
	rb_define_singleton_method(rb_cTime, "default_zone", rb_Time_singleton_get_default_zone, 0);
	rb_define_singleton_method(rb_cTime, "pattern", rb_Time_singleton_pattern, -1);
	rb_define_singleton_method(rb_cTime, "week_start", rb_Time_singleton_week_start, -1);
}
