PKG_VERSION = '0.9.2'

require 'rake/gempackagetask'
require 'rake/testtask'

spec = Gem::Specification.new do |s|
  s.name = 'icu'
  s.version = PKG_VERSION
  s.authors = ['Joshua Llorach', "Rich Healey"]
  s.email = 'richard.healey@99designs.com'
  s.homepage = 'http://github.com/99designs/icu'
  s.platform = Gem::Platform::RUBY
  s.summary = 'International Components for Unicode for Ruby'
  s.files = FileList['{ext,lib,test}/**/*'].to_a << 'COPYING' << 'COPYING.LESSER'
  s.require_path = 'lib'
  s.autorequire = 'icu'
  s.extensions = ['ext/extconf.rb']
  s.test_files = FileList['{test}/**/*'].to_a
  s.has_rdoc = true
  s.extra_rdoc_files = ['README']
  s.rubyforge_project = 'icu'
end

Rake::GemPackageTask.new(spec) do |pkg|
  pkg.need_tar = true
end

desc "Publish the release files to RubyForge."
task :release => [ :package ] do
  require 'rubyforge'

  rubyforge = RubyForge.new
  rubyforge.login
  rubyforge.add_release('icu', 'icu', "ICU #{PKG_VERSION}", "pkg/icu-#{PKG_VERSION}.gem", "pkg/icu-#{PKG_VERSION}.tgz")
end

Rake::TestTask.new do |t|
  t.libs << "lib"
  t.test_files = FileList['test/test*.rb']
  t.verbose = true
end

task :build do
  sh "cd ext; ruby extconf.rb; make"
end
