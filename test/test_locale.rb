require 'test/unit'
require File.expand_path(File.join(File.dirname(__FILE__), '..', 'lib', 'icu'))

class TestLocale < Test::Unit::TestCase
  def setup
    ICU::Locale.default = 'en_US'
  end

  def test_default
  	ICU::Locale.default = 'es_ES'
  	assert_equal('es_ES', ICU::Locale.default.locale_id)
  	ICU::Locale.default = ICU::Locale.get('en_US')
  	assert_equal('en_US', ICU::Locale.default.locale_id)
  	assert_raise(ArgumentError) { ICU::Locale.default = 'worg locale' }
  end

  def test_get
  	assert_equal(ICU::Locale.get('es_ES').object_id, ICU::Locale.get('es_ES').object_id)
  	assert_equal(ICU::Locale.get('en_US').object_id, ICU::Locale.get('en_US').object_id)
  	assert_raise(ArgumentError) { ICU::Locale.get('worg locale') }
  end

  def test_parent
  	locale = ICU::Locale.get('es_ES').parent
  	assert_equal('es', locale.locale_id)
  	locale = locale.parent
  	assert_equal(nil, locale)

  	assert_equal(nil, ICU::Locale.get('es_ES').parent.parent)
  end

  def test_country
  	country = ICU::Locale.get('es_ES').country
  	assert_equal(ICU::Locale::Country.get('ES').object_id, country.object_id)
  	assert_equal('ES', country.code)
  	assert_equal('Spain', country.name)
  	assert_equal('España', country.name('es_ES'))
  	assert_equal('España', country.name(ICU::Locale.get('es_ES')))
  	assert_nil(ICU::Locale.get('es').country)
  	assert_raise(ArgumentError) { ICU::Locale::Country.get('worg country') }
  end

  def test_language
  	language = ICU::Locale.get('es_ES').language
  	assert_equal(ICU::Locale::Language.get('es').object_id, language.object_id)
  	assert_equal('es', language.code)
  	assert_equal('Spanish', language.name)
  	assert_equal('español', language.name('es_ES'))
  	assert_equal('español', language.name(ICU::Locale.get('es_ES')))
  	assert_nil(ICU::Locale.get('_ES').language)
  	assert_raise(ArgumentError) { ICU::Locale::Language.get('worg language') }
  end

  def test_available
  	assert_equal(["Afrikaans", "af"], ICU::Locale.available[0])
  	assert_equal(["afrikaans (Afrique du Sud)", "af_ZA"], ICU::Locale.available(:locale => 'fr_FR')[1])
  	assert_equal(["albanski (Albanija)", "sq_AL"], ICU::Locale.available(:locale => ICU::Locale.get('hr_HR'))[3])

  	assert_equal([["English (United States)", "en_US"], ["Spanish (Spain)", "es_ES"]], ICU::Locale.available(:allowed => ['es_ES', 'en_US']))
  	assert_equal(["English", "en"], ICU::Locale.available(:languages => ['es', 'en'])[0])
  	assert_equal(["Basque (Spain)", "eu_ES"], ICU::Locale.available(:countries => ['ES', 'US'])[0])
  	assert_equal(
  		[["Spanish (Spain)", "es_ES"], ["Spanish (United States)", "es_US"]],
  		ICU::Locale.available(:allowed => ['es_ES', 'ca_ES', 'en_US', 'en_GB', 'es_US'], :languages => ['es'], :countries => ['ES', 'US'])
  	)

  	assert_equal(["Albanian", [["Albanian", "sq"], ["Albanian (Albania)", "sq_AL"]]], ICU::Locale.available(:group => true)[1])
  end

  def test_determine
  	assert_equal('es_ES', ICU::Locale.determine_from_http('es_ES,us_US'))
  end
end
