class HelpController < ApplicationController
  caches_page :index
  layout 'main'

  def index
    #@top_partial = 'help'
    @side_partial = 'side'
    @page = 'help';
  end

  def sites

  end
end
