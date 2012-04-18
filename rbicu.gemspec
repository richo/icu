# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)

Gem::Specification.new do |s|
  s.name = %q{rbicu}
  s.version = "0.0.2"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Joshua Llorach", "Rich Healey"]
  s.date = %q{2012-04-16}
  s.description = %q{Thin wrapper for icu}
  s.email = ["richard.healey@99designs.com"]
  s.extensions = ["ext/extconf.rb"]
  s.files = ["ext/extconf.rb", "lib/icu.rb"]
  s.homepage = %q{https://github.com/99designs/rbicu}
  s.require_paths = ["lib", "ext/icu"]
  s.rubygems_version = %q{1.6.2}
  s.summary = %q{Thin wrapper for icu}

end
