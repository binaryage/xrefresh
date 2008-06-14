class PresetsController < ApplicationController
  layout 'rainbow', :except => ['css']
  before_filter :header_css, :except => ['index']

  def index
    @page = 'presets'
    @snippets = Snippet.find(:all)
  end

  def header_css
    response.headers['Content-type'] = 'text/css'
  end

  def css
    @snippets = Snippet.find(:all)
  end

end
