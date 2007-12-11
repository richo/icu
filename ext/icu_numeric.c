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

#include "unicode/unum.h"

/******************* Numeric */

/**
 *  call-seq:
 *     23874.localize(:decimal, 'es_ES') => 23.874
 */
VALUE rb_numeric_localize(int argc, VALUE *argv, VALUE self)
{
	VALUE style, options;
	UNumberFormatStyle formatStyle;
	char *locale = NULL;
	
	UNumberFormat *format;
	UErrorCode status;
	
	UChar result[256];
	/* arguments */
	rb_scan_args(argc, argv, "02", &style, &options);
	if (style == Qnil) {
		formatStyle = UNUM_DECIMAL;
	} else {
		ID style_ID;
		
		Check_Type(style, T_SYMBOL);
		style_ID = SYM2ID(style);
		if (style_ID == rb_intern("decimal")) {
			formatStyle = UNUM_DECIMAL;
		} else if (style_ID == rb_intern("currency")) {
			formatStyle = UNUM_CURRENCY;
		} else if (style_ID == rb_intern("percent")) {
			formatStyle = UNUM_PERCENT;
		} else if (style_ID == rb_intern("scientific")) {
			formatStyle = UNUM_SCIENTIFIC;
		} else if (style_ID == rb_intern("spellout")) {
			formatStyle = UNUM_SPELLOUT;
		} else {
			rb_raise(rb_eArgError, "unsupported format style %s", rb_id2name(style_ID));
		}
	}
	if (options != Qnil) {
		VALUE rb_locale;
		
		Check_Type(options, T_HASH);
		rb_locale = rb_hash_aref(options, ID2SYM(rb_intern("locale")));
		if (rb_locale != Qnil) {
			locale = StringValuePtr(rb_locale);
		}
	}
	/* formatter */
	status = U_ZERO_ERROR;
    format = unum_open(formatStyle, NULL, 0, locale, NULL, &status);
    RAISE_ON_ERROR(status);
    /* set format attributes */
	if (options != Qnil) {
		VALUE currency, precision, round_mode, round_increment;
		
		switch (formatStyle) {
			case UNUM_CURRENCY:
				currency = rb_hash_aref(options, ID2SYM(rb_intern("currency")));
				if (currency != Qnil) {
					UChar *uStr;
					int32_t uStrLen;
					
					uStr = u_strFromRString(currency, &uStrLen);
					status = U_ZERO_ERROR;
					unum_setTextAttribute(format, UNUM_CURRENCY_CODE, uStr, uStrLen, &status);
					RAISE_ON_ERROR(status);
				}
			case UNUM_DECIMAL:
				/* precision */
				precision = rb_hash_aref(options, ID2SYM(rb_intern("precision")));
				if (precision != Qnil) {
					Check_Type(precision, T_FIXNUM);
					status = U_ZERO_ERROR;
					unum_setAttribute(format, UNUM_FRACTION_DIGITS, NUM2INT(precision));
					RAISE_ON_ERROR(status);
				}
				
				round_mode = rb_hash_aref(options, ID2SYM(rb_intern("round_mode")));
				if (round_mode != Qnil) {
					ID round_mode_ID;
					UNumberFormatRoundingMode rounding_mode;
					
					Check_Type(round_mode, T_SYMBOL);
					round_mode_ID = SYM2ID(round_mode);
					if (round_mode_ID == rb_intern("ceil")) {
						rounding_mode = UNUM_ROUND_CEILING;
					} else if (round_mode_ID == rb_intern("floor")) {
						rounding_mode = UNUM_ROUND_FLOOR;
					} else if (round_mode_ID == rb_intern("down")) {
						rounding_mode = UNUM_ROUND_DOWN;
					} else if (round_mode_ID == rb_intern("up")) {
						rounding_mode = UNUM_ROUND_UP;
					} else if (round_mode_ID == rb_intern("halfeven")) {
						rounding_mode = UNUM_FOUND_HALFEVEN;
					} else if (round_mode_ID == rb_intern("halfdown")) {
						rounding_mode = UNUM_ROUND_HALFDOWN;
					} else if (round_mode_ID == rb_intern("halfup")) {
						rounding_mode = UNUM_ROUND_HALFUP;
					} else {
						rb_raise(rb_eArgError, "unsupported rounding mode '%s'", rb_id2name(round_mode_ID));
					}
					status = U_ZERO_ERROR;
					unum_setAttribute(format, UNUM_ROUNDING_MODE, rounding_mode);
					RAISE_ON_ERROR(status);
				}
				round_increment = rb_hash_aref(options, ID2SYM(rb_intern("round_increment")));
				if (round_increment != Qnil) {
					Check_Type(round_increment, T_FLOAT);
					status = U_ZERO_ERROR;
					unum_setDoubleAttribute(format, UNUM_ROUNDING_INCREMENT, NUM2DBL(round_increment));
					RAISE_ON_ERROR(status);
				}
		}
	}
	/* format */
	status = U_ZERO_ERROR;
    switch (TYPE(self)) {
    	case T_FIXNUM:
    		unum_format(format, NUM2INT(self), result, 256, NULL, &status);
    		break;
    	case T_FLOAT:
    		unum_formatDouble(format, NUM2DBL(self), result, 256, NULL, &status);
    		break;
    	case T_BIGNUM:
			unum_formatInt64(format, rb_big2ll(self), result, 256, NULL, &status);
    		break;
			
    }
    RAISE_ON_ERROR(status);
    /* free resources */
    unum_close(format);
   
    return u_strToRString(result, -1);
}

void Init_ICU_Numeric(void)
{
	rb_define_method(rb_cNumeric, "localize", rb_numeric_localize, -1);
}
