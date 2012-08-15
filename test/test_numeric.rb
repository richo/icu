require 'test/unit'
require File.expand_path(File.join(File.dirname(__FILE__), '..', 'lib', 'icu'))

class TestNumeric < Test::Unit::TestCase
  def setup
    ICU::Locale.default = 'en_US'
  end

  def test_types
    assert_raise(TypeError) { 13243412.233.localize(false) }
    assert_raise(TypeError) { 13243412.233.localize(:currency, false) }
    assert_raise(TypeError) { 13243412.233.localize(:currency, :locale => false) }
    assert_raise(TypeError) { 13243412.233.localize(:currency, :round_increment => false) }
    assert_raise(TypeError) { 13243412.233.localize(:currency, :round_mode => false) }
    assert_raise(TypeError) { 13243412.233.localize(:currency, :precision => false) }
  end

  def test_limits
    # Fixnum
    assert_equal('1,073,741,823', 1073741823.localize)
    assert_equal('-1,073,741,823', -1073741823.localize)
    # Bignum
    assert_equal('9,223,372,036,854,775,807', 9223372036854775807.localize)
    assert_equal('-9,223,372,036,854,775,807', -9223372036854775807.localize)
    assert_raise(RangeError) { 9223372036854775808.localize }
    # Float
    assert_equal('13,243,412.233', 13243412.233.localize)
    assert_equal('-13,243,412.233', -13243412.233.localize)
  end

  def test_decimal
    assert_equal('3,490,835,234.656', 3490835234.6556.localize)
    assert_equal('3.490.835.234,656', 3490835234.6556.localize(:decimal, :locale => 'es_ES'))
    assert_equal('٣٤٩٠٨٣٥٢٣٤٫٦٥٦', 3490835234.6556.localize(:decimal, :locale => ICU::Locale.get('ar_SY')))
  end

  def test_currency
    assert_equal('$1,073,741,823.32', 1073741823.323.localize(:currency))
    assert_equal('($1,073,741,823.32)', -1073741823.323.localize(:currency))
    assert_equal('€1,073,741,823.32', 1073741823.323.localize(:currency, :currency => 'EUR'))

    assert_equal('1.073.741.823,32 €', 1073741823.323.localize(:currency, :locale => 'es_ES'))
    assert_equal('-1.073.741.823,32 €', -1073741823.323.localize(:currency, :locale => 'es_ES'))
    assert_equal('1.073.741.823,32 US$', 1073741823.323.localize(:currency, :locale => 'es_ES', :currency => 'USD'))

    assert_equal('1,073,741,823.32 ₪', 1073741823.323.localize(:currency, :locale => 'he_IL'))
    assert_equal('-1,073,741,823.32 ₪', -1073741823.323.localize(:currency, :locale => 'he_IL'))
    assert_equal('1,073,741,823.32 US$', 1073741823.323.localize(:currency, :locale => 'he_IL', :currency => 'USD'))
  end

  def test_rounding
    assert_equal('$2,314.30', 2314.3289.localize(:currency, :round_increment => 0.1))
    assert_equal('$2,314.40', 2314.3289.localize(:currency, :round_increment => 0.1, :round_mode => :ceil))
  end

  def test_precision
    assert_equal('$2,314.3290', 2314.32896.localize(:currency, :precision => 4))
    assert_equal('$1,234.674', 1234.67433.localize(:currency, :precision => 3))
  end

  def test_percent
    assert_equal('12%', 0.12.localize(:percent))
    assert_equal('1,200%', 12.localize(:percent))
  end

  def test_scientific
    assert_equal('4.3289E4', 43289.localize(:scientific))
    assert_equal('1.1239087212E0', 1.1239087212.localize(:scientific))
  end

  def test_spellout
    assert_equal('forty-three thousand two hundred and eighty-nine', 43289.localize(:spellout))
    assert_equal('one point one two three nine', 1.1239.localize(:spellout))

    assert_equal('cuarenta y tres mil doscientos ochenta y nueve', 43289.localize(:spellout, :locale => 'es_ES'))
    assert_equal('uno punto uno dos tres nueve', 1.1239.localize(:spellout, :locale => 'es_ES'))
  end
end
