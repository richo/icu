require 'test/unit'
require File.expand_path(File.join(File.dirname(__FILE__), '..', 'lib', 'icu'))

class TestTime < Test::Unit::TestCase
  def setup
    ICU::Locale.default = 'en_US'
  end

  def test_localize_zone
  	Time.default_zone = 'Europe/Paris'
  	assert_equal('10:31:34 AM CEDT', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => false, :time => :full))
  	assert_equal('8:31:34 AM GMT+00:00', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => false, :time => :full, :zone => 'GMT'))
  	assert_equal('3:31:34 AM GMT-05:00', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => false, :time => :full, :zone => 'GMT-05:00'))
  end

  def test_localize_style
  	assert_equal('Monday, May 8, 2006 8:31:34 AM', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => :full))
  	assert_equal('May 8, 2006 8:31:34 AM', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => :long))
  	assert_equal('May 8, 2006 8:31:34 AM', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => :medium))
  	assert_equal('5/8/06 8:31:34 AM', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => :short))
  	assert_equal('8:31:34 AM', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => false))
  	assert_equal('May 8, 2006 8:31:34 AM GMT+00:00', Time.utc(2006, 5, 8, 8, 31, 34).localize(:time => :full))
  	assert_equal('May 8, 2006 8:31:34 AM GMT+00:00', Time.utc(2006, 5, 8, 8, 31, 34).localize(:time => :long))
  	assert_equal('May 8, 2006 8:31:34 AM', Time.utc(2006, 5, 8, 8, 31, 34).localize(:time => :medium))
  	assert_equal('May 8, 2006 8:31 AM', Time.utc(2006, 5, 8, 8, 31, 34).localize(:time => :short))
  	assert_equal('May 8, 2006', Time.utc(2006, 5, 8, 8, 31, 34).localize(:time => false))
  end

  def test_localize_locale
  	assert_equal('lunes 8 de mayo de 2006 8:31:34', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => :full, :locale => 'es_ES'))
  	assert_equal('08:31:34 8 מאי 2006', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => :long, :locale => ICU::Locale.get('he_IL')))
  	assert_equal('2006. svibnja 08 08:31:34', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => :long, :locale => 'hr_HR'))
  end

  def test_zones
	assert_instance_of(Array, Time.zones)
	assert_equal(["Africa/Ceuta", "Atlantic/Canary", "Europe/Madrid"], Time.zones(:countries => ['ES']))
  	assert_equal(
  		[["Central European Standard Time", "ECT"], ["Central European Standard Time", "Europe/Paris"], ["Greenwich Mean Time", "Europe/London"], ["Greenwich Mean Time", "Europe/Belfast"], ["Greenwich Mean Time", "GB"], ["Greenwich Mean Time", "GB-Eire"]],
  		Time.zones(:countries => ['FR', 'GB'], :type => :standard)
  	)
  	assert_equal([["Spain", ["Africa/Ceuta", "Atlantic/Canary", "Europe/Madrid"]]], Time.zones(:countries => ['ES'], :group => true))
  end

  def test_default_zone
  	Time.default_zone = 'Europe/Paris'
  	assert_equal('Europe/Paris', Time.default_zone)
  	assert_equal('10:31:34 AM CEDT', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => false, :time => :full))
  	Time.default_zone = 'UTC'
  	assert_equal('UTC', Time.default_zone)
  	assert_equal('8:31:34 AM GMT+00:00', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => false, :time => :full))
  	Time.default_zone = 'worg time zone'
  	assert_equal('GMT', Time.default_zone)
  	assert_equal('8:31:34 AM GMT+00:00', Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => false, :time => :full))
  end

  def test_pattern
  	assert_equal("EEEE d' de 'MMMM' de 'yyyy H:mm:ss", Time.pattern(:date => :full, :locale => 'es_ES'))
  	assert_equal('yyyy. MMMM dd HH:mm:ss', Time.pattern(:date => :long, :locale => ICU::Locale.get('hr_HR')))
  	assert_equal('HH:mm:ss dd/MM/yyyy', Time.pattern(:date => :medium, :locale => 'he_IL'))
  	assert_equal('M/d/yy h:mm:ss a', Time.pattern(:date => :short))
  	assert_equal('h:mm:ss a', Time.pattern(:date => false))
  	assert_equal("dd/MM/yyyy HH'H'mm''ss\" z", Time.pattern(:time => :full, :locale => 'es_ES'))
  	assert_equal('yyyy.MM.dd HH:mm:ss z', Time.pattern(:time => :long, :locale => 'hr_HR'))
  	assert_equal('HH:mm:ss dd/MM/yyyy', Time.pattern(:time => :medium, :locale => 'he_IL'))
  	assert_equal('MMM d, yyyy h:mm a', Time.pattern(:time => :short))
  	assert_equal('MMM d, yyyy', Time.pattern(:time => false))
  end

  def test_week_start
  	assert_equal(1, Time.week_start)
  	assert_equal(2, Time.week_start(:locale => 'hr_HR'))
  	assert_equal(2, Time.week_start(:locale => ICU::Locale.get('hr_HR')))
  end
end
