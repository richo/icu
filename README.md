# ICU -- International Components for Unicode for Ruby

Pragmatic integration of ICU (International Components for Unicode) into Ruby. Format numbers, dates, times and currency amounts according the conventions of a chosen locale.

## Examples

```ruby

ICU::Locale.default = 'en_US'
=> 'en_US'
locale = ICU::Locale.get('es_ES')
=> #<ICU::Locale:0x2ac572348c00 @locale_id="en_US">
locale.country.name
=> 'Spain'
locale.language.name
=> 'Spanish'
locale.country.name('es_ES')
=> 'España'
locale.language.name('es_ES')
=> 'español'

ICU::Locale.available
=> [all installed locales]
ICU::Locale.available(:allowed => ['es_ES', 'en_US'])
=> [["English (United States)", "en_US"], ["Spanish (Spain)", "es_ES"]]

ICU::Locale.determine_from_http(request.env['HTTP_ACCEPT_LANGUAGE'])
=> 'es_ES'

1073741823.localize
=> '1,073,741,823'
-1073741823.323.localize(:currency, :locale => 'he_IL')
=> '-1,073,741,823.32 ₪'
1.1239.localize(:spellout)
=> 'one point one two three nine'
43289.localize(:spellout, :locale => 'es_ES')
=> 'cuarenta y tres mil doscientos ochenta y nueve'

Time.utc(2006, 5, 8, 8, 31, 34).localize(:date => :full, :locale => 'es_ES')
=> 'lunes 8 de mayo de 2006 8:31:34'
Time.pattern(:time => :full, :locale => 'es_ES')
=> "dd/MM/yyyy HH'H'mm''ss\" z"

```

See more in units tests in test/


## Download

The latest version of ICU can be found at

* http://rubyforge.org/frs/?group_id=5009


## Requeriments

You must install ICU 3.6 or 3.8 from http://icu-project.org/download/

See ext/extconf.rb


## Installation

The prefered method of installing ICU is through its GEM file. You'll need to have
[RubyGems](http://rubygems.rubyforge.org/wiki/wiki.pl) installed for that, though. If you have,
then use:

```bash

[sudo] gem install icu
```

If fails try with:

```bash

[sudo] gem install icu -- --with-icu-lib=/usr/local/lib --with-icu-include=/usr/local/src/icu/source/i18n
```

See ext/extconf.rb


## Support

You can find the ICU RubyForge page at http://rubyforge.org/projects/icu.
For other information, feel free to contact mailto:josh@vectrice.com.


## License

Copyright (c) 2007 Joshua Llorach, released under the GNU Lesser General Public License
