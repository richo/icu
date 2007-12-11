PKG_VERSION = '0.9.1'

require 'rake/gempackagetask'

spec = Gem::Specification.new do |s| 
  s.name = 'icu'
  s.version = PKG_VERSION
  s.author = 'Joshua Llorach'
  s.email = 'josh@vectrice.com'
  s.homepage = 'http://vectrice.com/'
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